#ifndef MAILBOX_MANAGER_H
#define MAILBOX_MANAGER_H

#include <iostream>
#include <vector>

#include <windows.h>

#include "Utility.h"

class Mailbox_Manager
{
private:
	HANDLE m_read_mailbox = nullptr;
	HANDLE m_write_mailbox = nullptr;

	unsigned long int m_last_error = 0;

public:
	Mailbox_Manager();
	~Mailbox_Manager();

public:
	void create_mailbox(const std::string& _name);
	void connect_to_mailbox(const std::string& _name);
	void close_mailbox();
	void disconnect();

public:
	unsigned long int last_error() const;

public:
	std::vector<std::string> get_messages();
	void send_message(const std::string& _msg);

};

#endif
