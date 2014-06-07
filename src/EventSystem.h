#pragma once

//Implmentation adapted from GCC4
//Stripped multithreading code and multiple Event queue support since they were not needed

#include <strstream>
#include <ostream>
#include <istream>
#include <memory>
#include <list>
#include <map>
#include "..\3rdParty\include\FastDelegate\FastDelegate.h"

#ifndef EVENT_CAST
#define EVENT_CAST(type, ptr) static_pointer_cast<type>(ptr)
#endif

class IEventData;

typedef unsigned long EventType;
typedef std::shared_ptr<IEventData> IEventDataPtr;
typedef fastdelegate::FastDelegate1<IEventDataPtr> EventListenerDelegate;

class IEventData
{
public:
	virtual ~IEventData() {}
	virtual const EventType& getEventType() const = 0;
	virtual float getTimeStamp() const = 0;
	//virtual void Serialize(std::ostrstream& out) const = 0;
    //virtual void Deserialize(std::istrstream& in) = 0;
	virtual IEventDataPtr copy() const = 0;
    virtual const char* getName() const = 0;
};

class BaseEventData : public IEventData
{
    const float m_timeStamp;

public:
	explicit BaseEventData(const float timeStamp = 0.0f) : m_timeStamp(timeStamp) { }

	// Returns the type of the event
	virtual const EventType& getEventType() const = 0;

	virtual float getTimeStamp() const { return m_timeStamp; }

	// Serializing for network input / output
	//virtual void Serialize(std::ostrstream &out) const	{ }
    //virtual void Deserialize(std::istrstream& in) { }
};

class EventSystem
{
    typedef std::list<EventListenerDelegate> EventListenerList;
    typedef std::map<EventType, EventListenerList> EventListenerMap;
    typedef std::list<IEventDataPtr> EventQueue;

	EventListenerMap mEventListeners;
    EventQueue mQueue;

	static EventSystem* mpInstance;

public:
	EventSystem();
	~EventSystem();

	static EventSystem* get();

    bool addListener(const EventType& type, const EventListenerDelegate& eventDelegate);
    bool removeListener(const EventType& type, const EventListenerDelegate& eventDelegate);

    bool triggerEvent(const IEventDataPtr& pEvent) const;
    bool queueEvent(const IEventDataPtr& pEvent);
    //bool abortEvent(const EventType& type, bool allOfType = false);

	void clear();

    bool update(unsigned long maxMillis = 0xffffffff);
};
