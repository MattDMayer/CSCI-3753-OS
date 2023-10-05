#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 756
#define MAJOR_NUMBER 240
#define DEVICE_NAME "pa2"

MODULE_AUTHOR("Matthew Mayer");
MODULE_AUTHOR("GPL");

/* Define device_buffer and other global data structures you will need here */

int openNum = 0;
int releaseNum = 0;
static char *deviceBuffer;

ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */

	int maxBytes = BUFFER_SIZE - *offset;	//calculates the maximum amount of bytes we can read from.
	int jobBytes;	//The amount of bytes to read in this current opperation.
	int readBytes; //Bytes already read in at this point.

	printk(KERN_ALERT, "Entering the %s function.\n", __FUNCTION__);

	if(length > (BUFFER_SIZE - *offset) || *offset < 0) //Out of bounds check.
	{
		printk(KERN_ALERT "Trying to read outside of the buffer.");
		return -1;
	}

	//Check if max number of bytes is bigger than the length of the user buffer. If it is the bytes in this job will be set to length.
	if(maxBytes > length)
	{
		jobBytes = length;
	}
	else //If we aren't at the max we can read from the full buffer space.
	{
		jobBytes = maxBytes;
	}

	readBytes = jobBytes - copy_to_user(buffer, deviceBuffer + *offset, jobBytes); //Copy to user space.
	*offset += readBytes; //Offset position moved.
	printk(KERN_ALERT "Device read: %d bytes. The offset is: %d", readBytes, *offset);//Print number of bytes read and new offset.
	return readBytes;
}

ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */

	int maxBytes = BUFFER_SIZE - *offset; //Calculates the max number of bytes we can write.
	int jobBytes; //Bytes to write.
	int writtenBytes; //Bytes already written.

	printk(KERN_ALERT, "Entering the %s function.\n", __FUNCTION__);

	if(length > (BUFFER_SIZE - *offset) || *offset < 0) //Out of bounds check.
	{
		printk(KERN_ALERT "Trying to write outside of the buffer.");
		return -1;
	}

	//Check if max number of bytes is bigger than the length of the user buffer. If it is the bytes in this job will be set to length.
	if(maxBytes > length)
	{
		jobBytes = length;
	}
	else //If we aren't at the max we can write too the full buffer space.
	{
		jobBytes = maxBytes;
	}

	writtenBytes = jobBytes - copy_from_user(deviceBuffer + *offset, buffer, jobBytes);
	*offset += jobBytes;
	printk(KERN_ALERT "Device wrote: %d bytes. The offset is: %d", jobBytes, *offset);
	return writtenBytes;
}

int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	printk(KERN_ALERT "The device has opened.\n");
	openNum ++;
	printk(KERN_ALERT "The device has opened %d times.\n", openNum);
	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	printk(KERN_ALERT "The device has closed.\n");
	releaseNum ++;
	printk(KERN_ALERT "The device has closed %d times.\n", releaseNum);
	return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
    /* Update open file position according to the values of offset and whence */
    loff_t newPos = 0;
    switch (whence)
    {
    case 0:
        newPos = offset; 
        
        break;
    case 1:
        newPos = (pfile -> f_pos) + offset;
        break;
    
    case 2: 
        newPos = BUFFER_SIZE + offset;
    	break;

    default:
        printk("whence broke");
        return -1;
        break;
    }
    if(newPos >= BUFFER_SIZE){
        newPos = newPos % BUFFER_SIZE;
    }
    else if(newPos < 0){
        newPos = BUFFER_SIZE + newPos;
    }
    pfile -> f_pos = newPos;
    return newPos; 
}

struct file_operations pa2_char_driver_file_operations = 
{
	.owner = THIS_MODULE,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
	.open = pa2_char_driver_open,
	.release = pa2_char_driver_close,
	.read = pa2_char_driver_read,
	.write = pa2_char_driver_write,
	.llseek = pa2_char_driver_seek
};

static int pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */
	printk(KERN_ALERT "Now inside the %s function.\n", __FUNCTION__);
	deviceBuffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &pa2_char_driver_file_operations);
	return 0;
}

static void pa2_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	printk(KERN_ALERT "Now in the %s function.\n", __FUNCTION__);
	kfree(deviceBuffer);
	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);