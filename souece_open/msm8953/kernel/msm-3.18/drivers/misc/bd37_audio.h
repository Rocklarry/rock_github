/*
 * Copyright (c) 2012  Bosch Sensortec GmbH
 * Copyright (c) 2012  Unixphere AB
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _BMP085_H
#define _BMP085_H

#include <linux/regmap.h>

#define BD37_AUDIO_NAME		"bd37_audio"

extern struct regmap_config bd37_audio_regmap_config;

int bd37_audio_probe(struct device *dev, struct regmap *regmap, struct i2c_client *client );
int bd37_audio_remove(struct device *dev);

#endif
