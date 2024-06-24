#include "mqttService.h"

void initMQTTClient(PubSubClient* mqttClient) {
  mqttClient->setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient->setCallback(mqttCallback);
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
    if (mqttClient->connect(MQTT_CLIENT_ID , MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT Broker");
      // SerialPort.println("Connected to MQTT Broker");
      mqttClient->subscribe("espLTN/onoff");
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
  String receivedData;
  for (unsigned int i = 0; i < length; i++) {
    receivedData += (char)payload[i];
  }
  if(receivedData.startsWith("on")){
    lcdPort.write(data_on, 8);
    Serial.println("led on");

  } else if(receivedData.startsWith("off")){
    lcdPort.write(data_off, 8);
    Serial.println("led off");
  } else {

    const int MAX_MESSAGE_LENGTH = 200; // Adjust this based on your maximum expected message length
    byte messageArray[MAX_MESSAGE_LENGTH + 8];

    messageArray[0] = 0x5A;
    messageArray[1] = 0xA5;
    messageArray[2] = 0xA9;
    messageArray[3] = 0x82;
    messageArray[4] = 0x52;
    messageArray[5] = 0x40;
    
    // Copy payload to array
    int arrayIndex = 6;
    for (size_t i = 0; i < length && arrayIndex < MAX_MESSAGE_LENGTH + 6; i++) {
      messageArray[arrayIndex++] = payload[i];
    }
    
    // Add footer bytes
    messageArray[arrayIndex++] = 0xFF;
    messageArray[arrayIndex++] = 0xFF;
    
    // Print the entire array in hex
    // Serial.print("Message array in HEX: ");
    for (int i = 0; i < 3; i++) {
      // char hex[3];
      // sprintf(hex, "%02X", messageArray[i]);
      lcdPort.write(messageArray, arrayIndex);
      Serial.write(messageArray, arrayIndex);
    }
  // Serial.println();
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

