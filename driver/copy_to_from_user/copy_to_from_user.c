/*************************************************************************
	> File Name: copy_to_from_user.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: 2020年04月01日 星期三 16时11分05秒

	sudo insmod copy_to_from_user.ko
	设备号 major = 243; minor = 10
	创建设备 sudo  mknod /dev/simple c 243 10
		sudo ./main
	 sudo chmod 666 /dev/simple
		 ./main
	
 ************************************************************************/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>



MODULE_LICENSE("Dual BSD/GPL");

static int count = 10;
static char *init_mesg = "hello,world\n";
static char *exit_mesg = "goodbye\n";
static int major ;
static int minor ;
dev_t devnum;

int static simple_dev_open(struct inode *inode, struct file *file)
{
	printk("file open in simple_dev_open......finished!\n");
	return 0;
}

int static simple_dev_release(struct inode *inode, struct file *file)
{
	printk("file release in simple_dev_release......finished!\n");
	return 0;
}

ssize_t simple_dev_read(struct file *file, char __user *buf,size_t count, loff_t *offset)

{
	char alpha[27];
	int i, cnt;
	printk("***********************\n");
	memset(alpha, 0, 27);
	for(i = 0; i < 26; i++)
		alpha[i] = 'a' + i;

	if(count > 26)
		cnt = 26;
	else
		cnt = count;

	//使用copy_to_user ()函数从driver读数据到user
	if(!copy_to_user((char *)buf, alpha, cnt))
		{
		printk(alpha);
		printk("======%s========= \n",__func__);
		return cnt;
		}
	else
		return -1;
}

ssize_t simple_dev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	char alpha[27];
	int cnt;
	printk("***********************\n");
	memset(alpha, 0, 27);
	if(count > 26)
		cnt = 26;
	else
		cnt = count;
		//使用copy_from_user()函数从user写数据到driver
	if(!copy_from_user((char *)alpha, buf, cnt))
	  {  
		printk(alpha);
		printk("======%s========= \n",__func__);
		return cnt;
	  }  
	else
		return -1;
}

static struct cdev simple_dev;

static struct file_operations fops ={

	.owner = THIS_MODULE,
	.open  = simple_dev_open,
	.release = simple_dev_release,
	.read = simple_dev_read,
	.write = simple_dev_write,

};

 

static int __init simple_init(void)
{
  int i,ret;
  for(i = 0; i < count; i++)
	printk(init_mesg);

  ret = alloc_chrdev_region(&devnum, 10, 1, "simple_dev");

  if(!ret)
  {
	major = MAJOR(devnum);
	minor = MINOR(devnum);
	printk("major = %d; minor = %d\n", major, minor);
  }

  cdev_init(&simple_dev, &fops);
  ret = cdev_add(&simple_dev, devnum, 1);
  return ret;
}

static int __exit simple_exit(void)

{
  printk(exit_mesg);
  cdev_del(&simple_dev);
  unregister_chrdev_region(MKDEV(major, minor),1);
  return 0;
}

 

void simple(void)
{
	printk("good mornig1\n");
}

 

module_param(count, int, S_IRUGO);
module_param(init_mesg, charp, S_IRUGO);
module_param(exit_mesg, charp, S_IRUGO);

EXPORT_SYMBOL_GPL(simple);

module_init(simple_init);
module_exit(simple_exit);

 