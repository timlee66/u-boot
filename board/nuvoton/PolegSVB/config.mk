#Including Common config file
include $(srctree)/board/nuvoton/common/config.mk

#Setting board depending flags
PLATFORM_CPPFLAGS += -DCHIP_NAME=npcm750
