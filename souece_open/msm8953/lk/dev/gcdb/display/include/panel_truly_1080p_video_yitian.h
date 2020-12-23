/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PANEL_TRULY_1080P_VIDEO_H_

#define _PANEL_TRULY_1080P_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/

static struct panel_config truly_1080p_video_panel_data = {
  "qcom,mdss_dsi_truly_1080p_video", "dsi:0:", "qcom,mdss-dsi-panel",
    10, 0, "DISPLAY_1", 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL
 /* 10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, NULL*/
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution truly_1080p_video_panel_res = {
  720, 1280,  52, 100, 24, 0,8, 40, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0
  /*1080, 1920, 96, 64, 16, 0, 4, 16, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0*/
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info truly_1080p_video_color = {
  24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Command information                                                 */
/*---------------------------------------------------------------------------*/


static char otm1287a_yitian_720p_video_on_cmd0[] = {
0x04, 0x00, 0x39, 0xC0,
0xB9, 0xFF, 0x83, 0x94,
};

static char otm1287a_yitian_720p_video_on_cmd1[] = {
0x07, 0x00, 0x39, 0xC0,
0xBA, 0x63, 0x03, 0x68,
0x6B, 0xB2, 0xC0, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd2[] = {
0x0B, 0x00, 0x39, 0xC0,
0xB1, 0x48, 0x12, 0x72,
0x09, 0x32, 0x44, 0x71,
0x31, 0x4F, 0x35, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd3[] = {
0x06, 0x00, 0x39, 0xC0,
0xB2, 0x00, 0x80, 0x64,
0x05, 0x07, 0xFF, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd4[] = {
				0x1F, 0x00, 0x39, 0xC0,
				0xB4, 0x26 ,0x76 ,0x26,
                                0x76, 0x26 ,0x26 ,0x05,
                                0x10, 0x86 ,0x35 ,0x00,
                                0x3F, 0x26 ,0x76 ,0x26,
                                0x76, 0x26 ,0x26 ,0x05,
                                0x10, 0x86 ,0x3F ,0x00,
                                0xFF, 0x81 ,0x81 ,0x81,
                                0x81, 0x08 ,0x01 ,0xff,

};

static char otm1287a_yitian_720p_video_on_cmd5[] = {
				0x22, 0x00, 0x39, 0xC0,
				0xD3 ,0x00 ,0x00 ,0x0F,
                                0x0F ,0x01 ,0x01 ,0x10,
                                0x10 ,0x32 ,0x10 ,0x00,
                                0x00 ,0x00 ,0x32 ,0x15,
                                0x04 ,0x05 ,0x04 ,0x32,
                                0x15 ,0x14 ,0x05 ,0x14,
                                0x37 ,0x33 ,0x04 ,0x04,
                                0x37 ,0x00 ,0x00 ,0x47,
                                0x05 ,0x40 ,0xff ,0xff,

};

static char otm1287a_yitian_720p_video_on_cmd6[] = {
				0x2D, 0x00, 0x39, 0xC0,
				0xD5,0x18 ,0x18 ,0x25 ,
                                0x24,0x27 ,0x26 ,0x11 ,
                                0x10,0x15 ,0x14 ,0x13 ,
                                0x12,0x17 ,0x16 ,0x01 ,
                                0x00,0x18 ,0x18 ,0x18 ,
                                0x18,0x18 ,0x18 ,0x18 ,
                                0x18,0x18 ,0x18 ,0x05 ,
                                0x04,0x03 ,0x02 ,0x07 ,
                                0x06,0x18 ,0x18 ,0x18 ,
                                0x18,0x21 ,0x20 ,0x23 ,
                                0x22,0x18 ,0x18 ,0x18 ,
                                0x18,0xff ,0xff ,0xff ,
};

static char otm1287a_yitian_720p_video_on_cmd7[] = {
				0x2D, 0x00, 0x39, 0xC0,
				0xD6, 0x18, 0x18 ,0x22,
                                0x23, 0x20, 0x21 ,0x12,
                                0x13, 0x16, 0x17 ,0x10,
                                0x11, 0x14, 0x15 ,0x06,
                                0x07, 0x18, 0x18 ,0x18,
                                0x18, 0x18, 0x18 ,0x18,
                                0x18, 0x18, 0x18 ,0x02,
                                0x03, 0x04, 0x05 ,0x00,
                                0x01, 0x18, 0x18 ,0x18,
                                0x18, 0x26, 0x27 ,0x24,
                                0x25, 0x18, 0x18 ,0x18,
                                0x18, 0xff, 0xff ,0xff,

};

static char otm1287a_yitian_720p_video_on_cmd8[] = {
				0x3B, 0x00, 0x39, 0xC0,
 				0xE0, 0x00 ,0x03, 0x0B,
                                0x0E, 0x10 ,0x13, 0x17,
                                0x15, 0x2D ,0x3D, 0x51,
                                0x51, 0x5E ,0x75, 0x7C,
                                0x84, 0x94 ,0x9A, 0x98,
                                0xA6, 0xB2 ,0x57, 0x57,
                                0x5A, 0x60 ,0x64, 0x6A,
                                0x72, 0x7F ,0x00, 0x03,
                                0x0B, 0x0E ,0x10, 0x13,
                                0x17, 0x15 ,0x2D, 0x3D,
                                0x51, 0x51 ,0x5E, 0x75,
                                0x7C, 0x84 ,0x94, 0x9A,
                                0x98, 0xA6 ,0xB2, 0x57,
                                0x57, 0x5A ,0x60, 0x64,
                                0x6A, 0x72 ,0x7F, 0xff,
};

static char otm1287a_yitian_720p_video_on_cmd9[] = {
0x03, 0x00, 0x39, 0xC0,
0xB6, 0x4e, 0x4e, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd10[] = {
0x02, 0x00, 0x39, 0xC0,
0xCC, 0x0B, 0xFF, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd11[] = {
0x03, 0x00, 0x39, 0xC0,
0xC0, 0x1F, 0x31, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd12[] = {
0x02, 0x00, 0x39, 0xC0,
0xD2, 0x88, 0xFF, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd13[] = {
0x02, 0x00, 0x39, 0xC0,
0xD4, 0x02, 0xFF, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd14[] = {
0x02, 0x00, 0x39, 0xC0,
0xBD, 0x01, 0xFF, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd15[] = {
0x02, 0x00, 0x39, 0xC0,
0xB1, 0x60, 0xFF, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd16[] = {
0x02, 0x00, 0x39, 0xC0,
0xBD, 0x00, 0xFF, 0xFF,
};

static char otm1287a_yitian_720p_video_on_cmd17[] = {
0x08, 0x00, 0x39, 0xC0,
0xBF, 0x40, 0x81, 0x50,
0x00, 0x1A, 0xFC, 0x01,
};

static char otm1287a_yitian_720p_video_on_cmd18[] = {
0x11, 0x00, 0x05, 0x80,
};

static char otm1287a_yitian_720p_video_on_cmd19[] = {
0x29, 0x00, 0x05, 0x80,
};

/*
static struct mipi_dsi_cmd otm1287a_yitian_720p_video_on_command[] = {


#if 1
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd0, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd1, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd2, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd3, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd4, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd5, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd6, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd7, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd8, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd9, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd10, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd11, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd12, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd13, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd14, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd15, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd16, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd17, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd18, 0),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_on_cmd19, 0),
#endif 
};

#define OTM1287A_YITIAN_720P_VIDEO_ON_COMMAND ARRAY_SIZE(otm1287a_yitian_720p_video_on_command)*/



static struct mipi_dsi_cmd truly_1080p_video_on_command[] = {
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd0 ), otm1287a_yitian_720p_video_on_cmd0  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd1 ), otm1287a_yitian_720p_video_on_cmd1  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd2 ), otm1287a_yitian_720p_video_on_cmd2  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd3 ), otm1287a_yitian_720p_video_on_cmd3  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd4 ), otm1287a_yitian_720p_video_on_cmd4  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd5 ), otm1287a_yitian_720p_video_on_cmd5  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd6 ), otm1287a_yitian_720p_video_on_cmd6  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd7 ), otm1287a_yitian_720p_video_on_cmd7  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd8 ), otm1287a_yitian_720p_video_on_cmd8  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd9 ) ,otm1287a_yitian_720p_video_on_cmd9  ,  	0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd10), otm1287a_yitian_720p_video_on_cmd10,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd11) ,otm1287a_yitian_720p_video_on_cmd11,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd12) ,otm1287a_yitian_720p_video_on_cmd12,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd13) ,otm1287a_yitian_720p_video_on_cmd13,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd14), otm1287a_yitian_720p_video_on_cmd14,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd15) ,otm1287a_yitian_720p_video_on_cmd15,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd16) ,otm1287a_yitian_720p_video_on_cmd16,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd17), otm1287a_yitian_720p_video_on_cmd17,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd18) ,otm1287a_yitian_720p_video_on_cmd18,  0x00},
{       ARRAY_SIZE(otm1287a_yitian_720p_video_on_cmd19) ,otm1287a_yitian_720p_video_on_cmd19,  0x00},
};
#define TRULY_1080P_VIDEO_ON_COMMAND 20


#if 0
static char otm1287a_yitian_720p_video_off_cmd0[] = {
0x28, 0x00, 0x05, 0x80,
};

static char otm1287a_yitian_720p_video_off_cmd1[] = {
0x10, 0x00, 0x05, 0x80,
};
static struct mipi_dsi_cmd otm1287a_yitian_720p_video_off_command[] = {
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_off_cmd0, 50),
MIPI_DSI_CMD_WAIT(otm1287a_yitian_720p_video_off_cmd1, 120),
};
#endif 
static char truly_1080p_video_off_cmd0[] = {
0x28, 0x00, 0x05, 0x80,
/*0x28, 0x00, 0x05, 0x80 */};


static char truly_1080p_video_off_cmd1[] = {
/*0x10, 0x00, 0x05, 0x80*/ 
0x10, 0x00, 0x05, 0x80,};


static struct mipi_dsi_cmd truly_1080p_video_off_command[] = {
{ 0x4 , truly_1080p_video_off_cmd0, 0x0},
{ 0x4 , truly_1080p_video_off_cmd1, 0x0}
};
#define TRULY_1080P_VIDEO_OFF_COMMAND 2


static struct command_state truly_1080p_video_state = {
  0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/

static struct commandpanel_info truly_1080p_video_command_panel = {
/*  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0*/
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0      
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/

static struct videopanel_info truly_1080p_video_video_panel = {
/*  0, 0, 0, 0, 1, 1, 1, 0, 0x9*/
    1, 0, 0, 0, 1, 1, 2, 0, 0x9       
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/

static struct lane_configuration truly_1080p_video_lane_config = {
  4, 0, 1, 1, 1, 1, 0
};


/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/
const uint32_t truly_1080p_video_timings[] = {
 /* 0xE6, 0x38, 0x26, 0x00, 0x68, 0x6e, 0x2A, 0x3c, 0x44, 0x03, 0x04, 0x00*/
   0x79 ,0x1a ,0x12 ,0x00 ,0x3e ,0x42 ,0x16 ,0x1e ,0x15 ,0x03 ,0x04 ,0x00 
};

static const uint32_t truly_1080p_14nm_video_timings[] = {
	0x23, 0x1e, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1a, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
};

static struct panel_timing truly_1080p_video_timing_info = {
  /*0, 4, 0x02, 0x2d*/
      0, 4, 0x04, 0x1b         
};

static struct panel_reset_sequence truly_1080p_video_panel_reset_seq = {
/*{ 1, 0, 1, }, { 200, 200, 200, }, 2*/
  { 1, 0, 1, }, { 20, 1, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Dynamic fps supported frequencies by panel                                */
/*---------------------------------------------------------------------------*/
static const struct dfps_panel_info truly_1080p_video_dfps = {
	1, 13, {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60}
};

/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/

static struct backlight truly_1080p_video_backlight = {
  1, 1, 4095, 100, 1, "PMIC_8941"
};

#define TRULY_1080P_VIDEO_SIGNATURE 0xFFFF

#endif /*_TRULY_1080P_VIDEO_H_*/
