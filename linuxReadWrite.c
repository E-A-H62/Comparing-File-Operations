/* Code referenced from:
https://sysprog21.github.io/lkmpg/#read-and-write-a-proc-file */

#include <linux/kernel.h> /* Doing kernel work */ 
#include <linux/module.h> /* Specifically, a module */ 
#include <linux/proc_fs.h> /* Necessary since using proc fs */ 
#include <linux/uaccess.h> /* for copy_from_user */ 
#include <linux/version.h> 
 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0) 
#define HAVE_PROC_OPS 
#endif 
 
#define PROCFS_MAX_SIZE 1024 // max size for proc file's buffer
#define PROCFS_NAME "buffer1k" // name of proc file
 
/* This structure hold information about the /proc file */ 
static struct proc_dir_entry *our_proc_file; 
 
/* The buffer used to store character for this module */ 
static char procfs_buffer[PROCFS_MAX_SIZE]; 
 
/* The size of the buffer */ 
static unsigned long procfs_buffer_size = 0; 
 
/* This function is called then the /proc file is read */ 
static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
    char s[13] = "HelloWorld!\n"; // string returned when reading
    int len = sizeof(s); // length of string
    ssize_t ret = len; // default return value
 
    // checks if offset greater than data length, or if copy_to_user failed
    if (*offset >= len || copy_to_user(buffer, s, len)) { 
        pr_info("copy_to_user failed\n"); 
        ret = 0; 
    } else {
        // does not need to import data because it is already in the kernel space
        // logs the read file action
        pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name); 
        *offset += len; // incremenets offset so next read returns 0
    } 
 
    return ret; 
} 
 
/* This function is called with the /proc file is written. */ 
static ssize_t procfile_write(struct file *file, const char __user *buff, 
                              size_t len, loff_t *off) 
{ 
    procfs_buffer_size = len; // buffer size set to length of data to be written
    
    // makes sure buffer doesn't exceed max size
    if (procfs_buffer_size >= PROCFS_MAX_SIZE) 
        procfs_buffer_size = PROCFS_MAX_SIZE - 1; 

    // checks if data copied from user
    // needds to import data because it comes from the user
    if (copy_from_user(procfs_buffer, buff, procfs_buffer_size)) 
        return -EFAULT; 
 
    // null-terminates string
    procfs_buffer[procfs_buffer_size] = '\0';
    // updates offset after writing
    *off += procfs_buffer_size;
    // logs the write file action
    pr_info("procfile write %s\n", procfs_buffer); 
 
    return procfs_buffer_size; 
} 

// determines which structure to use for file operations based on kernel version
#ifdef HAVE_PROC_OPS 
static const struct proc_ops proc_file_fops = { 
    .proc_read = procfile_read, 
    .proc_write = procfile_write, 
}; 
#else 
static const struct file_operations proc_file_fops = { 
    .read = procfile_read, 
    .write = procfile_write, 
}; 
#endif 

// funtion to initialize file
static int __init procfs2_init(void) 
{
    // creates /proc file with read/write permissions (0644)
    our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
    // checks if file created
    if (NULL == our_proc_file) { 
        pr_alert("Error:Could not initialize /proc/%s\n", PROCFS_NAME); 
        return -ENOMEM; 
    } 
    // logs creationg of file
    pr_info("/proc/%s created\n", PROCFS_NAME); 
    return 0; 
} 

// function to clean up file
static void __exit procfs2_exit(void) 
{ 
    proc_remove(our_proc_file); 
    pr_info("/proc/%s removed\n", PROCFS_NAME); 
} 
 
module_init(procfs2_init); 
module_exit(procfs2_exit); 
 
MODULE_LICENSE("GPL");
