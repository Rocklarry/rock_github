/* Copyright (c) 2013-2016, The Linux Foundation. All rights reserved.
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

#define SENSOR_DRIVER_I2C "i2c_camera"
/* Header file declaration */
#include "msm_sensor.h"
#include "msm_sd.h"
#include "camera.h"
#include "msm_cci.h"
#include "msm_camera_dt_util.h"
#include "../msm.h"
#include "../../../../../pinctrl/qcom/pinctrl-msm.h"
#include <linux/io.h>
#include <linux/module.h>
extern void __iomem *base_iomem(void);
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
//A: wming for camera out detect @{
#define SENSOR_PLUGOUT_DETECT
#ifdef SENSOR_PLUGOUT_DETECT
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#endif
//@}
/* Logging macro */
#undef CDBG
#define CDBG(fmt, args...) pr_debug(fmt, ##args)

#include <linux/syscalls.h>
#include <linux/reboot.h>

#define SENSOR_MAX_MOUNTANGLE (360)
//A: wming for camera out detect @{
#ifdef SENSOR_PLUGOUT_DETECT
static struct task_struct *cameraout_detect_task;
static DEFINE_MUTEX(cameraout_detect_mutex);

static struct input_dev *detect_input;
static uint16_t last_plug_status = 0;
static uint16_t cur_plug_status = 0;
static bool not_prepare = TRUE;
#endif
//@}
static int normal_status = 1;
static int cmr_rs_flag = 0;
static int cmr_rs_start = 0;
static int cmr_rs_hpn = 0;

static struct v4l2_file_operations msm_sensor_v4l2_subdev_fops;
static int32_t msm_sensor_driver_platform_probe(struct platform_device *pdev);
static int detect_flag = 0;

static struct msm_camera_i2c_client *sensor_i2c_client_plug = NULL;

void set_detect_flag(int detect){
	pr_err("stephen set set_detect_flag detect:%d\n",detect);
	detect_flag = detect;
}

int get_detect_flag(void){
	pr_err("stephen get get_detect_flag detect:%d\n",detect_flag);
	return detect_flag;
}



void set_camrs_run(int value){
	pr_err("stephen set cmr_rs_hpn:%d\n",value);
	cmr_rs_hpn = value;
}


void set_camrs_flag(int value){
	pr_err("stephen set cmr_rs_flag:%d\n",value);
	cmr_rs_flag = value;
}

int get_camrs_flag(void){
	pr_err("stephen get cmr_rs_flag:%d\n",cmr_rs_flag);
	return cmr_rs_flag;
}


/* Static declaration */
static struct msm_sensor_ctrl_t *g_sctrl[MAX_CAMERAS];

static int msm_sensor_platform_remove(struct platform_device *pdev)
{
	struct msm_sensor_ctrl_t  *s_ctrl;

	pr_err("%s: sensor FREE\n", __func__);

	s_ctrl = g_sctrl[pdev->id];
	if (!s_ctrl) {
		pr_err("%s: sensor device is NULL\n", __func__);
		return 0;
	}

	msm_sensor_free_sensor_data(s_ctrl);
	kfree(s_ctrl->msm_sensor_mutex);
	kfree(s_ctrl->sensor_i2c_client);
	kfree(s_ctrl);
	g_sctrl[pdev->id] = NULL;

	return 0;
}


static const struct of_device_id msm_sensor_driver_dt_match[] = {
	{.compatible = "qcom,camera"},
	{}
};

MODULE_DEVICE_TABLE(of, msm_sensor_driver_dt_match);

static struct platform_driver msm_sensor_platform_driver = {
	.probe = msm_sensor_driver_platform_probe,
	.driver = {
		.name = "qcom,camera",
		.owner = THIS_MODULE,
		.of_match_table = msm_sensor_driver_dt_match,
	},
	.remove = msm_sensor_platform_remove,
};

static struct v4l2_subdev_info msm_sensor_driver_subdev_info[] = {
	{
		.code = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt = 1,
		.order = 0,
	},
};

static int32_t msm_sensor_driver_create_i2c_v4l_subdev
			(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint32_t session_id = 0;
	struct i2c_client *client = s_ctrl->sensor_i2c_client->client;

	CDBG("%s %s I2c probe succeeded\n", __func__, client->name);
	rc = camera_init_v4l2(&client->dev, &session_id);
	if (rc < 0) {
		pr_err("failed: camera_init_i2c_v4l2 rc %d", rc);
		return rc;
	}
	CDBG("%s rc %d session_id %d\n", __func__, rc, session_id);
	snprintf(s_ctrl->msm_sd.sd.name,
		sizeof(s_ctrl->msm_sd.sd.name), "%s",
		s_ctrl->sensordata->sensor_name);
	v4l2_i2c_subdev_init(&s_ctrl->msm_sd.sd, client,
		s_ctrl->sensor_v4l2_subdev_ops);
	v4l2_set_subdevdata(&s_ctrl->msm_sd.sd, client);
	s_ctrl->msm_sd.sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	media_entity_init(&s_ctrl->msm_sd.sd.entity, 0, NULL, 0);
	s_ctrl->msm_sd.sd.entity.type = MEDIA_ENT_T_V4L2_SUBDEV;
	s_ctrl->msm_sd.sd.entity.group_id = MSM_CAMERA_SUBDEV_SENSOR;
	s_ctrl->msm_sd.sd.entity.name =	s_ctrl->msm_sd.sd.name;
	s_ctrl->sensordata->sensor_info->session_id = session_id;
	s_ctrl->msm_sd.close_seq = MSM_SD_CLOSE_2ND_CATEGORY | 0x3;
	msm_sd_register(&s_ctrl->msm_sd);
	msm_sensor_v4l2_subdev_fops = v4l2_subdev_fops;
#ifdef CONFIG_COMPAT
	msm_sensor_v4l2_subdev_fops.compat_ioctl32 =
		msm_sensor_subdev_fops_ioctl;
#endif
	s_ctrl->msm_sd.sd.devnode->fops =
		&msm_sensor_v4l2_subdev_fops;
	CDBG("%s:%d\n", __func__, __LINE__);
	return rc;
}

static int32_t msm_sensor_driver_create_v4l_subdev
			(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint32_t session_id = 0;

	rc = camera_init_v4l2(&s_ctrl->pdev->dev, &session_id);
	if (rc < 0) {
		pr_err("failed: camera_init_v4l2 rc %d", rc);
		return rc;
	}
	CDBG("rc %d session_id %d", rc, session_id);
	s_ctrl->sensordata->sensor_info->session_id = session_id;

	/* Create /dev/v4l-subdevX device */
	v4l2_subdev_init(&s_ctrl->msm_sd.sd, s_ctrl->sensor_v4l2_subdev_ops);
	snprintf(s_ctrl->msm_sd.sd.name, sizeof(s_ctrl->msm_sd.sd.name), "%s",
		s_ctrl->sensordata->sensor_name);
	v4l2_set_subdevdata(&s_ctrl->msm_sd.sd, s_ctrl->pdev);
	s_ctrl->msm_sd.sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	media_entity_init(&s_ctrl->msm_sd.sd.entity, 0, NULL, 0);
	s_ctrl->msm_sd.sd.entity.type = MEDIA_ENT_T_V4L2_SUBDEV;
	s_ctrl->msm_sd.sd.entity.group_id = MSM_CAMERA_SUBDEV_SENSOR;
	s_ctrl->msm_sd.sd.entity.name = s_ctrl->msm_sd.sd.name;
	s_ctrl->msm_sd.close_seq = MSM_SD_CLOSE_2ND_CATEGORY | 0x3;
	msm_sd_register(&s_ctrl->msm_sd);
	msm_cam_copy_v4l2_subdev_fops(&msm_sensor_v4l2_subdev_fops);
#ifdef CONFIG_COMPAT
	msm_sensor_v4l2_subdev_fops.compat_ioctl32 =
		msm_sensor_subdev_fops_ioctl;
#endif
	s_ctrl->msm_sd.sd.devnode->fops =
		&msm_sensor_v4l2_subdev_fops;

	return rc;
}

static int32_t msm_sensor_fill_eeprom_subdevid_by_name(
				struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	const char *eeprom_name;
	struct device_node *src_node = NULL;
	uint32_t val = 0, eeprom_name_len;
	int32_t *eeprom_subdev_id, i, userspace_probe = 0;
	int32_t count = 0;
	struct  msm_sensor_info_t *sensor_info;
	struct device_node *of_node = s_ctrl->of_node;
	const void *p;

	if (!s_ctrl->sensordata->eeprom_name || !of_node)
		return -EINVAL;

	eeprom_name_len = strlen(s_ctrl->sensordata->eeprom_name);
	if (eeprom_name_len >= MAX_SENSOR_NAME)
		return -EINVAL;

	sensor_info = s_ctrl->sensordata->sensor_info;
	eeprom_subdev_id = &sensor_info->subdev_id[SUB_MODULE_EEPROM];
	/*
	 * string for eeprom name is valid, set sudev id to -1
	 *  and try to found new id
	 */
	*eeprom_subdev_id = -1;

	if (0 == eeprom_name_len)
		return 0;

	p = of_get_property(of_node, "qcom,eeprom-src", &count);
	if (!p || !count)
		return 0;

	count /= sizeof(uint32_t);
	for (i = 0; i < count; i++) {
		userspace_probe = 0;
		eeprom_name = NULL;
		src_node = of_parse_phandle(of_node, "qcom,eeprom-src", i);
		if (!src_node) {
			pr_err("eeprom src node NULL\n");
			continue;
		}
		/* In the case of eeprom probe from kernel eeprom name
			should be present, Otherwise it will throw as errors */
		rc = of_property_read_string(src_node, "qcom,eeprom-name",
			&eeprom_name);
		if (rc < 0) {
			pr_err("%s:%d Eeprom userspace probe for %s\n",
				__func__, __LINE__,
				s_ctrl->sensordata->eeprom_name);
			of_node_put(src_node);
			userspace_probe = 1;
			if (count > 1)
				return -EINVAL;
		}
		if (!userspace_probe &&
			strcmp(eeprom_name, s_ctrl->sensordata->eeprom_name))
			continue;

		rc = of_property_read_u32(src_node, "cell-index", &val);
		if (rc < 0) {
			pr_err("%s qcom,eeprom cell index %d, rc %d\n",
				__func__, val, rc);
			of_node_put(src_node);
			if (userspace_probe)
				return -EINVAL;
			continue;
		}

		*eeprom_subdev_id = val;
		CDBG("%s:%d Eeprom subdevice id is %d\n",
			__func__, __LINE__, val);
		of_node_put(src_node);
		src_node = NULL;
		break;
	}

	return rc;
}

static int32_t msm_sensor_fill_actuator_subdevid_by_name(
				struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct device_node *src_node = NULL;
	uint32_t val = 0, actuator_name_len;
	int32_t *actuator_subdev_id;
	struct  msm_sensor_info_t *sensor_info;
	struct device_node *of_node = s_ctrl->of_node;

	if (!s_ctrl->sensordata->actuator_name || !of_node)
		return -EINVAL;

	actuator_name_len = strlen(s_ctrl->sensordata->actuator_name);
	if (actuator_name_len >= MAX_SENSOR_NAME)
		return -EINVAL;

	sensor_info = s_ctrl->sensordata->sensor_info;
	actuator_subdev_id = &sensor_info->subdev_id[SUB_MODULE_ACTUATOR];
	/*
	 * string for actuator name is valid, set sudev id to -1
	 * and try to found new id
	 */
	*actuator_subdev_id = -1;

	if (0 == actuator_name_len)
		return 0;

	src_node = of_parse_phandle(of_node, "qcom,actuator-src", 0);
	if (!src_node) {
		CDBG("%s:%d src_node NULL\n", __func__, __LINE__);
	} else {
		rc = of_property_read_u32(src_node, "cell-index", &val);
		CDBG("%s qcom,actuator cell index %d, rc %d\n", __func__,
			val, rc);
		if (rc < 0) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			return -EINVAL;
		}
		*actuator_subdev_id = val;
		of_node_put(src_node);
		src_node = NULL;
	}

	return rc;
}

static int32_t msm_sensor_fill_ois_subdevid_by_name(
				struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct device_node *src_node = NULL;
	uint32_t val = 0, ois_name_len;
	int32_t *ois_subdev_id;
	struct  msm_sensor_info_t *sensor_info;
	struct device_node *of_node = s_ctrl->of_node;

	if (!s_ctrl->sensordata->ois_name || !of_node)
		return -EINVAL;

	ois_name_len = strlen(s_ctrl->sensordata->ois_name);
	if (ois_name_len >= MAX_SENSOR_NAME)
		return -EINVAL;

	sensor_info = s_ctrl->sensordata->sensor_info;
	ois_subdev_id = &sensor_info->subdev_id[SUB_MODULE_OIS];
	/*
	 * string for ois name is valid, set sudev id to -1
	 * and try to found new id
	 */
	*ois_subdev_id = -1;

	if (0 == ois_name_len)
		return 0;

	src_node = of_parse_phandle(of_node, "qcom,ois-src", 0);
	if (!src_node) {
		CDBG("%s:%d src_node NULL\n", __func__, __LINE__);
	} else {
		rc = of_property_read_u32(src_node, "cell-index", &val);
		CDBG("%s qcom,ois cell index %d, rc %d\n", __func__,
			val, rc);
		if (rc < 0) {
			pr_err("%s failed %d\n", __func__, __LINE__);
			return -EINVAL;
		}
		*ois_subdev_id = val;
		of_node_put(src_node);
		src_node = NULL;
	}

	return rc;
}

static int32_t msm_sensor_fill_slave_info_init_params(
	struct msm_camera_sensor_slave_info *slave_info,
	struct msm_sensor_info_t *sensor_info)
{
	struct msm_sensor_init_params *sensor_init_params;
	if (!slave_info ||  !sensor_info)
		return -EINVAL;

	sensor_init_params = &slave_info->sensor_init_params;
	if (INVALID_CAMERA_B != sensor_init_params->position)
		sensor_info->position =
			sensor_init_params->position;

	if (SENSOR_MAX_MOUNTANGLE > sensor_init_params->sensor_mount_angle) {
		sensor_info->sensor_mount_angle =
			sensor_init_params->sensor_mount_angle;
		sensor_info->is_mount_angle_valid = 1;
	}

	if (CAMERA_MODE_INVALID != sensor_init_params->modes_supported)
		sensor_info->modes_supported =
			sensor_init_params->modes_supported;

	return 0;
}


static int32_t msm_sensor_validate_slave_info(
	struct msm_sensor_info_t *sensor_info)
{
	if (INVALID_CAMERA_B == sensor_info->position) {
		sensor_info->position = BACK_CAMERA_B;
		CDBG("%s:%d Set default sensor position\n",
			__func__, __LINE__);
	}
	if (CAMERA_MODE_INVALID == sensor_info->modes_supported) {
		sensor_info->modes_supported = CAMERA_MODE_2D_B;
		CDBG("%s:%d Set default sensor modes_supported\n",
			__func__, __LINE__);
	}
	if (SENSOR_MAX_MOUNTANGLE <= sensor_info->sensor_mount_angle) {
		sensor_info->sensor_mount_angle = 0;
		CDBG("%s:%d Set default sensor mount angle\n",
			__func__, __LINE__);
		sensor_info->is_mount_angle_valid = 1;
	}
	return 0;
}

#ifdef CONFIG_COMPAT
static int32_t msm_sensor_get_pw_settings_compat(
	struct msm_sensor_power_setting *ps,
	struct msm_sensor_power_setting *us_ps, uint32_t size)
{
	int32_t rc = 0, i = 0;
	struct msm_sensor_power_setting32 *ps32 =
		kzalloc(sizeof(*ps32) * size, GFP_KERNEL);

	if (!ps32) {
		pr_err("failed: no memory ps32");
		return -ENOMEM;
	}
	if (copy_from_user(ps32, (void *)us_ps, sizeof(*ps32) * size)) {
		pr_err("failed: copy_from_user");
		kfree(ps32);
		return -EFAULT;
	}
	for (i = 0; i < size; i++) {
		ps[i].config_val = ps32[i].config_val;
		ps[i].delay = ps32[i].delay;
		ps[i].seq_type = ps32[i].seq_type;
		ps[i].seq_val = ps32[i].seq_val;
	}
	kfree(ps32);
	return rc;
}
#endif

static int32_t msm_sensor_create_pd_settings(void *setting,
	struct msm_sensor_power_setting *pd, uint32_t size_down,
	struct msm_sensor_power_setting *pu)
{
	int32_t rc = 0;
	int c, end;
	struct msm_sensor_power_setting pd_tmp;

	pr_err("Generating power_down_setting");

#ifdef CONFIG_COMPAT
	if (is_compat_task()) {
		int i = 0;
		struct msm_sensor_power_setting32 *power_setting_iter =
		(struct msm_sensor_power_setting32 *)compat_ptr((
		(struct msm_camera_sensor_slave_info32 *)setting)->
		power_setting_array.power_setting);

		for (i = 0; i < size_down; i++) {
			pd[i].config_val = power_setting_iter[i].config_val;
			pd[i].delay = power_setting_iter[i].delay;
			pd[i].seq_type = power_setting_iter[i].seq_type;
			pd[i].seq_val = power_setting_iter[i].seq_val;
		}
	} else
#endif
	{
		if (copy_from_user(pd, (void *)pu, sizeof(*pd) * size_down)) {
			pr_err("failed: copy_from_user");
			return -EFAULT;
		}
	}
	/* reverse */
	end = size_down - 1;
	for (c = 0; c < size_down/2; c++) {
		pd_tmp = pd[c];
		pd[c] = pd[end];
		pd[end] = pd_tmp;
		end--;
	}
	return rc;
}

static int32_t msm_sensor_get_power_down_settings(void *setting,
	struct msm_camera_sensor_slave_info *slave_info,
	struct msm_camera_power_ctrl_t *power_info)
{
	int32_t rc = 0;
	uint16_t size_down = 0;
	uint16_t i = 0;
	struct msm_sensor_power_setting *pd = NULL;

	/* DOWN */
	size_down = slave_info->power_setting_array.size_down;
	if (!size_down || size_down > MAX_POWER_CONFIG)
		size_down = slave_info->power_setting_array.size;
	/* Validate size_down */
	if (size_down > MAX_POWER_CONFIG) {
		pr_err("failed: invalid size_down %d", size_down);
		return -EINVAL;
	}
	/* Allocate memory for power down setting */
	pd = kzalloc(sizeof(*pd) * size_down, GFP_KERNEL);
	if (!pd)
		return -EFAULT;

	if (slave_info->power_setting_array.power_down_setting) {
#ifdef CONFIG_COMPAT
		if (is_compat_task()) {
			rc = msm_sensor_get_pw_settings_compat(
				pd, slave_info->power_setting_array.
				power_down_setting, size_down);
			if (rc < 0) {
				pr_err("failed");
				kfree(pd);
				return -EFAULT;
			}
		} else
#endif
		if (copy_from_user(pd, (void *)slave_info->power_setting_array.
				power_down_setting, sizeof(*pd) * size_down)) {
			pr_err("failed: copy_from_user");
			kfree(pd);
			return -EFAULT;
		}
	} else {

		rc = msm_sensor_create_pd_settings(setting, pd, size_down,
			slave_info->power_setting_array.power_setting);
		if (rc < 0) {
			pr_err("failed");
			kfree(pd);
			return -EFAULT;
		}
	}

	/* Fill power down setting and power down setting size */
	power_info->power_down_setting = pd;
	power_info->power_down_setting_size = size_down;

	/* Print power setting */
	for (i = 0; i < size_down; i++) {
		CDBG("DOWN seq_type %d seq_val %d config_val %ld delay %d",
			pd[i].seq_type, pd[i].seq_val,
			pd[i].config_val, pd[i].delay);
	}
	return rc;
}

static int32_t msm_sensor_get_power_up_settings(void *setting,
	struct msm_camera_sensor_slave_info *slave_info,
	struct msm_camera_power_ctrl_t *power_info)
{
	int32_t rc = 0;
	uint16_t size = 0;
	uint16_t i = 0;
	struct msm_sensor_power_setting *pu = NULL;

	size = slave_info->power_setting_array.size;

	/* Validate size */
	if ((size == 0) || (size > MAX_POWER_CONFIG)) {
		pr_err("failed: invalid power_setting size_up = %d\n", size);
		return -EINVAL;
	}

	/* Allocate memory for power up setting */
	pu = kzalloc(sizeof(*pu) * size, GFP_KERNEL);
	if (!pu)
		return -ENOMEM;

#ifdef CONFIG_COMPAT
	if (is_compat_task()) {
		rc = msm_sensor_get_pw_settings_compat(pu,
			slave_info->power_setting_array.
				power_setting, size);
		if (rc < 0) {
			pr_err("failed");
			kfree(pu);
			return -EFAULT;
		}
	} else
#endif
	{
		if (copy_from_user(pu,
			(void *)slave_info->power_setting_array.power_setting,
			sizeof(*pu) * size)) {
			pr_err("failed: copy_from_user");
			kfree(pu);
			return -EFAULT;
		}
	}

	/* Print power setting */
	for (i = 0; i < size; i++) {
		CDBG("UP seq_type %d seq_val %d config_val %ld delay %d",
			pu[i].seq_type, pu[i].seq_val,
			pu[i].config_val, pu[i].delay);
	}


	/* Fill power up setting and power up setting size */
	power_info->power_setting = pu;
	power_info->power_setting_size = size;

	return rc;
}

static int32_t msm_sensor_get_power_settings(void *setting,
	struct msm_camera_sensor_slave_info *slave_info,
	struct msm_camera_power_ctrl_t *power_info)
{
	int32_t rc = 0;

	rc = msm_sensor_get_power_up_settings(setting, slave_info, power_info);
	if (rc < 0) {
		pr_err("failed");
		return -EINVAL;
	}

	rc = msm_sensor_get_power_down_settings(setting, slave_info,
		power_info);
	if (rc < 0) {
		pr_err("failed");
		return -EINVAL;
	}
	return rc;
}

static void msm_sensor_fill_sensor_info(struct msm_sensor_ctrl_t *s_ctrl,
	struct msm_sensor_info_t *sensor_info, char *entity_name)
{
	uint32_t i;

	if (!s_ctrl || !sensor_info) {
		pr_err("%s:failed\n", __func__);
		return;
	}

	strlcpy(sensor_info->sensor_name, s_ctrl->sensordata->sensor_name,
		MAX_SENSOR_NAME);

	sensor_info->session_id = s_ctrl->sensordata->sensor_info->session_id;

	s_ctrl->sensordata->sensor_info->subdev_id[SUB_MODULE_SENSOR] =
		s_ctrl->sensordata->sensor_info->session_id;
	for (i = 0; i < SUB_MODULE_MAX; i++) {
		sensor_info->subdev_id[i] =
			s_ctrl->sensordata->sensor_info->subdev_id[i];
		sensor_info->subdev_intf[i] =
			s_ctrl->sensordata->sensor_info->subdev_intf[i];
	}

	sensor_info->is_mount_angle_valid =
		s_ctrl->sensordata->sensor_info->is_mount_angle_valid;
	sensor_info->sensor_mount_angle =
		s_ctrl->sensordata->sensor_info->sensor_mount_angle;
	sensor_info->modes_supported =
		s_ctrl->sensordata->sensor_info->modes_supported;
	sensor_info->position =
		s_ctrl->sensordata->sensor_info->position;

	strlcpy(entity_name, s_ctrl->msm_sd.sd.entity.name, MAX_SENSOR_NAME);
}
//lihy ahd camera 20190225
int32_t msm_sensor_get_dev_info(void *setting,char *entity_name){
	int32_t                              rc = 0;
	struct msm_sensor_ctrl_t            *s_ctrl = NULL;
	struct msm_camera_sensor_slave_info *slave_info = NULL;
	int i;

	if (!setting) {
		pr_err("failed: slave_info %pK", setting);
		return -EINVAL;
	}

       slave_info = kzalloc(sizeof(*slave_info), GFP_KERNEL);
        if (!slave_info)
           return -ENOMEM;
#ifdef CONFIG_COMPAT
	if (is_compat_task()) {
		struct msm_camera_sensor_slave_info32 *slave_info32 =
			kzalloc(sizeof(*slave_info32), GFP_KERNEL);
		if (!slave_info32) {
			pr_err("failed: no memory for slave_info32 %pK\n",
				slave_info32);
			rc = -ENOMEM;
			goto free_slave_info;
		}
		if (copy_from_user((void *)slave_info32, setting,
			sizeof(*slave_info32))) {
				pr_err("failed: copy_from_user");
				rc = -EFAULT;
				kfree(slave_info32);
				goto free_slave_info;
			}

		strlcpy(slave_info->sensor_name, slave_info32->sensor_name,
			sizeof(slave_info->sensor_name));

		//slave_info->slave_addr = slave_info32->slave_addr;
		kfree(slave_info32);
	} else
#endif
	{
		if (copy_from_user(slave_info,
					(void *)setting, sizeof(*slave_info))) {
			pr_err("failed: copy_from_user");
			rc = -EFAULT;
			goto free_slave_info;
		}
	}
        for(i = 0; i < MAX_CAMERAS; i++){
            s_ctrl = g_sctrl[i];
            if (!(strcmp(slave_info->sensor_name, s_ctrl->sensordata->cam_slave_info->sensor_name))) {
                strlcpy(entity_name, s_ctrl->msm_sd.sd.entity.name, MAX_SENSOR_NAME);
                break;
            }
        }

    free_slave_info:
	kfree(slave_info);
	return rc;
}
//end

/* static function definition */
int32_t msm_sensor_driver_probe(void *setting,
	struct msm_sensor_info_t *probed_info, char *entity_name)
{
	int32_t                              rc = 0;
	struct msm_sensor_ctrl_t            *s_ctrl = NULL;
	struct msm_camera_cci_client        *cci_client = NULL;
	struct msm_camera_sensor_slave_info *slave_info = NULL;
	struct msm_camera_slave_info        *camera_info = NULL;

	unsigned long                        mount_pos = 0;
	uint32_t                             is_yuv;

	/* Validate input parameters */
	if (!setting) {
		pr_err("failed: slave_info %pK", setting);
		return -EINVAL;
	}

	/* Allocate memory for slave info */
	slave_info = kzalloc(sizeof(*slave_info), GFP_KERNEL);
	if (!slave_info)
		return -ENOMEM;
#ifdef CONFIG_COMPAT
	if (is_compat_task()) {
		struct msm_camera_sensor_slave_info32 *slave_info32 =
			kzalloc(sizeof(*slave_info32), GFP_KERNEL);
		if (!slave_info32) {
			pr_err("failed: no memory for slave_info32 %pK\n",
				slave_info32);
			rc = -ENOMEM;
			goto free_slave_info;
		}
		if (copy_from_user((void *)slave_info32, setting,
			sizeof(*slave_info32))) {
				pr_err("failed: copy_from_user");
				rc = -EFAULT;
				kfree(slave_info32);
				goto free_slave_info;
			}

		strlcpy(slave_info->actuator_name, slave_info32->actuator_name,
			sizeof(slave_info->actuator_name));

		strlcpy(slave_info->eeprom_name, slave_info32->eeprom_name,
			sizeof(slave_info->eeprom_name));

		strlcpy(slave_info->sensor_name, slave_info32->sensor_name,
			sizeof(slave_info->sensor_name));

		strlcpy(slave_info->ois_name, slave_info32->ois_name,
			sizeof(slave_info->ois_name));

		strlcpy(slave_info->flash_name, slave_info32->flash_name,
			sizeof(slave_info->flash_name));

		slave_info->addr_type = slave_info32->addr_type;
		slave_info->camera_id = slave_info32->camera_id;

		slave_info->i2c_freq_mode = slave_info32->i2c_freq_mode;
		slave_info->sensor_id_info = slave_info32->sensor_id_info;

		slave_info->slave_addr = slave_info32->slave_addr;
		slave_info->power_setting_array.size =
			slave_info32->power_setting_array.size;
		slave_info->power_setting_array.size_down =
			slave_info32->power_setting_array.size_down;
		// lihy_ahd
		//slave_info->power_setting_array.size_down =
		//	slave_info32->power_setting_array.size_down;
		slave_info->power_setting_array.power_setting =
			compat_ptr(slave_info32->
				power_setting_array.power_setting);
		slave_info->power_setting_array.power_down_setting =
			compat_ptr(slave_info32->
				power_setting_array.power_down_setting);
		slave_info->sensor_init_params =
			slave_info32->sensor_init_params;
		slave_info->output_format =
			slave_info32->output_format;
		kfree(slave_info32);
	} else
#endif
	{
		if (copy_from_user(slave_info,
					(void *)setting, sizeof(*slave_info))) {
			pr_err("failed: copy_from_user");
			rc = -EFAULT;
			goto free_slave_info;
		}
	}

	/* Print slave info */
	CDBG("camera id %d Slave addr 0x%X addr_type %d\n",
		slave_info->camera_id, slave_info->slave_addr,
		slave_info->addr_type);
	CDBG("sensor_id_reg_addr 0x%X sensor_id 0x%X sensor id mask %d",
		slave_info->sensor_id_info.sensor_id_reg_addr,
		slave_info->sensor_id_info.sensor_id,
		slave_info->sensor_id_info.sensor_id_mask);
	CDBG("power up size %d power down size %d\n",
		slave_info->power_setting_array.size,
		slave_info->power_setting_array.size_down);
	CDBG("position %d",
		slave_info->sensor_init_params.position);
	CDBG("mount %d",
		slave_info->sensor_init_params.sensor_mount_angle);

	/* Validate camera id */
	if (slave_info->camera_id >= MAX_CAMERAS) {
		pr_err("failed: invalid camera id %d max %d",
			slave_info->camera_id, MAX_CAMERAS);
		rc = -EINVAL;
		goto free_slave_info;
	}

	/* Extract s_ctrl from camera id */
	s_ctrl = g_sctrl[slave_info->camera_id];
	if (!s_ctrl) {
		pr_err("failed: s_ctrl %pK for camera_id %d", s_ctrl,
			slave_info->camera_id);
		rc = -EINVAL;
		goto free_slave_info;
	}

	CDBG("s_ctrl[%d] %pK", slave_info->camera_id, s_ctrl);

	if (s_ctrl->is_probe_succeed == 1) {
	//if (0) {//modify stephen
		/*
		 * Different sensor on this camera slot has been connected
		 * and probe already succeeded for that sensor. Ignore this
		 * probe
		 */
		if (slave_info->sensor_id_info.sensor_id ==
			s_ctrl->sensordata->cam_slave_info->
				sensor_id_info.sensor_id) {
			pr_err("slot%d: sensor id%d already probed\n",
				slave_info->camera_id,
				s_ctrl->sensordata->cam_slave_info->
					sensor_id_info.sensor_id);
			msm_sensor_fill_sensor_info(s_ctrl,
				probed_info, entity_name);
		} else
			pr_err("slot %d has some other sensor\n",
				slave_info->camera_id);

		rc = 0;
		goto free_slave_info;
	}

	if (slave_info->power_setting_array.size == 0 &&
		slave_info->slave_addr == 0) {
		s_ctrl->is_csid_tg_mode = 1;
		goto CSID_TG;
	}

	rc = msm_sensor_get_power_settings(setting, slave_info,
		&s_ctrl->sensordata->power_info);
	if (rc < 0) {
		pr_err("failed");
		goto free_slave_info;
	}


	camera_info = kzalloc(sizeof(struct msm_camera_slave_info), GFP_KERNEL);
	if (!camera_info)
		goto free_slave_info;

	s_ctrl->sensordata->slave_info = camera_info;

	/* Fill sensor slave info */
	camera_info->sensor_slave_addr = slave_info->slave_addr;
	camera_info->sensor_id_reg_addr =
		slave_info->sensor_id_info.sensor_id_reg_addr;
	camera_info->sensor_id = slave_info->sensor_id_info.sensor_id;
	camera_info->sensor_id_mask = slave_info->sensor_id_info.sensor_id_mask;

	/* Fill CCI master, slave address and CCI default params */
	if (!s_ctrl->sensor_i2c_client) {
		pr_err("failed: sensor_i2c_client %pK",
			s_ctrl->sensor_i2c_client);
		rc = -EINVAL;
		goto free_camera_info;
	}
	/* Fill sensor address type */
	s_ctrl->sensor_i2c_client->addr_type = slave_info->addr_type;
	if (s_ctrl->sensor_i2c_client->client)
		s_ctrl->sensor_i2c_client->client->addr =
			camera_info->sensor_slave_addr;

	cci_client = s_ctrl->sensor_i2c_client->cci_client;
	if (!cci_client) {
		pr_err("failed: cci_client %pK", cci_client);
		goto free_camera_info;
	}
	cci_client->cci_i2c_master = s_ctrl->cci_i2c_master;
	cci_client->sid = slave_info->slave_addr >> 1;
	cci_client->retries = 3;
	cci_client->id_map = 0;
	cci_client->i2c_freq_mode = slave_info->i2c_freq_mode;

	/* Parse and fill vreg params for powerup settings */
	rc = msm_camera_fill_vreg_params(
		s_ctrl->sensordata->power_info.cam_vreg,
		s_ctrl->sensordata->power_info.num_vreg,
		s_ctrl->sensordata->power_info.power_setting,
		s_ctrl->sensordata->power_info.power_setting_size);
	if (rc < 0) {
		pr_err("failed: msm_camera_get_dt_power_setting_data rc %d",
			rc);
		goto free_camera_info;
	}

	/* Parse and fill vreg params for powerdown settings*/
	rc = msm_camera_fill_vreg_params(
		s_ctrl->sensordata->power_info.cam_vreg,
		s_ctrl->sensordata->power_info.num_vreg,
		s_ctrl->sensordata->power_info.power_down_setting,
		s_ctrl->sensordata->power_info.power_down_setting_size);
	if (rc < 0) {
		pr_err("failed: msm_camera_fill_vreg_params for PDOWN rc %d",
			rc);
		goto free_camera_info;
	}

CSID_TG:
	/* Update sensor, actuator and eeprom name in
	*  sensor control structure */
	s_ctrl->sensordata->sensor_name = slave_info->sensor_name;
	s_ctrl->sensordata->eeprom_name = slave_info->eeprom_name;
	s_ctrl->sensordata->actuator_name = slave_info->actuator_name;
	s_ctrl->sensordata->ois_name = slave_info->ois_name;
	/*
	 * Update eeporm subdevice Id by input eeprom name
	 */
	rc = msm_sensor_fill_eeprom_subdevid_by_name(s_ctrl);
	if (rc < 0) {
		pr_err("%s failed %d\n", __func__, __LINE__);
		goto free_camera_info;
	}
	/*
	 * Update actuator subdevice Id by input actuator name
	 */
	rc = msm_sensor_fill_actuator_subdevid_by_name(s_ctrl);
	if (rc < 0) {
		pr_err("%s failed %d\n", __func__, __LINE__);
		goto free_camera_info;
	}

	rc = msm_sensor_fill_ois_subdevid_by_name(s_ctrl);
	if (rc < 0) {
		pr_err("%s failed %d\n", __func__, __LINE__);
		goto free_camera_info;
	}

	/* Power up and probe sensor */
	rc = s_ctrl->func_tbl->sensor_power_up(s_ctrl);
	if (rc < 0) {
		pr_err("%s power up failed", slave_info->sensor_name);
		goto free_camera_info;
	}

	pr_err("%s probe succeeded", slave_info->sensor_name);

	/*
	  Set probe succeeded flag to 1 so that no other camera shall
	 * probed on this slot
	 */
	s_ctrl->is_probe_succeed = 1;

	/*
	 * Update the subdevice id of flash-src based on availability in kernel.
	 */
	if (strlen(slave_info->flash_name) == 0) {
		s_ctrl->sensordata->sensor_info->
			subdev_id[SUB_MODULE_LED_FLASH] = -1;
	}

	/*
	 * Create /dev/videoX node, comment for now until dummy /dev/videoX
	 * node is created and used by HAL
	 */

	if (s_ctrl->sensor_device_type == MSM_CAMERA_PLATFORM_DEVICE)
		rc = msm_sensor_driver_create_v4l_subdev(s_ctrl);
	else
		rc = msm_sensor_driver_create_i2c_v4l_subdev(s_ctrl);
	if (rc < 0) {
		pr_err("failed: camera creat v4l2 rc %d", rc);
		goto camera_power_down;
	}

	/* Power down */
	s_ctrl->func_tbl->sensor_power_down(s_ctrl);

	rc = msm_sensor_fill_slave_info_init_params(
		slave_info,
		s_ctrl->sensordata->sensor_info);
	if (rc < 0) {
		pr_err("%s Fill slave info failed", slave_info->sensor_name);
		goto free_camera_info;
	}
	rc = msm_sensor_validate_slave_info(s_ctrl->sensordata->sensor_info);
	if (rc < 0) {
		pr_err("%s Validate slave info failed",
			slave_info->sensor_name);
		goto free_camera_info;
	}
	/* Update sensor mount angle and position in media entity flag */
	is_yuv = (slave_info->output_format == MSM_SENSOR_YCBCR) ? 1 : 0;
	mount_pos = is_yuv << 25 |
		(s_ctrl->sensordata->sensor_info->position << 16) |
		((s_ctrl->sensordata->
		sensor_info->sensor_mount_angle / 90) << 8);

	s_ctrl->msm_sd.sd.entity.flags = mount_pos | MEDIA_ENT_FL_DEFAULT;

	/*Save sensor info*/
	s_ctrl->sensordata->cam_slave_info = slave_info;

	msm_sensor_fill_sensor_info(s_ctrl, probed_info, entity_name);

	return rc;

camera_power_down:
	s_ctrl->func_tbl->sensor_power_down(s_ctrl);
free_camera_info:
	kfree(camera_info);
free_slave_info:
	kfree(slave_info);
	return rc;
}

static int32_t msm_sensor_driver_get_dt_data(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t                              rc = 0;
	struct msm_camera_sensor_board_info *sensordata = NULL;
	struct device_node                  *of_node = s_ctrl->of_node;
	uint32_t cell_id;

	s_ctrl->sensordata = kzalloc(sizeof(*sensordata), GFP_KERNEL);
	if (!s_ctrl->sensordata) {
		pr_err("failed: no memory");
		return -ENOMEM;
	}

	sensordata = s_ctrl->sensordata;

	/*
	 * Read cell index - this cell index will be the camera slot where
	 * this camera will be mounted
	 */
	rc = of_property_read_u32(of_node, "cell-index", &cell_id);
	if (rc < 0) {
		pr_err("failed: cell-index rc %d", rc);
		goto FREE_SENSOR_DATA;
	}
	s_ctrl->id = cell_id;

	/* Validate cell_id */
	if (cell_id >= MAX_CAMERAS) {
		pr_err("failed: invalid cell_id %d", cell_id);
		rc = -EINVAL;
		goto FREE_SENSOR_DATA;
	}

	/* Check whether g_sctrl is already filled for this cell_id */
	if (g_sctrl[cell_id]) {
		pr_err("failed: sctrl already filled for cell_id %d", cell_id);
		rc = -EINVAL;
		goto FREE_SENSOR_DATA;
	}

	/* Read subdev info */
	rc = msm_sensor_get_sub_module_index(of_node, &sensordata->sensor_info);
	if (rc < 0) {
		pr_err("failed");
		goto FREE_SENSOR_DATA;
	}

	/* Read vreg information */
	rc = msm_camera_get_dt_vreg_data(of_node,
		&sensordata->power_info.cam_vreg,
		&sensordata->power_info.num_vreg);
	if (rc < 0) {
		pr_err("failed: msm_camera_get_dt_vreg_data rc %d", rc);
		goto FREE_SUB_MODULE_DATA;
	}

	/* Read gpio information */
	rc = msm_sensor_driver_get_gpio_data
		(&(sensordata->power_info.gpio_conf), of_node);
	if (rc < 0) {
		pr_err("failed: msm_sensor_driver_get_gpio_data rc %d", rc);
		goto FREE_VREG_DATA;
	}

	/* Get CCI master */
	rc = of_property_read_u32(of_node, "qcom,cci-master",
		&s_ctrl->cci_i2c_master);
	CDBG("qcom,cci-master %d, rc %d", s_ctrl->cci_i2c_master, rc);
	if (rc < 0) {
		/* Set default master 0 */
		s_ctrl->cci_i2c_master = MASTER_0;
		rc = 0;
	}

	/* Get mount angle */
	if (0 > of_property_read_u32(of_node, "qcom,mount-angle",
		&sensordata->sensor_info->sensor_mount_angle)) {
		/* Invalidate mount angle flag */
		sensordata->sensor_info->is_mount_angle_valid = 0;
		sensordata->sensor_info->sensor_mount_angle = 0;
	} else {
		sensordata->sensor_info->is_mount_angle_valid = 1;
	}
	CDBG("%s qcom,mount-angle %d\n", __func__,
		sensordata->sensor_info->sensor_mount_angle);
	if (0 > of_property_read_u32(of_node, "qcom,sensor-position",
		&sensordata->sensor_info->position)) {
		CDBG("%s:%d Invalid sensor position\n", __func__, __LINE__);
		sensordata->sensor_info->position = INVALID_CAMERA_B;
	}
	if (0 > of_property_read_u32(of_node, "qcom,sensor-mode",
		&sensordata->sensor_info->modes_supported)) {
		CDBG("%s:%d Invalid sensor mode supported\n",
			__func__, __LINE__);
		sensordata->sensor_info->modes_supported = CAMERA_MODE_INVALID;
	}
	/* Get vdd-cx regulator */
	/*Optional property, don't return error if absent */
	of_property_read_string(of_node, "qcom,vdd-cx-name",
		&sensordata->misc_regulator);
	CDBG("qcom,misc_regulator %s", sensordata->misc_regulator);

	s_ctrl->set_mclk_23880000 = of_property_read_bool(of_node,
						"qcom,mclk-23880000");

	CDBG("%s qcom,mclk-23880000 = %d\n", __func__,
		s_ctrl->set_mclk_23880000);

	return rc;

FREE_VREG_DATA:
	kfree(sensordata->power_info.cam_vreg);
FREE_SUB_MODULE_DATA:
	kfree(sensordata->sensor_info);
FREE_SENSOR_DATA:
	kfree(sensordata);
	return rc;
}

static int32_t msm_sensor_driver_parse(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t                   rc = 0;

	CDBG("Enter");
	/* Validate input parameters */


	/* Allocate memory for sensor_i2c_client */
	s_ctrl->sensor_i2c_client = kzalloc(sizeof(*s_ctrl->sensor_i2c_client),
		GFP_KERNEL);
	if (!s_ctrl->sensor_i2c_client) {
		pr_err("failed: no memory sensor_i2c_client %pK",
			s_ctrl->sensor_i2c_client);
		return -ENOMEM;
	}

	/* Allocate memory for mutex */
	s_ctrl->msm_sensor_mutex = kzalloc(sizeof(*s_ctrl->msm_sensor_mutex),
		GFP_KERNEL);
	if (!s_ctrl->msm_sensor_mutex) {
		pr_err("failed: no memory msm_sensor_mutex %pK",
			s_ctrl->msm_sensor_mutex);
		rc = -ENOMEM;
		goto FREE_SENSOR_I2C_CLIENT;
	}

	/* Parse dt information and store in sensor control structure */
	rc = msm_sensor_driver_get_dt_data(s_ctrl);
	if (rc < 0) {
		pr_err("failed: rc %d", rc);
		goto FREE_MUTEX;
	}

	/* Initialize mutex */
	mutex_init(s_ctrl->msm_sensor_mutex);

	/* Initilize v4l2 subdev info */
	s_ctrl->sensor_v4l2_subdev_info = msm_sensor_driver_subdev_info;
	s_ctrl->sensor_v4l2_subdev_info_size =
		ARRAY_SIZE(msm_sensor_driver_subdev_info);

	/* Initialize default parameters */
	rc = msm_sensor_init_default_params(s_ctrl);
	if (rc < 0) {
		pr_err("failed: msm_sensor_init_default_params rc %d", rc);
		goto FREE_DT_DATA;
	}

	/* Store sensor control structure in static database */
	g_sctrl[s_ctrl->id] = s_ctrl;
	CDBG("g_sctrl[%d] %pK", s_ctrl->id, g_sctrl[s_ctrl->id]);

	return rc;

FREE_DT_DATA:
	kfree(s_ctrl->sensordata->power_info.gpio_conf->gpio_num_info);
	kfree(s_ctrl->sensordata->power_info.gpio_conf->cam_gpio_req_tbl);
	kfree(s_ctrl->sensordata->power_info.gpio_conf);
	kfree(s_ctrl->sensordata->power_info.cam_vreg);
	kfree(s_ctrl->sensordata);
FREE_MUTEX:
	kfree(s_ctrl->msm_sensor_mutex);
FREE_SENSOR_I2C_CLIENT:
	kfree(s_ctrl->sensor_i2c_client);
	return rc;
}
//A: wming for camera out detect @{
#ifdef SENSOR_PLUGOUT_DETECT
static int detect_input_init(void)
{
	struct input_dev *input;
	int ret;
	
	input = input_allocate_device();
	if (input == NULL)
		return -ENOMEM;
	
	input->name = "CSensor";
	
	__set_bit(EV_KEY, input->evbit);
	__set_bit(KEY_F1, input->keybit);
	__set_bit(KEY_F2, input->keybit);
	__set_bit(KEY_F3, input->keybit);
	__set_bit(KEY_F4, input->keybit);
	__set_bit(KEY_F5, input->keybit);
	__set_bit(KEY_F6, input->keybit);
	__set_bit(KEY_F7, input->keybit);
	__set_bit(KEY_F8, input->keybit);
	
	ret = input_register_device(input);
	if (ret < 0)
		goto error;
	
	detect_input = input;
	return 0;
	
error:
	input_free_device(input);
	return ret;
}

static int cameraout_detect_thread(void *data)
{
	int rc = 0;
	int i;
//	uint16_t bank = 0;
	uint16_t diff = 0;
	unsigned int report_keys[] = {
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8
	};
	
	struct msm_sensor_ctrl_t *s_ctrl = (struct msm_sensor_ctrl_t *)data;
	struct msm_camera_i2c_client *sensor_i2c_client;
	
	struct sched_param param = {.sched_priority = 6};
	sched_setscheduler(current, SCHED_RR, &param);
	
	if (!s_ctrl->sensor_i2c_client) {
		pr_err("Failed: sensor_i2c_client is null");
		return -ENOMEM;
	}
	sensor_i2c_client = s_ctrl->sensor_i2c_client;
	
	while (!kthread_should_stop()) {
		msleep(1000);
		if(normal_status == 0)
			last_plug_status = 0;
		if(!detect_flag || !normal_status || cmr_crash) continue;	
		mutex_lock(&cameraout_detect_mutex);
#if 0
        rc = sensor_i2c_client->i2c_func_tbl->i2c_read(
		    sensor_i2c_client, 0xff,
		    &bank, MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0) {
			pr_err("%s: Read bank failed\n", __func__);
		} else {
			if (bank != 0x00) {
				rc = sensor_i2c_client->i2c_func_tbl->i2c_write(
		            sensor_i2c_client, 0xff,
		            0x00, MSM_CAMERA_I2C_BYTE_DATA);
		        if (rc < 0) {
			        pr_err("%s: write data failed\n", __func__);
		        }
			}
		}
#endif
		
		rc = sensor_i2c_client->i2c_func_tbl->i2c_write(
		            sensor_i2c_client, 0xff,
		            0x00, MSM_CAMERA_I2C_BYTE_DATA);
		if (rc < 0) {
			pr_err("%s: write bank 0 failed\n", __func__);
		    }
		
		rc = sensor_i2c_client->i2c_func_tbl->i2c_read(
		    sensor_i2c_client, 0xa0,
		    &cur_plug_status, MSM_CAMERA_I2C_BYTE_DATA);
			
		if(rc < 0)	
			pr_err("read cur_plug_status failed\n ");
		//pr_err("zjy cur_status: 0x%x, last_status: 0x%x\n", cur_plug_status, last_plug_status);
		if (rc < 0) {
		    pr_err("%s: read status failed\n", __func__);
	    } else {
			if (cur_plug_status > 0x0f) {   // register data error
				mutex_unlock(&cameraout_detect_mutex);
				continue;
			} else if (not_prepare && (cur_plug_status == 0x0f) && (last_plug_status == 0x00)) { // boot step
				mutex_unlock(&cameraout_detect_mutex);
				continue;
			} else if (cur_plug_status == last_plug_status) {
				mutex_unlock(&cameraout_detect_mutex);
				continue;
			} else {
				not_prepare = FALSE;
				
				if (detect_input) {					
					diff = cur_plug_status ^ last_plug_status;
					for (i=0; i<4; i++) {
						if ((diff >> i) & 0x01) {
						    if ((cur_plug_status >> i) & 0x01) {  //plugout
							    pr_debug("num %d camera plug out\n", i);
								input_report_key(detect_input, report_keys[i], 1);
		                        input_sync(detect_input);
								input_report_key(detect_input, report_keys[i], 0);
		                        input_sync(detect_input);
							} else {     //plugin
							    pr_debug("num %d camera plug in\n", i);
								input_report_key(detect_input, report_keys[i+4], 1);
		                        input_sync(detect_input);
								input_report_key(detect_input, report_keys[i+4], 0);
		                        input_sync(detect_input);
							}
						}
					}
					last_plug_status = cur_plug_status;
				}
			}		
		}
		
		mutex_unlock(&cameraout_detect_mutex);
	}
	
	return 0;
}
#endif
//@}

//add ++

static void rst_all(void){
	char cmd_start[] = "/system/bin/start";        
	char cmd_stop[] = "/system/bin/stop";        
    char* cmd_argv_start[] = {cmd_start,"qcamerasvr",NULL};
    char* cmd_argv_stop[] = {cmd_stop,"qcamerasvr",NULL};

	char* cmd_argv_start_a[] = {cmd_start,"mediacodec",NULL};
#if 0
    char* cmd_argv_stop_a[] = {cmd_stop,"mediacodec",NULL};
#endif
	char* cmd_argv_start_b[] = {cmd_start,"cameraserver",NULL};
#if 0
    char* cmd_argv_stop_b[] = {cmd_stop,"cameraserver",NULL};
#endif
    char* cmd_envp[] = {"PATH=/sbin:/system/bin",NULL};
	int ret = 0;


#if 0
	ret = call_usermodehelper(cmd_stop, cmd_argv_stop_a, cmd_envp, UMH_WAIT_PROC);
	if(ret < 0)
			pr_err("error to running user helper cmd_stop_a\n");
	ret = call_usermodehelper(cmd_start, cmd_argv_start_a, cmd_envp, UMH_WAIT_PROC);
	if(ret < 0)
			pr_err("error to running user helper cmd_start_a\n");

	ret = call_usermodehelper(cmd_stop, cmd_argv_stop_b, cmd_envp, UMH_WAIT_PROC);
	if(ret < 0)
			pr_err("error to running user helper cmd_stop_b\n");
	ret = call_usermodehelper(cmd_start, cmd_argv_start_b, cmd_envp, UMH_WAIT_PROC);
	if(ret < 0)
			pr_err("error to running user helper cmd_start_b\n");
	
	msleep(1000);
#endif
	ret = call_usermodehelper(cmd_stop, cmd_argv_stop, cmd_envp, UMH_WAIT_PROC);
	if(ret < 0)
			pr_err("error to running user helper cmd_stop\n");
	ret = call_usermodehelper(cmd_start, cmd_argv_start, cmd_envp, UMH_WAIT_PROC);
	if(ret < 0)
			pr_err("error to running user helper cmd_start\n");	
	
#if 1
	ret = call_usermodehelper(cmd_start, cmd_argv_start_a, cmd_envp, UMH_WAIT_PROC);
	if(ret < 0)
			pr_err("error to running user helper cmd_start_a\n");
	
	ret = call_usermodehelper(cmd_start, cmd_argv_start_b, cmd_envp, UMH_WAIT_PROC);
	if(ret < 0)
			pr_err("error to running user helper cmd_start_b\n");
#endif
}


struct workqueue_struct *cam_rs_wq;
struct work_struct cmrs_work;

static ssize_t camera_pwr_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf,size_t count)
{
 char *endp;
 void __iomem *regs;
 int rc;
// uint16_t bank = 0;
 uint16_t cur_plug_status = 0;
 int status = simple_strtoul(buf, &endp, 0);
 pr_err("status = %d\n",status);
 regs = base_iomem();
 if (buf == endp)
  return -EINVAL;
 if (status == 0) {
	normal_status = 0;
	rst_all();
	writel(0x201, regs + 0x1c000);//gpio28--->0x1c
	writel(0x00,  regs + 0x1c004);//gpio28--->0x1c
	writel(0x201, regs + 0x87000);//gpio135--->0x87
	writel(0x00,  regs + 0x87004);//gpio135--->0x87
	writel(0x201, regs + 0x8a000);//gpio138--->0x8a
	writel(0x00,  regs + 0x8a004);//gpio138--->0x8a
	msleep(1000);
	writel(0x201, regs + 0x1c000);//gpio28--->0x1c
	writel(0x03,  regs + 0x1c004);//gpio28--->0x1c
	writel(0x201, regs + 0x87000);//gpio135--->0x87
	writel(0x03,  regs + 0x87004);//gpio135--->0x87
	writel(0x201, regs + 0x8a000);//gpio138--->0x8a
	writel(0x03,  regs + 0x8a004);//gpio138--->0x8a
	//normal_status = 1;
	msleep(1000);
	
	cmr_rs_start = 1;
	pr_err("stephen camera reg off-\n");
 }
 else if(status == 1) {
	 
	writel(0x201, regs + 0x1c000);//gpio28--->0x1c
	writel(0x03,  regs + 0x1c004);//gpio28--->0x1c
	writel(0x201, regs + 0x87000);//gpio135--->0x87
	writel(0x03,  regs + 0x87004);//gpio135--->0x87
	writel(0x201, regs + 0x8a000);//gpio138--->0x8a
	writel(0x03,  regs + 0x8a004);//gpio138--->0x8a
	msleep(2000);
	//normal_status = 1;
	pr_err("stephen camera reg on\n");
  
 }else if(status == 2){
	writel(0x201, regs + 0x1c000);//gpio28--->0x1c
	writel(0x00,  regs + 0x1c004);//gpio28--->0x1c
	writel(0x201, regs + 0x87000);//gpio135--->0x87
	writel(0x00,  regs + 0x87004);//gpio135--->0x87
	writel(0x201, regs + 0x8a000);//gpio138--->0x8a
	writel(0x00,  regs + 0x8a004);//gpio138--->0x8a
	normal_status = 0;
	cmr_rs_start = 1;
 }else if(status == 10){
	detect_flag = 0;
	pr_err("stephen camera flag off\n");
 }else if(status == 11){
	detect_flag = 1;
	normal_status = 1;
	pr_err("stephen camera flag on\n"); 
 }else if(status == 12){//start consult
	queue_work(cam_rs_wq,&cmrs_work); 
 }else if(status == 13){
	cmr_rs_flag = 1; //test use case
 }else if(status == 14){
	pr_err("stephen ps hold\n"); 
	//writel(0x00,  regs + 0x820); 
	//mdelay(10000);
	kernel_restart(NULL);	
 }else if(status == 15){
	 pr_err("stephen  rst_all\n"); 
	 rst_all();
 }else if(status == 16){
	 /*
	 rc = sensor_i2c_client_plug->i2c_func_tbl->i2c_read(
	 sensor_i2c_client_plug,
	 0xff,
	 &bank,MSM_CAMERA_I2C_BYTE_DATA);
	 if(rc<0)
		 pr_err("read bank failed \n");
	*/
	if(sensor_i2c_client_plug != NULL)
	{
		rc = sensor_i2c_client_plug->i2c_func_tbl->i2c_write(
			sensor_i2c_client_plug,0xff,
			0x00,MSM_CAMERA_I2C_BYTE_DATA);
		if(rc < 0)
			pr_err("stephen write bank 0xff failed\n");
		rc = sensor_i2c_client_plug->i2c_func_tbl->i2c_read(
			sensor_i2c_client_plug,
			0xa0,
			&cur_plug_status,MSM_CAMERA_I2C_BYTE_DATA);
		if(rc < 0)
			pr_err("stephen read cur_plug_status failed\n");
		else
			pr_err("stephen get cur_plug_status=0x%x\n",cur_plug_status);
		
	}else
		pr_err("stephen sensor_i2c_client_plug  is not valid\n");
 }
 return count;
}

static ssize_t camera_pwr_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{	
	void __iomem *reg;
	reg = base_iomem();
	return sprintf(buf, "28:%x,135:%x,138:%x,39:%x\n", readl(reg +0x1c004),readl(reg +0x87004),readl(reg +0x8a004),readl(reg +0x27004)); 
}

static struct kobj_attribute camera_pwr_attr = {
 .attr = {
  .name = "camera_pwr",
  .mode = S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH,
 },
 .show = camera_pwr_show,
 .store = camera_pwr_store,
};


static struct attribute *camera_pwr_attributes[] = {
 &camera_pwr_attr.attr,
 NULL
};

static struct attribute_group camera_pwr_group = {
 .attrs = camera_pwr_attributes
};
//2020-1-2++
static int value;
static ssize_t camera_rs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf,size_t count)
{
	char *end;
	value = simple_strtoul(buf, &end, 0);
	return count;
}

static ssize_t camera_rs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{	
	return sprintf(buf, "%d\n", value); 
}

static struct kobj_attribute camera_rs_attr = {
 .attr = {
  .name = "camera_rs",
  .mode = S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH,
 },
 .show = camera_rs_show,
 .store = camera_rs_store,
};

static struct attribute *camera_rs_attributes[] = {
 &camera_rs_attr.attr,
 NULL
};

static struct attribute_group camera_rs_group = {
 .attrs = camera_rs_attributes
};

//2020-1-2--
static int camera_sys = 0;
static int count = 0;
static int count_a = 0;
static int count_b = 0;
static int count_c = 0;
static int count_d = 0;
static int count_e = 0;
static int count_f = 0;
static int count_g = 0;
static int count_h = 0;
//static int need_rst = 0;

static void cam_rst(void){
	
	void __iomem *regs;
	
	regs = base_iomem();
	pr_err("cam_rst ===\n");
	writel(0x201, regs + 0x1c000);//gpio28--->0x1c
	writel(0x00, regs + 0x1c004);//gpio28--->0x1c
	writel(0x201, regs + 0x87000);//gpio135--->0x87
	writel(0x00, regs + 0x87004);//gpio135--->0x87
	writel(0x201, regs + 0x8a000);//gpio138--->0x8a
	writel(0x00, regs + 0x8a004);//gpio138--->0x8a
	
	msleep(1000);
	writel(0x201, regs + 0x1c000);//gpio28--->0x1c
	writel(0x03, regs + 0x1c004);//gpio28--->0x1c
	writel(0x201, regs + 0x87000);//gpio135--->0x87
	writel(0x03, regs + 0x87004);//gpio135--->0x87
	writel(0x201, regs + 0x8a000);//gpio138--->0x8a
	writel(0x03, regs + 0x8a004);//gpio138--->0x8a
	
	//rst_all();
	
}

static void cam_rs_over(void){
	struct file *file = NULL;
	char buf[8];
	mm_segment_t old_fs;
	sprintf(buf,"%s", "11");
	file = filp_open("sys/camera_rs/camera_rs", O_RDWR, 0644);
	if (IS_ERR(file)) {
		printk("%s: open file: %s error!\n", __FUNCTION__, "sys/camera_rs/camera_rs");
	} else {
		old_fs = get_fs();
		set_fs(KERNEL_DS);
		file->f_op->write(file, (char *)buf, sizeof(buf), &file->f_pos);
		set_fs(old_fs);
		filp_close(file, NULL);
		file = NULL;
		cmr_rs_start = 0;
		cmr_rs_flag = 0;
		cmr_rs_hpn = 0;
		count = 0;
		count_a = 0;
		count_b = 0;
		count_c = 0;
		count_d = 0;
		count_e = 0;
		count_f = 0;
		count_g = 0;
		count_h = 0;
		set_crash_flag(0);
	}
	msleep(1000);
}



void cam_work_func(struct work_struct *work){
		pr_err("stephen cmr_rs_start=%d,cmr_rs_flag=%d,cmr_crash=%d,cmr_rs_hpn=%d\n",cmr_rs_start,cmr_rs_flag,cmr_crash,cmr_rs_hpn);
		if      ((cmr_rs_start == 1) && (cmr_rs_flag == 1) && (cmr_crash == 1) && (cmr_rs_hpn == 1)){
				cam_rst();
				cam_rs_over();
		}else if((cmr_rs_start == 1) && (cmr_rs_flag == 0) && (cmr_crash == 1) && (cmr_rs_hpn == 1)){
				msleep(500);
				count++;
				if(count > 50){
					pr_err("stephen time over count\n");
					cam_rst();
					cam_rs_over();
					count = 0;
					
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}
		}else if((cmr_rs_start == 1) && (cmr_rs_flag == 0) && (cmr_crash == 1) && (cmr_rs_hpn == 0)){
				msleep(500);
				count_a++;
				if(count_a > 50){
					pr_err("stephen time over count_a\n");
					cam_rst();
					cam_rs_over();
					count_a = 0;				
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}	
		}else if((cmr_rs_start == 0) && (cmr_rs_flag == 1) && (cmr_crash == 1) && (cmr_rs_hpn == 1)){
				//cam_rst();
				//cam_rs_over();//应用没有写cmr_rs_start情况
				msleep(500);
				count_h++;
				if(count_h > 10){
					pr_err("stephen time over count_h\n");
					cam_rst();
					cam_rs_over();
					count_h = 0;				
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}	
			
		}else if((cmr_rs_start == 0) && (cmr_rs_flag == 0) && (cmr_crash == 1) && (cmr_rs_hpn == 1)){
				msleep(500);
				count_b++;
				if(count_b > 50){
					pr_err("stephen time over count_b\n");
					cam_rst();
					cam_rs_over();
					count_b = 0;
					
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}
		}else if((cmr_rs_start == 0) && (cmr_rs_flag == 0) && (cmr_crash == 1) && (cmr_rs_hpn == 0)){
				msleep(500);
				count_c++;
				if(count_c > 50){
					pr_err("stephen time over count_c\n");
					cam_rst();
					cam_rs_over();
					count_c = 0;
					
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}
		}
		else if((cmr_rs_start == 0) && (cmr_rs_flag == 1) && (cmr_crash == 0) && (cmr_rs_hpn == 1)){//think is first
				pr_err("stephen time over may first\n");
				//cam_rst();// just test
				cam_rs_over();			
		}else if((cmr_rs_start == 0) && (cmr_rs_flag == 0) && (cmr_crash == 0) && (cmr_rs_hpn == 1)){
				msleep(500);
				count_d++;
				if(count_d > 10){
					pr_err("stephen time over count_d\n");
					//cam_rst(); //非crash 不做reset
					cam_rs_over();
					count_d = 0;
					
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}
		}else if((cmr_rs_start == 0) && (cmr_rs_flag == 0) && (cmr_crash == 0) && (cmr_rs_hpn == 0)){
				msleep(500);
				count_e++;
				if(count_e > 10){
					pr_err("stephen time over count_e\n");
					//cam_rst();
					cam_rs_over();
					count_e = 0;
					
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}
		}else if((cmr_rs_start == 1) && (cmr_rs_flag == 1) && (cmr_crash == 0) && (cmr_rs_hpn == 1)){
				pr_err("stephen camera ok\n");
				cam_rs_over();
		}else if((cmr_rs_start == 1) && (cmr_rs_flag == 0) && (cmr_crash == 0) && (cmr_rs_hpn == 1)){
				msleep(500);
				count_f++;
				if(count_f > 10){
					pr_err("stephen time over count_f\n");
					cam_rs_over();
					count_f = 0;
					
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}
		}else if((cmr_rs_start == 1) && (cmr_rs_flag == 0) && (cmr_crash == 0) && (cmr_rs_hpn == 0)){
				msleep(500);
				count_g++;
				if(count_g > 10){
					pr_err("stephen time over count_g\n");
					cam_rs_over();
					count_g = 0;
					
				}else{
					queue_work(cam_rs_wq,&cmrs_work); 
				}
		}
		else{
			cam_rs_over();
			pr_err("other need to check\n");
		}
		
	
}

//add --
static int32_t msm_sensor_driver_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	
	struct msm_sensor_ctrl_t *s_ctrl = NULL;

	/* Create sensor control structure */
	//add ++
		struct kobject *properties_kobj;
		struct kobject *rs_kobj;
		if(camera_sys == 0){
		properties_kobj = kobject_create_and_add("camera_pwr", NULL);
		rs_kobj 		= kobject_create_and_add("camera_rs", NULL);
		if (properties_kobj)
		rc = sysfs_create_group(properties_kobj, &camera_pwr_group);
		if(rc)
			pr_err("camera_pwr faile\n");
		if (rs_kobj)
		rc = sysfs_create_group(rs_kobj, &camera_rs_group);
		if(rc)
			pr_err("camera_rs faile\n");
		camera_sys = 1;
		pr_err("stephen msm_sensor_driver_platform_probe creat sys\n");
		cam_rs_wq = create_singlethread_workqueue("cam_rs");
		INIT_WORK(&cmrs_work,cam_work_func); 
	}
	
	//add --
	
	s_ctrl = kzalloc(sizeof(*s_ctrl), GFP_KERNEL);
	if (!s_ctrl)
		return -ENOMEM;

	platform_set_drvdata(pdev, s_ctrl);

	/* Initialize sensor device type */
	s_ctrl->sensor_device_type = MSM_CAMERA_PLATFORM_DEVICE;
	s_ctrl->of_node = pdev->dev.of_node;

	/*fill in platform device*/
	s_ctrl->pdev = pdev;

	rc = msm_sensor_driver_parse(s_ctrl);
	if (rc < 0) {
		pr_err("failed: msm_sensor_driver_parse rc %d", rc);
		goto FREE_S_CTRL;
	}

	/* Get clocks information */
	rc = msm_camera_get_clk_info(s_ctrl->pdev,
		&s_ctrl->sensordata->power_info.clk_info,
		&s_ctrl->sensordata->power_info.clk_ptr,
		&s_ctrl->sensordata->power_info.clk_info_size);
	if (rc < 0) {
		pr_err("failed: msm_camera_get_clk_info rc %d", rc);
		goto FREE_S_CTRL;
	}

	/* Fill platform device id*/
	pdev->id = s_ctrl->id;

	/* Fill device in power info */
	s_ctrl->sensordata->power_info.dev = &pdev->dev;
	sensor_i2c_client_plug = s_ctrl->sensor_i2c_client;

	detect_input_init();

   	 cameraout_detect_task = kthread_run(cameraout_detect_thread, s_ctrl, "msm_sensor:camera-out");
	 if (IS_ERR(cameraout_detect_task)) {
		pr_err("Failed to create cameraout detect thread. err:%ld\n",
				PTR_ERR(cameraout_detect_task));
         }
	return rc;
FREE_S_CTRL:
	kfree(s_ctrl);
	return rc;
}

static int32_t msm_sensor_driver_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int32_t rc = 0;
	struct msm_sensor_ctrl_t *s_ctrl;

	CDBG("\n\nEnter: msm_sensor_driver_i2c_probe");
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("%s %s i2c_check_functionality failed\n",
			__func__, client->name);
		rc = -EFAULT;
		return rc;
	}

	/* Create sensor control structure */
	s_ctrl = kzalloc(sizeof(*s_ctrl), GFP_KERNEL);
	if (!s_ctrl)
		return -ENOMEM;

	i2c_set_clientdata(client, s_ctrl);

	/* Initialize sensor device type */
	s_ctrl->sensor_device_type = MSM_CAMERA_I2C_DEVICE;
	s_ctrl->of_node = client->dev.of_node;

	rc = msm_sensor_driver_parse(s_ctrl);
	if (rc < 0) {
		pr_err("failed: msm_sensor_driver_parse rc %d", rc);
		goto FREE_S_CTRL;
	}

	if (s_ctrl->sensor_i2c_client != NULL) {
		s_ctrl->sensor_i2c_client->client = client;
		s_ctrl->sensordata->power_info.dev = &client->dev;

		/* Get clocks information */
		rc = msm_camera_i2c_dev_get_clk_info(
			&s_ctrl->sensor_i2c_client->client->dev,
			&s_ctrl->sensordata->power_info.clk_info,
			&s_ctrl->sensordata->power_info.clk_ptr,
			&s_ctrl->sensordata->power_info.clk_info_size);
		if (rc < 0) {
			pr_err("failed: msm_camera_i2c_dev_get_clk_info rc %d",
				rc);
			goto FREE_S_CTRL;
		}
	}
	return rc;
FREE_S_CTRL:
	kfree(s_ctrl);
	return rc;
}

static int msm_sensor_driver_i2c_remove(struct i2c_client *client)
{
	struct msm_sensor_ctrl_t  *s_ctrl = i2c_get_clientdata(client);

	pr_err("%s: sensor FREE\n", __func__);

	if (!s_ctrl) {
		pr_err("%s: sensor device is NULL\n", __func__);
		return 0;
	}

	g_sctrl[s_ctrl->id] = NULL;
	msm_sensor_free_sensor_data(s_ctrl);
	kfree(s_ctrl->msm_sensor_mutex);
	kfree(s_ctrl->sensor_i2c_client);
	kfree(s_ctrl);

	return 0;
}

static const struct i2c_device_id i2c_id[] = {
	{SENSOR_DRIVER_I2C, (kernel_ulong_t)NULL},
	{ }
};

static struct i2c_driver msm_sensor_driver_i2c = {
	.id_table = i2c_id,
	.probe  = msm_sensor_driver_i2c_probe,
	.remove = msm_sensor_driver_i2c_remove,
	.driver = {
		.name = SENSOR_DRIVER_I2C,
	},
};

static int __init msm_sensor_driver_init(void)
{
	int32_t rc = 0;

	CDBG("%s Enter\n", __func__);
	rc = platform_driver_register(&msm_sensor_platform_driver);
	if (rc)
		pr_err("%s platform_driver_register failed rc = %d",
			__func__, rc);
	rc = i2c_add_driver(&msm_sensor_driver_i2c);
	if (rc)
		pr_err("%s i2c_add_driver failed rc = %d",  __func__, rc);

	return rc;
}

static void __exit msm_sensor_driver_exit(void)
{
	CDBG("Enter");
	platform_driver_unregister(&msm_sensor_platform_driver);
	i2c_del_driver(&msm_sensor_driver_i2c);
	return;
}

module_init(msm_sensor_driver_init);
module_exit(msm_sensor_driver_exit);
MODULE_DESCRIPTION("msm_sensor_driver");
MODULE_LICENSE("GPL v2");
