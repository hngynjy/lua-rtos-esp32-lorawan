/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Bleeper board SPI driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "board.h"
#include "spi-board.h"

#include <sys/syslog.h>

#include <drivers/spi.h>

#define LORA_SPI_KHZ    1000

void SpiInit(Spi_t *obj, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss )
{
	driver_error_t *error;

	BoardDisableIrq();

	int spi_instance = 0;

	if ((error = spi_setup(CONFIG_LUA_RTOS_LORA_NODE_SPI, 1, CONFIG_LUA_RTOS_LORA_NODE_CS, 0, LORA_SPI_KHZ * 1000, &spi_instance))) {
        syslog(LOG_ERR, "LoRa node cannot open spi%u", CONFIG_LUA_RTOS_LORA_NODE_SPI);
        return;
    }

	printf("SpiInit spi_instance: %d\r\n",spi_instance);

	memset(obj,0,sizeof(Spi_t));

	obj->Miso.pin = miso;
	obj->Mosi.pin = mosi;

	obj->Nss.pin = nss;
	obj->Nss.port = (uint32_t *)spi_instance;

	obj->Sclk.pin = sclk;

	obj->Spi.Instance = (int *)spi_instance;

	BoardEnableIrq();
}

void SpiDeInit( Spi_t *obj )
{
}

void SpiFormat( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave )
{
}

void SpiFrequency( Spi_t *obj, uint32_t hz )
{
}

uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
	uint8_t rxData;

	spi_ll_select((int)obj->Spi.Instance);
	spi_ll_transfer((int)obj->Spi.Instance, (uint8_t)outData, &rxData);
	spi_ll_deselect((int)obj->Spi.Instance);

	return rxData;
}
