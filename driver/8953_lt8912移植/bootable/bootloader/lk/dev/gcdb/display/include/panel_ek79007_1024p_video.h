//#ifndef _PANEL_ILI7807D_1080P_VIDEO_H_
#ifndef _PANEL_EK79007_1024P_VIDEO_H_

#define _PANEL_EK79007_1024P_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/

static struct panel_config 	EK79007_1024p_video_panel_data = {
  ///"qcom,mdss_dsi_ili7807d_1080p_video", "dsi:0:", "qcom,mdss-dsi-panel",
  "qcom,mdss_dsi_ek79007_1024p_video", "dsi:0:", "qcom,mdss-dsi-panel",
  10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution EK79007_1024p_video_panel_res = {
	1024, 600, 156, 114, 20, 0, 12, 20, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/

static struct color_info EK79007_1024p_video_color = {
  24, 0, 0xff, 0, 0, 0
};   


/*
static char ek79007_1024p_video_on_cmd0[] = {
	//0x04, 0x00, 0x29, 0xC0,
	//0xB9, 0xFF, 0x83, 0x94,
};


static char ek79007_1024p_video_on_cmd1[] = {
	//0x29, 0x00, 0x05, 0x80
};
*/
 

static struct mipi_dsi_cmd EK79007_1024p_video_on_command[]={
/*
	//{0x8, ek79007_1024p_video_on_cmd0, 0x00},
	//{0x4, ek79007_1024p_video_on_cmd1, 0x14}
*/

};

#define EK79007_1024P_VIDEO_ON_COMMAND  0

/* static char EK79007_1024p_video_off_cmd0[] = {
	//0x28, 0x00, 0x05, 0x80
};

static char EK79007_1024P_video_off_cmd1[] = {
	//0x10, 0x00, 0x05, 0x80
};
*/

static struct mipi_dsi_cmd EK79007_1024p_video_off_command[] = {
/*
	//{0x4, EK79007_1024P_video_off_cmd0, 0x32},
	//{0x4, EK79007_1024P_video_off_cmd1, 0x78}
*/
};

#define EK79007_1024P_VIDEO_OFF_COMMAND 0




static struct command_state EK79007_1024p_video_state = {
  0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/


static struct commandpanel_info EK79007_1024p_video_command_panel = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/


static struct videopanel_info EK79007_1024p_video_video_panel = {
	//1, 0, 0, 0, 1, 1, 2, 0, 0x9
	0, 0, 0, 0, 1, 1, 1, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/


static struct lane_configuration EK79007_1024p_video_lane_config = {

	4, 0, 1, 1, 1, 1, 0
};


/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/

const uint32_t EK79007_1024p_video_timings[] = {
	0x6A, 0x16, 0x1E, 0x00, 0x38, 0x3A, 0x12, 0x18, 0x10, 0x03, 0x04, 0x00

};

static const uint32_t EK79007_1024p_14nm_video_timings[] = {
	0x1d, 0x19, 0x04, 0x05, 0x01, 0x02, 0x04, 0xa0,
	0x1d, 0x19, 0x04, 0x05, 0x01, 0x02, 0x04, 0xa0,
	0x1d, 0x19, 0x04, 0x05, 0x01, 0x02, 0x04, 0xa0,
	0x1d, 0x19, 0x04, 0x05, 0x01, 0x02, 0x04, 0xa0,
	0x1d, 0x0a, 0x03, 0x04, 0x01, 0x02, 0x04, 0xa0
};


static struct panel_timing EK79007_1024p_video_timing_info = {
	// 0, 4, 3, 41
	0, 4, 0x05, 0x22
};


static struct panel_reset_sequence EK79007_1024p_video_panel_reset_seq = {
{ 1, 0, 1, }, { 20, 20, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Dynamic fps supported frequencies by panel                                */
/*---------------------------------------------------------------------------*/
static const struct dfps_panel_info EK79007_1024p_video_dfps = {
        1, 13, {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60}
};



/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/


static struct backlight EK79007_1024p_video_backlight = {
  1, 1, 4095, 100, 1, "PMIC_8941"
};


#define EK79007_1024P_VIDEO_SIGNATURE 0xFFFF

#endif /*_TRULY_1080P_VIDEO_H_*/
