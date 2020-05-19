
/*****************************************************************************
 *
 * File Name: achroimx_led.c
 *
 * Copyright (C) 2014 HUINS,Inc.
 * Programmed by Kim suhak
 * email : shkim@huins.com
 * file creation : 2014/07/14
 *
 *****************************************************************************/
#include <asm/io.h>

void achroimx_led(void) {
/*    volatile u32 *reg;
    reg = 0x020E01C4;
    *reg = 0x15;
 
    reg = 0x020AC004;
    *reg = 0x8000;

    reg = 0x020AC000;
    *reg = 0x8000;
*/

    writel(0x15, 0x020E01C4);
    writel(0x8000, 0x020AC004);
    writel(0x8000, 0x020AC000);

    //do {} while(1);
}
