#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#define MAJOR_NUMBER 61
#define NUM_OF_BYTES 4096
#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, unsigned long)
#define SCULL_READ  _IOR(SCULL_IOC_MAGIC, 3, unsigned long)
#define SCULL_IOC_MAXNR 15
/* forward declaration */
char dev_msg[NUM_OF_BYTES];

long ioctl_example(struct file *flip, unsigned int cmd, unsigned long arg)
{
  int err= 0;
  int tmp;
  int retval = 0;

  if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
  if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;
  
  if (_IOC_DIR(cmd) & _IOC_READ)
    err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
  else if (_IOC_DIR(cmd) & _IOC_WRITE)
    err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
  if (err) return -EFAULT;
  switch(cmd){
  case SCULL_HELLO:
    printk(KERN_WARNING "hello\n");
    break;
  default:
    return -ENOTTY;
  }
  return retval;
}

long ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
  int err= 0;
  int tmp;
  int retval = 0;

  if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
  if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;
  
  if (_IOC_DIR(cmd) & _IOC_READ)
    err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
  else if (_IOC_DIR(cmd) & _IOC_WRITE)
    err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
  if (err) return -EFAULT;
  switch(cmd){
  case SCULL_WRITE:
    printk(KERN_WARNING "write\n");
    copy_from_user(dev_msg, (char*)arg, NUM_OF_BYTES);
    break;
  case SCULL_READ:
    printk(KERN_WARNING "read\n");
    copy_to_user((char*)arg, dev_msg, NUM_OF_BYTES);
    break;
  default:
    return -ENOTTY;
  }
  return retval;
}

int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf,
size_t count, loff_t *f_pos);
static loff_t onebyte_lseek(struct file *file, loff_t offset, int orig);

static void onebyte_exit(void);

/* definition of file_operation structure */
struct file_operations onebyte_fops = {
	read:	onebyte_read,
	write:	onebyte_write,
	open:	onebyte_open,
	release: onebyte_release,
        llseek: onebyte_lseek,
	unlocked_ioctl: ioctl
};
char *onebyte_data = NULL;

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
  int maxbytes;
  int bytes_to_do;
  int nbytes;
  maxbytes = NUM_OF_BYTES - *f_pos;
  if (maxbytes > count)
    bytes_to_do = count;
  else
    bytes_to_do = maxbytes;
  if (bytes_to_do == 0)
  {
    printk(KERN_INFO "ZYM: Reached end of device\n");
    return -ENOSPC;
  }
  nbytes = bytes_to_do - copy_to_user(buf, onebyte_data + *f_pos, bytes_to_do);
  *f_pos += nbytes;
  printk(KERN_INFO "ZYM: copy %d character", nbytes);
  return nbytes;
}

ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
/*please complete the function on your own*/
  int nbytes;
  int bytes_to_do;
  int maxbytes;
  maxbytes = NUM_OF_BYTES - *f_pos;
  if (maxbytes > count)
    bytes_to_do = count;
  else
    bytes_to_do = maxbytes;

  if (bytes_to_do == 0){
    printk(KERN_INFO "ZYM: Reached end of device\n");
    return -ENOSPC;
  }
  nbytes = bytes_to_do - copy_from_user(onebyte_data + *f_pos, buf, bytes_to_do);
  *f_pos += nbytes;
  printk(KERN_ALERT "ZYM: %d character given\n", nbytes);
  //printk(KERN_INFO "ZYM: writing:\n%s", onebyte_data);
  return nbytes;
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
  }
  // unregister the device
  unregister_chrdev(MAJOR_NUMBER, "onebyte");
  printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

static loff_t onebyte_lseek(struct file *file, loff_t offset, int orig)
{
  loff_t new_pos = 0;
  switch(orig)
  {
    case 0:
      new_pos = offset;
      break;
    case 1:
      new_pos = file->f_pos + offset;
      break;
    case 2:
      new_pos = NUM_OF_BYTES - offset;
      break;
  }
  if (new_pos > NUM_OF_BYTES) new_pos = NUM_OF_BYTES;
  if (new_pos < 0) new_pos = 0;
  file->f_pos = new_pos;
  return new_pos;
}
MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
