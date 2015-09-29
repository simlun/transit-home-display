#include "EventBusHandlers.h"
#include "constants.h"

WPA2ConnectHandler::WPA2ConnectHandler(EventBus * eventBus,
                                       WiFi * wifi)
    : EventHandler(eventBus),
      wifi(wifi) {}

Event WPA2ConnectHandler::event() {
    return WPA2;
}

void WPA2ConnectHandler::handle() {
    wifi->wpa2Connect();
}
