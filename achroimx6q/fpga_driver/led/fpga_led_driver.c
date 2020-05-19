#include "../include/fpga_driver.h"

static int led_port_usage = 0;
static unsigned char *iom_fpga_led_addr;     // 가상 주소를 저장할 변수

// prototypes
static ssize_t iom_led_write(struct file *file, const char *buf, size_t count, loff_t *f_pos);
static ssize_t iom_led_read(struct file *file, char *buf, size_t count, loff_t *f_pos);
static int iom_led_open(struct inode *inode, struct file *file);
static int iom_led_release(struct inode *inode, struct file *file);

struct file_operations iom_led_fops = {
	.owner	=	THIS_MODULE,
	.open	=	iom_led_open,
	.write	=	iom_led_write,
	.read	=	iom_led_read,
	.release   =	iom_led_release
};

static int iom_led_open(struct inode *inode, struct file *file) {
	if (led_port_usage)
		return -EBUSY; 

	led_port_usage = 1;
	return 0;
}

static int iom_led_release(struct inode *inode, struct file *file) {
	led_port_usage = 0;
	return 0;
}

static ssize_t iom_led_write(struct file *file, const char *buf, size_t count, loff_t *f_pos) {
	unsigned char value;
	unsigned short _s_value;

	if (copy_from_user(&value, buf, 1))   // 정상 종료 시 0을 반환
		return -EFAULT;
	_s_value = (unsigned short)value;
	outw(_s_value, (unsigned int)iom_fpga_led_addr);
	return 1;
}

static ssize_t iom_led_read(struct file *file, char *buf, size_t count, loff_t *f_pos) {
	unsigned char value;
	unsigned short _s_value;
	
	_s_value = inw((unsigned int)iom_fpga_led_addr);
	value = _s_value & 0xFF;
	if (copy_to_user(buf, &value, 1))   // 정상 종료 시 0을 반환
		return -EFAULT;
	return 1;
}

// __init : <linux/init.h>, this function is called for OS initialization only
int __init iom_led_init(void) {
	int result;
	result = register_chrdev(IOM_LED_MAJOR, IOM_LED_NAME, &iom_led_fops);

	if (result < 0) {
		printk(KERN_WARNING "Can't get any major number\n");
		return result;
	}
	
	// 물리 주소를 가상 주소에 mapping한다.
	iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
	printk(KERN_WARNING "init module %s, major number: %d\n", IOM_LED_NAME, IOM_LED_MAJOR);
	return 0;
}



void __exit iom_led_exit(void) {
	iounmap(iom_fpga_led_addr);
	unregister_chrdev(IOM_LED_MAJOR, IOM_LED_NAME);
}

module_init(iom_led_init);
module_exit(iom_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yechan Yun");
MODULE_DESCRIPTION("FPGA LED driver");
