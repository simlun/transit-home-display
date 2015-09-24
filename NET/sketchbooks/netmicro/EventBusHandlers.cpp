#include "EventBusHandlers.h"
#include "constants.h"

WPA2ConnectHandler::WPA2ConnectHandler(EventBus * eventBus) : EventHandler(eventBus) {}

Event WPA2ConnectHandler::event() {
    return WPA2;
}

void WPA2ConnectHandler::handle() {
    Serial.println("Connecting to WPA2 WiFi");
}
