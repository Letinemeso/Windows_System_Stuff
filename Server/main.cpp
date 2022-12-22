#include "Event.h"
#include "Shared_Memory.h"

#include <thread>
#include <chrono>

int main()
{
	Shared_Memory memory;
	memory.use_memory("memory", 256);
	if(!memory.valid())
		return 1;

	Event event_from_server;
	event_from_server.connect("event_from_server");
	if(!event_from_server.valid())
		return 1;

	Event event_from_client;
	event_from_client.create("event_from_client");
	if(!event_from_client.valid())
		return 1;

	event_from_server.activate();

//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	event_from_client.wait();

	memory.set("test 1");

//	std::this_thread::sleep_for(std::chrono::seconds(2));

	event_from_server.activate();

//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	event_from_client.wait();

	event_from_client.abandon();
	event_from_server.abandon();
	
	return 0;
}
