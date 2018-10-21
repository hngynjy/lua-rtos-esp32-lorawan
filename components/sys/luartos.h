/*
 * Copyright (C) 2015 - 2018, IBEROXARXA SERVICIOS INTEGRALES, S.L.
 * Copyright (C) 2015 - 2018, Jaume Olivé Petrus (jolive@whitecatboard.org)
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *     * The WHITECAT logotype cannot be changed, you can remove it, but you
 *       cannot change it in any way. The WHITECAT logotype is:
 *
 *          /\       /\
 *         /  \_____/  \
 *        /_____________\
 *        W H I T E C A T
 *
 *     * Redistributions in binary form must retain all copyright notices printed
 *       to any local or remote output device. This include any reference to
 *       Lua RTOS, whitecatboard.org, Lua, and other copyright notices that may
 *       appear in the future.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Lua RTOS main include file
 *
 */

#ifndef LUA_RTOS_LUARTOS_H_
#define LUA_RTOS_LUARTOS_H_

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_task.h"

/* Board type */
#if CONFIG_LUA_RTOS_BOARD_WHITECAT_N1ESP32
#define LUA_RTOS_BOARD "N1ESP32"
#endif

#if CONFIG_LUA_RTOS_BOARD_WHITECAT_N1ESP32_DEVKIT
#define LUA_RTOS_BOARD "N1ESP32-DEVKIT"
#endif

#if CONFIG_LUA_RTOS_BOARD_WHITECAT_ESP32_LORA_GW
#define LUA_RTOS_BOARD "WHITECAT-ESP32-LORA-GW"
#endif

#if CONFIG_LUA_RTOS_BOARD_ESP32_CORE_BOARD
#define LUA_RTOS_BOARD "ESP32COREBOARD"
#endif

#if CONFIG_LUA_RTOS_BOARD_ESP32_WROVER_KIT
#define LUA_RTOS_BOARD "ESP-WROVER-KIT"
#endif

#if CONFIG_LUA_RTOS_BOARD_ESP32_THING
#define LUA_RTOS_BOARD "ESP32THING"
#endif

#if CONFIG_LUA_RTOS_BOARD_ADAFRUIT_HUZZAH32
#define LUA_RTOS_BOARD "HUZZAH32"
#endif

#if CONFIG_LUA_RTOS_BOARD_ESP32_GATEWAY
#define LUA_RTOS_BOARD "ESP32-GATEWAY"
#endif

#if CONFIG_LUA_RTOS_BOARD_ESP32_EVB
#define LUA_RTOS_BOARD "ESP32-EVB"
#endif

#if CONFIG_LUA_RTOS_BOARD_FIPY
#define LUA_RTOS_BOARD "FIPY"
#endif

#if CONFIG_LUA_RTOS_BOARD_OTHER
#define LUA_RTOS_BOARD "GENERIC"
#endif

#if CONFIG_LUA_RTOS_BOARD_TTGO_LORA32V20
//#define LUA_RTOS_BOARD "TTGO-LORA32-V2.0"
#define LUA_RTOS_BOARD "GENERIC"
#endif

#if CONFIG_LUA_RTOS_BOARD_M5STACK
#define LUA_RTOS_BOARD "M5STACK"
#endif

#ifndef LUA_RTOS_BOARD
#define LUA_RTOS_BOARD "GENERIC"
#endif

/*
 * Lua RTOS
 */
#define LUA_USE_ROTABLE	   1

// Get the UART assigned to the console
#if CONFIG_LUA_RTOS_CONSOLE_UART0
#define CONSOLE_UART 0
#endif

#if CONFIG_LUA_RTOS_CONSOLE_UART1
#define CONSOLE_UART 1
#endif

#if CONFIG_LUA_RTOS_CONSOLE_UART2
#define CONSOLE_UART 2
#endif

// Get the console baud rate
#if CONFIG_LUA_RTOS_CONSOLE_BR_57600
#define CONSOLE_BR 57600
#endif

#if CONFIG_LUA_RTOS_CONSOLE_BR_115200
#define CONSOLE_BR 115200
#endif

// Get the console buffer length
#ifdef CONFIG_LUA_RTOS_CONSOLE_BUFFER_LEN
#define CONSOLE_BUFFER_LEN CONFIG_LUA_RTOS_CONSOLE_BUFFER_LEN
#else
#define CONSOLE_BUFFER_LEN 1024
#endif

#ifndef CONSOLE_UART
#define CONSOLE_UART 1
#endif

#ifndef CONSOLE_BR
#define CONSOLE_BR 115200
#endif

// LoRa WAN
#if CONFIG_LUA_RTOS_LORA_HW_TYPE_SX1276
#define CONFIG_LUA_RTOS_LUA_USE_LORA 1
#else
#if CONFIG_LUA_RTOS_LORA_HW_TYPE_SX1272
#define CONFIG_LUA_RTOS_LUA_USE_LORA 1
#else
#if CONFIG_LUA_RTOS_LORA_HW_TYPE_SX1301
#define CONFIG_LUA_RTOS_LUA_USE_LORA 1
#else
#define CONFIG_LUA_RTOS_LUA_USE_LORA 0
#endif
#endif
#endif

#define US_PER_OSTICK   20
#define OSTICKS_PER_SEC 50000
#define LMIC_SPI_KHZ    1000

#if CONFIG_LUA_RTOS_LORA_HW_TYPE_SX1276
	#define CFG_sx1276_radio 1
#else
	#if CONFIG_LUA_RTOS_LORA_HW_TYPE_SX1272
		#define CFG_sx1272_radio 1
	#else
		#define CFG_sx1276_radio 1
	#endif
#endif

// https://github.com/mcci-catena/arduino-lorawan
#if CONFIG_LUA_RTOS_LORA_BAND_EU868 || CONFIG_LUA_RTOS_LORA_BAND_AS923
	#define CFG_as923 1
	// #define CFG_eu868 1
#else
	#if CONFIG_LUA_RTOS_LORA_BAND_US915
		#define CFG_us915 1
	#else
		#define CFG_eu868 1
	#endif
#endif

//#define LMIC_DEBUG_LEVEL	2
// define this in lmic_project_config.h to disable all code related to joining
//#define DISABLE_JOIN
// define this in lmic_project_config.h to disable all code related to ping
//#define DISABLE_PING
// define this in lmic_project_config.h to disable all code related to beacon tracking.
// Requires ping to be disabled too
//#define DISABLE_BEACONS

// define these in lmic_project_config.h to disable the corresponding MAC commands.
// Class A
//#define DISABLE_MCMD_DCAP_REQ // duty cycle cap
//#define DISABLE_MCMD_DN2P_SET // 2nd DN window param
//#define DISABLE_MCMD_SNCH_REQ // set new channel
// Class B
#define DISABLE_MCMD_PING_SET // set ping freq, automatically disabled by DISABLE_PING
#define DISABLE_MCMD_BCNI_ANS // next beacon start, automatically disabled by DISABLE_BEACON

// In LoRaWAN, a gateway applies I/Q inversion on TX, and nodes do the
// same on RX. This ensures that gateways can talk to nodes and vice
// versa, but gateways will not hear other gateways and nodes will not
// hear other nodes. By defining this macro in lmic_project_config.h,
// this inversion is disabled and this node can hear other nodes. If
// two nodes both have this macro set, they can talk to each other
// (but they can no longer hear gateways). This should probably only
// be used when debugging and/or when talking to the radio directly
// (e.g. like in the "raw" example).
//#define DISABLE_INVERT_IQ_ON_RX

#define LMIC_REGION_eu868	1
#define LMIC_REGION_us915	2
#define LMIC_REGION_cn783	3
#define LMIC_REGION_eu433	4
#define LMIC_REGION_au921	5
#define LMIC_REGION_cn490	6
#define LMIC_REGION_as923	7
#define LMIC_REGION_kr921	8
#define LMIC_REGION_in866	9

// the selected region.
#if defined(CFG_eu868)
# define CFG_region     LMIC_REGION_eu868
#elif defined(CFG_us915)
# define CFG_region     LMIC_REGION_us915
#elif defined(CFG_cn783)
# define CFG_region     LMIC_REGION_cn783
#elif defined(CFG_eu433)
# define CFG_region     LMIC_REGION_eu433
#elif defined(CFG_au921)
# define CFG_region     LMIC_REGION_au921
#elif defined(CFG_cn490)
# define CFG_region     LMIC_REGION_cn490
#elif defined(CFG_as923)
# define CFG_region     LMIC_REGION_as923
#elif defined(CFG_kr921)
# define CFG_region     LMIC_REGION_kr921
#elif defined(CFG_in866)
# define CFG_region     LMIC_REGION_in866
#else
# define CFG_region     0
#endif

#define LMIC_DR_LEGACY		1
#define CFG_LMIC_EU_like	1
#define CFG_LMIC_US_like	0

// Some regions have country-specific overrides. For generality, we specify
// country codes using the LMIC_COUNTY_CODE_C() macro These values are chosen
// from the 2-letter domain suffixes standardized by ISO-3166-1 alpha2 (see
// https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2). They are therefore
// 16-bit constants. By convention, we use UPPER-CASE letters, thus
// LMIC_COUNTRY_CODE('J', 'P'), not ('j', 'p').
#define LMIC_COUNTRY_CODE_C(c1, c2)     ((c1) * 256 + (c2))

// this special code means "no country code defined"
#define LMIC_COUNTRY_CODE_NONE  0

// specific countries. Only the ones that are needed by the code are defined.
#define LMIC_COUNTRY_CODE_JP    LMIC_COUNTRY_CODE_C('J', 'P')
#define LMIC_COUNTRY_CODE	LMIC_COUNTRY_CODE_NONE


#if CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS <= 1
#error "Please, review the 'Number of thread local storage pointers' settings in kconfig. Must be >= 2."
#endif

#define THREAD_LOCAL_STORAGE_POINTER_ID (CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS - 1)
// External GPIO
#define EXTERNAL_GPIO 0
#define EXTERNAL_GPIO_PINS 0
#define EXTERNAL_GPIO_PORTS 0
#if CONFIG_GPIO_PCA9698 ||  CONFIG_GPIO_PCA9505
	#undef EXTERNAL_GPIO_PINS
	#define EXTERNAL_GPIO_PINS 40
	#undef EXTERNAL_GPIO_PORTS
	#define EXTERNAL_GPIO_PORTS 5
	#undef EXTERNAL_GPIO
	#define EXTERNAL_GPIO 1

	#if CONFIG_GPIO_PCA9698
	#define EXTERNAL_GPIO_NAME "PCA9698"
	#endif

#if CONFIG_GPIO_PCA9505
	#define EXTERNAL_GPIO_NAME "PCA9505"
	#endif
#endif

// OpenVPN
#if CONFIG_LUA_RTOS_USE_OPENVPN
#if !CONFIG_MBEDTLS_BLOWFISH_C
#error "OpenVPM requires CONFIG_MBEDTLS_BLOWFISH_C = 1. Please activate it with make menuconfig, enabling option in mbedTLS -> Symmetric Ciphers -> Blowfish block cipher."
#endif
#endif

#if __has_include("luartos_custom.h")
#include "luartos_custom.h"
#endif

// Curl
#if CONFIG_LUA_RTOS_LUA_USE_CURL_NET
#if !CONFIG_MBEDTLS_DES_C
#error "curl requires CONFIG_MBEDTLS_DES_C = 1. Please activate it with make menuconfig, enabling option in mbedTLS -> Symmetric Ciphers -> DES block cipher."
#endif
#endif

// BT
#if CONFIG_LUA_RTOS_LUA_USE_BT
#if !CONFIG_BT_ENABLED
#error "Bluetooth requires CONFIG_BT_ENABLED = 1. Please activate it with make menuconfig, enabling option in Component config -> Bluetooth."
#endif
#endif

#if CONFIG_LUA_RTOS_LUA_USE_NUM_64BIT
#if CONFIG_NEWLIB_NANO_FORMAT
#error "Use 64 bits for integer and real is not compatible with CONFIG_NEWLIB_NANO_FORMAT = 1. Please disable it with make menuconfig, disabling option in Component config -> ESP32-especific -> Enable 'nano' formatting options for printf/scanf family  ."
#endif
#endif

// Root file system
#if CONFIG_LUA_RTOS_RAM_FS_ROOT_FS
#define CONFIG_LUA_RTOS_ROOT_FS "rfs"
#elif CONFIG_LUA_RTOS_SPIFFS_ROOT_FS
#define CONFIG_LUA_RTOS_ROOT_FS "spiffs"
#elif CONFIG_LUA_RTOS_LFS_ROOT_FS
#define CONFIG_LUA_RTOS_ROOT_FS "lfs"
#elif CONFIG_LUA_RTOS_FAT_ROOT_FS
#define CONFIG_LUA_RTOS_ROOT_FS "fat"
#endif

#endif
