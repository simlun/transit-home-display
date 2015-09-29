#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <Arduino.h>

#include <StandardCplusplus.h>
#include <map>
#include <queue>

typedef byte Event;

class EventBus;

class EventHandler {
    protected:
        EventBus * eventBus;
    public:
        EventHandler(EventBus *);
        virtual Event event() = 0;
        virtual void handle() = 0;
};

class EventBus {
    private:
        std::map<Event, EventHandler *> eventHandlers;
        std::queue<Event> eventQueue;
    public:
        void registerHandler(EventHandler *);
        void post(Event);
        void process();
};

#endif
