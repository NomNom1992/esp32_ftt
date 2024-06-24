#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <PubSubClient.h>
#include "global.h"

#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "clientId-zRgpGTt97l"
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#define TOPIC_ONOFF = "espLTN/onoff";


const uint8_t data_on[8] = {0x5A, 0xA5, 0x05, 0x82, 0x51, 0x00, 0x00, 0x01};
const uint8_t data_off[8] = {0x5A, 0xA5, 0x05, 0x82, 0x51, 0x00, 0x00, 0x00};


void initMQTTClient(PubSubClient* mqttClient);
int mqttConnect(PubSubClient* mqttClient);
void reconnectMQTT(PubSubClient* mqttClient);
void mqttLoop(PubSubClient* mqttClient);
void publishData(PubSubClient* mqttClient, const char* topic, String data);
void mqttCallback(char* topic, byte* payload, unsigned int length);


#endif