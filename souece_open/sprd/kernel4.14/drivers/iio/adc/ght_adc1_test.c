#include <linux/io.h>
#include <linux/iio/consumer.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/string.h>

struct adc1_vol {
	struct platform_device	*pdev;
	struct iio_channel	*channel_vol;
};

static uint32_t test_adc_to_vol(struct iio_channel *channel)
{
        int err;
        uint32_t val;

        err = iio_read_channel_processed(channel, &val);
        if (err < 0)
                return err;
        return val;
}

static ssize_t adc1_volatge_show(struct device *dev,
                struct device_attribute *attr, char *buf)
{
        struct adc1_vol *adc1 = dev_get_drvdata(dev);
        int vol;

	vol = test_adc_to_vol(adc1->channel_vol);
        return sprintf(buf, "%d\n", vol);
}

DEVICE_ATTR(voltage, S_IRUGO, adc1_volatge_show, NULL);

static int adc1_vol_probe(struct platform_device *pdev) {
	struct adc1_vol* adc1;
	struct device *dev = &pdev->dev;

	adc1 = devm_kzalloc(&pdev->dev, sizeof(*adc1), GFP_KERNEL);
	if (!adc1)
		return -ENOMEM;

	adc1->pdev = pdev;
	adc1->channel_vol = iio_channel_get(&pdev->dev, "adc1_vol");
	if (IS_ERR(adc1->channel_vol)) {
		dev_err(&pdev->dev, "get iio channel adc vol fail\n");
		return PTR_ERR(adc1->channel_vol);
	}

	device_create_file(dev, &dev_attr_voltage);
	dev_set_drvdata(&pdev->dev, adc1);

	return 0;
}

static int adc1_vol_remove(struct platform_device *pdev) {
	dev_dbg(&pdev->dev, "remove adc1_vol driver!\n");
	return 0;
}

static struct of_device_id of_match_table[] = {
	{ .compatible = "ght,adc1-test", },
	{}
};

static struct platform_driver adc1_vol_driver = {
	.driver	= {
		.name   = "ght,adc1-test",
		.of_match_table = of_match_table,
	},
	.probe          = adc1_vol_probe,
	.remove         = adc1_vol_remove,
};

module_platform_driver(adc1_vol_driver);
