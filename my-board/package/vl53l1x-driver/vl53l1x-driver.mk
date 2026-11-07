################################################################################
#
# vl53l1x-driver
#
################################################################################

VL53L1X_DRIVER_VERSION = 1.0
VL53L1X_DRIVER_SITE = $(BR2_EXTERNAL_BOARD_PATH)/package/vl53l1x-driver/src
VL53L1X_DRIVER_SITE_METHOD = local

VL53L1X_DRIVER_LICENSE = GPL-2.0

# Kernel module
VL53L1X_DRIVER_MODULE_SUBDIRS = .

$(eval $(kernel-module))
$(eval $(generic-package))