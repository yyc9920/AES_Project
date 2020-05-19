#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#define DEV_NAME "test_dd" 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name");
MODULE_DESCRIPTION("test device driver");

static int tdd_major_req = 0;

static int tdd_major;

//functions
static int tdd_init(void);
static void tdd_cleanup(void);
module_init(tdd_init);
module_exit(tdd_cleanup);

//functions (Device Driver)
static int tdd_open(struct inode *inode, struct file *file);
static int tdd_release(struct inode *inode, struct file *flie);
static ssize_t tdd_read(struct file *file, char *buf, size_t count, loff_t *f_pos);
static ssize_t tdd_write(struct file *file, const char *buf, size_t count, loff_t *f_pos);

struct file_operations test_fops = {
        open:			tdd_open,
        read:			tdd_read,
        write:			tdd_write,
     release:			tdd_release
};

static int tdd_init(void) {
	int result = register_chrdev(tdd_major_req, DEV_NAME, &test_fops);
	if (result < 0) {
		printk(KERN_WARNING "%s: can't get major %d\n", DEV_NAME, tdd_major_req);
		return result;
	}
	tdd_major = result;
	printk(KERN_WARNING "test device driver: init(), %s, major number=%d\n", DEV_NAME, tdd_major);
	return 0;
}


static void tdd_cleanup(void) {
	unregister_chrdev(tdd_major, DEV_NAME);
	printk(KERN_WARNING "test device driver: cleanup(), %s\n", DEV_NAME);
}

static int tdd_open(struct inode *inode, struct file *file) {
	printk(KERN_WARNING "test device driver: open()\n");
	try_module_get(THIS_MODULE);
	return 0;
}

static int tdd_release(struct inode *inode, struct file *flie) {
	printk(KERN_WARNING "test device driver: release()\n");
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t tdd_read(struct file *file, char *buf, size_t count, loff_t *f_pos) {
	printk(KERN_WARNING "test device driver: read()\n");
	return 0;
}

static ssize_t tdd_write(struct file *file, const char *buf, size_t count, loff_t *f_pos) {
	printk(KERN_WARNING "test device driver: write()\n");
	return 0;	
}
