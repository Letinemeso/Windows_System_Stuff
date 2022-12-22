#include "Event.h"


Event::Event()
{

}

Event::~Event()
{
	abandon();
}



void Event::create(const std::string &_name)
{
	abandon();

	m_event_name = _name;

	wchar_t* name = WSS_Utility::convert_to_wide_char(_name);
	m_event_handle = CreateEvent(NULL, FALSE, FALSE, name);
	delete[] name;

	if(m_event_handle == INVALID_HANDLE_VALUE)
	{
		m_last_error = GetLastError();
		m_event_handle = nullptr;
	}
}

void Event::connect(const std::string &_name)
{
	abandon();

	m_event_name = _name;

	wchar_t* name = WSS_Utility::convert_to_wide_char(_name);
	m_event_handle = OpenEvent(EVENT_ALL_ACCESS, FALSE, name);
	delete[] name;

	if(m_event_handle == INVALID_HANDLE_VALUE)
	{
		m_last_error = GetLastError();
		m_event_handle = nullptr;
	}
}


void Event::abandon()
{
	if(m_event_handle == nullptr)
		return;

	CloseHandle(m_event_handle);
	m_event_handle = nullptr;

	m_event_name.clear();
	m_last_error = 0;
}



void Event::wait()
{
	if(!valid())
		return;

	unsigned long int result = WaitForSingleObject(m_event_handle, INFINITE);

	if (result == WAIT_ABANDONED_0 || result == WAIT_FAILED)
	{
		abandon();
		m_last_error = 6;
		return;
	}

	ResetEvent(m_event_handle);
}

void Event::activate()
{
	if(!valid())
		return;

	if(!SetEvent(m_event_handle))
	{
		abandon();
		m_last_error = 6;
	}
}



bool Event::valid() const
{
	return m_event_handle != nullptr;
}

unsigned int Event::last_error() const
{
	return m_last_error;
}







































