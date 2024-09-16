#include "global.h"
#include <EEPROM.h>

void system_config_server(String command) {
  String serverAddress = command.substring(7); // Bỏ qua 7 ký tự đầu "server:"
  serverAddress.trim(); // Loại bỏ khoảng trắng thừa ở đầu và cuối
  
  if (serverAddress.length() > 0) {
    strncpy(systemManager.server, serverAddress.c_str(), sizeof(systemManager.server) - 1);
    systemManager.server[sizeof(systemManager.server) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
    
    Serial.print("Server address set to: ");
    Serial.println(systemManager.server);
  } else {
    Serial.println("Invalid server address");
  }
}

