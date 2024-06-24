#include "updateFirmware.h"

void checkFirmwareUpdate(String *host,String *path, String *currentVersion) {
  HTTPClient http;
  // String firmwareURL =*host + *path;
  String latestVersion = getLatestVersion(path);

  if (latestVersion.compareTo(*currentVersion) > 0) {
    Serial.println("New firmware version available: " + latestVersion);
    // updateFirmware(firmwareURL);

    const char* hostCStr = host->c_str();
    const char* pathCStr = path->c_str();

    if (!SPIFFS.begin(true)) {
      Serial.println("SPIFFS Mount Failed");
      return;
    }
    getFileFromServer(hostCStr, pathCStr);
    performOTAUpdateFromSPIFFS();
  } else {
    Serial.println("Current firmware is up to date.");
  }
}

String getLatestVersion(String *firmwareURL) {
  // Tìm vị trí của "version_" trong URL
  int versionIndex = firmwareURL->lastIndexOf("version_");
  
  if (versionIndex != -1) {
    // Tìm vị trí của ".bin" sau "version_"
    int dotIndex = firmwareURL->lastIndexOf(".bin");
    
    if (dotIndex != -1 && dotIndex > versionIndex) {
      // Trích xuất chuỗi phiên bản từ URL
      String version = firmwareURL->substring(versionIndex + 8, dotIndex);
      return version;
    }
  }
  
  return "";
}

void getFileFromServer(const char* host, const char* path) {
  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect(host, PORT)) {
    Serial.println("Connected to server");
    client.print("GET " + String(path) + " HTTP/1.1\r\n");
    client.print("Host: " + String(host) + "\r\n");
    client.println("Connection: close\r\n");
    client.println();

    File file = SPIFFS.open("/" + String(FILE_NAME), FILE_WRITE);
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }

    bool endOfHeaders = false;
    String headers = "";
    String http_response_code = "error";
    const size_t bufferSize = 1024;
    uint8_t buffer[bufferSize];

    while (client.connected() && !endOfHeaders) {
      if (client.available()) {
        char c = client.read();
        headers += c;
        if (headers.startsWith("HTTP/1.1")) {
          http_response_code = headers.substring(9, 12);
        }
        if (headers.endsWith("\r\n\r\n")) {
          endOfHeaders = true;
        }
      }
    }

    Serial.println("HTTP response code: " + http_response_code);

    while (client.connected()) {
      if (client.available()) {
        size_t bytesRead = client.readBytes(buffer, bufferSize);
        file.write(buffer, bytesRead);
      }
    }
    file.close();
    client.stop();
    Serial.println("File saved successfully");
  }
  else {
    Serial.println("Failed to connect to server");
  }
}

void performOTAUpdateFromSPIFFS() {
  File file = SPIFFS.open("/" + String(FILE_NAME), FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Starting update..");
  size_t fileSize = file.size();
  Serial.println(fileSize);

  if (!Update.begin(fileSize, U_FLASH)) {
    Serial.println("Cannot do the update");
    return;
  }

  Update.writeStream(file);

  if (Update.end()) {
    Serial.println("Successful update");
  }
  else {
    Serial.println("Error Occurred:" + String(Update.getError()));
    return;
  }

  file.close();
  Serial.println("Reset in 4 seconds....");
  delay(4000);
  ESP.restart();
}