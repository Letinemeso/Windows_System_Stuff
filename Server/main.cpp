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

	mailbox_manager.send_message("test 1");
	mailbox_manager.send_message("test 2");
	mailbox_manager.send_message("test 3");
	mailbox_manager.send_message("test 4");
	mailbox_manager.send_message("test 5");

	std::cout << "Server: messages sent\n";

//	std::this_thread::sleep_for(std::chrono::seconds(2));

	event_from_server.activate();

//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	event_from_client.wait();

	std::cout << "Server: exiting\n";

	event_from_client.abandon();
	event_from_server.abandon();
	
	return 0;
}
