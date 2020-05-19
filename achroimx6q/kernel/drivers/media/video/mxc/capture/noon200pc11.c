/*
 * Copyright (C) 2012-2013 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>
#include <linux/fsl_devices.h>
#include <media/v4l2-chip-ident.h>
#include <media/v4l2-int-device.h>
#include "mxc_v4l2_capture.h"

#define MIN_FPS 15
#define MAX_FPS 56
#define DEFAULT_FPS 30

#define NOON200PC11_XCLK_MIN 60000000
#define NOON200PC11_XCLK_MAX 24000000

#define NOON200PC11_CHIP_ID_ADDR	0x28
#define NOON200PC11_CHIP_ID	        0x13

#define NOON200PC11_DRIVER_NAME "noon200pc11"
#define LOG(x, ...) printk(KERN_INFO NOON200PC11_DRIVER_NAME": "x"\n", ##__VA_ARGS__)

enum noon200pc11_mode {
	noon200pc11_mode_MIN = 0,
	noon200pc11_mode_QQSVGA_200_150 = 0,
	noon200pc11_mode_QSVGA_400_300 = 1 ,
	noon200pc11_mode_SVGA_800_600 = 2,
	noon200pc11_mode_UXGA_1600_1200 = 3,
	noon200pc11_mode_MAX = 3
};

enum noon200pc11_frame_rate {
	noon200pc11_15_fps,
	noon200pc11_30_fps,
	noon200pc11_56_fps
};

static int noon200pc11_framerates[] = {
	[noon200pc11_15_fps] = 15,
	[noon200pc11_30_fps] = 30,
	[noon200pc11_56_fps] = 56,
};

struct reg_value {
	u16 u16RegAddr;
	u8 u8Val;
	u8 u8Mask;
	u32 u32Delay_ms;
};

struct noon200pc11_mode_info {
	enum noon200pc11_mode mode;
	u32 width;
	u32 height;
	struct reg_value *init_data_ptr;
	u32 init_data_size;
};

/*!
 * Maintains the information on the current state of the sesor.
 */
static struct sensor_data noon200pc11_data;
 static struct reg_value noon200pc11_initial_setting[] = {
	//{0x00, 0x00, 0, 0}, 
    /* YUV */          
	{0x00,0x11}, 
    /*
    {0x0b,0x1c},   
    {0x0e,0x68},   
    {0x00,0x11},   
    {0x01,0x1a},   
    {0x02,0x0c},     // Polarity, Clock     
    {0x03,0xaf},           
    {0x05,0x31},        // 24MHz*2 /2 = 12MHz   
    {0x06,0x0d},   
    {0x07,0x1f},   
    {0x08,0x71},   
    {0x09,0x30},   
    {0x0a,0xc3},   
    {0x0c,0x65},   
    {0x0d,0x63},   
    {0x0f,0x04},   
    {0x10,0x80},   
    {0x11,0x80},   
    {0x12,0x01},   
    {0x13,0x00},   
    {0x14,0x80},   
    {0x15,0x80},   
    {0x16,0x10},   
    {0x17,0x80},   
    {0x18,0x80},   
    {0x19,0x30},   
    {0x1a,0xa0},   
    {0x1b,0x40},   
    {0x1c,0x80},   
    {0x1d,0x80},   
    {0x1e,0x7f},   
    {0x1f,0x08},   
    {0x20,0x30},   
    {0x21,0xf0},   
    {0x22,0x00},   
    {0x23,0x00},   
    {0x24,0x22},   
    {0x25,0x18},   
    {0x26,0x2f},   
    {0x27,0xe8},   
    //  {0x28,0x13);   
    //  {0x29,0xa9);   
    {0x2a,0x64},   
    {0x2b,0x37},   
    {0x2c,0x12},   
    {0x2d,0x14},   
    {0x2e,0x62},   
    {0x2f,0x0d},   
    {0x30,0x14},   
    {0x31,0x61},   
    {0x32,0xb8},   
    {0x33,0x00},   
    {0x34,0x04},   
    {0x35,0x09},   
    {0x36,0x12},   
    {0x37,0x1a},   
    {0x38,0x2a},   
    {0x39,0x42},   
    {0x3a,0x59},   
    {0x3b,0x6c},   
    {0x3c,0x8c},   
    {0x3d,0xa8},   
    {0x3e,0xd9},   
    {0x3f,0xfe},   
    {0x40,0x80},   
    {0x41,0x80},   
    {0x42,0x13},   
    {0x43,0x30},   
    {0x44,0x78},   
    {0x45,0x45},   
    {0x46,0x20},   
    {0x47,0x48},   
    {0x48,0x00},   
    {0x49,0x76},   
    {0x4a,0x80},   
    {0x4b,0x80},   
    {0x4c,0x40},   
    {0x4d,0x20},   
    {0x4e,0x8d},   
    {0x4f,0x78},       // Lum level to converge at outdoor condition   
    {0x50,0x21},   
    {0x51,0x20},   
    {0x52,0x43},   
    {0x53,0x58},   
    {0x54,0x1b},   
    {0x55,0x70},   
    {0x56,0x28},   
    {0x57,0x48},   
    {0x58,0x40},   
    {0x59,0xc7},   
    {0x5a,0x50},   
    {0x5b,0x20},   
    {0x5c,0x20},   
    {0x5d,0x08},   
    {0x5e,0x00},   
    {0x5f,0x2a},   
    {0x60,0x28},   
    {0x61,0x10},   
    {0x62,0x55},   
    {0x63,0x10},   
    {0x64,0x10},   
    {0x65,0x28},   
    {0x66,0x20},   
    {0x67,0x30},   
    {0x68,0x50},   
    {0x69,0x01},   
    {0x6a,0xd9},   
    {0x6b,0xa2},   
    {0x6c,0x0f},   
    {0x6d,0x42},   
    {0x6e,0x40},   
    {0x6f,0x01},   
    {0x70,0xc8},   
    {0x71,0x0f},   
    {0x72,0x42},   
    {0x73,0x40},   
    {0x74,0x75},   
    {0x75,0x30},   
    {0x76,0x61},   
    {0x77,0xa8},   
    {0x78,0x00},   
    {0x79,0x78},   
    {0x7a,0x12},   
    {0x7b,0x4f},   
    {0x7c,0x80},   
    {0x7d,0x11},   
    {0x7e,0xd4},   
    {0x7f,0x72},   
    {0x80,0x00},   
    {0x81,0x00},   
    {0x82,0x00},   
    {0x83,0x01},   
    {0x84,0x04},   
    {0x85,0xb0},   
    {0x86,0x06},   
    {0x87,0x40},   
    {0x88,0x01},   
    {0x89,0x7c},   
    {0x8a,0x00},   
    {0x8b,0x10},   
    {0x8c,0x09},   
    {0x8d,0x03},   
    {0x8e,0x03},   
    {0x8f,0x03},   
    {0x90,0x00},   
    {0x91,0x00},   
    {0x92,0x00},   
    {0x93,0x3f},   
    {0x94,0x45},   
    {0x95,0x4e},   
    {0x96,0x46},   
    {0x97,0x77},   
    {0x98,0x70},   
    {0x99,0x37},   
    {0x9a,0x10},   
    {0x9b,0x61},   
    {0x9c,0x00},   
    {0x9d,0x00},   
    {0x9e,0x03},   
    {0x9f,0x01},   
    {0xa0,0x42},   
    {0xa1,0x25},   
    {0xa2,0x41},   
    {0xa3,0x02},   
    {0xa4,0x20},   
    {0xa5,0x01},   
    {0xa6,0x10},   
    {0xa7,0x10},   
    {0xa8,0x24},   
    {0xa9,0x3a},   
    {0xaa,0x5e},   
    {0xab,0x8c},   
    {0xac,0xb0},   
    {0xad,0xc8},   
    {0xae,0xec},   
    {0xaf,0x20},   
    {0xb0,0x80},   
    {0xb1,0x60},   
    {0xb2,0x60},   
    {0xb3,0x80},   
    {0xb4,0x80},   
    {0xb5,0x0a},   
    {0xb6,0x10},   
    {0xb7,0x09},   
    {0xb8,0x0c},   
    {0xb9,0x00},   
    {0xba,0xda},   
    {0xbb,0x44},   
    {0xbc,0x68},   
    {0xbd,0x60},   
    {0xbe,0x40},   
    {0xbf,0x30},   
    {0xc0,0x00},   
    {0xc1,0x10},   
    {0xc2,0x54},   
    {0xc3,0x74},   
    {0xc4,0x10},   
    {0xc5,0x3b},   
    {0xc6,0x5b},   
    {0xc7,0x00},   
    {0xc8,0x00},   
    {0xc9,0x00},   
    {0xca,0x00},   
    {0xcb,0x00},   
    {0xcc,0x00},   
    {0xcd,0x03},   
    {0xce,0x20},   
    {0xcf,0x02},   
    {0xd0,0x58},   
    {0xd1,0x00},   
    {0xd2,0xc8},   
    {0xd3,0x00},   
    {0xd4,0x96},   
    {0xd5,0x01},   
    {0xd6,0x40},   
    {0xd7,0x00},   
    {0xd8,0xf0},   
    {0xd9,0x40},   
    {0xda,0x30},   
    {0xdb,0x00},   
    {0xdc,0xc8},   
    {0xdd,0x00},   
    {0xde,0x96},   
    {0xdf,0x23},   
    {0xe0,0x48},   
    {0xe1,0x38},   
    {0xe2,0x38},   
    {0xe3,0x28},   
    {0xe4,0x88},   
    {0xe5,0x88},   
    {0xe6,0x44},   
    {0xe7,0x61},   
    {0xe8,0x30},   
    {0xe9,0x00},   
    {0xea,0xff},   
    {0xeb,0x70},   
    {0xec,0x90},   
    {0xed,0x00},   
    {0xee,0x88},   
    {0xef,0x10},   
    {0xf0,0x48},   
    {0xf1,0x20},   
    {0xf2,0x02},   
    {0xf3,0x07},   
    {0xf4,0x89},   
    {0xf5,0x81},   
    {0xf6,0x07},   
    {0xf7,0x86},   
    {0xf8,0x8a},   
    {0xf9,0x94},   
    {0xfa,0x1f},   
    {0xfb,0x10},   
    {0xfc,0x11},   
    {0xfd,0x00},   
    {0xfe,0x00},   
    {0xff,0x00},   

    {0x0f,0x24},   
    {0x0f,0x04},   
    {0x0b,0x9c},   
    {0x0e,0xe8},       
    */
};

static struct reg_value noon200pc11_setting_15fps_QQSVGA_200_150[] = {
	{0x00, 0x00}, 
};

static struct reg_value noon200pc11_setting_15fps_QSVGA_400_300[] = {
	{0x00, 0x10},
};

static struct reg_value noon200pc11_setting_15fps_SVGA_800_600[] = {
	{0x00, 0x20},
};

static struct reg_value noon200pc11_setting_15fps_UXGA_1600_1200[] = {
	{0x00, 0x30},
};

static struct reg_value noon200pc11_setting_30fps_QSVGA_400_300[] = {
	{0x00, 0x11},
};

static struct reg_value noon200pc11_setting_30fps_SVGA_800_600[] = {
	{0x00, 0x21},
};

static struct reg_value noon200pc11_setting_30fps_UXGA_1600_1200[] = {
	{0x00, 0x31},
};

static struct reg_value noon200pc11_setting_56fps_SVGA_800_600[] = {
	{0x00, 0x22},
};

static struct reg_value noon200pc11_setting_56fps_UXGA_1600_1200[] = {
	{0x00, 0x32},
};


static struct noon200pc11_mode_info noon200pc11_mode_info_data[3][noon200pc11_mode_MAX + 1] = {
	{
        {noon200pc11_mode_QQSVGA_200_150,    200,  150,
            noon200pc11_setting_15fps_QQSVGA_200_150,
            ARRAY_SIZE(noon200pc11_setting_15fps_QQSVGA_200_150)},
        {noon200pc11_mode_QSVGA_400_300,   400,  300,
            noon200pc11_setting_15fps_QSVGA_400_300,
            ARRAY_SIZE(noon200pc11_setting_15fps_QSVGA_400_300)},
        {noon200pc11_mode_SVGA_800_600,   800,  600,
            noon200pc11_setting_15fps_SVGA_800_600,
            ARRAY_SIZE(noon200pc11_setting_15fps_SVGA_800_600)},
        {noon200pc11_mode_UXGA_1600_1200,   1600,  1200,
            noon200pc11_setting_15fps_UXGA_1600_1200,
            ARRAY_SIZE(noon200pc11_setting_15fps_UXGA_1600_1200)},
    },
    {
        {noon200pc11_mode_QSVGA_400_300,   400,  300,
            noon200pc11_setting_30fps_QSVGA_400_300,
            ARRAY_SIZE(noon200pc11_setting_30fps_QSVGA_400_300)},
        {noon200pc11_mode_SVGA_800_600,   800,  600,
            noon200pc11_setting_30fps_SVGA_800_600,
            ARRAY_SIZE(noon200pc11_setting_30fps_SVGA_800_600)},
        {noon200pc11_mode_UXGA_1600_1200,   1600,  1200,
            noon200pc11_setting_30fps_UXGA_1600_1200,
            ARRAY_SIZE(noon200pc11_setting_30fps_UXGA_1600_1200)},
    },
    {
        {noon200pc11_mode_SVGA_800_600,   800,  600,
            noon200pc11_setting_56fps_SVGA_800_600,
            ARRAY_SIZE(noon200pc11_setting_56fps_SVGA_800_600)},
        {noon200pc11_mode_UXGA_1600_1200,   1600,  1200,
            noon200pc11_setting_56fps_UXGA_1600_1200,
            ARRAY_SIZE(noon200pc11_setting_56fps_UXGA_1600_1200)},

    },

};

static struct fsl_mxc_camera_platform_data *camera_plat;

static int noon200pc11_probe(struct i2c_client *adapter,
				const struct i2c_device_id *device_id);
static int noon200pc11_remove(struct i2c_client *client);

static s32 noon200pc11_read_reg(u8 reg, u8 *val);
static s32 noon200pc11_write_reg(u8 reg, u8 val);

static const struct i2c_device_id noon200pc11_id[] = {
	{NOON200PC11_DRIVER_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, noon200pc11_id);

static struct i2c_driver noon200pc11_i2c_driver = {
	.driver = {
		  .owner = THIS_MODULE,
		  .name  = NOON200PC11_DRIVER_NAME,
		  },
	.probe  = noon200pc11_probe,
	.remove = noon200pc11_remove,
	.id_table = noon200pc11_id,
};

static s32 noon200pc11_write_reg(u8 reg, u8 val)
{
    u8 au8Buf[2] = {0};

    au8Buf[0] = reg;
    au8Buf[1] = val;

    if (i2c_master_send(noon200pc11_data.i2c_client, au8Buf, 2) < 0) {
        LOG("%s() send address failed :reg=%x, ,val=%x", __FUNCTION__, reg, val);
        return -1;
    }

    return 0;
}

static s32 noon200pc11_read_reg(u8 reg, u8 *val)
{
    u8 u8RdVal = 0;

    if (i2c_master_send(noon200pc11_data.i2c_client, &reg, 1) < 0) {
        LOG("%s() send address failed :reg=%x", __FUNCTION__, reg);
        return -1;
    }

    if (1 != i2c_master_recv(noon200pc11_data.i2c_client, &u8RdVal, 1)) {
        LOG("%s() recv address failed :reg=%x, ,val=%x", __FUNCTION__, reg, u8RdVal);
        return -1;
    }

	*val = u8RdVal;

	return u8RdVal;
}

static int noon200pc11_set_rotate_mode(struct reg_value *rotate_mode)
{
    printk("[DEBUG] == %s == \n", __func__);
    /*
	s32 i = 0;
	s32 iModeSettingArySize = 2;
	register u32 Delay_ms = 0;
	register u16 RegAddr = 0;
	register u8 Mask = 0;
	register u8 Val = 0;
	u8 RegVal = 0;
	int retval = 0;
	for (i = 0; i < iModeSettingArySize; ++i, ++rotate_mode) {
		Delay_ms = rotate_mode->u32Delay_ms;
		RegAddr = rotate_mode->u16RegAddr;
		Val = rotate_mode->u8Val;
		Mask = rotate_mode->u8Mask;

		if (Mask) {
			retval = noon200pc11_read_reg(RegAddr, &RegVal);
			if (retval < 0) {
				pr_err("%s, read reg 0x%x failed\n", __FUNCTION__, RegAddr);
				goto err;
			}

			Val |= RegVal;
			Val &= Mask;
		}

		retval = noon200pc11_write_reg(RegAddr, Val);
		if (retval < 0) {
			pr_err("%s, write reg 0x%x failed\n", __FUNCTION__, RegAddr);
			goto err;
		}

		if (Delay_ms)
			mdelay(Delay_ms);
	}
err:
	return retval;
*/
    return 0;
}
static int noon200pc11_init_mode(enum noon200pc11_frame_rate frame_rate,
		enum noon200pc11_mode mode);

static int noon200pc11_write_snapshot_para(enum noon200pc11_frame_rate frame_rate,
		enum noon200pc11_mode mode);

static int noon200pc11_change_mode(enum noon200pc11_frame_rate new_frame_rate,
		enum noon200pc11_frame_rate old_frame_rate,
		enum noon200pc11_mode new_mode,
		enum noon200pc11_mode orig_mode)
{
	struct reg_value *pModeSetting = NULL;
	s32 i = 0;
	s32 iModeSettingArySize = 0;
	register u32 Delay_ms = 0;
    register u16 RegAddr = 0;
    register u8 Mask = 0;
    register u8 Val = 0;
    u8 RegVal = 0;
    int retval = 0;

    printk("[DEBUG] == %s == frame rate: %d -> %d, mode %d-> %d \n", __func__, old_frame_rate, new_frame_rate, orig_mode, new_mode);

    if (new_mode > noon200pc11_mode_MAX || new_mode < noon200pc11_mode_MIN) {
        pr_err("Wrong noon200pc11 mode detected!\n");
        return -1;
    }

    /*
    if ((new_frame_rate == old_frame_rate) &&
            (new_mode == noon200pc11_mode_VGA_640_480) &&
            (orig_mode == noon200pc11_mode_QSXGA_2592_1944)) {
        pModeSetting = noon200pc11_setting_QSXGA_2_VGA;
        iModeSettingArySize = ARRAY_SIZE(noon200pc11_setting_QSXGA_2_VGA);
        noon200pc11_data.pix.width = 640;
        noon200pc11_data.pix.height = 480;
    } else if ((new_frame_rate == old_frame_rate) &&
            (new_mode == noon200pc11_mode_QVGA_320_240) &&
            (orig_mode == noon200pc11_mode_VGA_640_480)) {
        pModeSetting = noon200pc11_setting_VGA_2_QVGA;
        iModeSettingArySize = ARRAY_SIZE(noon200pc11_setting_VGA_2_QVGA);
        noon200pc11_data.pix.width = 320;
        noon200pc11_data.pix.height = 240;
    } else {
        retval = noon200pc11_write_snapshot_para(new_frame_rate, new_mode);
        goto err;
    }

    */

    pModeSetting = noon200pc11_setting_30fps_SVGA_800_600;
    iModeSettingArySize = ARRAY_SIZE(noon200pc11_setting_30fps_SVGA_800_600);
    noon200pc11_data.pix.width = 800;
    noon200pc11_data.pix.height = 600;

    if (noon200pc11_data.pix.width == 0 || noon200pc11_data.pix.height == 0 ||
            pModeSetting == NULL || iModeSettingArySize == 0)
        return -EINVAL;

    for (i = 0; i < iModeSettingArySize; ++i, ++pModeSetting) {
        Delay_ms = pModeSetting->u32Delay_ms;
        RegAddr = pModeSetting->u16RegAddr;
        Val = pModeSetting->u8Val;
        Mask = pModeSetting->u8Mask;

        if (Mask) {
            retval = noon200pc11_read_reg(RegAddr, &RegVal);
            if (retval < 0) {
				pr_err("read reg error addr=0x%x", RegAddr);
				goto err;
			}

			RegVal &= ~(u8)Mask;
			Val &= Mask;
			Val |= RegVal;
		}

		retval = noon200pc11_write_reg(RegAddr, Val);
		if (retval < 0) {
			pr_err("write reg error addr=0x%x", RegAddr);
			goto err;
		}

		if (Delay_ms)
			msleep(Delay_ms);
	}
err:
	return retval;
}
static int noon200pc11_init_mode(enum noon200pc11_frame_rate frame_rate,
			    enum noon200pc11_mode mode)
{
	struct reg_value *pModeSetting = NULL;
	s32 i = 0;
	s32 iModeSettingArySize = 0;
	register u32 Delay_ms = 0;
	register u16 RegAddr = 0;
	register u8 Mask = 0;
	register u8 Val = 0;
	u8 RegVal = 0;
	int retval = 0;

    printk("[DEBUG] == %s == \n", __func__);

	if (mode > noon200pc11_mode_MAX || mode < noon200pc11_mode_MIN) {
		pr_err("Wrong noon200pc11 mode detected!\n");
		return -1;
	}

	pModeSetting = noon200pc11_mode_info_data[frame_rate][mode].init_data_ptr;
	iModeSettingArySize =
		noon200pc11_mode_info_data[frame_rate][mode].init_data_size;

	noon200pc11_data.pix.width = noon200pc11_mode_info_data[frame_rate][mode].width;
	noon200pc11_data.pix.height = noon200pc11_mode_info_data[frame_rate][mode].height;

	if (noon200pc11_data.pix.width == 0 || noon200pc11_data.pix.height == 0 ||
	    pModeSetting == NULL || iModeSettingArySize == 0)
		return -EINVAL;

	for (i = 0; i < iModeSettingArySize; ++i, ++pModeSetting) {
		Delay_ms = pModeSetting->u32Delay_ms;
		RegAddr = pModeSetting->u16RegAddr;
		Val = pModeSetting->u8Val;
		Mask = pModeSetting->u8Mask;

		if (Mask) {
			retval = noon200pc11_read_reg(RegAddr, &RegVal);
			if (retval < 0) {
				pr_err("read reg error addr=0x%x", RegAddr);
				goto err;
			}

			RegVal &= ~(u8)Mask;
			Val &= Mask;
			Val |= RegVal;
		}

		retval = noon200pc11_write_reg(RegAddr, Val);
		if (retval < 0) {
			pr_err("write reg error addr=0x%x", RegAddr);
			goto err;
		}

		if (Delay_ms)
			msleep(Delay_ms);
	}
err:
	return retval;
}

static int noon200pc11_write_snapshot_para(enum noon200pc11_frame_rate frame_rate,
       enum noon200pc11_mode mode)
{
	int ret = 0;
	bool m_60Hz = false;
	u16 capture_frame_rate = 50;
	u16 g_preview_frame_rate = 225;

	u8 exposure_low, exposure_mid, exposure_high;
	u8 ret_l, ret_m, ret_h, gain, lines_10ms;
	u16 ulcapture_exposure, icapture_gain, preview_maxlines;
	u32 ulcapture_exposure_gain, capture_maxlines, g_preview_exposure;

    LOG("[DEBUG] snapshot param");
    printk("[DEBUG] == %s == \n", __func__);


	noon200pc11_write_reg(0x3503, 0x07);

	ret_h = ret_m = ret_l = 0;
	g_preview_exposure = 0;
	noon200pc11_read_reg(0x3500, &ret_h);
	noon200pc11_read_reg(0x3501, &ret_m);
	noon200pc11_read_reg(0x3502, &ret_l);
	g_preview_exposure = (ret_h << 12) + (ret_m << 4) + (ret_l >> 4);

	ret_h = ret_m = ret_l = 0;
	preview_maxlines = 0;
	noon200pc11_read_reg(0x380e, &ret_h);
	noon200pc11_read_reg(0x380f, &ret_l);
	preview_maxlines = (ret_h << 8) + ret_l;
	/*Read back AGC Gain for preview*/
	gain = 0;
	noon200pc11_read_reg(0x350b, &gain);

	ret = noon200pc11_init_mode(frame_rate, mode);
	if (ret < 0)
		return ret;

	ret_h = ret_m = ret_l = 0;
	noon200pc11_read_reg(0x380e, &ret_h);
	noon200pc11_read_reg(0x380f, &ret_l);
	capture_maxlines = (ret_h << 8) + ret_l;
	if (m_60Hz == true)
		lines_10ms = capture_frame_rate * capture_maxlines/12000;
	else
		lines_10ms = capture_frame_rate * capture_maxlines/10000;

	if (preview_maxlines == 0)
		preview_maxlines = 1;

	ulcapture_exposure = (g_preview_exposure*(capture_frame_rate)*(capture_maxlines))/
		(((preview_maxlines)*(g_preview_frame_rate)));
	icapture_gain = (gain & 0x0f) + 16;
	if (gain & 0x10)
		icapture_gain = icapture_gain << 1;

	if (gain & 0x20)
		icapture_gain = icapture_gain << 1;

	if (gain & 0x40)
		icapture_gain = icapture_gain << 1;

	if (gain & 0x80)
		icapture_gain = icapture_gain << 1;

	ulcapture_exposure_gain = 2 * ulcapture_exposure * icapture_gain;

	if (ulcapture_exposure_gain < capture_maxlines*16) {
		ulcapture_exposure = ulcapture_exposure_gain/16;
		if (ulcapture_exposure > lines_10ms) {
			ulcapture_exposure /= lines_10ms;
			ulcapture_exposure *= lines_10ms;
		}
	} else
		ulcapture_exposure = capture_maxlines;

	if (ulcapture_exposure == 0)
		ulcapture_exposure = 1;

	icapture_gain = (ulcapture_exposure_gain*2/ulcapture_exposure + 1)/2;
	exposure_low = ((unsigned char)ulcapture_exposure)<<4;
	exposure_mid = (unsigned char)(ulcapture_exposure >> 4) & 0xff;
	exposure_high = (unsigned char)(ulcapture_exposure >> 12);

	gain = 0;
	if (icapture_gain > 31) {
		gain |= 0x10;
		icapture_gain = icapture_gain >> 1;
	}
	if (icapture_gain > 31) {
		gain |= 0x20;
		icapture_gain = icapture_gain >> 1;
	}
	if (icapture_gain > 31) {
		gain |= 0x40;
		icapture_gain = icapture_gain >> 1;
	}
	if (icapture_gain > 31) {
		gain |= 0x80;
		icapture_gain = icapture_gain >> 1;
	}
	if (icapture_gain > 16)
		gain |= ((icapture_gain - 16) & 0x0f);

	if (gain == 0x10)
		gain = 0x11;

	noon200pc11_write_reg(0x350b, gain);
	noon200pc11_write_reg(0x3502, exposure_low);
	noon200pc11_write_reg(0x3501, exposure_mid);
	noon200pc11_write_reg(0x3500, exposure_high);
	msleep(500);

	return ret ;
}


/* --------------- IOCTL functions from v4l2_int_ioctl_desc --------------- */

static int ioctl_g_ifparm(struct v4l2_int_device *s, struct v4l2_ifparm *p)
{
    printk("[DEBUG] == %s == \n", __func__);
	if (s == NULL) {
		pr_err("   ERROR!! no slave device set!\n");
		return -1;
	}

	memset(p, 0, sizeof(*p));
	p->u.bt656.clock_curr = noon200pc11_data.mclk;
	pr_debug("   clock_curr=mclk=%d\n", noon200pc11_data.mclk);
	p->if_type = V4L2_IF_TYPE_BT656;
	p->u.bt656.mode = V4L2_IF_TYPE_BT656_MODE_NOBT_8BIT;
	p->u.bt656.clock_min = NOON200PC11_XCLK_MIN;
	p->u.bt656.clock_max = NOON200PC11_XCLK_MAX;
	p->u.bt656.bt_sync_correct = 1;  /* Indicate external vsync */

	return 0;
}

/*!
 * ioctl_s_power - V4L2 sensor interface handler for VIDIOC_S_POWER ioctl
 * @s: pointer to standard V4L2 device structure
 * @on: indicates power mode (on or off)
 *
 * Turns the power on or off, depending on the value of on and returns the
 * appropriate error code.
 */
static int ioctl_s_power(struct v4l2_int_device *s, int on)
{
	struct sensor_data *sensor = s->priv;

    printk("[DEBUG] == %s == \n", __func__);
	if (on && !sensor->on) {
 		/* Make sure power on */
		if (camera_plat->pwdn)
			camera_plat->pwdn(0);

	} else if (!on && sensor->on) {
   
		if (camera_plat->pwdn)
			camera_plat->pwdn(1);
	}

	sensor->on = on;

	return 0;
}

/*!
 * ioctl_g_parm - V4L2 sensor interface handler for VIDIOC_G_PARM ioctl
 * @s: pointer to standard V4L2 device structure
 * @a: pointer to standard V4L2 VIDIOC_G_PARM ioctl structure
 *
 * Returns the sensor's video CAPTURE parameters.
 */
static int ioctl_g_parm(struct v4l2_int_device *s, struct v4l2_streamparm *a)
{
	struct sensor_data *sensor = s->priv;
	struct v4l2_captureparm *cparm = &a->parm.capture;
	int ret = 0;

    printk("[DEBUG] == %s == \n", __func__);
	switch (a->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		memset(a, 0, sizeof(*a));
		a->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cparm->capability = sensor->streamcap.capability;
		cparm->timeperframe = sensor->streamcap.timeperframe;
		cparm->capturemode = sensor->streamcap.capturemode;
		ret = 0;
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		ret = -EINVAL;
		break;

	default:
		pr_debug("   type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/*!
 * ioctl_s_parm - V4L2 sensor interface handler for VIDIOC_S_PARM ioctl
 * @s: pointer to standard V4L2 device structure
 * @a: pointer to standard V4L2 VIDIOC_S_PARM ioctl structure
 *
 * Configures the sensor to use the input parameters, if possible.  If
 * not possible, reverts to the old parameters and returns the
 * appropriate error code.
 */
static int ioctl_s_parm(struct v4l2_int_device *s, struct v4l2_streamparm *a)
{
	struct sensor_data *sensor = s->priv;
	struct v4l2_fract *timeperframe = &a->parm.capture.timeperframe;
	u32 tgt_fps, old_fps;	/* target frames per secound */
	enum noon200pc11_frame_rate new_frame_rate, old_frame_rate;
	int ret = 0;

    printk("[DEBUG] == %s == \n", __func__);
	/* Make sure power on */
	if (camera_plat->pwdn)
		camera_plat->pwdn(0);

	switch (a->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		/* Check that the new frame rate is allowed. */
		if ((timeperframe->numerator == 0) ||
		    (timeperframe->denominator == 0)) {
			timeperframe->denominator = DEFAULT_FPS;
			timeperframe->numerator = 1;
		}

		tgt_fps = timeperframe->denominator /
			  timeperframe->numerator;

		if (tgt_fps > MAX_FPS) {
			timeperframe->denominator = MAX_FPS;
			timeperframe->numerator = 1;
		} else if (tgt_fps < MIN_FPS) {
			timeperframe->denominator = MIN_FPS;
			timeperframe->numerator = 1;
		}

		/* Actual frame rate we use */
		tgt_fps = timeperframe->denominator /
			  timeperframe->numerator;

		if (tgt_fps == 15)
			new_frame_rate = noon200pc11_15_fps;
		else if (tgt_fps == 30)
			new_frame_rate = noon200pc11_30_fps;
		else {
			pr_err(" The camera frame rate is not supported!\n");
			return -EINVAL;
		}

		if (sensor->streamcap.timeperframe.numerator != 0)
			old_fps = sensor->streamcap.timeperframe.denominator /
				sensor->streamcap.timeperframe.numerator;
		else
			old_fps = 30;

		if (old_fps == 15)
			old_frame_rate = noon200pc11_15_fps;
		else if (old_fps == 30)
			old_frame_rate = noon200pc11_30_fps;
		else {
			pr_warning(" No valid frame rate set!\n");
			old_frame_rate = noon200pc11_30_fps;
		}

		ret = noon200pc11_change_mode(new_frame_rate, old_frame_rate,
				a->parm.capture.capturemode,
				sensor->streamcap.capturemode);
		if (ret < 0)
			return ret;

		sensor->streamcap.timeperframe = *timeperframe;
		sensor->streamcap.capturemode =
				(u32)a->parm.capture.capturemode;
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		pr_debug("   type is not " \
			"V4L2_BUF_TYPE_VIDEO_CAPTURE but %d\n",
			a->type);
		ret = -EINVAL;
		break;

	default:
		pr_debug("   type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/*!
 * ioctl_g_fmt_cap - V4L2 sensor interface handler for ioctl_g_fmt_cap
 * @s: pointer to standard V4L2 device structure
 * @f: pointer to standard V4L2 v4l2_format structure
 *
 * Returns the sensor's current pixel format in the v4l2_format
 * parameter.
 */
static int ioctl_g_fmt_cap(struct v4l2_int_device *s, struct v4l2_format *f)
{
	struct sensor_data *sensor = s->priv;

    printk("[DEBUG] == %s == \n", __func__);
	f->fmt.pix = sensor->pix;

	return 0;
}
static int noon200oc11_set_format(struct sensor *s, int format)
{
	int ret = 0;
    /*

	if (i2c_smbus_write_byte_data(s->i2c_client, 0xff, 0x00) < 0)
		ret = -EPERM;

	if (format == V4L2_PIX_FMT_RGB565) {
		if (i2c_smbus_write_byte_data(s->i2c_client, 0xda, 0x08) < 0)
			ret = -EPERM;

		if (i2c_smbus_write_byte_data(s->i2c_client, 0xd7, 0x03) < 0)
			ret = -EPERM;
	} else if (format == V4L2_PIX_FMT_YUV420) {
		if (i2c_smbus_write_byte_data(s->i2c_client, 0xda, 0x00) < 0)
			ret = -EPERM;

		if (i2c_smbus_write_byte_data(s->i2c_client, 0xd7, 0x1b) < 0)
			ret = -EPERM;
	} else {
		pr_debug("format not supported\n");
	}

    */
	return ret;
}
static int ioctl_s_fmt_cap(struct v4l2_int_device *s, struct v4l2_format *f)
{
	struct sensor *sensor = s->priv;
	u32 format = f->fmt.pix.pixelformat;
	int size = 0, ret = 0;

	size = f->fmt.pix.width * f->fmt.pix.height;
	switch (format) {
        /*
	case V4L2_PIX_FMT_RGB565:
		if (size > 640 * 480)
			sensor->streamcap.capturemode = V4L2_MODE_HIGHQUALITY;
		else
			sensor->streamcap.capturemode = 0;
		ret = noon200oc11_init_mode(sensor);

		ret = noon200oc11_set_format(sensor, V4L2_PIX_FMT_RGB565);
		break;
	case V4L2_PIX_FMT_UYVY:
		if (size > 640 * 480)
			sensor->streamcap.capturemode = V4L2_MODE_HIGHQUALITY;
		else
			sensor->streamcap.capturemode = 0;
		ret = noon200oc11_init_mode(sensor);
		break;
        */
        /*
	case V4L2_PIX_FMT_YUV420:
		if (size > 640 * 480)
			sensor->streamcap.capturemode = V4L2_MODE_HIGHQUALITY;
		else
			sensor->streamcap.capturemode = 0;
            */
		//ret = noon200oc11_init_mode(sensor);

		/* YUYV: width * 2, YY: width */
		ret = noon200oc11_set_format(sensor, V4L2_PIX_FMT_YUV420);
		break;
	default:
		pr_debug("case not supported\n");
		break;
	}

	return ret;
}



/*!
 * ioctl_g_ctrl - V4L2 sensor interface handler for VIDIOC_G_CTRL ioctl
 * @s: pointer to standard V4L2 device structure
 * @vc: standard V4L2 VIDIOC_G_CTRL ioctl structure
 *
 * If the requested control is supported, returns the control's current
 * value from the video_control[] array.  Otherwise, returns -EINVAL
 * if the control is not supported.
 */
static int ioctl_g_ctrl(struct v4l2_int_device *s, struct v4l2_control *vc)
{
	int ret = 0;
    printk("[DEBUG] == %s == \n", __func__);

	switch (vc->id) {
	case V4L2_CID_BRIGHTNESS:
		vc->value = noon200pc11_data.brightness;
		break;
	case V4L2_CID_HUE:
		vc->value = noon200pc11_data.hue;
		break;
	case V4L2_CID_CONTRAST:
		vc->value = noon200pc11_data.contrast;
		break;
	case V4L2_CID_SATURATION:
		vc->value = noon200pc11_data.saturation;
		break;
	case V4L2_CID_RED_BALANCE:
		vc->value = noon200pc11_data.red;
		break;
	case V4L2_CID_BLUE_BALANCE:
		vc->value = noon200pc11_data.blue;
		break;
	case V4L2_CID_EXPOSURE:
		vc->value = noon200pc11_data.ae_mode;
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

/*!
 * ioctl_s_ctrl - V4L2 sensor interface handler for VIDIOC_S_CTRL ioctl
 * @s: pointer to standard V4L2 device structure
 * @vc: standard V4L2 VIDIOC_S_CTRL ioctl structure
 *
 * If the requested control is supported, sets the control's current
 * value in HW (and updates the video_control[] array).  Otherwise,
 * returns -EINVAL if the control is not supported.
 */
static int ioctl_s_ctrl(struct v4l2_int_device *s, struct v4l2_control *vc)
{
	int retval = 0;
	struct sensor_data *sensor = s->priv;
	__u32 captureMode = sensor->streamcap.capturemode;

    printk("[DEBUG] == %s == \n", __func__);
	pr_debug("In noon200pc11:ioctl_s_ctrl %d\n",
		 vc->id);

	switch (vc->id) {
	case V4L2_CID_BRIGHTNESS:
		break;
	case V4L2_CID_CONTRAST:
		break;
	case V4L2_CID_SATURATION:
		break;
	case V4L2_CID_HUE:
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		break;
	case V4L2_CID_DO_WHITE_BALANCE:
		break;
	case V4L2_CID_RED_BALANCE:
		break;
	case V4L2_CID_BLUE_BALANCE:
		break;
	case V4L2_CID_GAMMA:
		break;
	case V4L2_CID_EXPOSURE:
		break;
	case V4L2_CID_AUTOGAIN:
		break;
	case V4L2_CID_GAIN:
		break;
	case V4L2_CID_HFLIP:
		break;
	case V4L2_CID_VFLIP:
		break;
	case V4L2_CID_MXC_ROT:
	case V4L2_CID_MXC_VF_ROT:
        /*
		switch (vc->value) {
		case V4L2_MXC_ROTATE_NONE:
				if (noon200pc11_set_rotate_mode(noon200pc11_rotate_none_VGA))
					retval = -EPERM;
			break;
		case V4L2_MXC_ROTATE_VERT_FLIP:
				if (noon200pc11_set_rotate_mode(noon200pc11_rotate_vert_flip_VGA))
					retval = -EPERM;
			break;
		case V4L2_MXC_ROTATE_HORIZ_FLIP:
				if (noon200pc11_set_rotate_mode(noon200pc11_rotate_horiz_flip_VGA))
					retval = -EPERM;
			break;
		case V4L2_MXC_ROTATE_180:
				if (noon200pc11_set_rotate_mode(noon200pc11_rotate_180_VGA))
					retval = -EPERM;
			break;
		default:
			retval = -EPERM;
			break;
		}
        */
		break;
	default:
		retval = -EPERM;
		break;
	}

	return retval;
}

/*!
 * ioctl_enum_framesizes - V4L2 sensor interface handler for
 *			   VIDIOC_ENUM_FRAMESIZES ioctl
 * @s: pointer to standard V4L2 device structure
 * @fsize: standard V4L2 VIDIOC_ENUM_FRAMESIZES ioctl structure
 *
 * Return 0 if successful, otherwise -EINVAL.
 */
static int ioctl_enum_framesizes(struct v4l2_int_device *s,
				 struct v4l2_frmsizeenum *fsize)
{
    printk("[DEBUG] == %s == \n", __func__);
	if (fsize->index > noon200pc11_mode_MAX)
		return -EINVAL;

	fsize->pixel_format = noon200pc11_data.pix.pixelformat;
	fsize->discrete.width =
			max(noon200pc11_mode_info_data[0][fsize->index].width,
			    noon200pc11_mode_info_data[1][fsize->index].width);
	fsize->discrete.height =
			max(noon200pc11_mode_info_data[0][fsize->index].height,
			    noon200pc11_mode_info_data[1][fsize->index].height);
	return 0;
}

/*!
 * ioctl_enum_frameintervals - V4L2 sensor interface handler for
 *			       VIDIOC_ENUM_FRAMEINTERVALS ioctl
 * @s: pointer to standard V4L2 device structure
 * @fival: standard V4L2 VIDIOC_ENUM_FRAMEINTERVALS ioctl structure
 *
 * Return 0 if successful, otherwise -EINVAL.
 */
static int ioctl_enum_frameintervals(struct v4l2_int_device *s,
					 struct v4l2_frmivalenum *fival)
{
	int i, j, count;
    printk("[DEBUG] == %s == \n", __func__);

	if (fival->index < 0 || fival->index > noon200pc11_mode_MAX)
		return -EINVAL;

	if (fival->pixel_format == 0 || fival->width == 0 || fival->height == 0) {
		pr_warning("Please assign pixelformat, width and height.\n");
		return -EINVAL;
	}

	fival->type = V4L2_FRMIVAL_TYPE_DISCRETE;
	fival->discrete.numerator = 1;

	count = 0;
	for (i = 0; i < ARRAY_SIZE(noon200pc11_mode_info_data); i++) {
		for (j = 0; j < (noon200pc11_mode_MAX + 1); j++) {
			if (fival->pixel_format == noon200pc11_data.pix.pixelformat
			 && fival->width == noon200pc11_mode_info_data[i][j].width
			 && fival->height == noon200pc11_mode_info_data[i][j].height
			 && noon200pc11_mode_info_data[i][j].init_data_ptr != NULL) {
				count++;
			}
			if (fival->index == (count - 1)) {
				fival->discrete.denominator =
						noon200pc11_framerates[i];
				return 0;
			}
		}
	}

	return -EINVAL;
}

/*!
 * ioctl_g_chip_ident - V4L2 sensor interface handler for
 *			VIDIOC_DBG_G_CHIP_IDENT ioctl
 * @s: pointer to standard V4L2 device structure
 * @id: pointer to int
 *
 * Return 0.
 */
static int ioctl_g_chip_ident(struct v4l2_int_device *s, int *id)
{
    printk("[DEBUG] == %s == \n", __func__);
	((struct v4l2_dbg_chip_ident *)id)->match.type =
					V4L2_CHIP_MATCH_I2C_DRIVER;
	strcpy(((struct v4l2_dbg_chip_ident *)id)->match.name, "noon200pc11_camera");

	return 0;
}

/*!
 * ioctl_init - V4L2 sensor interface handler for VIDIOC_INT_INIT
 * @s: pointer to standard V4L2 device structure
 */
static int ioctl_init(struct v4l2_int_device *s)
{

    printk("[DEBUG] == %s == \n", __func__);
	return 0;
}

/*!
 * ioctl_enum_fmt_cap - V4L2 sensor interface handler for VIDIOC_ENUM_FMT
 * @s: pointer to standard V4L2 device structure
 * @fmt: pointer to standard V4L2 fmt description structure
 *
 * Return 0.
 */
static int ioctl_enum_fmt_cap(struct v4l2_int_device *s,
			      struct v4l2_fmtdesc *fmt)
{
    printk("[DEBUG] == %s == \n", __func__);
	if (fmt->index > 0)	/* only 1 pixelformat support so far */
		return -EINVAL;

	fmt->pixelformat = noon200pc11_data.pix.pixelformat;

	return 0;
}

/*!
 * ioctl_dev_init - V4L2 sensor interface handler for vidioc_int_dev_init_num
 * @s: pointer to standard V4L2 device structure
 *
 * Initialise the device when slave attaches to the master.
 */
static int ioctl_dev_init(struct v4l2_int_device *s)
{
	struct reg_value *pModeSetting = NULL;
	s32 i = 0;
	s32 iModeSettingArySize = 0;
	register u32 Delay_ms = 0;
	register u16 RegAddr = 0;
	register u8 Mask = 0;
	register u8 Val = 0;
	u8 RegVal = 0;
	int retval = 0;

	struct sensor_data *sensor = s->priv;
	u32 tgt_xclk;	/* target xclk */
	u32 tgt_fps;	/* target frames per secound */
	enum noon200pc11_frame_rate frame_rate;

	noon200pc11_data.on = true;

	/* mclk */
	tgt_xclk = noon200pc11_data.mclk;
	tgt_xclk = min(tgt_xclk, (u32)NOON200PC11_XCLK_MAX);
	tgt_xclk = max(tgt_xclk, (u32)NOON200PC11_XCLK_MIN);
	noon200pc11_data.mclk = tgt_xclk;

	pr_debug("   Setting mclk to %d MHz\n", tgt_xclk / 1000000);
	set_mclk_rate(&noon200pc11_data.mclk, noon200pc11_data.mclk_source);

	/* Default camera frame rate is set in probe */
	tgt_fps = sensor->streamcap.timeperframe.denominator /
		  sensor->streamcap.timeperframe.numerator;

    printk("[DEBUG] == %s ==  %d \n", __func__, tgt_fps);

	if (tgt_fps == 15)
		frame_rate = noon200pc11_15_fps;
	else if (tgt_fps == 30)
		frame_rate = noon200pc11_30_fps;
    else if (tgt_fps == 56)
		frame_rate = noon200pc11_56_fps;
	else
		return -EINVAL; /* Only support 15fps or 30fps now. */

	pModeSetting = noon200pc11_initial_setting;
	iModeSettingArySize = ARRAY_SIZE(noon200pc11_initial_setting);

	for (i = 0; i < iModeSettingArySize; ++i, ++pModeSetting) {
		Delay_ms = pModeSetting->u32Delay_ms;
		RegAddr = pModeSetting->u16RegAddr;
		Val = pModeSetting->u8Val;
		Mask = pModeSetting->u8Mask;
		if (Mask) {
			retval = noon200pc11_read_reg(RegAddr, &RegVal);
			if (retval < 0)
				goto err;

			RegVal &= ~(u8)Mask;
			Val &= Mask;
			Val |= RegVal;
		}

		retval = noon200pc11_write_reg(RegAddr, Val);
		if (retval < 0)
			goto err;

		if (Delay_ms)
			msleep(Delay_ms);
	}
err:
	return retval;
}

/*!
 * ioctl_dev_exit - V4L2 sensor interface handler for vidioc_int_dev_exit_num
 * @s: pointer to standard V4L2 device structure
 *
 * Delinitialise the device when slave detaches to the master.
 */
static int ioctl_dev_exit(struct v4l2_int_device *s)
{
	return 0;
}

/*!
 * This structure defines all the ioctls for this module and links them to the
 * enumeration.
 */
static struct v4l2_int_ioctl_desc noon200pc11_ioctl_desc[] = {
	{vidioc_int_dev_init_num, (v4l2_int_ioctl_func *)ioctl_dev_init},
	{vidioc_int_dev_exit_num, ioctl_dev_exit},
	{vidioc_int_s_power_num, (v4l2_int_ioctl_func *)ioctl_s_power},
	{vidioc_int_g_ifparm_num, (v4l2_int_ioctl_func *)ioctl_g_ifparm},
/*	{vidioc_int_g_needs_reset_num,
				(v4l2_int_ioctl_func *)ioctl_g_needs_reset}, */
/*	{vidioc_int_reset_num, (v4l2_int_ioctl_func *)ioctl_reset}, */
	{vidioc_int_init_num, (v4l2_int_ioctl_func *)ioctl_init},
	{vidioc_int_enum_fmt_cap_num,
				(v4l2_int_ioctl_func *)ioctl_enum_fmt_cap},
/*	{vidioc_int_try_fmt_cap_num,
				(v4l2_int_ioctl_func *)ioctl_try_fmt_cap}, */
	{vidioc_int_g_fmt_cap_num, (v4l2_int_ioctl_func *)ioctl_g_fmt_cap},
	{vidioc_int_s_fmt_cap_num, (v4l2_int_ioctl_func *)ioctl_s_fmt_cap},
	{vidioc_int_g_parm_num, (v4l2_int_ioctl_func *)ioctl_g_parm},
	{vidioc_int_s_parm_num, (v4l2_int_ioctl_func *)ioctl_s_parm},
/*	{vidioc_int_queryctrl_num, (v4l2_int_ioctl_func *)ioctl_queryctrl}, */
	{vidioc_int_g_ctrl_num, (v4l2_int_ioctl_func *)ioctl_g_ctrl},
	{vidioc_int_s_ctrl_num, (v4l2_int_ioctl_func *)ioctl_s_ctrl},
	{vidioc_int_enum_framesizes_num,
				(v4l2_int_ioctl_func *)ioctl_enum_framesizes},
	{vidioc_int_enum_frameintervals_num,
				(v4l2_int_ioctl_func *)ioctl_enum_frameintervals},
	{vidioc_int_g_chip_ident_num,
				(v4l2_int_ioctl_func *)ioctl_g_chip_ident},
};

static struct v4l2_int_slave noon200pc11_slave = {
	.ioctls = noon200pc11_ioctl_desc,
	.num_ioctls = ARRAY_SIZE(noon200pc11_ioctl_desc),
};

static struct v4l2_int_device noon200pc11_int_device = {
	.module = THIS_MODULE,
	.name = "noon200pc11",
	.type = v4l2_int_type_slave,
	.u = {
		.slave = &noon200pc11_slave,
	},
};

/*!
 * noon200pc11 I2C probe function
 *
 * @param adapter            struct i2c_adapter *
 * @return  Error code indicating success or failure
 */
static int noon200pc11_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int retval;
	struct i2c_adapter *adapter;
	struct fsl_mxc_camera_platform_data *plat_data = client->dev.platform_data;
	u8 chip_id;

    LOG("[DEBUG] initializing");

	adapter = to_i2c_adapter(client->dev.parent);
	if (!i2c_check_functionality(adapter,
				     I2C_FUNC_SMBUS_BYTE |
				     I2C_FUNC_SMBUS_BYTE_DATA |
				     I2C_FUNC_SMBUS_I2C_BLOCK))
    {
        LOG("can NOT use I2C");
		return -EIO;
    }

	/* Set initial values for the sensor struct. */
	memset(&noon200pc11_data, 0, sizeof(noon200pc11_data));
	noon200pc11_data.mclk = 24000000; /* 6 - 54 MHz, typical 24MHz */
	noon200pc11_data.mclk = plat_data->mclk;
	noon200pc11_data.mclk_source = plat_data->mclk_source;
	noon200pc11_data.csi = plat_data->csi;
	noon200pc11_data.io_init = plat_data->io_init;

	noon200pc11_data.i2c_client = client;
	noon200pc11_data.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	noon200pc11_data.pix.width = 640;
	noon200pc11_data.pix.height = 480;
	noon200pc11_data.streamcap.capability = V4L2_MODE_HIGHQUALITY |
					   V4L2_CAP_TIMEPERFRAME;
	noon200pc11_data.streamcap.capturemode = 0;
	noon200pc11_data.streamcap.timeperframe.denominator = DEFAULT_FPS;
	noon200pc11_data.streamcap.timeperframe.numerator = 1;

    if (plat_data->io_init)
        plat_data->io_init();

    if (plat_data->pwdn)
        plat_data->pwdn(0);

   retval = noon200pc11_read_reg(NOON200PC11_CHIP_ID_ADDR, &chip_id);
    if (retval < 0 || chip_id != NOON200PC11_CHIP_ID) {
		LOG("read chip ID 0x%x is not equal to 0x%x!", retval, NOON200PC11_CHIP_ID );
        retval = -ENODEV;
        goto err;
    }

	if (plat_data->pwdn)
		plat_data->pwdn(1);

	camera_plat = plat_data;

	noon200pc11_int_device.priv = &noon200pc11_data;
	retval = v4l2_int_device_register(&noon200pc11_int_device);

    LOG("camera is found");
	return retval;

   err:
	return retval;
}

/*!
 * noon200pc11 I2C detach function
 *
 * @param client            struct i2c_client *
 * @return  Error code indicating success or failure
 */
static int noon200pc11_remove(struct i2c_client *client)
{
	v4l2_int_device_unregister(&noon200pc11_int_device);

	return 0;
}

static __init int noon200pc11_init(void)
{
	u8 err;

	err = i2c_add_driver(&noon200pc11_i2c_driver);
	if (err != 0)
		pr_err("%s:driver registration failed, error=%d \n",__func__, err);

	return err;
}

static void __exit noon200pc11_clean(void)
{
	i2c_del_driver(&noon200pc11_i2c_driver);
}

module_init(noon200pc11_init);
module_exit(noon200pc11_clean);

MODULE_AUTHOR("Huins, Inc.");
MODULE_DESCRIPTION("noon200pc11 Camera Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
MODULE_ALIAS("CSI");
