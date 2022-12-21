#include "Mailbox_Manager.h"

#include <thread>
#include <chrono>

int main()
{
	Mailbox_Manager mailbox_manager;

	mailbox_manager.connect_to_mailbox("\\\\.\\mailslot\\CLIENT_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	mailbox_manager.create_mailbox("\\\\.\\mailslot\\SERVER_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

//	std::this_thread::sleep_for(std::chrono::seconds(2));

	mailbox_manager.send_message("test 1");
	mailbox_manager.send_message("test 2");
	mailbox_manager.send_message("test 3");
	mailbox_manager.send_message("test 4");
	mailbox_manager.send_message("test 5");

	std::this_thread::sleep_for(std::chrono::seconds(2));
	
	return 0;
}
