#include "wifiCom.h"

const char* ssid = "Redmi";
const char* password = "Ljhki23132";

// Socket TCP
WiFiServer server(8080);
WiFiClient client;

uint16_t header = 0xABCD;
uint16_t footer = 0xDCBA;



void setupWifi(){
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connecté");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    server.begin();
}

void reconnect(){
    if (!client || !client.connected()) {
        client = server.available();
        Serial.println("client is not available");
        delay(1000);
        return;
    }
}

void sendImageWifi(uint16_t* frame, size_t len){
    size_t totalSize = sizeof(header) + len + sizeof(footer);
    uint8_t* buffer = (uint8_t*) malloc(totalSize);

    if (buffer) {
        memcpy(buffer, &header, sizeof(header));              // header = 2 octets
        memcpy(buffer + sizeof(header), frame, len);          // image
        memcpy(buffer + sizeof(header) + len, &footer, sizeof(footer)); // footer

        client.write(buffer, totalSize);
        free(buffer);
    }
}
