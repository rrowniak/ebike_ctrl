FW_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
include $(FW_DIR)/../../../../tools/common.mk
