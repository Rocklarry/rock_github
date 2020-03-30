#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include <linux/kthread.h>

//#define _DBG_

#define USB_SKEL_VENDOR_ID        0xaaaa
#define USB_SKEL_PRODUCT_ID       0xaa97
#define DSCONT  -2

MODULE_DEVICE_TABLE(usb, artosyn_table);
/* Get a minor range for your devices from the usb maintainer */
#define USB_SKEL_MINOR_BASE	192
/* our private defines. if this grows any larger, use your own .h file */
#define MAX_TRANSFER		2048
#define WRITES_IN_FLIGHT	8

/* table of devices that work with this driver */
static const struct usb_device_id artosyn_table[] = {
		{ USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa89) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa90) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa91) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa92) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa93) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa94) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa95) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa96) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa97) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa98) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa99) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa9a) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa9b) },
        { USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa9c) },
				{ USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa9d) },
				{ USB_DEVICE(USB_SKEL_VENDOR_ID, 0xaa9e) },
				{ }
};

/* Structure to hold all of our device specific stuff */
struct usb_artosyn {
	struct usb_device	*udev;			/* the usb device for this device */
	struct usb_interface	*interface;		/* the interface for this device */
	struct semaphore limit_sem;		/* limiting the number of writes in progress */
	struct usb_anchor	submitted;		/* in case we need to retract our submissions */
	struct urb	*bulk_in_urb;		/* the urb to read data with */
	unsigned char   *bulk_in_buffer;	/* the buffer to receive data */
	size_t	bulk_in_size;		/* the size of the receive buffer */
	size_t	bulk_in_filled;		/* number of bytes in the buffer */
	size_t	bulk_in_copied;		/* already copied to user space */
	volatile __u8	bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
	volatile __u8	bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	volatile __u8	bulk_inter_endpointAddr;	/* the address of the bulk out endpoint */
	int		errors;			/* the last request tanked */
	int		open_count;		/* count the number of openers */
	bool	ongoing_read;		/* a read is going on */
	bool	processed_urb;		/* indicates we haven't processed the urb */
	spinlock_t		err_lock;		/* lock for errors */
	struct kref		kref;
	struct mutex		io_mutex;		/* synchronize I/O with disconnect */
	struct completion	bulk_in_completion;	/* to wait for an ongoing read */
	struct completion	int_out_completion;	/* to wait for an ongoing read */
	struct completion	bulk_out_completion;	/* to wait for an ongoing read */
#ifdef _DBG_
	struct task_struct  *fifo_test;
#endif
	struct task_struct	*fifo_thread;
	volatile int	fifo_size;
	volatile char *fifo_buf;
	volatile int		*buf_filled_len;
	volatile struct mutex	fifo_mutex;
	volatile bool	stop;
	volatile bool	read_fifo_en;
	volatile int		wr_counter;
	volatile int		rd_index;
	volatile int		max_wr_counter;
	volatile int		wr_index;
	volatile int   thread_cnt;

};

#define to_artosyn_dev(d) container_of(d, struct usb_artosyn, kref)
static struct usb_driver artosyn_driver;
static void artosyn_draw_down(struct usb_artosyn *dev);

int getfilledbuf(void *_dev)
{
	struct usb_artosyn *dev = _dev;
	int data;
	while (dev->wr_counter == 0)
	{
		if(dev->stop)
		return DSCONT;
		usleep_range(1000,2000);
	}
	data = dev->rd_index;
	if (++dev->rd_index == dev->max_wr_counter) 
	dev->rd_index=0;
	return data;
}

int getfilledindex(void *_dev)
{
	struct usb_artosyn *dev = _dev;
	int data;
	if(dev->wr_counter == 0)
		return -1;
	data = dev->rd_index;
	return data;
}

int getemptybuf(void *_dev)
{
	struct usb_artosyn *dev = _dev;
	int data;
	if(dev->wr_counter == dev->max_wr_counter)
		return dev->max_wr_counter;
	data = dev->wr_index;
	if (++dev->wr_index == dev->max_wr_counter) 
	dev->wr_index= 0;
	return data;
}

static void artosyn_delete(struct kref *kref)
{
	struct usb_artosyn *dev = to_artosyn_dev(kref);
	dev->stop = 1;
	while(dev->thread_cnt > 0)
		usleep_range(10000,20000);
	usb_free_urb(dev->bulk_in_urb);
	usb_put_dev(dev->udev);
	kfree(dev->bulk_in_buffer);
	kfree(dev->fifo_buf);
	kfree(dev->buf_filled_len);
	kfree(dev);
}

static void stream_on_callback(struct urb *urb)
{
	struct usb_artosyn *dev;
	dev = urb->context;

	if (urb->status) {
		if (!(urb->status == -ENOENT ||
			urb->status == -ECONNRESET ||
		    urb->status == -ESHUTDOWN))
			printk("interface int ep err\n");

		spin_lock(&dev->err_lock);
		dev->errors = urb->status;
		spin_unlock(&dev->err_lock);
	}

	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,urb->transfer_buffer, urb->transfer_dma);
	complete(&dev->int_out_completion);
}

//open的时候开启视频流(固件特性)
int Stream_On(struct usb_artosyn *dev)
{
	struct urb *urb = NULL;
	char *buf = NULL;
	int retval;
		
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) 
	{
		retval = -ENOMEM;
		printk("urb alloc error\n");
		return retval;
	}
	buf = usb_alloc_coherent(dev->udev, 64, GFP_KERNEL,&urb->transfer_dma);
	if (!buf) 
	{
		retval = -ENOMEM;
		usb_free_urb(urb);
		printk("stream on alloc error\n");
		return retval;
	}
	buf[0] = 0x44;
	buf[1] = 0x44;
	buf[2] = 0x44;
	buf[3] = 0x44;
	usb_fill_int_urb(urb, dev->udev,usb_sndintpipe(dev->udev, 0x01),
									buf, 64, stream_on_callback, dev,1);		
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	usb_anchor_urb(urb, &dev->submitted);
	retval = usb_submit_urb(urb, GFP_KERNEL);
	wait_for_completion_interruptible_timeout(&dev->int_out_completion,10000);
	usb_unanchor_urb(urb);
	usb_free_urb(urb);
	
	return retval;
}

static int artosyn_open(struct inode *inode, struct file *file)
{
	struct usb_artosyn *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);
	interface = usb_find_interface(&artosyn_driver, subminor);
	if (!interface) 
	{
		printk("error, can't find device for minor %d",subminor);
		retval = -ENODEV;
		goto exit;
	}
	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}
	kref_get(&dev->kref);
	mutex_lock(&dev->io_mutex);
	if (!dev->open_count++) {
		retval = usb_autopm_get_interface(interface);
			if (retval) {
				dev->open_count--;
				mutex_unlock(&dev->io_mutex);
				kref_put(&dev->kref, artosyn_delete);
				goto exit;
			}
	}
	file->private_data = dev;
	if(dev->bulk_in_endpointAddr == 0x86)
	{
		if(Stream_On(dev) < 0)
		{
			printk("stream on err\n");
			retval = -ENODEV;
		}
	}
	mutex_unlock(&dev->io_mutex);

exit:
	return retval;
}

static int artosyn_release(struct inode *inode, struct file *file)
{
	struct usb_artosyn *dev;

	dev = file->private_data;
	if (dev == NULL)
		return -ENODEV;

	mutex_lock(&dev->io_mutex);
	if (!--dev->open_count && dev->interface)
		usb_autopm_put_interface(dev->interface);
	mutex_unlock(&dev->io_mutex);

	kref_put(&dev->kref, artosyn_delete);
	return 0;
}

static int artosyn_flush(struct file *file, fl_owner_t id)
{
	struct usb_artosyn *dev;
	int res;

	dev = file->private_data;
	if (dev == NULL)
		return -ENODEV;

	/* wait for io to stop */
	mutex_lock(&dev->io_mutex);
	artosyn_draw_down(dev);
	/* read out errors, leave subsequent opens a clean slate */
	spin_lock_irq(&dev->err_lock);
	res = dev->errors ? (dev->errors == -EPIPE ? -EPIPE : -EIO) : 0;
	dev->errors = 0;
	spin_unlock_irq(&dev->err_lock);

	mutex_unlock(&dev->io_mutex);

	return res;
}

static void artosyn_read_bulk_callback(struct urb *urb)
{
	struct usb_artosyn *dev;

	dev = urb->context;
	spin_lock(&dev->err_lock);
	/* sync/async unlink faults aren't errors */
	if (urb->status) 
	{
		if (!(urb->status == -ENOENT ||
		    urb->status == -ECONNRESET ||
		    urb->status == -ESHUTDOWN))
		printk("nonzero read bulk status received: %d",urb->status);
		dev->errors = urb->status;
	} 
	else 
	{
		dev->bulk_in_filled = urb->actual_length;
	}
	dev->ongoing_read = 0;
	spin_unlock(&dev->err_lock);

	complete(&dev->bulk_in_completion);
}

static int artosyn_do_read_io(struct usb_artosyn *dev, size_t count)
{
	int rv;

	/* prepare a read */
	usb_fill_bulk_urb(dev->bulk_in_urb,
			dev->udev,
			usb_rcvbulkpipe(dev->udev,
			dev->bulk_in_endpointAddr),
			dev->bulk_in_buffer,
			min(dev->bulk_in_size, count),
			artosyn_read_bulk_callback,
			dev);
	/* tell everybody to leave the URB alone */
	spin_lock_irq(&dev->err_lock);
	dev->ongoing_read = 1;
	spin_unlock_irq(&dev->err_lock);

	/* do it */
	rv = usb_submit_urb(dev->bulk_in_urb, GFP_KERNEL);
	if (rv < 0) 
	{
		printk("failed submitting read urb, error %d",rv);
		dev->bulk_in_filled = 0;
		rv = (rv == -ENOMEM) ? rv : -EIO;
		spin_lock_irq(&dev->err_lock);
		dev->ongoing_read = 0;
		spin_unlock_irq(&dev->err_lock);
	}
	return rv;
}

static ssize_t artosyn_read(struct file *file, char *buffer, size_t count,loff_t *ppos)
{
	int index;
	int rv;
	size_t cnt = count;
	struct usb_artosyn *dev;
	dev = file->private_data;

	if (!dev->bulk_in_urb)
		return 0;
	rv = mutex_lock_interruptible(&dev->io_mutex);
	if (rv < 0)
		return rv;
	if (!dev->interface) {		/* disconnect() was called */
		rv = -ENODEV;
		goto exit_read;
	}
	if(count > 0)
		dev->read_fifo_en = 1;
	else if(count == 0)
	{
		dev->read_fifo_en = 0;
		usleep_range(500,600);
		dev->wr_counter = 0;
		dev->rd_index = 0;
		dev->wr_index = 0;
		goto exit_read;	
		return 0;
	}

	while(cnt > 0)
	{
		index = getfilledindex(dev);

		if(index < 0 || dev->stop > 0)
			break;
		
		if(cnt >= dev->buf_filled_len[index])
		{
			copy_to_user(buffer,&dev->fifo_buf[index*dev->fifo_size],dev->buf_filled_len[index]);
			buffer += dev->buf_filled_len[index];
			cnt -= dev->buf_filled_len[index];

			if (++dev->rd_index == dev->max_wr_counter) 
				dev->rd_index=0;

			mutex_lock(&dev->fifo_mutex);
			--dev->wr_counter;
			mutex_unlock(&dev->fifo_mutex);
		}
		else
		{
			copy_to_user(buffer,&dev->fifo_buf[index*dev->fifo_size],cnt);
			buffer += cnt;
    	dev->buf_filled_len[index] -= cnt;
			memcpy(&dev->fifo_buf[index*dev->fifo_size],&dev->fifo_buf[index*dev->fifo_size+cnt],dev->buf_filled_len[index]);
			cnt = 0;
		}

	}

exit_read:
	mutex_unlock(&dev->io_mutex);
	if(rv < 0)
		return rv;
	if(dev->stop > 0)
		return -1;
	else
		return (count - cnt);
}

static void artosyn_write_bulk_callback(struct urb *urb)
{
	struct usb_artosyn *dev;

	dev = urb->context;
	/* sync/async unlink faults aren't errors */
	if (urb->status) 
	{
		if (!(urb->status == -ENOENT ||
		    urb->status == -ECONNRESET ||
		    urb->status == -ESHUTDOWN))
			printk("nonzero write bulk status received: %d",urb->status);

		spin_lock(&dev->err_lock);
		dev->errors = urb->status;
		spin_unlock(&dev->err_lock);
	}
	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			  urb->transfer_buffer, urb->transfer_dma);		  
	complete(&dev->bulk_out_completion);
}

static ssize_t artosyn_write(struct file *file, const char *user_buffer,
			  size_t count, loff_t *ppos)
{
	struct usb_artosyn *dev;
	int retval = 0;
	struct urb *urb = NULL;
	char *buf = NULL;
	size_t writesize = min(count, (size_t)MAX_TRANSFER);

	dev = file->private_data;
	if (count == 0)
		goto exit;
	retval = dev->errors;
	if (retval < 0) 
	{
		dev->errors = 0;
		retval = (retval == -EPIPE) ? retval : -EIO;
	}
	if (retval < 0)
		goto error;
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) 
	{
		retval = -ENOMEM;
		goto error;
	}
	buf = usb_alloc_coherent(dev->udev, writesize, GFP_KERNEL,&urb->transfer_dma);
	if (!buf) 
	{
		retval = -ENOMEM;
		goto error;
	}
	if (copy_from_user(buf, user_buffer, writesize)) 
	{
		retval = -EFAULT;
		goto error;
	}
	mutex_lock(&dev->io_mutex);
	if (!dev->interface) 
	{		
		mutex_unlock(&dev->io_mutex);
		retval = -ENODEV;
		goto error;
	}						
	usb_fill_int_urb(urb, dev->udev,
		usb_sndintpipe(dev->udev, dev->bulk_inter_endpointAddr),
		buf, writesize, artosyn_write_bulk_callback, dev,1);
		urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
		usb_anchor_urb(urb, &dev->submitted);
	retval = usb_submit_urb(urb, GFP_KERNEL);
	mutex_unlock(&dev->io_mutex);
	if (retval)
	{
		goto error_unanchor;
	}
	wait_for_completion_interruptible_timeout(&dev->bulk_out_completion,10000);
	usb_free_urb(urb);
	return writesize;

error_unanchor:
	usb_unanchor_urb(urb);
error:
	if (urb) {
		usb_free_coherent(dev->udev, writesize, buf, urb->transfer_dma);
		usb_free_urb(urb);
	}
	up(&dev->limit_sem);

exit:
	return retval;
}

static const struct file_operations artosyn_fops = {
	.owner =	THIS_MODULE,
	.read =		artosyn_read,
	.write =	artosyn_write,
	.open =		artosyn_open,
	.release =	artosyn_release,
	.flush =	artosyn_flush,
	.llseek =	noop_llseek,
};

static struct usb_class_driver artosyn_class = {
	.name =		"artosyn_port%d",
	.fops =		&artosyn_fops,
	.minor_base =	USB_SKEL_MINOR_BASE,
};

static int fifo_handle(void *_dev)
{
	struct usb_artosyn *dev = _dev;
	int *buf_filled_len = dev->buf_filled_len;
	int index;
	int rv;
	int max_flag;

	dev->thread_cnt ++;
	while(!dev->stop)
	{
		if(dev->read_fifo_en)
		{
			rv = artosyn_do_read_io(dev, dev->fifo_size);
			if (rv < 0)
			{
				printk("direct read error  %x \n",rv);
				usleep_range(10000,20000);
				if(-ENODEV == rv)
					break;
				continue;
			}
			do
			{
				rv = wait_for_completion_interruptible_timeout(&dev->bulk_in_completion,100);
			}while(rv == 0 || dev->stop > 0);

			rv = dev->errors;
			if (rv < 0) 
			{
				dev->errors = 0;
				rv = (rv == -EPIPE) ? rv : -EIO;
				dev->bulk_in_filled = 0;
				printk("read from usb error\n");
				continue;
			}
			
			max_flag = 0;
			while((index = getemptybuf(dev)) ==  dev->max_wr_counter)
			{
				if ( 0 == max_flag)
				{
					printk(KERN_ERR"this buf is full, waiting the next reading----\n");
					max_flag = 1;
				}
				if(!dev->read_fifo_en)
					break;
				else if(dev->stop)
				{
					if(dev->thread_cnt > 0)
					dev->thread_cnt --;
					return 0;
				}

				usleep_range(500,600);
			}
			memcpy((dev->fifo_buf + index * dev->fifo_size),dev->bulk_in_buffer,dev->bulk_in_filled);
			buf_filled_len[index] = dev->bulk_in_filled;
			mutex_lock(&dev->fifo_mutex);
			++dev->wr_counter;
			mutex_unlock(&dev->fifo_mutex);
		}
		else
			usleep_range(20000,25000);
	}

	if(dev->thread_cnt > 0)
		dev->thread_cnt --;

}
	
#ifdef _DBG_
static int fifo_handle_test(void *_dev)
{
	struct usb_artosyn *dev = _dev;
	int i;
	
	dev->thread_cnt ++;
	while(!dev->stop)
	{
		printk("buf cnt in drv : %d\n",dev->wr_counter);
		for(i=0;i<10;i++)
		{
			if(dev->stop)
				break;
			usleep_range(100000,110000);
		}
	}

	if(dev->thread_cnt > 0)
		dev->thread_cnt --;

}
#endif

static int artosyn_probe(struct usb_interface *interface,const struct usb_device_id *id)
{
	struct usb_artosyn *dev;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;

	/* allocate memory for our device state and initialize it */
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) 
	{
		printk("Out of memory\n");
		goto error;
	}
	kref_init(&dev->kref);
	sema_init(&dev->limit_sem, WRITES_IN_FLIGHT);
	mutex_init(&dev->io_mutex);
	spin_lock_init(&dev->err_lock);
	init_usb_anchor(&dev->submitted);
	init_completion(&dev->bulk_in_completion);
	init_completion(&dev->int_out_completion);
	init_completion(&dev->bulk_out_completion);
	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	/* set up the endpoint information */
	/* use only the first bulk-in and bulk-out endpoints */
	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) 
	{
		endpoint = &iface_desc->endpoint[i].desc;
		if (!dev->bulk_in_endpointAddr && usb_endpoint_is_bulk_in(endpoint)) 
		{
			/* we found a bulk in endpoint */
			buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
			dev->bulk_in_size = buffer_size;
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) 
			{
				printk("Could not allocate bulk_in_buffer\n");
				goto error;
			}
			dev->bulk_in_urb = usb_alloc_urb(0, GFP_KERNEL);
			if (!dev->bulk_in_urb) 
			{
				printk("Could not allocate bulk_in_urb\n");
				goto error;
			}
		}
		if (!dev->bulk_out_endpointAddr && usb_endpoint_is_bulk_out(endpoint)) 
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		if (!dev->bulk_inter_endpointAddr && usb_endpoint_is_int_out(endpoint)) 
			dev->bulk_inter_endpointAddr = endpoint->bEndpointAddress;
	}
	if (!(dev->bulk_in_endpointAddr) && !(dev->bulk_out_endpointAddr) && !(dev->bulk_inter_endpointAddr))
		goto error;
	/* save our data pointer in this interface device */
	usb_set_intfdata(interface, dev);
	/* we can register the device now, as it is ready */
	retval = usb_register_dev(interface, &artosyn_class);
	if (retval) {
		/* something prevented us from registering this driver */
		printk("Not able to get a minor for this device.\n");
		usb_set_intfdata(interface, NULL);
		goto error;
	}
	/* let the user know what node this device is now attached to */
	dev->thread_cnt = 0;
//fifo_init
	mutex_init(&dev->fifo_mutex);
	dev->fifo_size = dev->bulk_in_size;
	dev->max_wr_counter = 0x2000;
	dev->fifo_buf = kmalloc(dev->fifo_size * dev->max_wr_counter, GFP_KERNEL);
	if (!dev->fifo_buf) 
	{
		printk("Could not allocate fifo_buffer\n");
		goto error;
	}
	dev->buf_filled_len = kmalloc(dev->max_wr_counter * sizeof(int), GFP_KERNEL);
	if (!dev->buf_filled_len) 
	{
		printk("Could not allocate buf_filled_len\n");
		goto error;
	}
	dev->wr_counter = 0;
	dev->rd_index = 0;
	dev->wr_index = 0;
	dev->read_fifo_en = 0;
	dev->stop = 0;
	dev->fifo_thread =kthread_run(fifo_handle, dev, "usb fifo");
	if (IS_ERR(dev->fifo_thread)) 
	{
		PTR_ERR(dev->fifo_thread);
		dev->fifo_thread = NULL;
		goto error;
	}
#ifdef _DBG_
	dev->fifo_test =kthread_run(fifo_handle_test, dev, "usb fifo test");
	if (IS_ERR(dev->fifo_test))
	{
		PTR_ERR(dev->fifo_test);
		dev->fifo_test = NULL;
		goto error;
	}
#endif
	printk("usb fifo info: fifi_size = %d\n",dev->fifo_size);
	return 0;

error:
	if (dev)
		kref_put(&dev->kref, artosyn_delete);
	return retval;
}

static void artosyn_disconnect(struct usb_interface *interface)
{
	struct usb_artosyn *dev;
	int minor = interface->minor;

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);
	/* give back our minor */
	usb_deregister_dev(interface, &artosyn_class);
	/* prevent more I/O from starting */
	mutex_lock(&dev->io_mutex);
	dev->interface = NULL;
	mutex_unlock(&dev->io_mutex);
	usb_kill_anchored_urbs(&dev->submitted);
	/* decrement our usage count */
	kref_put(&dev->kref, artosyn_delete);
	dev_info(&interface->dev, "USB artosyn #%d now disconnected", minor);
}

static void artosyn_draw_down(struct usb_artosyn *dev)
{
	int time;

	time = usb_wait_anchor_empty_timeout(&dev->submitted, 1000);
	if (!time)
		usb_kill_anchored_urbs(&dev->submitted);
	usb_kill_urb(dev->bulk_in_urb);
}

static int artosyn_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct usb_artosyn *dev = usb_get_intfdata(intf);

	if (!dev)
		return 0;
	artosyn_draw_down(dev);
	return 0;
}

static int artosyn_resume(struct usb_interface *intf)
{
	return 0;
}

static int artosyn_pre_reset(struct usb_interface *intf)
{
	struct usb_artosyn *dev = usb_get_intfdata(intf);

	mutex_lock(&dev->io_mutex);
	artosyn_draw_down(dev);
	return 0;
}

static int artosyn_post_reset(struct usb_interface *intf)
{
	struct usb_artosyn *dev = usb_get_intfdata(intf);

	/* we are sure no URBs are active - no locking needed */
	dev->errors = -EPIPE;
	mutex_unlock(&dev->io_mutex);

	return 0;
}

static struct usb_driver artosyn_driver = {
	.name =		"skeleton",
	.probe =	artosyn_probe,
	.disconnect =	artosyn_disconnect,
	.suspend =	artosyn_suspend,
	.resume =	artosyn_resume,
	.pre_reset =	artosyn_pre_reset,
	.post_reset =	artosyn_post_reset,
	.id_table =	artosyn_table,
	.supports_autosuspend = 1,
};

static int __init usb_artosyn_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&artosyn_driver);
	if (result)
		printk("usb_register failed. Error number %d", result);

	return result;
}

static void __exit usb_artosyn_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&artosyn_driver);
}

module_init(usb_artosyn_init);
module_exit(usb_artosyn_exit);

MODULE_LICENSE("GPL");
