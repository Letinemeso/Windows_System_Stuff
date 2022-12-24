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

	event_from_client.wait();

	memory.set("file.txt");

	event_from_server.activate();

	event_from_client.wait();

	memory.set("-");

	event_from_server.activate();

	event_from_client.wait();

	event_from_client.abandon();
	event_from_server.abandon();

	memory.stop_sharing();
	
	return 0;
}
