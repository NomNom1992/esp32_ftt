#include "mqttService.h"

char topic_sub[TOTAL_ID_SUB][LENGHT_TOPIC_SUB];
char topic_pub[TOTAL_ID_PUB][LENGHT_TOPIC_PUB];
String client_id = "";

void mqtt_init_topic_pub() {
    sprintf(topic_pub[TOPIC_PING_ID], "payment/device/%s/ping", client_id);
    sprintf(topic_pub[TOPIC_CASH_ID], "payment/device/%s/cash", client_id);
    sprintf(topic_pub[TOPIC_RESPONSE_ID], "payment/device/%s/response", client_id);
    sprintf(topic_pub[TOPIC_QR_ID], "payment/device/%s/qr", client_id);
    sprintf(topic_pub[TOPIC_CONFIG_ID], "payment/device/%s/config", client_id);
    sprintf(topic_pub[TOPIC_DEBUG_ID], "payment/device/%s/debug", client_id);
}

void mqtt_init_topic_sub() {
    sprintf(topic_sub[TOPIC_SUB_PING], "payment/server/%s/ping", client_id);
    sprintf(topic_sub[TOPIC_SUB_QR], "payment/server/%s/qr", client_id);
    sprintf(topic_sub[TOPIC_SUB_CONTROL], "payment/server/%s/control", client_id);
    sprintf(topic_sub[TOPIC_SUB_UPDATE], "payment/server/%s/update", client_id);
    sprintf(topic_sub[TOPIC_SUB_CONFIG], "payment/server/%s/config", client_id);
    sprintf(topic_sub[TOPIC_SUB_DEBUG], "payment/server/%s/debug", client_id);
}




void initMQTTClient_andSubTopic(PubSubClient* mqttClient) {

  mqttClient->setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient->setCallback(mqttCallback);
  mqtt_init_topic_sub();
  mqtt_init_topic_pub();

  while (!mqttConnect(mqttClient)) {
      Serial.println("Attempting to connect to MQTT...");
      Serial.println(client_id);

      reconnectMQTT(mqttClient);
      delay(1000);  // Wait for 1 second before trying again
    }
    
    // Tạo tin nhắn login
    // String message = getTime() + String(client_id) + " online";
    // publishData(mqttClient, "espLTN/onoff", message);
    // Serial.println(message);
}

int mqttConnect(PubSubClient* mqttClient)
{
  if(mqttClient->connected())
  return 1;
  else 
  return 0;
}

void reconnectMQTT(PubSubClient* mqttClient) {
  while (!mqttClient->connected()) {
    if (mqttClient->connect(client_id.c_str())) {
      Serial.println("Connected to MQTT Broker");

      //vòng lặp sub vào các topic
      for(size_t i = 0; i < TOTAL_ID_SUB; i++){
        Serial.println(topic_sub[i]);
        mqttClient -> subscribe(topic_sub[i]);
      }

      mqttClient -> subscribe("espLTN/onoff");
    } else {
      Serial.print("Failed to connect to MQTT Broker, rc=");
      Serial.println(mqttClient->state());
      delay(1000);
    }
  }
}

void publishData(PubSubClient* mqttClient, const char* topic, String data) {
  mqttClient->publish(topic, data.c_str());
}


void mqttLoop(PubSubClient* mqttClient)
{
  mqttClient->loop();
}

// String toHexString(String data, size_t length) {
//   String hexString = "";
//   for (size_t i = 0; i < length; i++) {
//     char hexChar[3];
//     sprintf(hexChar, "%02X", data[i]);
//     hexString += hexChar;
//   }
//   return hexString;
// }
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Xử lý dữ liệu nhận được từ MQTT
  String receivedData = String((char*)payload, length);

  if (strcmp(topic, "espLTN/onoff") == 0) {
    if (receivedData.startsWith("on")) {
      lcdPort.write(data_on, sizeof(data_on));
      Serial.println("LED on");
    } else if (receivedData.startsWith("off")) {
      lcdPort.write(data_off, sizeof(data_off));
      Serial.println("LED off");
    }
  } else if (strcmp(topic, "espLTN/qr") == 0) {
    const int MAX_MESSAGE_LENGTH = 200; // Điều chỉnh dựa trên độ dài tối đa dự kiến của tin nhắn
    byte messageArray[MAX_MESSAGE_LENGTH + 8];

    // Xây dựng mảng tin nhắn
    messageArray[0] = 0x5A;
    messageArray[1] = 0xA5;
    messageArray[2] = length + 5;
    messageArray[3] = 0x82;
    messageArray[4] = 0x52;
    messageArray[5] = 0x40;

    // Sao chép payload vào mảng
    memcpy(&messageArray[6], payload, min(length, (unsigned int)(MAX_MESSAGE_LENGTH - 2)));

    // Thêm byte kết thúc
    int arrayIndex = min(length + 6, (unsigned int)(MAX_MESSAGE_LENGTH + 6));
    messageArray[arrayIndex++] = 0xFF;
    messageArray[arrayIndex++] = 0xFF;

    lcdPort.write(clear_qr, sizeof(clear_qr));
    lcdPort.write(messageArray, arrayIndex);
    Serial.write(messageArray, arrayIndex);
  }
}

