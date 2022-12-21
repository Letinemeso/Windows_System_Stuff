#include "Pipe_Manager.h"

Pipe_Manager::Pipe_Manager()
{

}

Pipe_Manager::~Pipe_Manager()
{
	if (m_pipe == nullptr)
		return;
	CloseHandle(m_pipe);
}



void Pipe_Manager::create_pipe(const std::string& _name)
{
	close_pipe();

	WCHAR* name = new WCHAR[_name.size() + 1];
	name[_name.size()] = 0;
	for (unsigned int i = 0; i < _name.size(); ++i)
		name[i] = _name[i];

	m_pipe = CreateNamedPipe(
		name,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		512, 512, 5000, NULL);

	delete[] name;

	m_last_error = GetLastError();

	if (m_pipe == INVALID_HANDLE_VALUE)
	{
		m_pipe = nullptr;
		return;
	}

	bool connected = ConnectNamedPipe(m_pipe, NULL);

	if (!connected)
	{
		close_pipe();
		m_last_error = GetLastError();
	}
}

void Pipe_Manager::connect_to_pipe(const std::string& _name)
{
	close_pipe();

	WCHAR* name = new WCHAR[_name.size() + 1];
	name[_name.size()] = 0;
	for (unsigned int i = 0; i < _name.size(); ++i)
		name[i] = _name[i];

	m_pipe = CreateFile(
		name, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);

	delete[] name;

	m_last_error = GetLastError();

	if (m_pipe == INVALID_HANDLE_VALUE)
		m_pipe = nullptr;
}

void Pipe_Manager::close_pipe()
{
	m_last_error = 0;

	if (m_pipe != nullptr)
	{
		CloseHandle(m_pipe);
		m_pipe = nullptr;
	}
}


void Pipe_Manager::set_max_message_length(unsigned int _value)
{
	m_max_message_length = _value;
}



unsigned long int Pipe_Manager::last_error() const
{
	return m_last_error;
}



std::string Pipe_Manager::get_message()
{
	if (m_pipe == nullptr)
		return "";

	char* msg_raw = new char[m_max_message_length];
	unsigned long int recieved_msg_length = 0;
	bool recieved_something = ReadFile(m_pipe, msg_raw, m_max_message_length, &recieved_msg_length, 0);

	if (!recieved_something)
	{
		close_pipe();
		delete[] msg_raw;
		m_last_error = GetLastError();
		return "";
	}

	msg_raw[recieved_msg_length] = 0;
	std::string result = msg_raw;

	delete[] msg_raw;
	return  result;
}

void Pipe_Manager::send_message(const std::string& _msg)
{
	if (m_pipe == nullptr)
		return;

	unsigned long int sent_length = 0;
	bool sent_something = WriteFile(m_pipe, _msg.c_str(), _msg.size() + 1, &sent_length, 0);

	if (!sent_something)
	{
		close_pipe();
		m_last_error = GetLastError();
		return;
	}
}