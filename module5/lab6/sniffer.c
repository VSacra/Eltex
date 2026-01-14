#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/inet.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/if_ether.h>

MODULE_AUTHOR("Sacra");
MODULE_DESCRIPTION("Netfilter module for filtering outgoing packets by IP blacklist");
MODULE_LICENSE("GPL");


struct blacklisted_ip {
    __be32 ip_addr;
    struct list_head list;
};


static LIST_HEAD(blacklist);
static DEFINE_SPINLOCK(blacklist_lock);
static struct kobject *blacklist_kobj;


static struct nf_hook_ops nf_out;


static ssize_t blacklist_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t blacklist_add_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
static ssize_t blacklist_remove_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);


static struct kobj_attribute blacklist_attribute = __ATTR(blacklist, 0444, blacklist_show, NULL);
static struct kobj_attribute add_attribute = __ATTR(add, 0200, NULL, blacklist_add_store);
static struct kobj_attribute remove_attribute = __ATTR(remove, 0200, NULL, blacklist_remove_store);

static struct attribute *attrs[] = {
    &blacklist_attribute.attr,
    &add_attribute.attr,
    &remove_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};


static bool is_ip_blacklisted(__be32 ip_addr)
{
    struct blacklisted_ip *entry;
    bool found = false;
    unsigned long flags;
    
    spin_lock_irqsave(&blacklist_lock, flags);
    list_for_each_entry(entry, &blacklist, list) {
        if (entry->ip_addr == ip_addr) {
            found = true;
            break;
        }
    }
    spin_unlock_irqrestore(&blacklist_lock, flags);
    
    return found;
}


static int add_to_blacklist(__be32 ip_addr)
{
    struct blacklisted_ip *new_entry;
    unsigned long flags;
    

    if (is_ip_blacklisted(ip_addr)) {
        printk(KERN_INFO "IP %pI4 already in blacklist\n", &ip_addr);
        return -EEXIST;
    }
    

    new_entry = kmalloc(sizeof(struct blacklisted_ip), GFP_KERNEL);
    if (!new_entry) {
        return -ENOMEM;
    }
    

    new_entry->ip_addr = ip_addr;
    

    spin_lock_irqsave(&blacklist_lock, flags);
    list_add_tail(&new_entry->list, &blacklist);
    spin_unlock_irqrestore(&blacklist_lock, flags);
    
    printk(KERN_INFO "Added IP %pI4 to blacklist\n", &ip_addr);
    return 0;
}


static int remove_from_blacklist(__be32 ip_addr)
{
    struct blacklisted_ip *entry, *tmp;
    unsigned long flags;
    bool found = false;
    
    spin_lock_irqsave(&blacklist_lock, flags);
    list_for_each_entry_safe(entry, tmp, &blacklist, list) {
        if (entry->ip_addr == ip_addr) {
            list_del(&entry->list);
            kfree(entry);
            found = true;
            break;
        }
    }
    spin_unlock_irqrestore(&blacklist_lock, flags);
    
    if (found) {
        printk(KERN_INFO "Removed IP %pI4 from blacklist\n", &ip_addr);
        return 0;
    } else {
        printk(KERN_INFO "IP %pI4 not found in blacklist\n", &ip_addr);
        return -ENOENT;
    }
}


static void clear_blacklist(void)
{
    struct blacklisted_ip *entry, *tmp;
    unsigned long flags;
    
    spin_lock_irqsave(&blacklist_lock, flags);
    list_for_each_entry_safe(entry, tmp, &blacklist, list) {
        list_del(&entry->list);
        kfree(entry);
    }
    spin_unlock_irqrestore(&blacklist_lock, flags);
}


static unsigned int hook_func_out(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct ethhdr *eth;
    struct iphdr *ip_header;
    
    eth = (struct ethhdr *)skb_mac_header(skb);
    ip_header = (struct iphdr *)skb_network_header(skb);
    
    if (!eth || !ip_header) {
        return NF_ACCEPT;
    }
    
    printk(KERN_INFO "src mac %pM, dst mac %pM\n", eth->h_source, eth->h_dest);
    printk(KERN_INFO "src IP addr: %pI4, dst IP addr: %pI4\n", 
           &ip_header->saddr, &ip_header->daddr);
    
    if (state->hook == NF_INET_POST_ROUTING) {
        if (is_ip_blacklisted(ip_header->daddr)) {
            printk(KERN_INFO "BLOCKED packet to blacklisted IP: %pI4\n", &ip_header->daddr);
            return NF_DROP;
        }
    }
    
    return NF_ACCEPT;
}

static ssize_t blacklist_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct blacklisted_ip *entry;
    unsigned long flags;
    ssize_t count = 0;
    
    spin_lock_irqsave(&blacklist_lock, flags);
    
    if (list_empty(&blacklist)) {
        count = scnprintf(buf, PAGE_SIZE, "Blacklist is empty\n");
    } else {
        count = scnprintf(buf, PAGE_SIZE, "Blacklisted IP addresses:\n");
        list_for_each_entry(entry, &blacklist, list) {
            count += scnprintf(buf + count, PAGE_SIZE - count, "%pI4\n", &entry->ip_addr);
        }
    }
    
    spin_unlock_irqrestore(&blacklist_lock, flags);
    
    return count;
}

static ssize_t blacklist_add_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    __be32 ip_addr;
    int ret;
    
    ret = in4_pton(buf, count, (u8 *)&ip_addr, '\0', NULL);
    if (ret != 1) {
        printk(KERN_ERR "Invalid IP address format\n");
        return -EINVAL;
    }
    
    ret = add_to_blacklist(ip_addr);
    if (ret) {
        return ret;
    }
    
    return count;
}

static ssize_t blacklist_remove_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    __be32 ip_addr;
    int ret;
    
    ret = in4_pton(buf, count, (u8 *)&ip_addr, '\0', NULL);
    if (ret != 1) {
        printk(KERN_ERR "Invalid IP address format\n");
        return -EINVAL;
    }
    
    ret = remove_from_blacklist(ip_addr);
    if (ret) {
        return ret;
    }
    
    return count;
}

static int __init init_main(void)
{
    int ret;
    
    printk(KERN_INFO "Netfilter blacklist module loading\n");
    
    INIT_LIST_HEAD(&blacklist);
    
    nf_out.hook = hook_func_out;
    nf_out.hooknum = NF_INET_POST_ROUTING; 
    nf_out.pf = PF_INET;
    nf_out.priority = NF_IP_PRI_FIRST;
    
    ret = nf_register_net_hook(&init_net, &nf_out);
    if (ret < 0) {
        printk(KERN_ERR "Failed to register netfilter hook\n");
        return ret;
    }
    
    blacklist_kobj = kobject_create_and_add("ip_blacklist", kernel_kobj);
    if (!blacklist_kobj) {
        nf_unregister_net_hook(&init_net, &nf_out);
        printk(KERN_ERR "Failed to create kobject\n");
        return -ENOMEM;
    }
    
    ret = sysfs_create_group(blacklist_kobj, &attr_group);
    if (ret) {
        kobject_put(blacklist_kobj);
        nf_unregister_net_hook(&init_net, &nf_out);
        printk(KERN_ERR "Failed to create sysfs group\n");
        return ret;
    }
    
    printk(KERN_INFO "Netfilter blacklist module loaded successfully\n");
    printk(KERN_INFO "Sysfs interface: /sys/kernel/ip_blacklist/\n");
    
    return 0;
}

static void __exit cleanup_main(void)
{
    sysfs_remove_group(blacklist_kobj, &attr_group);
    kobject_put(blacklist_kobj);
    
    nf_unregister_net_hook(&init_net, &nf_out);
    
    clear_blacklist();
    
    printk(KERN_INFO "Netfilter blacklist module unloaded\n");
}

module_init(init_main);
module_exit(cleanup_main);
