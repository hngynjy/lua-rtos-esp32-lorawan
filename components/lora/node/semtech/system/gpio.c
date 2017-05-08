/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Generic GPIO driver implementation

Comment: Relies on the specific board GPIO implementation as well as on
         IO expander driver implementation if one is available on the target
         board.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "board.h"

#include "esp_attr.h"
#include "esp_intr.h"
#include "driver/gpio.h"
#include <sys/status.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>

#if defined( BOARD_IOE_EXT )
#include "gpio-ioe.h"
#endif

#define SEMTECH_IS_RST_PIN(pin) \
	(pin == CONFIG_LUA_RTOS_LORA_NODE_RST)

#define SEMTECH_IS_NSS_PIN(pin) \
	(pin == CONFIG_LUA_RTOS_LORA_NODE_CS)

#define SEMTECH_IS_DIO_PIN(pin) \
	( \
		(pin == CONFIG_LUA_RTOS_LORA_NODE_DIO0) || \
		(pin == CONFIG_LUA_RTOS_LORA_NODE_DIO1) || \
		(pin == CONFIG_LUA_RTOS_LORA_NODE_DIO2) \
	)

void GpioInit( Gpio_t *obj, PinNames pin, PinModes mode,  PinConfigs config, PinTypes type, uint32_t value )
{
	if (pin == 0) return;

	if (SEMTECH_IS_DIO_PIN(pin)) {
		gpio_pin_input(pin);
		obj->pin = pin;
	} else if (SEMTECH_IS_RST_PIN(pin)) {
		#if !CONFIG_LUA_RTOS_USE_POWER_BUS
			if (mode == PIN_OUTPUT) {
				gpio_pin_output(CONFIG_LUA_RTOS_LORA_NODE_RST);
			} if (mode == PIN_INPUT) {
				gpio_pin_input(CONFIG_LUA_RTOS_LORA_NODE_RST);
			}

			if (val) {
				gpio_pin_clr(pin);
			} else {
				gpio_pin_set(pin);
			}

			obj->pin = pin;
		#else
			if (value) {
				pwbus_off();
				delay(1);
				pwbus_on();
			} else if (value == 0) {
				pwbus_off();
				delay(1);
				pwbus_on();
			} else {
				delay(5);
			}
		#endif

		obj->pin = pin;
	} else if (SEMTECH_IS_NSS_PIN(pin)) {
		obj->pin = pin;
	}
}

void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
	if (obj->pin == 0) return;

	if (SEMTECH_IS_DIO_PIN(obj->pin)) {
		if (!status_get(STATUS_ISR_SERVICE_INSTALLED)) {
			gpio_install_isr_service(0);

			status_set(STATUS_ISR_SERVICE_INSTALLED);
		}

		gpio_set_intr_type(obj->pin, GPIO_INTR_POSEDGE);
		gpio_isr_handler_add(obj->pin, irqHandler, (void*) obj->pin);
	}
}

void GpioWrite( Gpio_t *obj, uint32_t value )
{
	if (obj->pin == 0) return;

	if (SEMTECH_IS_NSS_PIN(obj->pin)) {
		if (value) {
			spi_ll_deselect((int)obj->port);
		} else {
			spi_ll_select((int)obj->port);
		}
	}
}
