#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include "bd37_audio.h"

#define BD37_I2C_ADDRESS	0x40

static int bd37_audio_i2c_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	int err;

	struct regmap *regmap = NULL;
	regmap = devm_regmap_init_i2c(client, &bd37_audio_regmap_config);

	pr_err("%s:probe into!\n", __func__);

	if (IS_ERR(regmap)) {
	    err = PTR_ERR(regmap);
	    dev_err(&client->dev, "Failed to init regmap: %d\n", err);
	    return err;
	}
	pr_err("%s:probe successfully!\n", __func__);
	//return bd37_audio_probe(&client->dev, regmap);
	return bd37_audio_probe(&client->dev, regmap,client);
}

static int bd37_audio_i2c_remove(struct i2c_client *client)
{
	return bd37_audio_remove(&client->dev);
}

static const struct of_device_id audio_match_table[] = {
    {.compatible = "bd37033,audio",},
    { },
};

static const struct i2c_device_id bd37_audio_id[] = {
    { "bd37_audio", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, bd37_audio_id);

static struct i2c_driver bd37_audio_i2c_driver = {
    .driver = {
        .owner	= THIS_MODULE,
        .name	= BD37_AUDIO_NAME,
        .of_match_table = audio_match_table,
    },
    .id_table	= bd37_audio_id,
    .probe		= bd37_audio_i2c_probe,
    .remove		= bd37_audio_i2c_remove,
};

module_i2c_driver(bd37_audio_i2c_driver);

MODULE_AUTHOR("Eric Andersson <eric.andersson@unixphere.com>");
MODULE_DESCRIPTION("BMP085 I2C bus driver");
MODULE_LICENSE("GPL");
