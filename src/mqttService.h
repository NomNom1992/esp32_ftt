#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <PubSubClient.h>
#include "global.h"
#include "ntp.h"
/***********************************************/
// #define MQTT_BROKER "mqtt.thanhtoanqrcode.vn"
// #define MQTT_PORT 1883

#define MQTT_BROKER "mqtt.thanhtoanqrcode.vn"
#define MQTT_PORT 1883


#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
/**********************************************/
#define LENGHT_TOPIC_PUB 128
#define LENGHT_TOPIC_SUB 128

//TOPIC ID
#define TOTAL_ID_PUB    7

#define TOPIC_CASH_ID       0
#define TOPIC_RESPONSE_ID   1
#define TOPIC_PING_ID       2
#define TOPIC_QR_ID         3
#define TOPIC_CONFIG_ID     4
#define TOPIC_DEBUG_ID      5
#define TOPIC_HOST_ID       6
#define TOPIC_LOGCASH_ID    7

extern char topic_pub[TOTAL_ID_PUB][LENGHT_TOPIC_PUB];

#define TOTAL_ID_SUB    6

#define TOPIC_SUB_QR         0
#define TOPIC_SUB_PING       1
#define TOPIC_SUB_CONTROL    2
#define TOPIC_SUB_UPDATE     3
#define TOPIC_SUB_CONFIG     4
#define TOPIC_SUB_DEBUG      5
#define TOPIC_SUB_HOST       6

extern char topic_sub[TOTAL_ID_SUB][LENGHT_TOPIC_SUB];

/**********************************************/
extern String client_id;

void mqtt_init_topic_pub();
void mqtt_init_topic_sub();

void initMQTTClient_andSubTopic(PubSubClient* mqttClient);
int mqttConnect(PubSubClient* mqttClient);
void reconnectMQTT(PubSubClient* mqttClient);
void mqttLoop(PubSubClient* mqttClient);
void publishData(PubSubClient* mqttClient, const char* topic, String data);
void mqttCallback(char* topic, byte* payload, unsigned int length);


#endif