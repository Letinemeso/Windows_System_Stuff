#ifndef EVENT_H
#define EVENT_H

#include <string>

#include <windows.h>

#include "Utility.h"

class Event
{
private:
	HANDLE m_event_handle = nullptr;
	std::string m_event_name;

	unsigned int m_last_error = 0;

public:
	Event();
	~Event();

public:
	void create(const std::string& _name);
	void connect(const std::string& _name);

	void abandon();

public:
	void wait();
	void activate();

public:
	bool valid() const;
	unsigned int last_error() const;

};

#endif // EVENT_H
