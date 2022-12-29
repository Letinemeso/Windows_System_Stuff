#include "Mailbox_Manager.h"
#include "Event.h"

#include <thread>
#include <chrono>

int main()
{
	Mailbox_Manager mailbox_manager;

	mailbox_manager.connect_to_mailbox("\\\\.\\mailslot\\CLIENT_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	Event event_from_server;
	event_from_server.connect("event_from_server");
	if(!event_from_server.valid())
		return 1;

	Event event_from_client;
	event_from_client.create("event_from_client");
	if(!event_from_client.valid())
		return 1;

	mailbox_manager.create_mailbox("\\\\.\\mailslot\\SERVER_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	event_from_server.activate();

//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	event_from_client.wait();

	std::vector<std::string> messages = mailbox_manager.get_messages();

	std::cout << messages.size();

	std::string buffer = messages[0];
	unsigned int size = buffer.size();
	char symbol_to_insert = messages[1][0];

	unsigned int newline_symbols_count = 0;
	for (unsigned int i = 0; i < size; ++i)
		if (buffer[i] == '\n')
			++newline_symbols_count;

	unsigned int modified_size = size + newline_symbols_count;
	char* modified_buffer = new char[modified_size];

	unsigned int stride = 0;
	for (unsigned int i = 0; i < size; ++i)
	{
		if (buffer[i] == '\r')
		{
			modified_buffer[i + stride] = symbol_to_insert;
			++stride;
		}
		modified_buffer[i + stride] = buffer[i];
	}

//	std::this_thread::sleep_for(std::chrono::seconds(2));

	mailbox_manager.send_message(modified_buffer);

	delete[] modified_buffer;

	event_from_server.activate();

//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	event_from_client.wait();

	event_from_client.abandon();
	event_from_server.abandon();
	
	return 0;
}
