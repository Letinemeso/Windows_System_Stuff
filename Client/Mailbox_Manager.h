#ifndef MAILBOX_MANAGER_H
#define MAILBOX_MANAGER_H

#include <iostream>
#include <vector>

#include <windows.h>

class Mailbox_Manager
{
private:
	HANDLE m_read_mailbox = nullptr;
	HANDLE m_write_mailbox = nullptr;
	unsigned int m_max_message_length = 512;

	unsigned long int m_last_error = 0;

public:
	Mailbox_Manager();
	~Mailbox_Manager();

private:
	wchar_t* M_convert_to_wide_char(const std::string& _str) const;

public:
	void create_mailbox(const std::string& _name);
	void connect_to_mailbox(const std::string& _name);
	void close_mailbox();
	void disconnect();

	void set_max_message_length(unsigned int _value);

public:
	unsigned long int last_error() const;

public:
	std::vector<std::string> get_messages();
	void send_message(const std::string& _msg);

};

#endif
