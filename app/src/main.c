/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>

#define STRIP_NODE		DT_ALIAS(led_strip)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)

#define DELAY_TIME K_MSEC(2000)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

static const struct led_rgb colors[] = {
	RGB(0xff, 0x00, 0x00), /* red */
	RGB(0x00, 0xff, 0x00), /* green */
	RGB(0x00, 0x00, 0xff), /* blue */
	RGB(0xff, 0xFF, 0xff), /* white */
	RGB(0xFF, 0xA9, 0x00), /* Yellow */
	RGB(0xA, 0x00, 0xff), /* ???? */
};

struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

#define _A 0
#define _B 1
#define _C 2
#define _D 3
#define _E 4
#define _F 5
#define _G 6
#define _H 7
#define _I 16
#define _J 15
#define _K 14
#define _L 13
#define _M 12
#define _N 11
#define _O 10
#define _P 9
#define _Q 8
#define _R 17
#define _S 18
#define _T 19
#define _U 20
#define _V 21
#define _W 22
#define _X 23
#define _Y 24
#define _Z 25

size_t cursor = 0, color = 0, pattern_counter = 0;
int rc;

void display_pattern(size_t *pattern,size_t len);
void display_pattern_all_on(void);
void display_pattern_all_off(void);
void display_pattern_all_white(void);
void display_pattern_line(size_t start,size_t end);

void main(void)
{

	size_t right_here[] = {_R,_I,_G,_H,_T,_H,_E,_R,_E};
	size_t run[] = {_R,_U,_N};
	size_t vecna_is_here[] = {_V,_E,_C,_N,_A,_I,_S,_H,_E,_R,_E};
	size_t are_you_looking_for_me[] = {_A,_R,_E,_Y,_O,_U,_L,_O,_O,_K,_I,_N,_G,_F,_O,_R,_M,_E};


	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return;
	}

	LOG_INF("Displaying pattern on strip");
	while (1) {
	//	display_pattern(abc,ARRAY_SIZE(abc));
		display_pattern(vecna_is_here,ARRAY_SIZE(vecna_is_here));
		display_pattern_all_off();
		k_sleep(K_MSEC(2000));
		display_pattern(right_here,ARRAY_SIZE(right_here));
		display_pattern_all_off();
		k_sleep(K_MSEC(1500));
		display_pattern(run,ARRAY_SIZE(run));
		display_pattern_all_off();
		k_sleep(K_MSEC(1500));
		display_pattern_all_on();
		k_sleep(K_MSEC(1500));	
		
		display_pattern_all_off(); // Glich 1
		k_sleep(K_MSEC(50));	
		display_pattern_all_on();
		k_sleep(K_MSEC(30));	
		display_pattern_all_off();
		k_sleep(K_MSEC(50));	
		display_pattern_all_on();
		k_sleep(K_MSEC(30));	
		display_pattern_all_off();
		
		k_sleep(K_MSEC(500));	  // Glich 2	
		display_pattern_all_on();
		k_sleep(K_MSEC(30));	
		display_pattern_all_off();
		k_sleep(K_MSEC(50));	
		display_pattern_all_on();
		k_sleep(K_MSEC(300));	
		display_pattern_all_off();
		
		k_sleep(K_MSEC(100));	  //Glisch 3
		display_pattern_all_on();
		k_sleep(K_MSEC(75));	
		display_pattern_all_off();
		k_sleep(K_MSEC(30));	
		display_pattern_all_on();
		k_sleep(K_MSEC(10));	
		display_pattern_all_off();
		
		display_pattern(are_you_looking_for_me,ARRAY_SIZE(are_you_looking_for_me));
		k_sleep(K_MSEC(2000));
		k_sleep(K_MSEC(75));	
		display_pattern_line(0,7);
		k_sleep(K_MSEC(30));	
		display_pattern_line(8,16);
		k_sleep(K_MSEC(40));	
		display_pattern_line(0,7);
		k_sleep(K_MSEC(50));	
		display_pattern_line(0,7);
		k_sleep(K_MSEC(75));	
		display_pattern_line(16,26);
		k_sleep(K_MSEC(50));	
		display_pattern_line(8,16);
		k_sleep(K_MSEC(100));	
		display_pattern_line(8,16);
		k_sleep(K_MSEC(75));	
		display_pattern_line(0,7);
		k_sleep(K_MSEC(50));	
		display_pattern_line(8,16);
		k_sleep(K_MSEC(75));	
		display_pattern_all_on();
		k_sleep(K_MSEC(30));	
		display_pattern_line(0,7);
		k_sleep(K_MSEC(75));	
		display_pattern_line(16,26);
		k_sleep(K_MSEC(50));	
		display_pattern_line(0,7);
		k_sleep(K_MSEC(100));	
		display_pattern_line(8,16);
		
		k_sleep(K_MSEC(500));
		display_pattern_all_white();	
		k_sleep(K_MSEC(3000));
		display_pattern_all_off();
		k_sleep(K_MSEC(7000));

	}
}



void display_pattern(size_t *pattern,size_t len)
{
	size_t clr_pntr = 0;

	for(int i=0 ; i < len ; i++)
	{
		memset(&pixels, 0x00, sizeof(pixels)); //clear the strip
		memcpy(&pixels[pattern[i]], &colors[clr_pntr], sizeof(struct led_rgb));
		rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
	
		if (rc) 
		{
			LOG_ERR("couldn't update strip: %d", rc);
		}
		
		clr_pntr++;
		if(clr_pntr >= ARRAY_SIZE(colors))
		{
			clr_pntr =0;
		}
		k_sleep(DELAY_TIME);
	}

}

void display_pattern_all_on(void)
{
	size_t clr_pntr = 0;
	memset(&pixels, 0x00, sizeof(pixels)); //clear the strip
	for(int i=0 ; i < STRIP_NUM_PIXELS ; i++)
	{
		memcpy(&pixels[i], &colors[clr_pntr], sizeof(struct led_rgb));
		
		clr_pntr++;
		if(clr_pntr >= ARRAY_SIZE(colors))
		{
			clr_pntr =0;
		}
	}
	rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
		if (rc) 
		{
			LOG_ERR("couldn't update strip: %d", rc);
		}
}

void display_pattern_all_off(void)
{
	memset(&pixels, 0x00, sizeof(pixels)); //clear the strip
	rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);

	if (rc) 
	{
		LOG_ERR("couldn't update strip: %d", rc);
	}

}

void display_pattern_all_white(void)
{
	memset(&pixels, 0x00, sizeof(pixels)); //clear the strip
	for(int i=0 ; i < STRIP_NUM_PIXELS ; i++)
	{
		memcpy(&pixels[i], &colors[3], sizeof(struct led_rgb));
		
	}
	rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
		if (rc) 
		{
			LOG_ERR("couldn't update strip: %d", rc);
		}

}

void display_pattern_line(size_t start,size_t end)
{
	size_t clr_pntr = 0;
	memset(&pixels, 0x00, sizeof(pixels)); //clear the strip
	for(int i = start ; i < end ; i++)
	{
		memcpy(&pixels[i], &colors[clr_pntr], sizeof(struct led_rgb));
		
		clr_pntr++;
		if(clr_pntr >= ARRAY_SIZE(colors))
		{
			clr_pntr =0;
		}
	}
	rc = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
		if (rc) 
		{
			LOG_ERR("couldn't update strip: %d", rc);
		}

}
