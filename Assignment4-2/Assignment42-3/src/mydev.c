#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  

#define DEVICE_MAJOR 0 ///< Requested device node major number or 0 for dynamic allocation
#define DEVICE_NAME "mydev1" ///< In this implementation, the device name has nothing to do with the name of the device in /dev. You must use mknod to create the device node in /dev


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arnar Sveinn");
MODULE_DESCRIPTION("A simple Linux LKM that accepts characters from the user.");
MODULE_VERSION("0.1");

// The prototype functions for the character driver -- must come before the struct definition
static int mydev_open(struct inode *, struct file *);
static int mydev_release(struct inode *, struct file *);
static ssize_t mydev_write(struct file *, const char __user *buf, size_t count, loff_t *ppos);

static struct file_operations fops = {
   .open = mydev_open,
   .write = mydev_write,
   .release = mydev_release,
};

static ssize_t mydev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    printk(KERN_INFO "mydev: accepting %zu bytes from the user\n", count);
    return count;
}

static int mydev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "mydev: Device successfully closed\n");
    return 0;
}

static int __init mydev_init(void)
{
    int majorNumber = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "mydev failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "mydev: registered with major number %d\n", majorNumber);
    return 0;
}

static void __exit mydev_exit(void)
{
    unregister_chrdev(DEVICE_MAJOR, DEVICE_NAME);
    printk(KERN_INFO "mydev: Goodbye from the LKM\n");
}

static int mydev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "mydev: Device has been opened\n");
    return 0;
}

module_init(mydev_init);
module_exit(mydev_exit);