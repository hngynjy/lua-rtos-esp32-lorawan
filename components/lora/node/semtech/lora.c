#include "sdkconfig.h"

#if CONFIG_LUA_RTOS_LORA_DEVICE_TYPE_NODE && CONFIG_LUA_RTOS_LORA_NODE_SEMTECH_STACK

#include "lora.h"
#include "board.h"
#include "LoRaMac.h"
#include "region.h"
#include "common.h"

#include <sys/driver.h>

// Data needed for ABP
static uint32_t DEVADDR = 0x00000000;
static uint8_t NWKSKEY[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint8_t APPSKEY[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint8_t ADR = 0;
static uint8_t DR = 0;

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
    mibReq.Param.Rx2DefaultChannel = ( Rx2ChannelParams_t ){ 869525000, DR_9 };
    status = LoRaMacMibSetRequestConfirm( &mibReq );
    if ((error = check(status))) {
    	return error;
    }

    mibReq.Type = MIB_RX2_CHANNEL;
    mibReq.Param.Rx2Channel = ( Rx2ChannelParams_t ){ 869525000, DR_9 };
    status = LoRaMacMibSetRequestConfirm( &mibReq );
    if ((error = check(status))) {
    	return error;
    }

    return NULL;
}

driver_error_t *_lora_mac_set(const char command, const char *value) {
	driver_error_t *error;
	MibRequestConfirm_t mibReq;

	switch(command) {
		case LORA_MAC_SET_DEVADDR:
			hex_string_to_val((char *)value, (char *)(&DEVADDR), 4, 1);

			mibReq.Type = MIB_DEV_ADDR;
			mibReq.Param.DevAddr = &DEVADDR;
		    if ((error = check(LoRaMacMibSetRequestConfirm(&mibReq)))) {
		    	return error;
		    }
			break;

		case LORA_MAC_SET_DEVEUI:
			//#if CONFIG_LUA_RTOS_READ_FLASH_UNIQUE_ID
			//mtx_unlock(&lora_mtx);
			//return driver_operation_error(LORA_DRIVER, LORA_ERR_INVALID_ARGUMENT, "in this board DevEui is assigned automatically");
			//#else
			// DEVEUI must be in little-endian format
			//hex_string_to_val((char *)value, (char *)DEVEUI, 8, 1);
			//#endif
			break;

		case LORA_MAC_SET_APPEUI:
			// APPEUI must be in little-endian format
			//hex_string_to_val((char *)value, (char *)APPEUI, 8, 1);
			break;

		case LORA_MAC_SET_NWKSKEY:
			hex_string_to_val((char *)value, (char *)NWKSKEY, 16, 0);

			mibReq.Type = MIB_NWK_SKEY;
			mibReq.Param.NwkSKey = NWKSKEY;
		    if ((error = check(LoRaMacMibSetRequestConfirm(&mibReq)))) {
		    	return error;
		    }
			break;

		case LORA_MAC_SET_APPSKEY:
            hex_string_to_val((char *)value, (char *)APPSKEY, 16, 0);

            mibReq.Type = MIB_APP_SKEY;
            mibReq.Param.AppSKey = APPSKEY;
		    if ((error = check(LoRaMacMibSetRequestConfirm(&mibReq)))) {
		    	return error;
		    }
			break;

		case LORA_MAC_SET_APPKEY:
			// APPKEY must be in big-endian format
			//hex_string_to_val((char *)value, (char *)APPKEY, 16, 0);
			break;

		case LORA_MAC_SET_DR:
			if ((atoi((char *)value) < 0) || (atoi((char *)value) > 15)) {
				return driver_operation_error(LORA_DRIVER, LORA_ERR_INVALID_DR, NULL);
			}

			//u1_t dr = data_rates[atoi((char *)value)];
			//if (dr == DR_NONE) {
			//	return driver_operation_error(LORA_DRIVER, LORA_ERR_INVALID_DR, NULL);
			//}

			DR = (uint8_t)atoi((char *)value);

			//if (!adr) {
#if CONFIG_LUA_RTOS_LORA_NODE_LMIC_STACK
				//LMIC_setDrTxpow(current_dr, 14);
#endif
			//}

			break;

		case LORA_MAC_SET_ADR:

			if (strcmp(value, "on") == 0) {
				ADR = 1;
			} else {
				ADR = 0;
			}

			mibReq.Type = MIB_ADR;
            mibReq.Param.AdrEnable = ADR;
		    if ((error = check(LoRaMacMibSetRequestConfirm(&mibReq)))) {
		    	return error;
		    }

            break;

		case LORA_MAC_SET_LINKCHK:
			//if (strcmp(value, "on") == 0) {
#if CONFIG_LUA_RTOS_LORA_NODE_LMIC_STACK
				//LMIC_setLinkCheckMode(1);
#endif
			//} else {
#if CONFIG_LUA_RTOS_LORA_NODE_LMIC_STACK
				//LMIC_setLinkCheckMode(0);
#endif
			//}
			break;

		case LORA_MAC_SET_RETX:
#if CONFIG_LUA_RTOS_LORA_NODE_LMIC_STACK
			//LMIC.txAttempts = atoi((char *)value);
#endif
			break;
	}

	return NULL;
}

driver_error_t *_lora_mac_get(const char command, char **value) {
	char *result = NULL;

	switch(command) {
		case LORA_MAC_GET_DEVADDR:
			break;

		case LORA_MAC_GET_DEVEUI:
			//result = (char *)malloc(17);

			// DEVEUI is in little-endian format
			//val_to_hex_string(result, (char *)DEVEUI, 8, 1);
			break;

		case LORA_MAC_GET_APPEUI:
			//result = (char *)malloc(17);

			// APPEUI is in little-endian format
			//val_to_hex_string(result, (char *)APPEUI, 8, 1);
			break;

		case LORA_MAC_GET_DR:
			result = (char *)malloc(2);
			if (result) {
				sprintf(result,"%d",DR);
			}
			break;

		case LORA_MAC_GET_ADR:
			if (ADR) {
				result = (char *)malloc(3);
				if (result) {
					strcpy(result, "on");
				}
			} else {
				result = (char *)malloc(4);
				if (result) {
					strcpy(result, "off");
				}
			}
			break;

		case LORA_MAC_GET_LINKCHK:
#if CONFIG_LUA_RTOS_LORA_NODE_LMIC_STACK
			//if (LMIC.adrAckReq == LINK_CHECK_INIT) {
			//	result = (char *)malloc(3);
			//	if (result) {
			//		strcpy(result, "on");
			//	}
			//} else {
			//	result = (char *)malloc(4);
			//	if (result) {
			//		strcpy(result, "off");
			//	}
			//}
#endif
			break;

		case LORA_MAC_GET_RETX:
			//result = (char *)malloc(2);
			//if (result) {
#if CONFIG_LUA_RTOS_LORA_NODE_LMIC_STACK
				//sprintf(result,"%d",LMIC.txAttempts);
#endif
			//}
			break;
	}

    *value = result;

	return NULL;
}

driver_error_t *_lora_tx(uint8_t cnf, uint8_t port, uint8_t *payload, uint16_t len) {
	driver_error_t *error;
	MibRequestConfirm_t mibReq;
    McpsReq_t mcpsReq;

    mibReq.Type = MIB_NETWORK_JOINED;
    mibReq.Param.IsNetworkJoined = true;
    if ((error = check(LoRaMacMibSetRequestConfirm(&mibReq)))) {
    	return error;
    }

    if (cnf) {
        mcpsReq.Type = MCPS_CONFIRMED;
        mcpsReq.Req.Confirmed.fPort = port;
        mcpsReq.Req.Confirmed.fBuffer = (void *)payload;
        mcpsReq.Req.Confirmed.fBufferSize = len;
        mcpsReq.Req.Confirmed.NbTrials = 2;
        mcpsReq.Req.Confirmed.Datarate = DR;
    } else {
    	mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fPort = port;
        mcpsReq.Req.Unconfirmed.fBuffer = (void *)payload;
        mcpsReq.Req.Unconfirmed.fBufferSize = len;
        mcpsReq.Req.Unconfirmed.Datarate = DR;
    }

    if ((error = check(LoRaMacMcpsRequest(&mcpsReq)))) {
    	return error;
    }

    return NULL;
}
#endif
