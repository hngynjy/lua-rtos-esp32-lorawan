#ifndef LORA_NODE_SEMTECH_H_
#define LORA_NODE_SEMTECH_H_

#include "esp_attr.h"

#include "sdkconfig.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <sys/delay.h>
#include <sys/syslog.h>

#define assert_param(cond) \
if (!cond) { \
	syslog(LOG_ERR, "assert at %s, line %s\n", __FILE__, __LINE__); \
}

#define DelayMs(ms) delay(ms)

typedef uint32_t RTC_DateTypeDef;
typedef uint32_t RTC_TimeTypeDef;
typedef uint32_t RTC_HandleTypeDef;

typedef struct {
	int *Instance;
} SPI_HandleTypeDef;;

#include "utilities.h"
#include "timer.h"
//#include "delay.h"
#include "gpio.h"
//#include "adc.h"
#include "spi.h"
//#include "i2c.h"
//#include "uart.h"
#include "radio.h"

#include "sx1272/sx1272.h"
#include "sx1276/sx1276.h"
//#include "mag3110.h"
//#include "mma8451.h"
//#include "mpl3115.h"
//#include "sx9500.h"
//#include "gps.h"
//#include "gps-board.h"
#include "rtc-board.h"
#include "sx1272-board.h"
#include "sx1276-board.h"
//#include "uart-board.h"

/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                                     1
#endif

//#ifndef FAIL
//#define FAIL                                        0
//#endif

/*!
 * Board IO Extender pins definitions
 */
#define IRQ_MPL3115                                 IOE_0
#define IRQ_MAG3110                                 IOE_1
#define GPS_POWER_ON                                IOE_2
#define SPARE_IO_EXT_3                              IOE_3
#define SPARE_IO_EXT_4                              IOE_4
#define SPARE_IO_EXT_5                              IOE_5
#define SPARE_IO_EXT_6                              IOE_6
#define SPARE_IO_EXT_7                              IOE_7
#define N_IRQ_SX9500                                IOE_8
#define IRQ_1_MMA8451                               IOE_9
#define IRQ_2_MMA8451                               IOE_10
#define TX_EN_SX9500                                IOE_11
#define LED_1                                       IOE_12
#define LED_2                                       IOE_13
#define LED_3                                       IOE_14
#define SPARE_IO_NC                                 IOE_15

/*!
 * Board MCU pins definitions
 */

#define RADIO_RESET                                 CONFIG_LUA_RTOS_LORA_NODE_RST

#if CONFIG_LUA_RTOS_LORA_NODE_SPI == 2
#define RADIO_MOSI                                  CONFIG_LUA_RTOS_SPI2_MOSI
#define RADIO_MISO                                  CONFIG_LUA_RTOS_SPI2_MISO
#define RADIO_SCLK                                  CONFIG_LUA_RTOS_SPI2_CLK
#endif

#if CONFIG_LUA_RTOS_LORA_NODE_SPI == 3
#define RADIO_MOSI                                  CONFIG_LUA_RTOS_SPI3_MOSI
#define RADIO_MISO                                  CONFIG_LUA_RTOS_SPI3_MISO
#define RADIO_SCLK                                  CONFIG_LUA_RTOS_SPI3_CLK
#endif

#define RADIO_NSS                                   CONFIG_LUA_RTOS_LORA_NODE_CS

#define RADIO_DIO_0                                 CONFIG_LUA_RTOS_LORA_NODE_DIO0
#define RADIO_DIO_1                                 CONFIG_LUA_RTOS_LORA_NODE_DIO1
#define RADIO_DIO_2                                 CONFIG_LUA_RTOS_LORA_NODE_DIO2
#define RADIO_DIO_3                                 0
#define RADIO_DIO_4                                 0
#define RADIO_DIO_5                                 0

#define RADIO_ANT_SWITCH_RX                         0
#define RADIO_ANT_SWITCH_TX                         0
#define RADIO_ANT_SWITCH_LF							0
#define RADIO_ANT_SWITCH_HF							0

/*!
 * LED GPIO pins objects
 */
extern Gpio_t Led1;
extern Gpio_t Led2;
extern Gpio_t Led3;

/*!
 * Debug GPIO pins objects
 */
#if defined( USE_DEBUG_PINS )
extern Gpio_t DbgPin1;
extern Gpio_t DbgPin2;
extern Gpio_t DbgPin3;
extern Gpio_t DbgPin4;
extern Gpio_t DbgPin5;
#endif

/*!
 * MCU objects
 */
//extern I2c_t I2c;
//extern Uart_t Uart1;
#if defined( USE_USB_CDC )
extern Uart_t UartUsb;
#endif

/*!
 * Possible power sources
 */
enum BoardPowerSources
{
    USB_POWER = 0,
    BATTERY_POWER,
};

/*!
 * \brief Disable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardDisableIrq( void );

/*!
 * \brief Enable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardEnableIrq( void );

/*!
 * \brief Initializes the target board peripherals.
 */
void BoardInitMcu( void );

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph( void );

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu( void );

/*!
 * \brief Measure the Battery voltage
 *
 * \retval value  battery voltage in volts
 */
uint32_t BoardGetBatteryVoltage( void );

/*!
 * \brief Get the current battery level
 *
 * \retval value  battery level [  0: USB,
 *                                 1: Min level,
 *                                 x: level
 *                               254: fully charged,
 *                               255: Error]
 */
uint8_t BoardGetBatteryLevel( void );

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed( void );

/*!
 * \brief Gets the board 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId( uint8_t *id );

/*!
 * \brief Get the board power source
 *
 * \retval value  power source [0: USB_POWER, 1: BATTERY_POWER]
 */
uint8_t GetBoardPowerSource( void );

#endif
