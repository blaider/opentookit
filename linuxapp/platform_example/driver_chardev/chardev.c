#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>

static int major;

typedef struct adv_driver{
	char buf[2048];
}ADV_DRIVER;

static int adv_open(struct inode *inode, struct file *file)
{
    ADV_DRIVER *p = file->private_data;
    printk("%s\n",__func__);

	if (!p) {
		p = kmalloc(sizeof(*p), GFP_KERNEL);
		if (!p)
			return -ENOMEM;
		file->private_data = p;
	}
	printk("f_mode:%x\n",file->f_mode);
	file->f_version = 0;
	file->f_mode &= ~FMODE_PWRITE;
    return 0;
}

static int adv_close(struct inode *inode, struct file *file)
{
	ADV_DRIVER *p = file->private_data;
	printk("%s\n",__func__);
	if(p)
	{
		kfree(p);
		file->private_data = NULL;
	}
	return 0;
}

static ssize_t adv_read (struct file *file, char __user *buff, size_t len, loff_t * off)
{
	ADV_DRIVER *m = file->private_data;
	printk("%s\n",__func__);
	printk("len:%d,off:%lld\n",len,*off);
	if(!copy_to_user((char *)buff, m->buf, cnt))
	         return cnt;
	else
	     return -1;
	return len;
}
static ssize_t adv_write (struct file *file, const char __user *buff, size_t len, loff_t * off)
{
	ADV_DRIVER *m = file->private_data;
	printk("%s\n",__func__);
	printk("len:%d,off:%lld\n",len,*off);
	copy_from_user(m->buf,buff, len);
	return len;
}

#define IOCTL_KLIB_CREATE_EVENT               _IO('k', 1)
static long adv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("%s\n",__func__);
   switch(cmd)
   {
   case IOCTL_KLIB_CREATE_EVENT:
	   break;
   default:
	   break;
   }
   return 0;
}

static struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open  = adv_open,
    .release        = adv_close,
    .read = adv_read,
    .write = adv_write,
    .unlocked_ioctl = adv_ioctl,
};

static struct cdev hello_cdev;
static struct class *cls;

static int hello_init(void)
{
    dev_t devid;

	/* (major,0~1) 对应 hello_fops, (major, 2~255)都不对应hello_fops */
	alloc_chrdev_region(&devid, 0, 1, "hello");
	major = MAJOR(devid);

    cdev_init(&hello_cdev, &hello_fops);
    cdev_add(&hello_cdev, devid, 1);

    cls = class_create(THIS_MODULE, "hello");
    device_create(cls, NULL, MKDEV(major, 0), NULL, "hello""%d", MINOR(devid)); /* /dev/hello0 */

    return 0;
}

static void hello_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);

    cdev_del(&hello_cdev);
    unregister_chrdev_region(MKDEV(major, 0), 1);
}

module_init(hello_init);
module_exit(hello_exit);


MODULE_LICENSE("GPL");
