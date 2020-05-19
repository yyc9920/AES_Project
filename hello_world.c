#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yechan Yun");

static int begin_hello(void){
printk(KERN_ALERT "Hello World\n");
	return 0;
}
static void end_hello(void) {
  printk(KERN_ALERT "Goodbye\n");
}
module_init(begin_hello);
module_exit(end_hello);
