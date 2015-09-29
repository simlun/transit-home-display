#include "EventBus.h"

EventHandler::EventHandler(EventBus * eventBus) : eventBus(eventBus) {}

void EventBus::registerHandler(EventHandler * handler) {
    eventHandlers[handler->event()] = handler;
}

void EventBus::post(Event event) {
    eventQueue.push(event);
}

void EventBus::process() {
    if (!eventQueue.empty()) {
        Event event = eventQueue.front();
        eventQueue.pop();
        if (eventHandlers.count(event) == 1) {
            EventHandler * handler = eventHandlers[event];
            handler->handle();
        }
    }
}
