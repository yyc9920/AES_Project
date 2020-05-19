#include "../include/fpga_driver.h"

static int fnd_port_usage = 0;
static unsigned char *iom_fpga_fnd_addr;     // 가상 주소를 저장할 변수

// prototypes
static ssize_t iom_fnd_write(struct file *file, const char *buf, size_t count, loff_t *f_pos);
static ssize_t iom_fnd_read(struct file *file, char *buf, size_t count, loff_t *f_pos);
static int iom_fnd_open(struct inode *inode, struct file *file);
static int iom_fnd_release(struct inode *inode, struct file *file);

struct file_operations iom_fnd_fops = {
	.owner	=	THIS_MODULE,
	.open	=	iom_fnd_open,
	.write	=	iom_fnd_write,
	.read	=	iom_fnd_read,
	.release   =	iom_fnd_release
};

static int iom_fnd_open(struct inode *inode, struct file *file) {
	if (fnd_port_usage)
		return -EBUSY; 

	fnd_port_usage = 1;
	return 0;
}

static int iom_fnd_release(struct inode *inode, struct file *file) {
	fnd_port_usage = 0;
	return 0;
}

static ssize_t iom_fnd_write(struct file *file, const char *buf, size_t count, loff_t *f_pos) {
	unsigned char value[4];
	unsigned short _s_value;

	if (copy_from_user(&value, buf, 4))   // 정상 종료 시 0을 반환
		return -EFAULT;
	_s_value = value[0] << 12 | value[1] << 8 | value[2] << 4 | value[3];
	outw(_s_value, (unsigned int)iom_fpga_fnd_addr);
	return 1;
}

static ssize_t iom_fnd_read(struct file *file, char *buf, size_t count, loff_t *f_pos) {
	unsigned char value[4];
	unsigned short _s_value;
	
	_s_value = inw((unsigned int)iom_fpga_fnd_addr);
	value[0] = (_s_value >> 12) & 0xF;
	value[1] = (_s_value >>  8) & 0xF;
	value[2] = (_s_value >>  4) & 0xF;
	value[3] = (_s_value >>  0) & 0xF;

	if (copy_to_user(buf, &value, 1))   // 정상 종료 시 0을 반환
		return -EFAULT;
	return 1;
}

// __init : <linux/init.h>, this function is calfnd for OS initialization only
int __init iom_fnd_init(void) {
	int result;
	result = register_chrdev(IOM_FND_MAJOR, IOM_FND_NAME, &iom_fnd_fops);

	if (result < 0) {
		printk(KERN_WARNING "Can't get any major number\n");
		return result;
	}
	
	// 물리 주소를 가상 주소에 mapping한다.
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	printk(KERN_WARNING "init module %s, major number: %d\n", IOM_FND_NAME, IOM_FND_MAJOR);
	return 0;
}



void __exit iom_fnd_exit(void) {
	iounmap(iom_fpga_fnd_addr);
	unregister_chrdev(IOM_FND_MAJOR, IOM_FND_NAME);
}

module_init(iom_fnd_init);
module_exit(iom_fnd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yechan Yun");
MODULE_DESCRIPTION("FPGA FND driver");
