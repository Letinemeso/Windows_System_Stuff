#include "Process_Manager.h"

Process_Manager::Process_Manager()
{
	ZeroMemory(&m_process_info, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&m_startup_info, sizeof(STARTUPINFO));
}

Process_Manager::~Process_Manager()
{
	if (m_process_handle == nullptr)
		return;

	CloseHandle(m_process_info.hProcess);
	CloseHandle(m_process_info.hThread);
	TerminateProcess(m_process_info.hProcess, 0);
}

void Process_Manager::create_process(const std::string& _executable_name)
{
	WCHAR* name = WSS_Utility::convert_to_wide_char(_executable_name);

	if (!CreateProcess(NULL, name, NULL, NULL, TRUE, NULL, NULL, NULL, &m_startup_info, &m_process_info))
	{
		ZeroMemory(&m_process_info, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&m_startup_info, sizeof(STARTUPINFO));

		m_last_error = GetLastError();

		delete[] name;
		return;
	}

	delete[] name;
	m_process_handle = m_process_info.hProcess;
}

void Process_Manager::wait_for_process_end()
{
	if (m_process_handle == nullptr)
		return;

	WaitForSingleObject(m_process_handle, INFINITE);
	bool closed_correctly = GetExitCodeProcess(m_process_handle, &m_prev_process_return_status);

	ZeroMemory(&m_process_info, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&m_startup_info, sizeof(STARTUPINFO));
	m_process_handle = nullptr;

	if (closed_correctly)
		m_last_error = 0;
	else
		m_last_error = GetLastError();
}

void Process_Manager::force_close_process()
{
	if (m_process_handle == nullptr)
		return;

	bool closed_correctly = true;
	closed_correctly = closed_correctly && CloseHandle(m_process_handle);
	closed_correctly = closed_correctly && CloseHandle(m_process_info.hThread);
	closed_correctly = closed_correctly && TerminateProcess(m_process_handle, 0);

	ZeroMemory(&m_process_info, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&m_startup_info, sizeof(STARTUPINFO));
	m_process_handle = nullptr;

	if (closed_correctly)
		m_last_error = 0;
	else
		m_last_error = GetLastError();
}



unsigned long int Process_Manager::last_error() const
{
	return m_last_error;
}

unsigned long int Process_Manager::prev_return_status() const
{
	return m_prev_process_return_status;
}
