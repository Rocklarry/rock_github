# SPDX-License-Identifier: GPL-2.0
ifeq ($(CONFIG_ARCH_SPRD), y)

ifeq (SU808, $(findstring SU808, $(BSP_SELECT_ID)))
dts-dirs += su808-evk
dts-dirs += su808-fttest
endif

ifeq (SU806, $(findstring SU806, $(BSP_SELECT_ID)))
ifneq ($(filter $(BSP_SELECT_ID), SU806-CN-10-00), )
dts-dirs += su806-v2-evk
dts-dirs += su806-v2-fttest
else
dts-dirs += su806-evk
dts-dirs += su806-fttest
endif
endif

endif

always      := $(dtb-y) $(dtbo-y)
subdir-y    := $(dts-dirs)
clean-files := *.dtb *.dtbo
