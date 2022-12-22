#include "Mailbox_Manager.h"

Mailbox_Manager::Mailbox_Manager()
{

}

Mailbox_Manager::~Mailbox_Manager()
{
	disconnect();
	close_mailbox();
}



wchar_t* Mailbox_Manager::M_convert_to_wide_char(const std::string &_str) const
{
	WCHAR* wstr = new WCHAR[_str.size() + 1];
	wstr[_str.size()] = 0;
	for (unsigned int i = 0; i < _str.size(); ++i)
		wstr[i] = _str[i];
	return wstr;
}



void Mailbox_Manager::create_mailbox(const std::string& _name)
{
	close_mailbox();

	WCHAR* name = M_convert_to_wide_char(_name);

	m_read_mailbox = CreateMailslot(name, 0, MAILSLOT_WAIT_FOREVER, NULL);

	delete[] name;

	m_last_error = GetLastError();

	if (m_read_mailbox == INVALID_HANDLE_VALUE)
	{
		m_read_mailbox = nullptr;
		return;
	}
}

void Mailbox_Manager::connect_to_mailbox(const std::string& _name)
{
	disconnect();

	WCHAR* name = M_convert_to_wide_char(_name);

	m_write_mailbox = CreateFile(name, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	delete[] name;

	m_last_error = GetLastError();

	if (m_write_mailbox == INVALID_HANDLE_VALUE)
		m_write_mailbox = nullptr;
}

void Mailbox_Manager::close_mailbox()
{
	m_last_error = 0;

	if (m_read_mailbox != nullptr)
	{
		CloseHandle(m_read_mailbox);
		m_read_mailbox = nullptr;
	}
}

void Mailbox_Manager::disconnect()
{
	m_last_error = 0;

	if (m_write_mailbox != nullptr)
	{
		CloseHandle(m_write_mailbox);
		m_write_mailbox = nullptr;
	}
}



unsigned long int Mailbox_Manager::last_error() const
{
	return m_last_error;
}



std::vector<std::string> Mailbox_Manager::get_messages()
{
	if (m_read_mailbox == nullptr)
		return {};

	unsigned long int next_message_size = 0;
	unsigned long int messages_left = 0;
	bool recieved_something = GetMailslotInfo(m_read_mailbox, nullptr, &next_message_size, &messages_left, nullptr);

	if (!recieved_something)
	{
		close_mailbox();
		m_last_error = GetLastError();
		return {};
	}

	if(messages_left == 0)
		return {};

	std::vector<std::string> result;
	result.resize(messages_left);

	const unsigned long int total_messages = messages_left;
	for(unsigned int i=0; i<total_messages; ++i)
	{
		recieved_something = GetMailslotInfo(m_read_mailbox, nullptr, &next_message_size, &messages_left, nullptr);

		if (!recieved_something)
		{
			close_mailbox();
			m_last_error = GetLastError();
			return {};
		}

		unsigned long int kostyl = 0;
		char* message_raw = new char[next_message_size];
		recieved_something = ReadFile(m_read_mailbox, message_raw, next_message_size, &kostyl, nullptr);

		if (!recieved_something)
		{
			close_mailbox();
			m_last_error = GetLastError();
			return {};
		}

		std::string message = message_raw;
		delete[] message_raw;

		result[i] = (std::string&&)message;
	}

	return result;
}

void Mailbox_Manager::send_message(const std::string& _msg)
{
	if (m_write_mailbox == nullptr)
		return;

	unsigned long int sent_length = 0;
	bool sent_something = WriteFile(m_write_mailbox, _msg.c_str(), _msg.size() + 1, &sent_length, 0);

	if (!sent_something)
	{
		disconnect();
		m_last_error = GetLastError();
	}
}
