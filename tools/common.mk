# Shared build configuration for all ebike_ctrl firmware projects.
#
# Include from each project's Makefile.inc:
#   FW_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
#   include $(FW_DIR)/../../../../tools/common.mk

REPO_ROOT ?= $(abspath $(FW_DIR)/../../../..)

# l_rr library; override with LRR_PATH=/path/to/l_rr
LRR_PATH ?= $(HOME)/dev/rrowniak.github.com/l_rr
LRR_INC := $(LRR_PATH)/include
LRR_SRC := $(LRR_PATH)/src
LRR_INC_STMFAKE := $(LRR_PATH)/stm32_drv_fake/include
LRR_SRC_STMFAKE := $(LRR_PATH)/stm32_drv_fake/src

# CAN wire format shared by both boards
PROTOCOL_INC := $(REPO_ROOT)/protocol
