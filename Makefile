#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

COMPONENT_ADD_FS ?=
COMPONENT_FS ?=

EXTRA_COMPONENT_DIRS := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))/components/lua/modules

EXTRA_COMPONENTS := $(dir $(foreach cd,$(EXTRA_COMPONENT_DIRS),                           \
					$(wildcard $(cd)/*/component.mk) $(wildcard $(cd)/component.mk) \
				))
EXTRA_COMPONENTS := $(sort $(foreach comp,$(EXTRA_COMPONENTS),$(lastword $(subst /, ,$(comp)))))
EXTRA_COMPONENT_PATHS := $(foreach comp,$(EXTRA_COMPONENTS),$(firstword $(foreach cd,$(EXTRA_COMPONENT_DIRS),$(wildcard $(dir $(cd))$(comp) $(cd)/$(comp)))))

BOARD_TYPE_REQUIRED := 1
VERSION_CHECK_REQUIRED := 1
BOARD_USB_VID_PID :=
BOARD_USB_EXP :=

ifneq (,$(findstring clean,$(MAKECMDGOALS)))
  BOARD_TYPE_REQUIRED := 0
  VERSION_CHECK_REQUIRED := 0
endif

ifneq (,$(findstring menuconfig,$(MAKECMDGOALS)))
  BOARD_TYPE_REQUIRED := 0
endif

ifeq ("$(SDKCONFIG_DEFAULTS)","sdkconfig.defaults")
  BOARD_TYPE_REQUIRED := 0
endif

ifneq ("$(SDKCONFIG_DEFAULTS)","")
  BOARDN := $(shell python boards/boards.py $(SDKCONFIG_DEFAULTS) number)
  TMP := $(shell echo $(BOARDN) > .board)
  BOARD_USB_VID_PID := $(shell python boards/boards.py $(BOARDN) usb_vid_pid)
  BOARD_USB_EXP := $(shell python boards/boards.py $(BOARDN) usb_port_exp)

  BOARD_TYPE_REQUIRED := 0
  override SDKCONFIG_DEFAULTS := boards/$(SDKCONFIG_DEFAULTS)
  MAKECMDGOALS += defconfig  
endif

ifneq (,$(findstring restore-idf,$(MAKECMDGOALS)))
  BOARD_TYPE_REQUIRED := 0
  VERSION_CHECK_REQUIRED := 0
  MAKECMDGOALS += defconfig
endif

ifneq (,$(findstring upgrade-idf,$(MAKECMDGOALS)))
  BOARD_TYPE_REQUIRED := 0
  VERSION_CHECK_REQUIRED := 0
  MAKECMDGOALS += defconfig
endif

# New line
define n


endef

# Use this esp-idf commit in build
CURRENT_IDF := 3276a1316f66a923ee2e75b9bd5c7f1006d160f5

# Project name
PROJECT_NAME := lua_rtos

# Detect OS
UNAME := $(shell uname)

# Set the OS environment variable to build mkspiffs in windows
ifeq ("$(UNAME)", "Linux")
export OS = LINUX
else
ifeq ("$(UNAME)", "Darwin")
export OS = OSX
else
export OS = Windows_NT
endif
endif

# Default filesystem
SPIFFS_IMAGE := default

# Lua RTOS has support for a lot of ESP32-based boards, but each board
# can have different configurations, such as the PIN MAP.
#
# This part ensures that the first time that Lua RTOS is build, the user specifies
# the board type with "make SDKCONFIG_DEFAULTS=board defconfig" or entering
# the board type through a keyboard option
ifeq ($(BOARD_TYPE_REQUIRED),1)
  ifneq ("$(SDKCONFIG_DEFAULTS)","")
    # If SDKCONFIG_DEFAULTS is specified check that the configuration exists
    ifneq ("$(shell test -e boards/$(SDKCONFIG_DEFAULTS) && echo ex)","ex")
      $(error "$(SDKCONFIG_DEFAULTS) does not exists")
    endif
  endif

  # Check if sdkconfig file exists. If this file exists means that at some point the user
  # has specified SDKCONFIG_DEFAULTS. It it don't exists we ask the the user to specify his board
  # type
  ifneq ("$(shell test -e sdkconfig && echo ex)","ex")
    $(info Please, enter your board type:)
    $(info )
    BOARDS := $(subst \,$(n),$(shell python boards/boards.py))
    $(info $(BOARDS))
    ifeq ("$(UNAME)", "Linux")
      BOARDN := $(shell read -p "Board type: " REPLY;echo $$REPLY)
    else
      BOARDN := $(shell read -p "Board type: ";echo $$REPLY)
    endif

    BOARD := $(subst \,$(n),$(shell python boards/boards.py $(BOARDN)))
    # Check if board exists
    ifneq ("$(shell test -e boards/$(BOARD) && echo ex)","ex")
      $(error "Invalid board type boards/$(BOARD)")
    else
      override SDKCONFIG_DEFAULTS := boards/$(BOARD)
      MAKECMDGOALS += defconfig
    endif      
    SPIFFS_IMAGE := $(shell python boards/boards.py $(BOARDN) filesystem)
    TMP := $(shell echo $(BOARDN) > .board)
    BOARD_USB_VID_PID := $(shell python boards/boards.py $(BOARDN) usb_vid_pid)    
    BOARD_USB_EXP := $(shell python boards/boards.py $(BOARDN) usb_port_exp)
  else
    ifneq ("$(SDKCONFIG_DEFAULTS)","")
      override SDKCONFIG_DEFAULTS := boards/$(SDKCONFIG_DEFAULTS)
    endif
    BOARDN := $(shell cat .board)
    SPIFFS_IMAGE := $(shell python boards/boards.py $(BOARDN) filesystem)
    BOARD_USB_VID_PID := $(shell python boards/boards.py $(BOARDN) usb_vid_pid)
    BOARD_USB_EXP := $(shell python boards/boards.py $(BOARDN) usb_port_exp)
  endif  
endif

# Although each board configuration has set the ESPTOOLPY_PORT variable in KConfig,
# the serial port name can vary for each build platform. This part, tries to find
# the serial port in which the board is connected inspecting the available serial
# ports, and matching the USB VID:PID, defined in the boards.json file with the 
# USB VID:PID expected for the board.
ifneq ("foo$(BOARD_USB_VID_PID)","foo")
  $(info Finding a serial port with USB VID:PID $(BOARD_USB_VID_PID) ...)
  # Get all the available serial ports that corresponds to the board USB VID:PID
  SERIAL_PORTS := $(shell python -m serial.tools.list_ports "$(BOARD_USB_VID_PID)" -q)
  ifneq ("foo$(SERIAL_PORTS)","foo")
    SERIAL_PORTS_FINDED := 0
    SERIAL_PORT := 
    ifneq ("foo$(BOARD_USB_EXP)","foo")
      # Some boards, like ESP-WROVER-KIT mount dual USB2UART adapters, so we need to inspect
      # the serial port device name to find the port. For example, ESP-WROVER-KIT exposes
      # two serial ports: /dev/.....A (JTAG), and /dev/.....B (UART). To inspect the
      # serial port device name, we use a regular expression defined in boards.json.
      
      # Count how many serial port device names match with the regular expression   
      $(foreach USB_PORT,$(SERIAL_PORTS), \
        $(if $(filter 1, $(shell python boards/test.py $(BOARD_USB_EXP) $(USB_PORT))), $(eval SERIAL_PORTS_FINDED=$(shell echo $$(($(SERIAL_PORTS_FINDED)+1))))) \
      )
      
      ifeq ("foo$(SERIAL_PORTS_FINDED)","foo1")
        # Only one serial port matches, so use this port
        $(foreach USB_PORT,$(SERIAL_PORTS), \
          $(if $(filter 1, $(shell python boards/test.py $(BOARD_USB_EXP) $(USB_PORT))), $(eval SERIAL_PORT=$(shell echo $(USB_PORT)))) \
        )
        $(info Finded $(SERIAL_PORT))
        ESPPORT ?= $(SERIAL_PORT)
      else
        # There are more that 1 matching, maybe 2 boards are connected, so use the defined in the board configuration  
        $(info 0 or more than 1 board attached, using the serial port defined in ESPTOOLPY_PORT)
      endif
    else
      # There is no need to inspect the serial port device name
      
      # Count how many serial port device names match with the regular expression
      $(foreach USB_PORT,$(SERIAL_PORTS), \
        $(eval SERIAL_PORTS_FINDED=$(shell echo $$(($(SERIAL_PORTS_FINDED)+1)))) \
      )
      
      ifeq ("foo$(SERIAL_PORTS_FINDED)","foo1")
        # Only one serial port matches, so use this port
        $(foreach USB_PORT,$(SERIAL_PORTS), \
          $(eval SERIAL_PORT=$(shell echo $(USB_PORT))) \
        )
        $(info Finded $(SERIAL_PORT))
        ESPPORT ?= $(SERIAL_PORT)
      else
        # There are more that 1 matching, maybe 2 boards are connected, so use the defined in the board configuration  
        $(info 0 or more than 1 board attached, using the serial port defined in ESPTOOLPY_PORT)
      endif
    endif
  else
    # No USB VID:PID defined in boards.json, so use the defined in the
    # board configuration
    $(info No serial ports defined in boards.json, using the serial port defined in ESPTOOLPY_PORT)
  endif
endif

ifeq ("$(VERSION_CHECK_REQUIRED)","1")
  # Check if esp-idf installation contains the required version to build Lua RTOS
  ifeq ("$(shell cd $(IDF_PATH) && git log --pretty="%H" | grep $(CURRENT_IDF))","")
  $(error Please, run "make upgrade-idf" before, to upgrade esp-idf to the version required by Lua RTOS)
  endif

  # Apply Lua RTOS patches
  ifneq ("$(shell test -e $(IDF_PATH)/lua_rtos_patches && echo ex)","ex")
    APPLY_PATCHES := 1
  else
    # Get previous hash of applyied patches
    PREV_HASH := $(shell cat $(IDF_PATH)/lua_rtos_patches)
  
    # Get current hash
    ifeq ("$(UNAME)", "Linux")
      CURR_HASH := $(shell cat components/sys/patches/*.patch | sha256sum)
    else
      CURR_HASH := $(shell cat components/sys/patches/*.patch | shasum -a 256 -p)
    endif
  
    ifneq ("$(PREV_HASH)","$(CURR_HASH)")
      APPLY_PATCHES := 1
    else
      APPLY_PATCHES := 0
    endif
  endif

  ifeq ("$(APPLY_PATCHES)","1")
    $(info Reverting previous Lua RTOS esp-idf patches ...)
    TMP := $(shell cd $(IDF_PATH) && git checkout -f .)
    TMP := $(shell cd $(IDF_PATH) && git checkout -f $(CURRENT_IDF))
    TMP := $(shell cd $(IDF_PATH) && git submodule update -f --init --recursive)
    TMP := $(info Applying Lua RTOS esp-idf patches ...)
    $(foreach PATCH,$(abspath $(wildcard components/sys/patches/*.patch)), \
      $(info Applying patch $(PATCH)...); \
      $(shell cd $(IDF_PATH) && git apply --whitespace=warn $(PATCH)) \
    )
    $(info Patches applied)
  
    # Compute and save new hash
    ifeq ("$(UNAME)", "Linux")
      TMP := $(shell cat components/sys/patches/*.patch | sha256sum > $(IDF_PATH)/lua_rtos_patches)
    else
      TMP := $(shell cat components/sys/patches/*.patch | shasum -a 256 -p > $(IDF_PATH)/lua_rtos_patches)
    endif
  endif
endif

include $(IDF_PATH)/make/project.mk

ifdef TOOLCHAIN_COMMIT_DESC
    ifneq ($(TOOLCHAIN_COMMIT_DESC), $(SUPPORTED_TOOLCHAIN_COMMIT_DESC))
        $(info Toolchain version is not supported: $(TOOLCHAIN_COMMIT_DESC))
        $(info Expected to see version: $(SUPPORTED_TOOLCHAIN_COMMIT_DESC))
        $(info Please check ESP-IDF setup instructions and update the toolchain.)
        $(error Aborting)
    endif
    ifeq (,$(findstring $(TOOLCHAIN_GCC_VER), $(SUPPORTED_TOOLCHAIN_GCC_VERSIONS)))
        $(info Compiler version is not supported: $(TOOLCHAIN_GCC_VER))
        $(info Expected to see version(s): $(SUPPORTED_TOOLCHAIN_GCC_VERSIONS))
        $(info Please check ESP-IDF setup instructions and update the toolchain.)
        $(error Aborting)
    endif
endif # TOOLCHAIN_COMMIT_DESC

ifeq ($(BOARD_TYPE_REQUIRED),1)
  #
  # This part generates the esptool arguments required for erase the otadata region. This is required in case that
  # an OTA firmware is build, so we want to update the factory partition when making "make flash".  
  #
  ifeq ("$(shell test -e $(PROJECT_PATH)/build/partitions.bin && echo ex)","ex")
    $(shell $(IDF_PATH)/components/partition_table/gen_esp32part.py --verify $(PROJECT_PATH)/$(PARTITION_TABLE_CSV_NAME) $(PROJECT_PATH)/build/partitions.bin)

    comma := ,

    ifeq ("$(PARTITION_TABLE_CSV_NAME)","partitions-ota.csv")
      OTA_PARTITION_INFO := $(shell $(IDF_PATH)/components/partition_table/gen_esp32part.py --quiet $(PROJECT_PATH)/build/partitions.bin | grep "otadata")

      OTA_PARTITION_ADDR        := $(word 4, $(subst $(comma), , $(OTA_PARTITION_INFO)))
      OTA_PARTITION_SIZE_INFO   := $(word 5, $(subst $(comma), , $(OTA_PARTITION_INFO)))
      OTA_PARTITION_SIZE_UNITS  := $(word 1, $(subst M, M, $(subst K, K, $(word 5, $(subst $(comma), , $(OTA_PARTITION_INFO))))))
      OTA_PARTITION_SIZE_UNIT   := $(word 2, $(subst M, M, $(subst K, K, $(word 5, $(subst $(comma), , $(OTA_PARTITION_INFO))))))

      OTA_PARTITION_SIZE_FACTOR := 1
      ifeq ($(OTA_PARTITION_SIZE_UNIT),K)
        OTA_PARTITION_SIZE_FACTOR := 1024
      endif

      ifeq ($(OTA_PARTITION_SIZE_UNIT),M)
        OTA_PARTITION_SIZE_FACTOR := 1048576
      endif

      OTA_PARTITION_SIZE := $(shell expr $(OTA_PARTITION_SIZE_UNITS) \* $(OTA_PARTITION_SIZE_FACTOR))

      ESPTOOL_ERASE_OTA_ARGS := $(ESPTOOLPY) --chip esp32 --port $(ESPPORT) --baud $(ESPBAUD) erase_region $(OTA_PARTITION_ADDR) $(OTA_PARTITION_SIZE)
    else
      ESPTOOL_ERASE_OTA_ARGS :=
    endif
 endif
 
  #
  # This part gets the information for the spiffs partition 
  #
  ifeq ("$(shell test -e $(PROJECT_PATH)/build/partitions.bin && echo ex)","ex")
    SPIFFS_PARTITION_INFO := $(shell $(IDF_PATH)/components/partition_table/gen_esp32part.py --quiet $(PROJECT_PATH)/build/partitions.bin | grep "spiffs")

    SPIFFS_BASE_ADDR   := $(word 4, $(subst $(comma), , $(SPIFFS_PARTITION_INFO)))
    SPIFFS_SIZE_INFO   := $(word 5, $(subst $(comma), , $(SPIFFS_PARTITION_INFO)))
    SPIFFS_SIZE_UNITS  := $(word 1, $(subst M, M, $(subst K, K, $(word 5, $(subst $(comma), , $(SPIFFS_PARTITION_INFO))))))
    SPIFFS_SIZE_UNIT   := $(word 2, $(subst M, M, $(subst K, K, $(word 5, $(subst $(comma), , $(SPIFFS_PARTITION_INFO))))))

    SPIFFS_SIZE_FACTOR := 1
    ifeq ($(SPIFFS_SIZE_UNIT),K)
      SPIFFS_SIZE_FACTOR := 1024
    endif

    ifeq ($(SPIFFS_SIZE_UNIT),M)
      SPIFFS_SIZE_FACTOR := 1048576
    endif

    ifeq ("foo$(SPIFFS_SIZE_UNIT)", "foo")
      SPIFFS_SIZE_UNITS := 512
      SPIFFS_SIZE_FACTOR := 1024
    endif

	SPIFFS_SIZE := $(shell expr $(SPIFFS_SIZE_UNITS) \* $(SPIFFS_SIZE_FACTOR))
  endif
endif

#
# Make rules
#
clean: restore-idf

flash: erase-ota-data

erase-ota-data: 
	$(ESPTOOL_ERASE_OTA_ARGS)
	
configure-idf-lua-rtos-tests:
	@echo "Configure esp-idf for Lua RTOS tests ..."
	@touch $(PROJECT_PATH)/components/sys/sys/sys_init.c
	@touch $(PROJECT_PATH)/components/sys/Lua/src/lbaselib.c
ifneq ("$(shell test -e  $(IDF_PATH)/components/sys && echo ex)","ex")
	@ln -s $(PROJECT_PATH)/main/test/lua_rtos $(IDF_PATH)/components/sys 2> /dev/null
endif

upgrade-idf: restore-idf
	@cd $(IDF_PATH) && git pull
	@cd $(IDF_PATH) && git submodule update -f --init --recursive
	
restore-idf:
	@echo "Reverting previous Lua RTOS esp-idf patches ..."
ifeq ("$(shell test -e $(IDF_PATH)/lua_rtos_patches && echo ex)","ex")
	@cd $(IDF_PATH) && git checkout -f .
	@cd $(IDF_PATH) && git checkout -f master
	@cd $(IDF_PATH) && git submodule update -f --init --recursive
	@rm $(IDF_PATH)/lua_rtos_patches
endif
	@rm -f sdkconfig || true
	@rm -f sdkconfig.old || true
	@rm -f sdkconfig.defaults || true
	@rm -f .board || true
		
flash-args:
	@echo $(subst --port $(ESPPORT),, \
			$(subst python /components/esptool_py/esptool/esptool.py,, \
				$(subst $(IDF_PATH),, $(ESPTOOLPY_WRITE_FLASH))\
			)\
	 	  ) \
	 $(subst /build/, , $(subst /build/bootloader/,, $(subst $(PROJECT_PATH), , $(ESPTOOL_ALL_FLASH_ARGS))))

#
# This part prepare the file system content into the build/tmp-fs folder. The file system content
# comes from the SPIFFS_IMAGE variable, that contains the main folder to use, and the COMPONENT_ADD_FS
# variable, that contains individual folders to add by component
#
COMPONENT_FS := 

define includeComponentFS
ifeq ("$(shell test -e $(1)/component.mk && echo ex)","ex")
$(eval include $(1)/component.mk)
COMPONENT_FS += $(addsuffix /*,$(addprefix $(1)/, $(COMPONENT_ADD_FS)))
endif
endef

fs-prepare:
	$(foreach componentpath,$(EXTRA_COMPONENT_PATHS), \
		$(eval $(call includeComponentFS,$(componentpath))))
	@rm -f -r $(PROJECT_PATH)/build/tmp-fs
	@mkdir -p $(PROJECT_PATH)/build/tmp-fs
	@cp -f -r $(COMPONENT_FS) $(PROJECT_PATH)/build/tmp-fs
	@cp -f -r $(PROJECT_PATH)/components/flashfs_image/$(SPIFFS_IMAGE)/* $(PROJECT_PATH)/build/tmp-fs