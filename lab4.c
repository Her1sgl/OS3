#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define PROCFS_NAME "lab3"

static struct proc_dir_entry *our_proc_file = NULL;
static unsigned long prev_value = 0;
static unsigned long current_value = 1;

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, 
                              size_t buffer_length, loff_t *offset)
{
    char s[32];
    int len;
    unsigned long next_value;

    if (*offset > 0)
        return 0;

    len = snprintf(s, sizeof(s), "%lu\n", current_value);

    if (copy_to_user(buffer, s, len))
        return -EFAULT;

    *offset += len;

    pr_info("procfile read: %s, current value: %lu\n", file_pointer->f_path.dentry->d_name.name, current_value);

    next_value = prev_value + current_value;
    prev_value = current_value;
    current_value = next_value;

    return len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

static int __init procfs1_init(void)
{
    our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
    pr_info("/proc/%s created\n", PROCFS_NAME);
    return 0;
}

static void __exit procfs1_exit(void)
{
    proc_remove(our_proc_file);
    pr_info("/proc/%s removed\n", PROCFS_NAME);
}

module_init(procfs1_init);
module_exit(procfs1_exit);
MODULE_LICENSE("GPL");
