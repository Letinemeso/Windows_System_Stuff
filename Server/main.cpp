#include "Process_Manager.h"
#include "Pipe_Manager.h"

int main()
{
	Pipe_Manager pipe_manager;

	Process_Manager process_manager;

	process_manager.create_process("../Client/debug/Client.exe");

	if (process_manager.last_error())
		return 1;

	pipe_manager.create_pipe("\\\\.\\pipe\\$MyPipe$");

	if (pipe_manager.last_error())
		return 1;

	std::string file_name;
	std::cin >> file_name;

	pipe_manager.send_message(file_name);

	if (pipe_manager.last_error())
		return 1;

	char symbol_to_insert = 0;
	std::cin >> symbol_to_insert;
	std::string sti_str;
	sti_str += symbol_to_insert;

	pipe_manager.send_message(sti_str);

	if (pipe_manager.last_error())
		return 1;

	process_manager.wait_for_process_end();

	if (process_manager.last_error())
		return 1;

	pipe_manager.close_pipe();
	
	return 0;
}
