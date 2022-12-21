#include "Process_Manager.h"
#include "Pipe_Manager.h"

int main()
{
	Pipe_Manager pipe_manager;

	Process_Manager process_manager;

	process_manager.create_process("C:/Users/User/Desktop/executable/client.exe");

	if (process_manager.last_error())
		return 1;

	pipe_manager.create_pipe("\\\\.\\pipe\\$MyPipe$");

	if (pipe_manager.last_error())
		return 1;

	pipe_manager.send_message("file.txt");

	if (pipe_manager.last_error())
		return 1;

	pipe_manager.send_message("-");

	if (pipe_manager.last_error())
		return 1;

	process_manager.wait_for_process_end();

	if (process_manager.last_error())
		return 1;

	pipe_manager.close_pipe();
	
	return 0;
}
