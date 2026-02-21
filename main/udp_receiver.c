/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include <ardupilotmega/mavlink.h>

static const char *TAG = "UDP";

//#define CONFIG_UDP_PORT	14550

// Bradcast Receive Task
void udp(void *pvParameters)
{
	ESP_LOGI(TAG, "Start. Wait for %d port", CONFIG_UDP_PORT);

	/* set up address to recvfrom */
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(CONFIG_UDP_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); /* senderInfo message from ANY */

	/* create the socket */
	int fd;
	int ret;
	fd = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
	LWIP_ASSERT("fd >= 0", fd >= 0);

#if 0
	/* set option */
	int broadcast=1;
	ret = lwip_setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
	LWIP_ASSERT("ret >= 0", ret >= 0);
#endif

	/* bind socket */
	ret = lwip_bind(fd, (struct sockaddr *)&addr, sizeof(addr));
	LWIP_ASSERT("ret >= 0", ret >= 0);

	/* senderInfo data */
	char buffer[128];
	struct sockaddr_in senderInfo;
	//socklen_t senderInfoLen = sizeof(senderInfo);

	mavlink_message_t _rxmsg;
	mavlink_status_t  _rxstatus;
	mavlink_message_t _message;
	mavlink_status_t  _mav_status;
	bzero(&_rxstatus, sizeof(mavlink_status_t));

	while(1) {
		socklen_t senderInfoLen = sizeof(senderInfo);
		memset(buffer, 0, sizeof(buffer));
		ret = lwip_recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderInfo, &senderInfoLen);
		LWIP_ASSERT("ret > 0", ret > 0);
		ESP_LOGD(TAG,"lwip_recv ret=%d",ret);
		if (ret == 0) continue;

		buffer[ret] = 0;
		//ESP_LOGI(TAG,"lwip_recv buffer=%s",buffer);
		ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, ret, ESP_LOG_DEBUG);
#if 0
		char senderstr[16];
		inet_ntop(AF_INET, &senderInfo.sin_addr, senderstr, sizeof(senderstr));
		ESP_LOGD(TAG,"recvfrom : %s, port=%d", senderstr, ntohs(senderInfo.sin_port));
#endif

#if 0
typedef struct __mavlink_message {
	uint16_t checksum;		///< sent at end of packet
	uint8_t magic;			///< protocol magic marker
	uint8_t len;			///< Length of payload
	uint8_t incompat_flags; ///< flags that must be understood
	uint8_t compat_flags;	///< flags that can be ignored if not understood
	uint8_t seq;			///< Sequence of packet
	uint8_t sysid;			///< ID of message sender system/aircraft
	uint8_t compid;			///< ID of the message sender component
	uint32_t msgid:24;		///< ID of message in payload
	uint64_t payload64[(MAVLINK_MAX_PAYLOAD_LEN+MAVLINK_NUM_CHECKSUM_BYTES+7)/8];
	uint8_t ck[2];			///< incoming checksum bytes
	uint8_t signature[MAVLINK_SIGNATURE_BLOCK_LEN];
}) mavlink_message_t;
#endif

		for (int index=0; index<ret; index++) {
			uint8_t result = buffer[index];
			uint8_t msgReceived = mavlink_frame_char_buffer(&_rxmsg, &_rxstatus, result, &_message, &_mav_status);
			ESP_LOGD(TAG,"msgReceived=%d", msgReceived);
			if (msgReceived == 1) {
				ESP_LOGD(TAG,"_message.msgid=%d _message.compid=%d", _message.msgid, _message.compid);

				// #0
				if (_message.msgid ==  MAVLINK_MSG_ID_HEARTBEAT) {
					mavlink_heartbeat_t param;
					mavlink_msg_heartbeat_decode(&_message, &param);
					ESP_LOGD("HEARTBEAT","type=%d autopilot=%d base_mode=%d custom_mode=0x%"PRIx32" system_status=%d mavlink_version=%d",
						param.type, param.autopilot, param.base_mode, param.custom_mode, param.system_status, param.mavlink_version);

				// #20
				} else if (_message.msgid ==  MAVLINK_MSG_ID_PARAM_REQUEST_READ) {
					mavlink_param_request_read_t param;
					mavlink_msg_param_request_read_decode(&_message, &param);
					ESP_LOGD("PARAM_REQUEST_READ","sysid=%d compid=%d seq=%d",_message.sysid, _message.compid, _message.seq);
					ESP_LOGI("PARAM_REQUEST_READ","target_system=%d target_component=%d param_id=[%16s] param_index=%d",param.target_system, param.target_component, param.param_id, param.param_index);

				// #21
				} else if (_message.msgid ==  MAVLINK_MSG_ID_PARAM_REQUEST_LIST) {
					ESP_LOGI("PARAM_REQUEST_LIST","sysid=%d compid=%d seq=%d",_message.sysid, _message.compid, _message.seq);

				// #22
				} else if (_message.msgid ==  MAVLINK_MSG_ID_PARAM_VALUE) {
					mavlink_param_value_t param;
					mavlink_msg_param_value_decode(&_message, &param);
					ESP_LOGD("PARAM_VALUE","sysid=%d compid=%d seq=%d",_message.sysid, _message.compid, _message.seq);
					ESP_LOGI("PARAM_VALUE","param_id=[%16s] param_value=%f param_type=%d param_count=%d param_index=%d",param.param_id, param.param_value, param.param_type, param.param_count, param.param_index);

				// #23
				} else if (_message.msgid ==  MAVLINK_MSG_ID_PARAM_SET) {
					mavlink_param_set_t param;
					mavlink_msg_param_set_decode(&_message, &param);
					ESP_LOGI("PARAM_SET","sysid=%d compid=%d seq=%d",_message.sysid, _message.compid, _message.seq);
					ESP_LOGI("PARAM_SET","target_system=%d target_component=%d param_id=[%16s] param_value=%f param_type=%d",param.target_system, param.target_component, param.param_id, param.param_value, param.param_type);



				// #109
				} else if (_message.msgid ==  MAVLINK_MSG_ID_RADIO_STATUS) {
					mavlink_radio_status_t param;
					mavlink_msg_radio_status_decode(&_message, &param);
					ESP_LOGD("RADIO_STATUS","sysid=%d compid=%d seq=%d",_message.sysid, _message.compid, _message.seq);
					ESP_LOGI("RADIO_STATUS","rssi=%d remrssi=%d txbuf=%d noise=%d remnoise=%d rxerrors=%d fixed=%d",param.rssi, param.remrssi, param.txbuf, param.noise, param.remnoise, param.rxerrors, param.fixed);


				// #147
				} else if (_message.msgid ==  MAVLINK_MSG_ID_BATTERY_STATUS) {
					mavlink_battery_status_t param;
					mavlink_msg_battery_status_decode(&_message, &param);
					ESP_LOGI("BATTERY_STATUS","battery_remaining=%d voltages=%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
						param.battery_remaining, param.voltages[0], param.voltages[1], param.voltages[2], param.voltages[3], param.voltages[4],
						param.voltages[5], param.voltages[6], param.voltages[7], param.voltages[8], param.voltages[9]);
				}

			} else if (msgReceived == 2) {
				ESP_LOGD(TAG,"_message.msgid=%d _message.compid=%d", _message.msgid, _message.compid);
			}

		}
	}

	/* close socket. Don't reach here. */
	ret = lwip_close(fd);
	LWIP_ASSERT("ret == 0", ret == 0);
	vTaskDelete( NULL );
}
