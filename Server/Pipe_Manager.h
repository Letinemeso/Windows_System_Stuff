#ifndef PIPE_MANAGER_H
#define PIPE_MANAGER_H

#include <iostream>

#include <windows.h>

class Pipe_Manager
{
private:
	HANDLE m_pipe = nullptr;
	unsigned int m_max_message_length = 512;

	unsigned long int m_last_error = 0;

public:
	Pipe_Manager();
	~Pipe_Manager();

public:
	void create_pipe(const std::string& _name);
	void connect_to_pipe(const std::string& _name);
	void close_pipe();

	void set_max_message_length(unsigned int _value);

public:
	unsigned long int last_error() const;

public:
	std::string get_message();
	void send_message(const std::string& _msg);

};

#endif