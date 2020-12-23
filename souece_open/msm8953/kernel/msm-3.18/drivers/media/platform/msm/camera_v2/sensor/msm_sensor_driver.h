/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef MSM_SENSOR_DRIVER_H
#define MSM_SENSOR_DRIVER_H

#include "msm_sensor.h"
//int cmr_rs_flag = 0;
//int cmr_rs_start = 0;
void cam_work_func(struct work_struct *work);
int32_t msm_sensor_driver_probe(void *setting,
	struct msm_sensor_info_t *probed_info, char *entity_name);
//lihy ahd camera 20190225
int32_t msm_sensor_get_dev_info(void *setting,char *entity_name);
void set_camrs_flag(int value);
int get_camrs_flag(void);
void set_camrs_run(int value);
void set_detect_flag(int detect);
int get_detect_flag(void);

#endif
