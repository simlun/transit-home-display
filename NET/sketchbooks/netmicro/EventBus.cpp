#include "debug.h"
#include "EventBus.h"

EventHandler::EventHandler(EventBus * eventBus) : eventBus(eventBus) {}

void EventBus::registerHandler(EventHandler * handler) {
    DEBUG Serial.println(F("Registering event bus event handler"));
    eventHandlers[handler->event()] = handler;
}

void EventBus::post(Event event) {
    eventQueue.push(event);
}

void EventBus::process() {
    if (!eventQueue.empty()) {
        DEBUG Serial.println(F("Found an event on the queue"));
        Event event = eventQueue.front();
        eventQueue.pop();
        if (eventHandlers.count(event) == 1) {
            DEBUG Serial.println(F("Calling event handler"));
            EventHandler * handler = eventHandlers[event];
            handler->handle();
        }
    }
}
