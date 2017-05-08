#include "sdkconfig.h"

#if CONFIG_LUA_RTOS_LORA_DEVICE_TYPE_NODE && CONFIG_LUA_RTOS_LORA_NODE_SEMTECH_STACK

#include "lora.h"
#include "board.h"
#include "LoRaMac.h"

#include <sys/driver.h>

static void McpsConfirm( McpsConfirm_t *mcpsConfirm ) {
	printf("McpsConfirm %d\r\n", mcpsConfirm->McpsRequest);
}

static void McpsIndication( McpsIndication_t *mcpsIndication ) {
	printf("McpsIndication %d\r\n", mcpsIndication->McpsIndication);
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm ) {
	printf("MlmeConfirm %d\r\n", mlmeConfirm->MlmeRequest);
}

static driver_error_t *check(LoRaMacStatus_t status) {
	switch (status) {
		case LORAMAC_STATUS_OK:
			return NULL;
			break;
		case LORAMAC_STATUS_BUSY:
			printf("LORAMAC_STATUS_BUSY\r\n");
			return NULL;
			break;
		case LORAMAC_STATUS_SERVICE_UNKNOWN:
			printf("LORAMAC_STATUS_SERVICE_UNKNOWN\r\n");
			return NULL;
			break;
		case LORAMAC_STATUS_PARAMETER_INVALID:
			return driver_operation_error(LORA_DRIVER, LORA_ERR_INVALID_ARGUMENT, NULL);
			break;
		case LORAMAC_STATUS_FREQUENCY_INVALID:
			printf("LORAMAC_STATUS_FREQUENCY_INVALID\r\n");
			return NULL;
			break;
		case LORAMAC_STATUS_DATARATE_INVALID:
			return driver_operation_error(LORA_DRIVER, LORA_ERR_INVALID_DR, NULL);
			break;
		case LORAMAC_STATUS_FREQ_AND_DR_INVALID:
			printf("LORAMAC_STATUS_FREQ_AND_DR_INVALID\r\n");
			return NULL;
			break;
		case LORAMAC_STATUS_NO_NETWORK_JOINED:
			return driver_operation_error(LORA_DRIVER, LORA_ERR_NOT_JOINED, NULL);
			break;
		case LORAMAC_STATUS_LENGTH_ERROR:
			printf("LORAMAC_STATUS_LENGTH_ERROR\r\n");
			return NULL;
			break;
		case LORAMAC_STATUS_MAC_CMD_LENGTH_ERROR:
			printf("LORAMAC_STATUS_MAC_CMD_LENGTH_ERROR\r\n");
			return NULL;
			break;
		case LORAMAC_STATUS_DEVICE_OFF:
			printf("LORAMAC_STATUS_DEVICE_OFF\r\n");
			return NULL;
			break;
		case LORAMAC_STATUS_REGION_NOT_SUPPORTED:
			return driver_operation_error(LORA_DRIVER, LORA_ERR_INVALID_BAND, NULL);
			break;
	}

	return NULL;
}

driver_error_t *_lora_setup() {
	driver_error_t *error;

    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    LoRaMacPrimitives_t LoRaMacPrimitives;
    LoRaMacCallback_t LoRaMacCallbacks;

    LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
    LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
    LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;

    LoRaMacCallbacks.GetBatteryLevel = BoardGetBatteryLevel;

    BoardInitMcu();

    status = LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU868 );
    if ((error = check(status))) {
    	return error;
    }

    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = 0;
    status = LoRaMacMibSetRequestConfirm( &mibReq );
    if ((error = check(status))) {
    	return error;
    }

    mibReq.Type = MIB_PUBLIC_NETWORK;
    mibReq.Param.EnablePublicNetwork = 1;
    status = LoRaMacMibSetRequestConfirm( &mibReq );
    if ((error = check(status))) {
    	return error;
    }

    return NULL;
}

#endif
