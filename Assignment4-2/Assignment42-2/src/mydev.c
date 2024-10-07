#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  // For copy_from_user

#define DEVICE_NAME "mydev1"
#define DEVICE_MAJOR 0

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arnar Sveinn");
MODULE_DESCRIPTION("A simple Linux LKM that accepts characters from the user.");
MODULE_VERSION("0.1");

// Declare the functions before using them in file_operations
static int mydev_open(struct inode *inodep, struct file *filep);
static ssize_t mydev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static int mydev_release(struct inode *inodep, struct file *filep);

static struct file_operations fops =
{
   .open = mydev_open,
   .write = mydev_write,
   .release = mydev_release,
};

// Define the write function
static ssize_t mydev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    printk(KERN_INFO "mydev: accepting %zu bytes from the user\n", count);
    return count;
}

// Define the open function
static int mydev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "mydev: Device has been opened\n");
    return 0;
}

// Define the release function
static int mydev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "mydev: Device successfully closed\n");
    return 0;
}

// Initialize the module
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

// Exit and cleanup the module
static void __exit mydev_exit(void)
{
    unregister_chrdev(DEVICE_MAJOR, DEVICE_NAME);
    printk(KERN_INFO "mydev: Goodbye from the LKM\n");
}

module_init(mydev_init);
module_exit(mydev_exit);