#include "Mailbox_Manager.h"
#include "Process_Manager.h"
#include "Event.h"

#include <thread>
#include <chrono>

int main()
{
	Process_Manager process_manager;

	Mailbox_Manager mailbox_manager;

	Event event_from_server;
	event_from_server.create("event_from_server");
	if(!event_from_server.valid())
		return 1;

	mailbox_manager.create_mailbox("\\\\.\\mailslot\\CLIENT_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	process_manager.create_process("../Server/debug/Server.exe");
	if(process_manager.last_error())
		return 1;

	event_from_server.wait();

	Event event_from_client;
	event_from_client.connect("event_from_client");
	if(!event_from_client.valid())
		return 1;

	mailbox_manager.connect_to_mailbox("\\\\.\\mailslot\\SERVER_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	event_from_client.activate();

//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	event_from_server.wait();

	std::vector<std::string> messages = mailbox_manager.get_messages();
	if(mailbox_manager.last_error())
		return 1;

	for(unsigned int i=0; i<messages.size(); ++i)
		std::cout << messages[i] << "\n";

	event_from_client.activate();

	process_manager.wait_for_process_end();

	event_from_client.abandon();
	event_from_server.abandon();

	return 0;
}

