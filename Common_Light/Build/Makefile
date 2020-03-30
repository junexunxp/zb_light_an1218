###############################################################################
#
# MODULE:   Makefile
#
# DESCRIPTION: Makefile for the ZLO Light devices
# 
 ############################################################################
#
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5179].
# You, and any third parties must reproduce the copyright and warranty notice
# and any other legend of ownership on each copy or partial copy of the
# software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Copyright NXP B.V. 2017. All rights reserved
#
############################################################################

LIGHT ?= ExtendedColorLight
GP_SUPPORT ?=0

# Application target name
TARGET = $(LIGHT)
PDM_BUILD_TYPE ?=_EEPROM
#PDM_BUILD_TYPE ?= _EXTERNAL_FLASH
ifeq ($(GP_SUPPORT), 1)
STACK_SIZE = 6500
else
STACK_SIZE = 5000
endif
MINIMUM_HEAP_SIZE = 2000
###############################################################################
#User definable make parameters that may be overwritten from the command line

# Default target device is the JN5179

JENNIC_CHIP ?= JN5179
JENNIC_CHIP_FAMILY ?=JN517x
JENNIC_PCB ?= DEVKIT5 

ifeq ($(JENNIC_CHIP_FAMILY), JN517x)
JENNIC_SDK              = JN-SW-4270
JENNIC_PCB              = DEVKIT5
ifeq ($(OTA),1)
	ZNCLKCMD            = AppBuildZBProOta.ld
else
	ZNCLKCMD            = AppBuildZBPro.ld
endif
ENDIAN                  = LITTLE_ENDIAN_PROCESSOR
DISABLE_LTO = 1
else 
ifeq ($(JENNIC_CHIP_FAMILY), JN516x)
JENNIC_SDK              = JN-SW-4170
JENNIC_PCB              = DEVKIT4
ZNCLKCMD                = AppBuildZBPro.ld
ENDIAN 	                = BIG_ENDIAN
#DISABLE_LTO = 1
endif
endif
TOUCHLINK ?= 1
###############################################################################
# Network settings

# Optional network.mk for channel
sinclude ../../../network.mk

# Channel (0 for default channels)
SINGLE_CHANNEL ?= 0
CFLAGS         += -DSINGLE_CHANNEL=$(SINGLE_CHANNEL)

#sub srings to identify hardware for use in the OTA string
ifeq ($(DR),OM15045)
OTA_HW = OM15045
endif
ifeq ($(DR),OM15053)
OTA_HW = OM15053
endif
ifeq ($(DR),DR1175)
OTA_HW = DR1175
endif

#sub srings to identify hardware varients for use in the OTA string
ifeq ($(TYPE),RGBW)
 OTA_HW_VARIANT = RGBW
else
 ifeq ($(TYPE),RGB)
  OTA_HW_VARIANT = RGB-
 else
  ifeq ($(OPT1),DUAL_PWM)
   OTA_HW_VARIANT = PWM2
  else
   ifeq ($(DR),DR1175)
    OTA_HW_VARIANT = r1v2-
   else
    OTA_HW_VARIANT = ----
   endif
  endif
 endif
endif

ifeq ($(OTA),1)
CFLAGS  += -DBUILD_OTA
MANUFACTURER_CODE = 0x1037
CFLAGS += -DCLD_OTA_MANF_ID_VALUE=$(MANUFACTURER_CODE)
APP_CLUSTERS_OTA_SRC ?= 1

OTA_ENCRYPTED ?= 0
ifeq ($(OTA_ENCRYPTED),1)
CFLAGS  += -DOTA_ENCRYPTED
endif

OTA_INTERNAL_STORAGE ?= 1
ifeq ($(OTA_INTERNAL_STORAGE),1)
CFLAGS  += -DOTA_INTERNAL_STORAGE
ifeq ($(OTA_ENCRYPTED),1)
CFLAGS += -DINTERNAL_ENCRYPTED
endif
endif

#OTA Parameters for Dimmable light
ifeq ($(TARGET),DimmableLight)
 ifeq ($(OTA_ENCRYPTED),0)
  OTA_DEVICE_ID = 0x0101
  OTA_STRING = $(OTA_HW)$(OTA_HW_VARIANT)-$(JENNIC_CHIP)00000000000000
 else
  OTA_DEVICE_ID = 0x1101
  OTA_STRING = $(OTA_HW)$(OTA_HW_VARIANT)-$(JENNIC_CHIP)--ENCRYPTED000
 endif
endif 

#OTA Parameters for Color Temperature light
ifeq ($(TARGET),ColorTemperatureLight)
  ifeq ($(OTA_ENCRYPTED),0)
  OTA_DEVICE_ID = 0x010c
  OTA_STRING = $(OTA_HW)$(OTA_HW_VARIANT)-$(JENNIC_CHIP)00000000000000
 else
  OTA_DEVICE_ID = 0x110c
  OTA_STRING = $(OTA_HW)$(OTA_HW_VARIANT)-$(JENNIC_CHIP)--ENCRYPTED000
 endif
endif


#OTA Parameters for Extended Color light
ifeq ($(TARGET),ExtendedColorLight)
  ifeq ($(OTA_ENCRYPTED),0)
  OTA_DEVICE_ID = 0x010d
  OTA_STRING = $(OTA_HW)$(OTA_HW_VARIANT)-$(JENNIC_CHIP)00000000000000
 else
  OTA_DEVICE_ID = 0x110d
  OTA_STRING = $(OTA_HW)$(OTA_HW_VARIANT)-$(JENNIC_CHIP)--ENCRYPTED000
 endif
 endif
endif




###############################################################################
# Default DK4 development kit target hardware

DR ?= DR1175
ifeq ($(DR), DR1175)
CFLAGS  += -DBUTTON_MAP_DR1175
endif

ifeq ($(DR), DR1173)
CFLAGS  += -DBUTTON_MAP_DR1175
endif

# NFC NTAG ICODE (new) functionality
APP_NTAG_ICODE ?= 0
ifeq ($(APP_NTAG_ICODE), 1)
CFLAGS  += -DAPP_NTAG_ICODE
endif

# NFC NTAG AES (old) functionality
APP_NTAG_AES ?= 0
ifeq ($(APP_NTAG_AES), 1)
CFLAGS  += -DAPP_NTAG_AES
endif

ifeq ($(APP_NTAG_ICODE), 1)
BDB_SUPPORT_OOBC ?= 1
endif
ifeq ($(APP_NTAG_AES), 1)
BDB_SUPPORT_OOBC ?= 1
endif

ifeq  ($(DR),OM15045)
OPT1 ?=NULL
OPT2 ?=NULL
CFLAGS += -D$(OPT1)
CFLAGS += -D$(OPT2)
endif

ifeq ($(TYPE),RGBW)
VARIANT := _RGBW
CFLAGS += -DINCLUDE_WHITE_CHANNEL
endif

CFLAGS += -DMK_USE_POSITIVE_PWM=TRUE

#Always tell any actual drivers they're running under an RTOS in this usage
CFLAGS  += -DRTOS

# Tell the code what target hardware is  
CFLAGS  += -D$(DR)
#DEBUG ?=HW
#DEBUG_PORT ?= UART1
#  DEBUG_MODE  ?=  SWD    # For ARM core

###############################################################################
# Select the network stack (e.g. MAC, ZBPro, SE, HA)
JENNIC_STACK ?= ZCL
JENNIC_MAC ?= MiniMacShim

APP_CLUSTER_LIGHTING_SRC ?= 1
APP_CLUSTER_COMMISSIONING_SRC ?= 1

# BDB features � Enable as required
#BDB_SUPPORT_TOUCHLINK_INITIATOR_END_DEVICE ?= 1
#BDB_SUPPORT_TOUCHLINK_INITIATOR_ROUTER ?= 1
ifeq ($(TOUCHLINK),1)
BDB_SUPPORT_TOUCHLINK_TARGET ?= 1
CFLAGS += -DAPP_TOUCHLINK_ENABLED
endif
BDB_SUPPORT_NWK_STEERING ?= 1
#BDB_SUPPORT_NWK_FORMATION ?= 1
#BDB_SUPPORT_FIND_AND_BIND_INITIATOR ?= 1
BDB_SUPPORT_FIND_AND_BIND_TARGET ?= 1


###############################################################################
# End device or router / coord
ZBPRO_DEVICE_TYPE ?= ZCR

###############################################################################
# Debug options define DEBUG either HW or SW
#DEBUG ?=SW

#
# Define which UART to use for HW debug


###############################################################################
# Define TRACE to use with DBG module
TRACE ?=1
ifeq ($(TRACE), 1)
CFLAGS  += -DDBG_ENABLE
#Define to bump debug baud rate to 921600
#CFLAGS += -DDEBUG_921600
endif

CFLAGS += -D$(TARGET)

LNT ?=0
ifeq ($(LNT), 1)
CFLAGS += -DLNT
endif

ifeq ($(JENNIC_CHIP_FAMILY),JN517x)
INTERRUPT_HANDLER_BBC = zps_isrMAC
LDFLAGS += -Wl,-u$(INTERRUPT_HANDLER_BBC) -Wl,-defsym,vAHI_IntHandlerBbc_select=$(INTERRUPT_HANDLER_BBC)
endif

###############################################################################
# Define GP Support to use 

ifeq ($(GP_SUPPORT), 1)
GP_DEVICE ?= COMBO_BASIC
ifeq ($(GP_DEVICE), COMBO_BASIC)
CFLAGS  +=-DGP_COMBO_BASIC_DEVICE
else
CFLAGS  +=-DGP_PROXY_BASIC_DEVICE
$(info  "NOTE: Make sure that app_GP.cfg Manually Changed to have device ID as 0x0061 and also remove GP cluster id from in cluster list of Endpoint 242 for Proxy build")
endif
CFLAGS  += -DGP_CERITIFICATION_TEST

CFLAGS  += -DCLD_GREENPOWER
APP_CLUSTERS_GREENPOWER_SRC ?=1
endif 

##############################################################################
# For 4x use string based PDM id's for newer families use 16 bit id numbers
ifneq ($(JENNIC_CHIP_FAMILY), JN514x)
CFLAGS   += -DPDM_USER_SUPPLIED_ID
endif

###############################################################################

#CFLAGS += -DNXP_HARDWARE
#CFLAGS += -DDEBUG_ZCL
#CFLAGS += -DDEBUG_APP
#CFLAGS += -DDEBUG_START_UP
#CFLAGS += -DDEBUG_LIGHT_NODE
#CFLAGS += -DDEBUG_LIGHT_TASK
#CFLAGS += -DDEBUG_JOIN
#CFLAGS += -DDEBUG_COMMISSION
#CFLAGS == -DDEBUG_TL_NEGATIVE
#CFLAGS += -DDEBUG_CLASSIC_JOIN
CFLAGS +=  -DDEBUG_EXCEPTIONS
#CFLAGS += -DDEBUG_TEMPERATURE
#CFLAGS += -DDEBUG_BULB_START

#CFLAGS += -DDEBUG_CLD_IDENTIFY 
#CFLAGS += -DDEBUG_CLD_LEVEL_CONTROL
#CFLAGS += -DDEBUG_CLD_ONOFF
#CFLAGS += -DDEBUG_LAMP
#CFLAGS += -DDEBUG_CLD_SCENES
#CFLAGS += -DDEBUG_CLD_COLOUR_CONTROL
#CFLAGS += -DDEBUG_CLD_COLOUR_CONTROL_UPDATES
#CFLAGS += -DDEBUG_CLD_COLOUR_CONTROL_CONVERSIONS
#CFLAGS += -DDEBUG_CLD_GROUPS
#CFLAGS += -DDEBUG_APP_OTA
#CFLAGS += -DDEBUG_REPORT

#CFLAGS += -DDEBUG_APP_GP
#CFLAGS += -DTRACE_GP_DEBUG
#CFLAGS += -DDEBUG_BDB
#CFLAGS += -DDEBUG_APP_NTAG
#CFLAGS += -DDEBUG_APP_ICODE
#CFLAGS  += -DSTRICT_PARAM_CHECK

###############################################################################
# Path definitions

# Use if application directory contains multiple targets
SDK_BASE_DIR        = ../../../../sdk/$(JENNIC_SDK)
APP_BASE            =  ../..
APP_BLD_DIR         = $(APP_BASE)/Common_Light/Build
APP_SRC_DIR         = $(APP_BASE)/Common_Light/Source
DEV_BLD_DIR         = $(APP_BASE)/$(TARGET)/Build
DEV_SRC_DIR         = $(APP_BASE)/$(TARGET)/Source

APP_DRIVER_SRC_DIR  = $(APP_BASE)/Common_Light/Source/DriverBulb
HW_SRC_DIR			= $(COMPONENTS_BASE_DIR)/HardwareAPI/Source

UTIL_SRC_DIR        = $(COMPONENTS_BASE_DIR)/ZigbeeCommon/Source
BDB_SRC_DIR        = $(COMPONENTS_BASE_DIR)/BDB/Source

###############################################################################
# Application Source files

# Note: Path to source file is found using vpath below, so only .c filename is required

ifeq ($(JENNIC_CHIP_FAMILY), JN517x)
APPSRC = port_JN517x.c
endif

ifeq ($(JENNIC_CHIP_FAMILY), JN516x)
APPSRC += irq_JN516x.S
APPSRC += portasm_JN516x.S
APPSRC += port_JN516x.c
endif

APPSRC += app_main.c
APPSRC += pdum_gen.c
APPSRC += pdum_apdu.S
APPSRC += zps_gen.c
APPSRC += app_start_light.c
APPSRC += app_scenes.c
APPSRC += app_pdm_convert.c

ifeq ($(JENNIC_CHIP_FAMILY), JN517x)
APPSRC += AHI_ModuleConfiguration.c 
endif
 
APPSRC += app_zlo_light_node.c
APPSRC += app_zcl_light_task.c
APPSRC += app_manage_temperature.c
ifeq ($(OTA),1)
APPSRC += app_ota_client.c
endif
APPSRC += app_light_interpolation.c
APPSRC += app_reporting.c
APPSRC += app_power_on_counter.c
ifeq ($(GP_SUPPORT), 1)
APPSRC += app_green_power.c
endif

#Light device type and it's associated driver 
APPSRC += App_$(TARGET).c
APPSRC += DriverBulb_$(DR).c
APPSRC += DriverBulb_Shim.c

ifeq ($(DR), OM15045)
CFLAGS  += -DBUTTON_MAP_OM15045
APPSRC += app_buttons.c
endif

ifeq ($(DR), OM15053)
CFLAGS  += -DBUTTON_MAP_OM15053
APPSRC += app_buttons.c
endif

ifeq ($(DR), DR1175)
APPSRC += app_buttons.c
CFLAGS  += -DBUTTON_MAP_DR1175
endif

ifeq ($(DR), DR1173)
APPSRC += app_buttons.c
endif

ifeq ($(APP_NTAG_ICODE),1)
APPSRC += app_ntag_icode.c
#APPSRC += app_icode.c
endif
ifeq ($(APP_NTAG_AES),1)
APPSRC += app_ntag_aes.c
endif

CFLAGS +=-D$(DR)
CFLAGS += -DEMBEDDED
CFLAGS += -DUSER_VSR_HANDLER

ifeq ($(TARGET),ExtendedColorLight)
CFLAGS += -DCOLORLIGHT
CFLAGS += -DRGB
endif
ifeq ($(TARGET),ColorLight)
CFLAGS += -DCOLORLIGHT
CFLAGS += -DRGB
endif
ifeq ($(TARGET),ColorTemperatureLight)
CFLAGS += -DCOLORTEMPERATURELIGHT
CFLAGS += -DRGB
endif
ifeq ($(TARGET),DimmableLight)
CFLAGS += -DDIMMABLELIGHT
CFLAGS += -DMONO_WITH_LEVEL
endif

#Synchronous driver has one option combinations OPT1 =SW555 
ifeq ($(DR), DR1192)
OPT1 ?=NULL
CFLAGS += -D$(OPT1)
endif

ifeq  ($(DR),DR1221_Dimic)

OPT1 ?=NULL
OPT2 ?=NULL
CFLAGS += -D$(OPT1)
CFLAGS += -D$(OPT2)
endif
LINEAR_MODE ?= FALSE
ifeq ($(LINEAR_MODE),TRUE)
CFLAGS += -DLINEAR_MODE
VARIANT += _LINEAR
endif

# Construct binary features list 

ifeq ($(APP_NTAG_ICODE),1)
TARGET_FEATURES:=$(TARGET_FEATURES)_NtagIcode
endif

ifeq ($(APP_NTAG_AES),1)
TARGET_FEATURES:=$(TARGET_FEATURES)_NtagAes
endif

ifeq ($(GP_SUPPORT), 1)
APP_ZPSCFG = app_GP.zpscfg
ifeq ($(GP_DEVICE), COMBO_BASIC)
TARGET_FEATURES:=$(TARGET_FEATURES)_GpCombo
else
TARGET_FEATURES:=$(TARGET_FEATURES)_GpProxy
endif
endif

ifeq ($(OTA),1)
TARGET_FEATURES:=$(TARGET_FEATURES)_Ota
ifeq ($(OTA_ENCRYPTED),1)
TARGET_FEATURES:=$(TARGET_FEATURES)Enc
endif
endif

# Construct binary hardware list

TARGET_HARDWARE:=_$(JENNIC_CHIP)_$(DR)$(VARIANT)

ifneq ($(SINGLE_CHANNEL), 0)
TARGET_HARDWARE:=$(TARGET_HARDWARE)_CH$(SINGLE_CHANNEL)
endif


ifeq ($(TOUCHLINK), 1)
ifeq ($(GP_SUPPORT), 1)
APP_ZPSCFG = app_GP.zpscfg
else
APP_ZPSCFG = app.zpscfg
endif
else
APP_ZPSCFG = app_gp_notl.zpscfg
endif

OPTIONAL_STACK_FEATURES = $(shell $(ZPSCONFIG) -n $(TARGET) -f $(APP_SRC_DIR)/$(APP_ZPSCFG) -y )


pdum_gen.o zps_gen.o app_start_light.o pdum_apdu.o : CFLAGS += -fno-lto

###############################################################################
# Standard Application header search paths

INCFLAGS += -I$(APP_SRC_DIR)
INCFLAGS += -I$(APP_SRC_DIR)/..
INCFLAGS += -I$(DEV_SRC_DIR)


# Application specific include files
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Common/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZigbeeCommon/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZCL/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/ZCIF/Include

INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Xcv/Include/
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Recal/Include/
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/MicroSpecific/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/HardwareAPI/Include

INCFLAGS += -I$(APP_DRIVER_SRC_DIR)

###############################################################################
# NTAG ICODE library
ifeq ($(APP_NTAG_ICODE),1)
# NTAG library folder
APP_NTAG_LIB_DIR  = $(APP_BASE)/NFC
# NTAG library
LDLIBS           += NTAG_$(JENNIC_CHIP_FAMILY)
# NTAG library path
LDFLAGS          += -L$(APP_NTAG_LIB_DIR)/Library
# NTAG include path
INCFLAGS         += -I$(APP_NTAG_LIB_DIR)/Include
endif

###############################################################################
# NTAG AES library
ifeq ($(APP_NTAG_AES),1)
# NTAG library folder
APP_NTAG_LIB_DIR  = $(APP_BASE)/NTAG
# NTAG library
LDLIBS           += NTAG_$(JENNIC_CHIP_FAMILY)
# NTAG library path
LDFLAGS          += -L$(APP_NTAG_LIB_DIR)/Library
# NTAG include path
INCFLAGS         += -I$(APP_NTAG_LIB_DIR)/Include
endif

LDLIBS += Recal_$(JENNIC_CHIP_FAMILY)

###############################################################################
# Application libraries
# Specify additional Component libraries

#APPLIBS +=
ifeq ($(GP_SUPPORT), 1)
APPLIBS += JPT
endif

###############################################################################

# You should not need to edit below this line

###############################################################################
###############################################################################
# Configure for the selected chip or chip family

include $(SDK_BASE_DIR)/Chip/Common/Build/config.mk
include $(SDK_BASE_DIR)/Platform/Common/Build/config.mk
include $(SDK_BASE_DIR)/Stack/Common/Build/config.mk
include $(SDK_BASE_DIR)/Components/BDB/Build/config.mk

###############################################################################

TEMP = $(APPSRC:.c=.o)
APPOBJS_TMP = $(TEMP:.S=.o)
APPOBJS := $(addprefix $(DEV_BLD_DIR)/,$(APPOBJS_TMP))

###############################################################################
# Application dynamic dependencies

APPDEPS_TMP = $(APPOBJS_TMP:.o=.d)
APPDEPS := $(addprefix $(DEV_BLD_DIR)/,$(APPDEPS_TMP))

###############################################################################
# Linker

# Add application libraries before chip specific libraries to linker so
# symbols are resolved correctly (i.e. ordering is significant for GCC)

APPLDLIBS := $(foreach lib,$(APPLIBS),$(if $(wildcard $(addprefix $(COMPONENTS_BASE_DIR)/Library/lib,$(addsuffix _$(JENNIC_CHIP).a,$(lib)))),$(addsuffix _$(JENNIC_CHIP),$(lib)),$(addsuffix _$(JENNIC_CHIP_FAMILY),$(lib))))
LDLIBS := $(APPLDLIBS) $(LDLIBS)
LDLIBS += JPT_$(JENNIC_CHIP)
 
###############################################################################
# Dependency rules

.PHONY: all clean
# Path to directories containing application source
vpath % $(APP_SRC_DIR):$(ZCL_SRC_DIRS):$(ZCL_SRC):$(UTIL_SRC_DIR):$(BDB_SRC_DIR):$(DEV_SRC_DIR):$(APP_DRIVER_SRC_DIR):$(HW_SRC_DIR)


#all: $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin
ifeq ($(OTA),1)
all: Client.bin	
else
all: $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin
endif

-include $(APPDEPS)
$(DEV_BLD_DIR)/%.d:
	rm -f $*.o

ifeq ($(JENNIC_CHIP_FAMILY), JN517x)
$(DEV_SRC_DIR)/pdum_gen.c $(DEV_SRC_DIR)/pdum_gen.h: $(APP_SRC_DIR)/$(APP_ZPSCFG) $(PDUMCONFIG)
	$(info Configuring the PDUM ...)
	$(PDUMCONFIG) -z $(TARGET)  -e $(ENDIAN) -f $< -o $(DEV_SRC_DIR)

$(DEV_SRC_DIR)/zps_gen.c $(DEV_SRC_DIR)/zps_gen.h: $(APP_SRC_DIR)/$(APP_ZPSCFG) $(ZPSCONFIG)
	$(info Configuring the Zigbee Protocol Stack ...)
	$(ZPSCONFIG) -n $(TARGET) -t $(JENNIC_CHIP_FAMILY) -l $(ZPS_NWK_LIB) -a $(ZPS_APL_LIB) -c $(TOOL_COMMON_BASE_DIR)/$(TOOLCHAIN_PATH) -e $(ENDIAN) -f $< -o $(DEV_SRC_DIR)

$(DEV_BLD_DIR)/pdum_gen.o: CFLAGS += -fno-lto
$(DEV_BLD_DIR)/zps_gen.o: CFLAGS += -fno-lto
endif

ifeq ($(JENNIC_CHIP_FAMILY), JN516x)
$(DEV_SRC_DIR)/pdum_gen.c $(DEV_SRC_DIR)/pdum_gen.h: $(APP_SRC_DIR)/$(APP_ZPSCFG) $(PDUMCONFIG)
	$(info Configuring the PDUM ...)
	$(PDUMCONFIG) -z $(TARGET)  -f $< -o $(DEV_SRC_DIR)

$(DEV_SRC_DIR)/zps_gen.c $(DEV_SRC_DIR)/zps_gen.h: $(APP_SRC_DIR)/$(APP_ZPSCFG) $(ZPSCONFIG)
	$(info Configuring the Zigbee Protocol Stack ...)
	$(ZPSCONFIG) -n $(TARGET) -t $(JENNIC_CHIP_FAMILY) -l $(ZPS_NWK_LIB) -a $(ZPS_APL_LIB) -c $(TOOL_COMMON_BASE_DIR)/$(TOOLCHAIN_PATH) -f $< -o $(DEV_SRC_DIR)
endif

$(DEV_BLD_DIR)/%.o: %.S
	$(info Assembling $< ...)
	$(CC) -c -o $(subst Source,Build,$@) $(CFLAGS) $(INCFLAGS) $< -MD -MF $(DEV_BLD_DIR)/$*.d -MP
	@echo

$(DEV_BLD_DIR)/%.o: %.c
	$(info Compiling $< ...)
	$(CC) -c -o $(subst Source,Build,$@) $(CFLAGS) $(INCFLAGS) $< -MD -MF $(DEV_BLD_DIR)/$*.d -MP
	@echo

$(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).elf: $(APPOBJS) $(addsuffix.a,$(addprefix $(COMPONENTS_BASE_DIR)/Library/lib,$(APPLDLIBS))) 
	$(info Linking $@ ...)
	$(CC) -Wl,--gc-sections -Wl,-u_AppColdStart -Wl,-u_AppWarmStart $(LDFLAGS) -T$(ZNCLKCMD) -L $(SDK_BASE_DIR)/Stack/ZCL/Build/  -o $@ -Wl,--start-group $(APPOBJS) $(addprefix -l,$(LDLIBS)) -lm -Wl,--end-group -Wl,-Map,$(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).map
	$(SIZE) $@ 
	date +%c >> $(APP_BASE)/Doc/size.txt
	$(SIZE) $@ >> $(APP_BASE)/Doc/size.txt
	
# Do a second linking with LTO disabled
	$(CC) -Wl,--cref -Wl,--gc-sections -Wl,-u_AppColdStart -Wl,-u_AppWarmStart $(LDFLAGS) -T$(ZNCLKCMD) -L $(SDK_BASE_DIR)/Stack/ZCL/Build/  -o $@ -Wl,--start-group $(APPOBJS) $(addprefix -l,$(LDLIBS)) -lm -Wl,--end-group -fno-lto -Wl,-Map,$(DEV_BLD_DIR)/$(TARGET)_$(JENNIC_CHIP)$(BIN_SUFFIX)_NOLTO.map
# Run the Pyt
	
ifeq ($(JENNIC_CHIP_FAMILY),JN517x)
$(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin: $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).elf 
	$(info Generating binary ...)
	$(OBJCOPY) -S -O binary $< $@
else
$(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin: $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).elf
	$(info Generating binary ...)
	$(OBJCOPY) -j .version -j .bir -j .flashheader -j .vsr_table -j .vsr_handlers  -j .rodata -j .text -j .data -j .bss -j .heap -j .stack -S -O binary $< $@
endif

ifeq ($(OTA),1)	
$(info Generating OTA binary ...)
ifeq ($(JENNIC_CHIP_FAMILY), JN516x)
     JET_VERSION=4
else ifeq ($(JENNIC_CHIP_FAMILY), JN517x)
	 JET_VERSION=5
endif

Client.bin:$(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin
	cp -f $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin $(DEV_BLD_DIR)/OTABuild/Light.bin
ifeq ($(OTA_ENCRYPTED),0)
ifeq ($(OS),Windows_NT)
	$(DEV_BLD_DIR)/OTABuild/LightCreateOtaClient.bat $(DEV_BLD_DIR)/OTABuild $(JENNIC_SDK) $(MANUFACTURER_CODE) $(OTA_STRING) $(JET_VERSION) $(JENNIC_CHIP_FAMILY) $(OTA_DEVICE_ID)
else
	/bin/bash $(DEV_BLD_DIR)/OTABuild/CreateOtaClient.sh $(DEV_BLD_DIR)/OTABuild $(JENNIC_SDK) $(MANUFACTURER_CODE) $(OTA_STRING) $(JET_VERSION) $(JENNIC_CHIP_FAMILY) $(OTA_DEVICE_ID)	
endif	
	mv $(DEV_BLD_DIR)/OTABuild/Client.bin $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin
	mv $(DEV_BLD_DIR)/OTABuild/ClientUpGradeImagewithOTAHeaderV2.bin $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_V2.bin
	mv $(DEV_BLD_DIR)/OTABuild/ClientUpGradeImagewithOTAHeaderV3.bin $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_V3.bin
	mv $(DEV_BLD_DIR)/OTABuild/ClientUpGradeImagewithOTAHeaderV2.ota $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_V2.ota
	mv $(DEV_BLD_DIR)/OTABuild/ClientUpGradeImagewithOTAHeaderV3.ota $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_V3.ota
else
ifeq ($(OS),Windows_NT)
	$(DEV_BLD_DIR)/OTABuild/LightCreateOtaEncClient.bat $(DEV_BLD_DIR)/OTABuild $(JENNIC_SDK) $(MANUFACTURER_CODE) $(OTA_STRING) $(JET_VERSION) $(JENNIC_CHIP_FAMILY) $(OTA_DEVICE_ID)
else
	/bin/bash $(DEV_BLD_DIR)/OTABuild/CreateOtaEncClient.sh $(DEV_BLD_DIR)/OTABuild $(JENNIC_SDK) $(MANUFACTURER_CODE) $(OTA_STRING) $(JET_VERSION) $(JENNIC_CHIP_FAMILY) $(OTA_DEVICE_ID)	
endif
	mv $(DEV_BLD_DIR)/OTABuild/Client.bin $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_ENC.bin
	mv $(DEV_BLD_DIR)/OTABuild/Client_UpgradeImagewithOTAHeaderV2_Enc.bin $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_V2_ENC.bin
	mv $(DEV_BLD_DIR)/OTABuild/Client_UpgradeImagewithOTAHeaderV3_Enc.bin $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_V3_ENC.bin	
	mv $(DEV_BLD_DIR)/OTABuild/Client_UpgradeImagewithOTAHeaderV2_Enc.ota $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_V2_ENC.ota
	mv $(DEV_BLD_DIR)/OTABuild/Client_UpgradeImagewithOTAHeaderV3_Enc.ota $(DEV_BLD_DIR)/OTABuild/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE)_V3_ENC.ota	
endif
	rm -f $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin
	rm -f $(DEV_BLD_DIR)/OTABuild/Light.bin
endif

#	$(SDK_BASE_DIR)/Tools/Memsize2/memsize.exe $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).elf $(DEV_BLD_DIR)/$(TARGET)_$(JENNIC_CHIP)$(BIN_SUFFIX)_NOLTO.map $(TARGET).json
	
###############################################################################

clean:
	rm -f $(APPOBJS) $(APPDEPS) $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).bin $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).elf $(DEV_BLD_DIR)/$(TARGET)$(TARGET_FEATURES)$(TARGET_HARDWARE).map
	rm -f $(DEV_SRC_DIR)/pdum_gen.* $(DEV_SRC_DIR)/zps_gen*.* $(DEV_SRC_DIR)/pdum_apdu.*

###############################################################################
