#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#define MAJOR_NUMBER 61
#define NUM_OF_BYTES 4096
/* forward declaration */
int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf,
size_t count, loff_t *f_pos);
static void onebyte_exit(void);

/* definition of file_operation structure */
struct file_operations onebyte_fops = {
	read:	onebyte_read,
	write:	onebyte_write,
	open:	onebyte_open,
	release: onebyte_release
};
char *onebyte_data = NULL;
int msgSize = 0;

int onebyte_open(struct inode *inode, struct file *filep)
{
  return 0; // always successful
}

int onebyte_release(struct inode *inode, struct file *filep)
{
  return 0; // always successful
}

ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
/*please complete the function on your own*/
   if (msgSize == 0) return 0;
   int tempSize = min(count, NUM_OF_BYTES);
   //if (copy_to_user(buf, onebyte_data, tempSize)) {
   //printk(KERN_INFO "ZYM: copy_to_user fails");
   //  return -EFAULT;
   //}
   int i;
   for (i = 0; i < tempSize; i++){
     put_user(onebyte_data[i], buf + i);
     if (buf[i] == '\0') break;
   }

   msgSize = 0;
   printk(KERN_INFO "ZYM: copy %d character", i);
   return i;
}

ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
/*please complete the function on your own*/
  if (count > NUM_OF_BYTES) {
    printk(KERN_ALERT "ZYM: %d char given, more than %d character given", count, NUM_OF_BYTES);
    return -ENOSPC;
  }
  if (count == 0) return 0;
  int tempSize = min(count, NUM_OF_BYTES);
  int i;
  for (i = 0; i < tempSize; i++)
    get_user(onebyte_data[i], buf + i);
  onebyte_data[tempSize] = '\0';

  msgSize = 1;
  printk(KERN_ALERT "ZYM: %d character given", tempSize);
  printk(KERN_INFO "ZYM: writing:\n%s", onebyte_data);
  return tempSize;
}

static int onebyte_init(void)
{
  int result;
  // register the device
  result = register_chrdev(MAJOR_NUMBER, "onebyte", &onebyte_fops);
  if (result < 0) {
    return result;
  }
  // allocate one byte of memory for storage
  // kmalloc is just like malloc, the second parameter is// the type of memory to be allocated.
  // To release the memory allocated by kmalloc, use kfree.
  onebyte_data = kmalloc(sizeof(char)*NUM_OF_BYTES, GFP_KERNEL);
  if (!onebyte_data) {
    onebyte_exit();
    // cannot allocate memory
    // return no memory error, negative signify a failure
    return -ENOMEM;
  }
  // initialize the value to be X
  *onebyte_data = 'X';
  msgSize = 1;
  printk(KERN_ALERT "This is a onebyte device module\n");
  return 0;
}

static void onebyte_exit(void)
{
  // if the pointer is pointing to something
  if (onebyte_data) {
    // free the memory and assign the pointer to NULL
    kfree(onebyte_data);
    onebyte_data = NULL;
    msgSize = 0;
  }
  // unregister the device
  unregister_chrdev(MAJOR_NUMBER, "onebyte");
  printk(KERN_ALERT "Onebyte device module is unloaded\n");
}
MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
