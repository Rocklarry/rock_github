#include <linux/module.h>
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/input.h>
#include <linux/input/mt.h>

struct virtual_touch_dev{
    struct input_dev *input_dev;
    struct kobject *kobj;
    uint32_t w;
    uint32_t h;
};

static struct virtual_touch_dev *dev = NULL;
static ssize_t virtual_touch_store(struct device *d,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
    if (sscanf(buf, "%d%d", &dev->w, &dev->h) == 2) {
        input_set_abs_params(dev->input_dev, ABS_MT_POSITION_X, 0,
                dev->w, 0, 0);
        input_set_abs_params(dev->input_dev, ABS_MT_POSITION_Y, 0,
                dev->h, 0, 0);
        printk("%s virtual touch screen resolution updated\n", __func__);
    }

    return count;
}

static ssize_t virtual_touch_show(struct device *d,
        struct device_attribute *attr,
        char *buf)
{
    return sprintf(buf, "%d %d\n", dev->w, dev->h);
}
//static DEVICE_ATTR(res,0777,virtual_touch_show,virtual_touch_store);//modify stephen
static DEVICE_ATTR(res,(S_IRUGO | S_IWUSR | S_IWGRP),virtual_touch_show,virtual_touch_store);


static int __init virtual_touch_init(void)
{
    int ret;
    struct input_dev * input_dev;

    dev = kzalloc(sizeof(struct virtual_touch_dev),GFP_KERNEL);
    if(dev == NULL){
        printk("%s get dev memory error\n",__func__);
        return -ENOMEM;
    }
    dev->kobj = kobject_create_and_add("virtual_touch", NULL);
    if(dev->kobj == NULL){
        ret = -ENOMEM;
        goto kobj_err;
    }

    ret = sysfs_create_file(dev->kobj,&dev_attr_res.attr);
    if(ret < 0){
        goto file_err;
    }
    input_dev = input_allocate_device();
    if (!input_dev) {
        ret = -ENOMEM;
        printk("%s failed to allocate input device\n", __func__);
        goto file_err;
    }
    dev->w = 720;
    dev->h = 1280;
    dev->input_dev = input_dev;
    input_dev->name ="virtual_touch";
    __set_bit(EV_KEY, input_dev->evbit);
    __set_bit(EV_ABS, input_dev->evbit);
    __set_bit(BTN_TOUCH, input_dev->keybit);
    __set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

    input_mt_init_slots(input_dev, 2, 0);
    input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0,
            dev->w, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0,
            dev->h, 0, 0);
    //input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, FT_PRESS, 0, 0);

    ret = input_register_device(input_dev);
    if (ret) {
        printk("%s Input device registration failed\n", __func__);
        goto free_inputdev;
    }
    return 0;
free_inputdev:
    input_free_device(input_dev);
file_err:
    kobject_del(dev->kobj);
kobj_err:
    kfree(dev);
    return ret;
}

static void __exit virtual_touch_exit(void)
{
    sysfs_remove_file(dev->kobj,&dev_attr_res.attr);
    kobject_del(dev->kobj);
    input_unregister_device(dev->input_dev);
    if(dev != NULL)
        kfree(dev);
}

module_init(virtual_touch_init);
module_exit(virtual_touch_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xugaoming");
