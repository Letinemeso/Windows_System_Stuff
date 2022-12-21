#include <Mailbox_Manager.h>
#include <Process_Manager.h>

#include <thread>
#include <chrono>

int main()
{
	Process_Manager process_manager;

	Mailbox_Manager mailbox_manager;

	mailbox_manager.create_mailbox("\\\\.\\mailslot\\CLIENT_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	process_manager.create_process("../Server/debug/Server.exe");
	if(process_manager.last_error())
		return 1;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	mailbox_manager.connect_to_mailbox("\\\\.\\mailslot\\SERVER_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::vector<std::string> messages = mailbox_manager.get_messages();
	if(mailbox_manager.last_error())
		return 1;

	for(unsigned int i=0; i<messages.size(); ++i)
		std::cout << messages[i] << "\n";

	process_manager.wait_for_process_end();

	return 0;
}
