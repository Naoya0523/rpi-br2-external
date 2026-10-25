LINUX_OVERRIDE_SRCDIR = $(BR2_EXTERNAL_MYBOARD_PATH)/linux-rpi

include $(sort $(wildcard $(BR2_EXTERNAL_MYBOARD_PATH)/package/*/*.mk))