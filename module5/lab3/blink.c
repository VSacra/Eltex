#include <linux/module.h> 
#include <linux/printk.h> 
#include <linux/kobject.h> 
#include <linux/sysfs.h> 
#include <linux/init.h> 
#include <linux/fs.h> 
#include <linux/string.h>
#include <linux/tty.h>          /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h>           /* For KDSETLED */
#include <linux/vt.h>
#include <linux/console_struct.h>       /* For vc_cons */
#include <linux/vt_kern.h>
#include <linux/timer.h>

static struct kobject *example_kobject;
struct timer_list my_timer;
struct tty_driver *my_driver;
static int _kbledstatus = 0;
static int test = 3;  
#define BLINK_DELAY   HZ/5
#define RESTORE_LEDS  0xFF

static void my_timer_func(struct timer_list *t)
{
    if (_kbledstatus == test)
        _kbledstatus = RESTORE_LEDS;
    else
        _kbledstatus = test;
    
    if (my_driver && my_driver->ops && my_driver->ops->ioctl) {
        (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, _kbledstatus);
    }
    
    mod_timer(&my_timer, jiffies + BLINK_DELAY);
}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
    return sprintf(buf, "%d\n", test);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count)
{
    unsigned long val;
    int ret;
    
    ret = kstrtoul(buf, 10, &val);
    if (ret < 0)
        return ret;
    
    if (val > 7)
        return -EINVAL;
    
    test = val;
    
    if (my_driver && my_driver->ops && my_driver->ops->ioctl) {
        (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, test);
        _kbledstatus = test;
    }
    
    return count;
}

static struct kobj_attribute foo_attribute = __ATTR(test, 0660, foo_show,
                                                   foo_store);

static int __init sys_init(void)
{
    int error = 0;
    int i;

    printk(KERN_INFO "Keyboard LED module: loading\n");
    
    example_kobject = kobject_create_and_add("systest", kernel_kobj);
    if (!example_kobject)
        return -ENOMEM;

    error = sysfs_create_file(example_kobject, &foo_attribute.attr);
    if (error) {
        printk(KERN_ERR "Failed to create sysfs file\n");
        kobject_put(example_kobject);
        return error;
    }

    printk(KERN_INFO "Keyboard LED module: fgconsole is %x\n", fg_console);
    
    for (i = 0; i < MAX_NR_CONSOLES; i++) {
        if (!vc_cons[i].d)
            break;
        printk(KERN_INFO "Keyboard LED module: console[%i/%i] #%i\n", i,
               MAX_NR_CONSOLES, vc_cons[i].d->vc_num);
    }
    
    if (vc_cons[fg_console].d && vc_cons[fg_console].d->port.tty) {
        my_driver = vc_cons[fg_console].d->port.tty->driver;
        if (my_driver && my_driver->ops && my_driver->ops->ioctl) {
            printk(KERN_INFO "Keyboard LED module: tty driver found\n");
            
            timer_setup(&my_timer, my_timer_func, 0);
            my_timer.expires = jiffies + BLINK_DELAY;
            add_timer(&my_timer);
            
            (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, test);
            _kbledstatus = test;
        }
    }

    return 0;
}

static void __exit sys_exit(void)
{
    printk(KERN_INFO "Keyboard LED module: unloading...\n");
    
    del_timer_sync(&my_timer);
    
    if (my_driver && my_driver->ops && my_driver->ops->ioctl) {
        (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
    }
    
    sysfs_remove_file(example_kobject, &foo_attribute.attr);
    kobject_put(example_kobject);
    
    printk(KERN_INFO "Keyboard LED module: unloaded successfully\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sacra");
module_init(sys_init);
module_exit(sys_exit);
