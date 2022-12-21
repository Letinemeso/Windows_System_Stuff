#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <iostream>

#include <windows.h>

class Process_Manager
{
private:
	STARTUPINFO m_startup_info;
	PROCESS_INFORMATION m_process_info;
	HANDLE m_process_handle = nullptr;

	unsigned long int m_last_error = 0;
	unsigned long int m_prev_process_return_status = 0;

public:
	Process_Manager();
	~Process_Manager();

public:
	void create_process(const std::string& _executable_name);
	void wait_for_process_end();
	void force_close_process();

public:
	unsigned long int last_error() const;
	unsigned long int prev_return_status() const;

};

#endif