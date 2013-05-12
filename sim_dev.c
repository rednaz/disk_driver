#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "sim_dev.h"
#include "disk.h"

// arbitrary number
#define SIM_DEV_MAJOR 567
#define SIM_DEV_NAME "sim_dev"

// this space holds the data that device users send to the device
static char *storage = NULL;
#define STORAGE_SIZE 4096

static disk_t myDisk;
static unsigned int address;

static unsigned long ioctl_control_data;
static unsigned long ioctl_status_data = 0xAABBCCDD;

static struct DISK_REGISTER *disk_reg;

// open function - called when the "file" /dev/sim_dev is opened in userspace
static int sim_dev_open (struct inode *inode, struct file *file)
{
   // this is a special print functions that allows a user to print from the kernel
	printk("sim_dev_open\n");
	return 0;
}

// close function - called when the "file" /dev/sim_dev is closed in userspace  
static int sim_dev_release (struct inode *inode, struct file *file)
{
	printk("sim_dev_release\n");
	return 0;
}

// read function called when  /dev/sim_dev is read
static ssize_t sim_dev_read( struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
   if (count > STORAGE_SIZE)
       return -EFAULT;

   // a special copy function that allows to copy from kernel space to user space
   if(copy_to_user(buf, storage, count) != 0)
      return -EFAULT;

   return count;
}

// write function called when /dev/sim_dev is written to
static ssize_t sim_dev_write( struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	//physaddr_t addr;

	int num_of_sectors = count/SECT_SIZE;
	int i=0;
   if (count > MAX_LOGICAL_SECTOR ||
		!log_to_phys(address))
       return -EFAULT;

	for (i=0; i<num_of_sectors; i++)
	{
		strncpy(myDisk[disk_reg->cylinder][disk_reg->head][disk_reg->sector], buf, SECT_SIZE);

		if (++disk_reg->sector == NUM_OF_SECTS)
		{
			disk_reg->sector = 0;
			
			if (++disk_reg->head == NUM_OF_HEADS)
			{
				disk_reg->head = 0;

				if (++disk_reg->cylinder == NUM_OF_CYLS)
					disk_reg->cylinder = 0;
			}
		}

		buf += SECT_SIZE;
	}

	return count;
}

// ioctl function called when /dev/sim_dev receives an ioctl command
// Ubuntu 10.10: static int sim_dev_ioctl(struct inode *inode, struct file *file, unsigned int command, unsigned long arg)
// Ubuntu 11.04:
static long sim_dev_unlocked_ioctl(struct file *file, unsigned int command, unsigned long arg)
{
	switch ( command )
	{
		case IOCTL_SIM_DEV_WRITE:/* for writing data to arg */
			if (copy_from_user(&ioctl_control_data, (int *)arg, sizeof(int)))
			   return -EFAULT;
			break;
			
		case IOCTL_SIM_DEV_READ:/* for reading data from arg */
			if (copy_to_user((int *)arg, &ioctl_status_data, sizeof(int)))
			   return -EFAULT;
			break;
			
		default:
			return -EINVAL;
	}
	return -EINVAL;
}

/**
This function translates a logical to a physical address.
**/
int log_to_phys(int logaddr/*, physaddr_t *phaddr*/)
{
	if (logaddr < 0 || logaddr >= MAX_LOGICAL_SECTOR)
		return 0;
	
	disk_reg->cylinder = logaddr / (NUM_OF_SECTS * NUM_OF_HEADS);
	disk_reg->head = (logaddr / NUM_OF_SECTS) % NUM_OF_HEADS;
	disk_reg->sector = (logaddr % NUM_OF_SECTS);

	return 1;
}

/**
This function translates a physical to a logical address.
**/
int phys_to_log(physaddr_t *phaddr)
{
	int cyl = phaddr->cyl;
  	int head = phaddr->head;
  	int sect = phaddr->sect;

	if (	cyl < 0 || cyl >= NUM_OF_CYLS ||
			head < 0 || head >= NUM_OF_HEADS ||
			sect < 0 || sect >= NUM_OF_SECTS)
		return -1;

	return ((cyl * NUM_OF_HEADS) + head) * NUM_OF_SECTS + sect - 1;
}

//
// mapping of file operations to the driver functions
//
struct file_operations sim_dev_file_operations = {
	.owner	=	THIS_MODULE,
	.llseek	=	NULL,
	.read		=	sim_dev_read,
	.write	=	sim_dev_write,
	.readdir	=	NULL,
	.poll		=	NULL,
//	.ioctl	=	sim_dev_ioctl, // Ubuntu 10.10
	.unlocked_ioctl	=	sim_dev_unlocked_ioctl, // Ubuntu 11.04
	.mmap		=	NULL,
	.open		=	sim_dev_open,
	.flush	=	NULL,
	.release	=	sim_dev_release,
	.fsync	=	NULL,
	.fasync	=	NULL,
	.lock		=	NULL,
};

// Loads a module in the kernel
static int sim_dev_init_module (void)
{
   // here we register sim_dev as a character device
	if (register_chrdev(SIM_DEV_MAJOR, SIM_DEV_NAME, &sim_dev_file_operations) != 0)
	   return -EIO;

	// reserve memory with kmalloc - Allocating Memory in the Kernel
	// GFP_KERNEL --> this does not have to be atomic, so kernel can sleep
	storage = kmalloc(STORAGE_SIZE, GFP_KERNEL);
	if (!storage) {
		printk("kmalloc failed\n");
		return -1;
	}

	disk_reg = kmalloc(sizeof(struct DISK_REGISTER), GFP_KERNEL);

   printk("Simulated Driver Module Installed\n");
   return 0;
}

//  Removes module from kernel
static void sim_dev_cleanup_module(void)
{
   // specialized function to free memeory in kernel
	kfree(storage);
	unregister_chrdev (SIM_DEV_MAJOR, SIM_DEV_NAME);
   printk("Simulated Driver Module Uninstalled\n");
}

// map the module initialization and cleanup functins
module_init(sim_dev_init_module);
module_exit(sim_dev_cleanup_module);

MODULE_AUTHOR("http://www.cs.csuci.edu/~ajbieszczad");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simulated Device Linux Device Driver");

