#include <linux/module.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/hrtimer.h>
#include <linux/ioctl.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/firmware.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#include <linux/slab.h>
#include "ep0700mlh0.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Suhak Kim<shkim@huins.com>");
MODULE_DESCRIPTION("Emerging Display Technologies EP0700MLH06 Touchscreen Driver");

#define EP0700MLH0_DRIVER_NAME "ep0700mlh0"
#define LOG(x, ...) printk(KERN_INFO EP0700MLH0_DRIVER_NAME": "x"\n", ##__VA_ARGS__)

#define RES_X			(1024)
#define RES_Y			(600)

struct ep0700mlh0_data_t {
	struct i2c_client *ts_i2c_client;
	struct input_dev *ts_input_dev;
	struct workqueue_struct *ts_workqueue;
	struct work_struct *ts_work;
};

struct ep0700mlh0_data_t *ep0700mlh0_data;

static int ep0700mlh0_i2c_read_array(uint8_t start_addr, uint8_t *rx_buffer, int count) {
	int ret;
	ret = i2c_master_send(ep0700mlh0_data->ts_i2c_client, &start_addr, 1);
	if (ret < 0) {
		LOG("%s() send address failed", __FUNCTION__);
		return ret;
	}
	ret = i2c_master_recv(ep0700mlh0_data->ts_i2c_client, rx_buffer, count);
	if (ret < 0) {
		LOG("%s() receive register failed", __FUNCTION__);
		return ret;
	}
	return 0;

}

static int ep0700mlh0_i2c_read_byte(uint8_t addr, uint8_t *rx_buffer) {
	return ep0700mlh0_i2c_read_array(addr, rx_buffer, 1);
}

static int ep0700mlh0_ts_wake_up_device(struct i2c_client *client)
{
	int gpio = irq_to_gpio(client->irq);
	int ret;

	ret = gpio_request(gpio, "ep0700mlh0_irq");
	if (ret < 0) {
		dev_err(&client->dev, "request gpio failed:%d\n", ret);
		return ret;
	}
	/* wake up controller via an falling edge on IRQ. */
	gpio_direction_output(gpio, 0);
	gpio_set_value(gpio, 1);
	/* controller should be waken up, return irq.  */
	gpio_direction_input(gpio);
	gpio_free(gpio);
	return 0;
}


static irqreturn_t ep0700mlh0_irq_handler(int irq, void *dev_id) {
	uint8_t tdev_status[3] = {0};
	uint8_t tdata[60] = {0};
	uint16_t touch_x, touch_y;
	uint8_t touch_event_flag, touch_id, touch_devmode, touch_gesture, touch_fingers;
	int calc_temp, i, ret;

	ret = ep0700mlh0_i2c_read_array(0x00, tdev_status, 3);
	if (ret) {
		LOG("read device status failed ret : %d", ret);
		return IRQ_HANDLED;
	} else {
		touch_devmode = (tdev_status[0] & 0x70) >> 4;
		touch_gesture = tdev_status[1];
		touch_fingers = (tdev_status[2] & 0x0F) >> 0;
	}

	if (touch_fingers != 0) {
		ret = ep0700mlh0_i2c_read_array(EP0700MLH0_TDATA_START, tdata, 0x6 * touch_fingers);
		if (ret) {
			LOG("read touch data failed");
			return IRQ_HANDLED;
		}
		input_report_key(ep0700mlh0_data->ts_input_dev, BTN_TOUCH, 1);
		for (i = 0; i < touch_fingers; i++) {
			touch_event_flag = (tdata[0 + (i * 0x6)] & EP0700MLH0_EVENT_FLAG_MASK) >> EP0700MLH0_EVENT_FLAG_OFFSET;
			if (touch_event_flag < 3) {
				touch_x = ((tdata[0 + (i * 0x6)] & EP0700MLH0_TOUCH_H_POS_MASK)<< 8)+tdata[1 + (i * 0x6)];
				touch_y = ((tdata[2 + (i * 0x6)] & EP0700MLH0_TOUCH_H_POS_MASK)<< 8)+tdata[3 + (i * 0x6)];

				calc_temp = touch_x * 100000;
				calc_temp = (calc_temp / 1791) * RES_X;
				calc_temp = calc_temp / 100000;
				touch_x = (uint16_t) calc_temp;

				calc_temp = touch_y * 100000;
				calc_temp = (calc_temp / 1024) * RES_Y;
				calc_temp = calc_temp / 100000;
				touch_y = (uint16_t) calc_temp;

                //LOG("read touch data : %d, %d", touch_x, touch_y);

				input_report_abs(ep0700mlh0_data->ts_input_dev, ABS_MT_POSITION_X, touch_x);
				input_report_abs(ep0700mlh0_data->ts_input_dev, ABS_MT_POSITION_Y, touch_y);
				input_mt_sync(ep0700mlh0_data->ts_input_dev);
			}
		}
		input_sync(ep0700mlh0_data->ts_input_dev);
	} else {
		input_mt_sync(ep0700mlh0_data->ts_input_dev);
		input_report_key(ep0700mlh0_data->ts_input_dev, BTN_TOUCH, 0);
		input_sync(ep0700mlh0_data->ts_input_dev);
	}
	return IRQ_HANDLED;
}

static int ep0700mlh0_probe(struct i2c_client *client, const struct i2c_device_id *i2c_id) {
	int ret;


	ep0700mlh0_data = kzalloc(sizeof(*ep0700mlh0_data), GFP_KERNEL);
	if (!ep0700mlh0_data) {
		LOG("memory allocation failed");
		return -ENOMEM;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		LOG("not compatible i2c function");
		return -ENODEV;
	} else {
		ep0700mlh0_data->ts_i2c_client = client;
	}

	ep0700mlh0_data->ts_input_dev = input_allocate_device();
	if (ep0700mlh0_data->ts_input_dev == NULL ) {
		LOG("ipnut device allocation failed");
		return -ENOMEM;
	}

	ep0700mlh0_ts_wake_up_device(client);

	ep0700mlh0_data->ts_input_dev->name = EP0700MLH0_DRIVER_NAME;
	ep0700mlh0_data->ts_input_dev->id.bustype = BUS_I2C;
	ep0700mlh0_data->ts_input_dev->id.vendor = 0x0002;
	ep0700mlh0_data->ts_input_dev->id.product = 0x0003;
	ep0700mlh0_data->ts_input_dev->id.version = 0x0200;
	ep0700mlh0_data->ts_input_dev->dev.parent = &client->dev;
	set_bit(EV_SYN, ep0700mlh0_data->ts_input_dev->evbit);
	set_bit(EV_ABS, ep0700mlh0_data->ts_input_dev->evbit);
	set_bit(EV_KEY, ep0700mlh0_data->ts_input_dev->evbit);
	set_bit(BTN_TOUCH, ep0700mlh0_data->ts_input_dev->keybit);
	input_set_abs_params(ep0700mlh0_data->ts_input_dev, ABS_MT_POSITION_X, 0, RES_X, 0, 0);
	input_set_abs_params(ep0700mlh0_data->ts_input_dev, ABS_MT_POSITION_Y, 0, RES_Y, 0, 0);

	ret = input_register_device(ep0700mlh0_data->ts_input_dev);
	if (ret) {
		LOG("input device registration failed");
		goto input_register_device_failed;
	}

	ret = request_threaded_irq(client->irq, NULL, ep0700mlh0_irq_handler,
            IRQF_TRIGGER_FALLING | IRQF_ONESHOT, 
			EP0700MLH0_DRIVER_NAME, ep0700mlh0_data);
	if (ret) {
		LOG("requesting irq failed");
		goto irq_request_failed;
	}
    else
    {
		LOG("requesting irq successfully!");
    }

	i2c_set_clientdata(client, ep0700mlh0_data);
	device_init_wakeup(&client->dev, 1);

	LOG("driver probed");

	return 0;

irq_request_failed: 
	input_unregister_device(ep0700mlh0_data->ts_input_dev);
input_register_device_failed:
	input_free_device(ep0700mlh0_data->ts_input_dev);
	return ret;

}
#ifdef CONFIG_PM
static int ep0700mlh0_ts_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);

	if (device_may_wakeup(&client->dev))
		enable_irq_wake(client->irq);
	else
		disable_irq(client->irq);

	return 0;
}

static int ep0700mlh0_ts_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);

	if (device_may_wakeup(&client->dev))
		disable_irq_wake(client->irq);
	else
		enable_irq(client->irq);

	return 0;
}

static const struct dev_pm_ops ep0700mlh0_pm_ops = {
	.suspend	= ep0700mlh0_ts_suspend,
	.resume		= ep0700mlh0_ts_resume,
};
#endif


static int ep0700mlh0_remove(struct i2c_client *client) {
	disable_irq(client->irq);
	free_irq(client->irq, ep0700mlh0_data);
	input_unregister_device(ep0700mlh0_data->ts_input_dev);
	input_free_device(ep0700mlh0_data->ts_input_dev);
	return 0;
}

static struct i2c_device_id ep0700mlh0_idtable[] = {
		{ EP0700MLH0_DRIVER_NAME, 0 },
		{} 
};

MODULE_DEVICE_TABLE(i2c, ep0700mlh0_idtable);

static struct i2c_driver ep0700mlh0_driver = {
		.probe = ep0700mlh0_probe,
		.remove = ep0700mlh0_remove,
		.id_table = ep0700mlh0_idtable,
	.driver = {
		.name	= EP0700MLH0_DRIVER_NAME,
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &ep0700mlh0_pm_ops,
#endif
	},
};

static int __devinit ep0700mlh0_init(void) {
	return i2c_add_driver(&ep0700mlh0_driver);
}

static void __exit ep0700mlh0_exit(void) {
	i2c_del_driver(&ep0700mlh0_driver);
}

module_init(ep0700mlh0_init);
module_exit(ep0700mlh0_exit);
