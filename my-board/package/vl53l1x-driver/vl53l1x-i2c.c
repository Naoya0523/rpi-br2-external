
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/unaligned.h>

#include <linux/iio/iio.h>
#include <linux/iio/buffer.h>
#include <linux/iio/trigger.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

// iio channel spec
static const struct iio_chan_spec vl53l1x_channels[] = {
	{
		.type = IIO_DISTANCE,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_SCALE),
		.scan_index = 0,
		.scan_type = {
			.sign = 'u',
			.realbits = 16,
			.storagebits = 16,
		},
	},
	IIO_CHAN_SOFT_TIMESTAMP(1),
};

// probe
static int vl53l1x_probe(struct i2c_client *client)
{

};


// device ID
static const struct i2c_device_id vl53l1x_id[] = {
    { "vl53l1x" },
    { }
};
MODULE_DEVICE_TABLE(i2c, vl53l1x_id);

// DTS match
static const struct of_device_id st_vl53l1x_dt_match[] = {
    { .compatible = "st,vl53l1x"},
    { }
};
MODULE_DEVICE_TABLE(of, st_vl53l1x_dt_match);

// driver
static struct i2c_driver vl53l1x_driver = {
    .driver = {
        .name = "vl53l1x-i2c",
        .of_match_table = st_vl53l1x_dt_match,
    },
    .probe = vl53l1x_probe,
    .id_table = vl53l1x_id,
};
module_i2c_driver(vl53l1x_driver);

// LICENSE
MODULE_AUTHOR("Naoya");
MODULE_DESCRIPTION("ST vl53l1x ToF ranging sensor driver");
MODULE_LICENSE("GPL v2");
