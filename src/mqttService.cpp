#include "mqttService.h"
#include <EEPROM.h>
#include "gpio.h"

char topic_sub[TOTAL_ID_SUB][LENGHT_TOPIC_SUB];
char topic_pub[TOTAL_ID_PUB][LENGHT_TOPIC_PUB];
String client_id = "";
int interval_30 = 30;
unsigned long last_pub_login = 0;


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
  } 
  // SẼ TÁCH THÀNH HÀM QR RIÊNG
  else if (strcmp(topic, topic_sub[TOPIC_SUB_QR]) == 0) {
    //xóa qr cũ trên lcd
    lcdPort.write(clear_qr, sizeof(clear_qr));
    // Tạo buffer để lưu chuỗi JSON
    char json[length + 1];
    memcpy(json, payload, length);
    json[length] = '\0';

    JsonDocument doc; // Kích thước có thể điều chỉnh tùy theo độ lớn của JSON
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }    
    // Lấy giá trị "value" từ JSON
    const char* qrValue = doc["value"];
    Serial.println(qrValue);

    if (!qrValue) {
        Serial.println("lỗi khi get qr");
        return;
    }
    size_t qrLength = strlen(qrValue);
    const int MAX_MESSAGE_LENGTH = 200; // Điều chỉnh dựa trên độ dài tối đa dự kiến của tin nhắn
    byte messageArray[MAX_MESSAGE_LENGTH + 8];

    // Xây dựng mảng tin nhắn
    messageArray[0] = 0x5A;
    messageArray[1] = 0xA5;
    messageArray[2] = qrLength + 5;
    messageArray[3] = 0x82;
    messageArray[4] = 0x52;
    messageArray[5] = 0x40;

    // Sao chép giá trị QR vào mảng
    memcpy(&messageArray[6], qrValue, min(qrLength, (size_t)(MAX_MESSAGE_LENGTH - 2)));

    // Thêm byte kết thúc
    int arrayIndex = min(qrLength + 6, (size_t)(MAX_MESSAGE_LENGTH + 6));
    messageArray[arrayIndex++] = 0xFF;
    messageArray[arrayIndex++] = 0xFF;

    lcdPort.write(messageArray, arrayIndex);
    Serial.write(messageArray, arrayIndex);
  }
  // thực hiện sau khi thanh toán thành công
  else if(strcmp(topic, topic_sub[TOPIC_SUB_CONTROL]) == 0){

    char json[length + 1];
    memcpy(json, payload, length);
    json[length] = '\0';

    JsonDocument doc; // Kích thước có thể điều chỉnh tùy theo độ lớn của JSON
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }    
    // Lấy giá trị "value" từ JSON
    const char* strvalue = doc["value"];
    uint8_t intvalue = (uint8_t)atoi(strvalue);
    Serial.println(intvalue);

    if (!intvalue) {
        Serial.println("lỗi khi nhận phản hồi thanh toán");
        return;
    }
    generatePulses_2(intvalue);

  }
}

void mqtt_ping_each_30s(PubSubClient* mqttClient){
    timeClient.update();
    time_now = timeClient.getEpochTime();

    if (time_now - last_pub_login >= interval_30){
      last_pub_login = time_now;
      time_cur = getTime();
      date_cur = getDate();
      char json_buffer_ping[100];
      sprintf(json_buffer_ping, "{\"date\":\"%s\",\"time\":\"%s\",\"value\":\"%s\"}", date_cur, time_cur, client_id);
      publishData(mqttClient, topic_pub[TOPIC_PING_ID], json_buffer_ping);
    }
}


void mqtt_pub_order(PubSubClient* mqttClient){
  byte data[12];
  int length = lcdPort.readBytes(data, 12);

  if (data[4] == 0x55 && data[5] == 0x00 && data[3] == 0x83) {
    uint16_t value = (data[length - 2] << 8) | data[length - 1];
    if (value >= 1 && value < 5) {


        int cashValue;
        EEPROM.get(100 + (value-1) * sizeof(int), cashValue);

        time_cur = getTime();
        date_cur = getDate();

        char json_buffer_qr[100];
        sprintf(json_buffer_qr, "{\"date\":\"%s\",\"time\":\"%s\",\"value\":\"%d\"}", date_cur, time_cur, cashValue);
        publishData(mqttClient, topic_pub[TOPIC_QR_ID], json_buffer_qr);
    }
  }
}
