#include <linux/cdev.h> 
#include <linux/delay.h> 
#include <linux/device.h> 
#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/irq.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/poll.h> 

MODULE_LICENSE("GPL");

/*  Prototypes - this would normally go in a .h file */ 
static int device_open(struct inode *, struct file *); 
static int device_release(struct inode *, struct file *); 
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *); 
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *); 

#define SUCCESS 0 
#define DEVICE_NAME "sacradev" /* Dev name as it appears in /proc/devices   */ 
#define BUF_LEN 80 /* Max length of the message from the device */ 
/* Global variables are declared as static, so are global within the file. */ 

static int major; /* major number assigned to our device driver */ 
enum { 
    CDEV_NOT_USED = 0, 
    CDEV_EXCLUSIVE_OPEN = 1, 
}; 

/* Is device open? Used to prevent multiple access to device */ 
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED); 
static struct class *cls; 
static struct file_operations chardev_fops = { 
    .read = device_read, 
    .write = device_write, 
    .open = device_open, 
    .release = device_release, 
}; 

static int __init chardev_init(void) { 
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops); 

    if (major < 0) { 
        pr_alert("Registering char device failed with %d\n", major); 
        return major; 
    } 
    pr_info("I was assigned major number %d.\n", major); 

    cls = class_create(DEVICE_NAME); 
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME); 
    pr_info("Device created on /dev/%s\n", DEVICE_NAME); 
    return SUCCESS; 
} 

static void __exit chardev_exit(void){ 
    device_destroy(cls, MKDEV(major, 0)); 
    class_destroy(cls); 

	/* Unregister the device */ 
    unregister_chrdev(major, DEVICE_NAME); 
} 

static char msg[BUF_LEN]; 
static int read_counter = 0; 
static char write_msg[BUF_LEN]; 
static bool has_user_message = false; 


/* Called when a process tries to open the device file */
static int device_open(struct inode *inode, struct file *file) {
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;
    
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/* Called when a process closes the device file */
static int device_release(struct inode *inode, struct file *file) {
    atomic_set(&already_open, CDEV_NOT_USED);
    module_put(THIS_MODULE);
    return SUCCESS;
}

/* Called when a process reads from the device */
static ssize_t device_read(struct file *filp, char __user *buffer,
                           size_t length, loff_t *offset) {
    int bytes_read = 0;
    const char *msg_ptr;
    

    if (has_user_message) {
        msg_ptr = write_msg; 
    } else {
        sprintf(msg, "I already told you %d times Hello world!\n", read_counter);
        msg_ptr = msg;
    }
    
    if (!has_user_message) {
        read_counter++;
    }
    
    if (!*(msg_ptr + *offset)) { 
        *offset = 0;
        
        if (has_user_message) {
            has_user_message = false;
        }
        
        return 0;
    }
    
    msg_ptr += *offset;
    
    while (length && *msg_ptr) {
        if (put_user(*(msg_ptr++), buffer++)) {
            return -EFAULT;
        }
        length--;
        bytes_read++;
    }
    
    *offset += bytes_read;
    return bytes_read;
}

/* Called when a process writes to dev file */
static ssize_t device_write(struct file *filp, const char __user *buff,
                            size_t len, loff_t *off) {
    int bytes_written = 0;
    
    if (len > BUF_LEN - 1) {
        len = BUF_LEN - 1;
    }
    
    while (len > 0) {
        char c;
        
        if (get_user(c, buff + bytes_written)) {
            return -EFAULT;
        }
        
        write_msg[bytes_written] = c;
        bytes_written++;
        len--;
        
        if (c == '\n') {
            break;
        }
    }
    

    write_msg[bytes_written] = '\0';
    

    has_user_message = true;
    

    pr_info("Received %d bytes from userspace: %s\n", bytes_written, write_msg);
    

    *off += bytes_written;
    
    return bytes_written;
}

module_init(chardev_init);
module_exit(chardev_exit);
