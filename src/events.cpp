#include "events.h"

EventDispatcher* EventDispatcher::instance() {
	EventDispatcher* inst = new EventDispatcher();
	return inst;
}

void EventDispatcher::init() {
    subQueue = std::vector<Subscriber*>();
}

void EventDispatcher::AddSubscriber(Subscriber* sub) {
    this->subQueue.push_back(sub);
}
