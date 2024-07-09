#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <PubSubClient.h>
#include "global.h"

#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
// #define MQTT_CLIENT_ID "clientId-zRgpGTt97l"
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#define TOPIC_ONOFF = "espLTN/onoff"
String static MQTT_CLIENT_ID = "";


void initMQTTClient_andSubTopic(PubSubClient* mqttClient);
int mqttConnect(PubSubClient* mqttClient);
void reconnectMQTT(PubSubClient* mqttClient);
void mqttLoop(PubSubClient* mqttClient);
void publishData(PubSubClient* mqttClient, const char* topic, String data);
void mqttCallback(char* topic, byte* payload, unsigned int length);


#endif