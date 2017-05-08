#include "sdkconfig.h"

#if CONFIG_LUA_RTOS_LORA_DEVICE_TYPE_NODE && CONFIG_LUA_RTOS_LORA_NODE_SEMTECH_STACK

#include "lora.h"
#include "board.h"
#include "LoRaMac.h"
#include "region.h"

#include <sys/driver.h>

#define LC4                { 867100000, 867100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 867300000, 867300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 867500000, 867500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC7                { 867700000, 867700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC8                { 867900000, 867900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC9                { 868800000, 868800000, { ( ( DR_7 << 4 ) | DR_7 ) }, 2 }
#define LC10               { 868300000, 868300000, { ( ( DR_6 << 4 ) | DR_6 ) }, 1 }

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

    LoRaMacTestSetDutyCycleOn(1);

    status = LoRaMacChannelAdd( 3, ( ChannelParams_t )LC4 );
    if ((error = check(status))) {
    	return error;
    }

    status = LoRaMacChannelAdd( 4, ( ChannelParams_t )LC5 );
    if ((error = check(status))) {
    	return error;
    }

    status = LoRaMacChannelAdd( 5, ( ChannelParams_t )LC6 );
    if ((error = check(status))) {
    	return error;
    }

    status = LoRaMacChannelAdd( 6, ( ChannelParams_t )LC7 );
    if ((error = check(status))) {
    	return error;
    }

    status = LoRaMacChannelAdd( 7, ( ChannelParams_t )LC8 );
    if ((error = check(status))) {
    	return error;
    }

    status = LoRaMacChannelAdd( 8, ( ChannelParams_t )LC9 );
    if ((error = check(status))) {
    	return error;
    }

    status = LoRaMacChannelAdd( 9, ( ChannelParams_t )LC10 );
    if ((error = check(status))) {
    	return error;
    }

    mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
    mibReq.Param.Rx2DefaultChannel = ( Rx2ChannelParams_t ){ 869525000, DR_3 };
    status = LoRaMacMibSetRequestConfirm( &mibReq );
    if ((error = check(status))) {
    	return error;
    }

    mibReq.Type = MIB_RX2_CHANNEL;
    mibReq.Param.Rx2Channel = ( Rx2ChannelParams_t ){ 869525000, DR_3 };
    status = LoRaMacMibSetRequestConfirm( &mibReq );
    if ((error = check(status))) {
    	return error;
    }

    return NULL;
}

#endif
