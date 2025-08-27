#pragma once

#include <Arduino.h>
#include <WiFi.h>


void setupWifi();

void sendImageWifi(uint16_t* frame, size_t len);
void reconnect();


