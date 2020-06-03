#pragma once
#include <functional>
#include <vector>


const enum Event_Type{
    EVENT_DIE,
    EVENT_TEST
};



typedef void (SubscriptionFunc(void* data));

class Subscriber {
public:
    std::function<SubscriptionFunc> method;

    enum Event_Type id;
    void init();

};



class EventDispatcher {
public:
	~EventDispatcher();

	static EventDispatcher* instance();

    //disallow copying
	EventDispatcher(const EventDispatcher&);
    EventDispatcher& operator= (const EventDispatcher&);

    void init();
    void AddSubscriber(Subscriber* sub);
    void DispatchEvent(double eventID, void* eventData);
    void Update(double dt);



    //std::vector<std::pair<double,void*>>* dispatchEvents;
    std::vector<Subscriber*> subQueue;
private:
    EventDispatcher() { }
};

