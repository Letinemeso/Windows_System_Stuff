#include "Process_Manager.h"
#include "Event.h"
#include "Shared_Memory.h"

#include <thread>
#include <chrono>

int main()
{

	while(true)
	{
		Shared_Memory m;
		m.allocate("id", 256);
		m.stop_sharing();
	}

//	Process_Manager process_manager;

//	Event event_from_server;
//	event_from_server.create("event_from_server");
//	if(!event_from_server.valid())
//		return 1;

//	Shared_Memory memory;
//	memory.allocate("memory", 256);
//	if(!memory.valid())
//		return 1;

//	process_manager.create_process("../Server/debug/Server.exe");
//	if(process_manager.last_error())
//		return 1;

//	event_from_server.wait();

//	Event event_from_client;
//	event_from_client.connect("event_from_client");
//	if(!event_from_client.valid())
//		return 1;

//	event_from_client.activate();

//	event_from_server.wait();

//	std::string data = memory.get();
//	if(!memory.valid())
//		return 1;

//	std::cout << data << "\n";

//	event_from_client.activate();

//	process_manager.wait_for_process_end();

//	event_from_client.abandon();
//	event_from_server.abandon();

	return 0;
}
