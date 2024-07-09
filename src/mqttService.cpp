#include "mqttService.h"

void initMQTTClient_andSubTopic(PubSubClient* mqttClient) {

  mqttClient->setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient->setCallback(mqttCallback);

  while (!mqttConnect(mqttClient)) {
      Serial.println("Attempting to connect to MQTT...");
      reconnectMQTT(mqttClient);
      delay(1000);  // Wait for 1 second before trying again
    }
    
    // Tạo tin nhắn và xuất bản lên MQTT
    String message = String(MQTT_CLIENT_ID) + " online";
    publishData(mqttClient, "espLTN/onoff", message);
    Serial.println(message);
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
    if (mqttClient->connect(MQTT_CLIENT_ID.c_str() , MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT Broker");
      // SerialPort.println("Connected to MQTT Broker");
      mqttClient->subscribe("espLTN/onoff");
      mqttClient->subscribe("espLTN/qr");
    } else {
      Serial.print("Failed to connect to MQTT Broker, rc=");
      Serial.println(mqttClient->state());
      delay(2000);
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

  // const byte prefix[] = {0x5A, 0xA5, 0xA9, 0x82, 0x52, 0x40};
  // const byte suffix[] = {0xFF, 0xFF};
  // unsigned int hexLength = sizeof(prefix) + receivedData.length() + sizeof(suffix);
  // byte* hexArray = new byte[hexLength];
  
  // // Sao chép các byte mở đầu vào mảng hex
  // memcpy(hexArray, prefix, sizeof(prefix));
  
  // // Chuyển từng byte của message thành mã hex và lưu vào mảng
  // for (unsigned int i = 0; i < receivedData.length(); i++) {
  //   hexArray[sizeof(prefix) + i] = receivedData[i];
  // }
  
  // // Sao chép các byte kết thúc vào mảng hex
  // memcpy(hexArray + sizeof(prefix) + receivedData.length(), suffix, sizeof(suffix));

  // lcdPort.write(hexArray, hexLength);
  // Serial.write(hexArray, hexLength);
  
  // // Giải phóng bộ nhớ của mảng hex
  // delete[] hexArray;
  
  


  // Xử lý dữ liệu tại đây
  // ...  
}

