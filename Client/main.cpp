#include "Process_Manager.h"
#include "Event.h"
#include "Shared_Memory.h"

#include <thread>
#include <chrono>

typedef bool(*read_from_file_func)(LPCWSTR, char*&, unsigned int&);
typedef bool(*save_to_file_func)(LPCWSTR, const char*, unsigned int);

int main()
{
	Process_Manager process_manager;

	Event event_from_server;
	event_from_server.create("event_from_server");
	if(!event_from_server.valid())
		return 1;

	Shared_Memory memory;
	memory.allocate("memory", 256);
	if(!memory.valid())
		return 1;

	process_manager.create_process("../Server/debug/Server.exe");
	if(process_manager.last_error())
		return 1;

	event_from_server.wait();

	Event event_from_client;
	event_from_client.connect("event_from_client");
	if(!event_from_client.valid())
		return 1;

	event_from_client.activate();

	event_from_server.wait();

	std::string file_name = memory.get();
	if (!memory.valid())
		return 1;

	event_from_client.activate();

	event_from_server.wait();

	std::string char_to_insert = memory.get();
	if(!memory.valid())
		return 1;

	event_from_client.activate();

	process_manager.wait_for_process_end();

	event_from_client.abandon();
	event_from_server.abandon();

	memory.stop_sharing();

	std::cout << "file_name: " << file_name << "\n";
	std::cout << "char_to_insert: " << char_to_insert << "\n\n";

	char line[80] = {}; //Записываем аргумент, который является названием файла.
	//swprintf(L"%s", argv[1]);
	strcat_s(line, file_name.c_str());
	printf("%s", line);

	wchar_t* inpfile = new wchar_t[80];
	MultiByteToWideChar(CP_ACP, 0, line, -1, inpfile, 80);

	strcat_s(line, ".out");
	wchar_t* outfile = new wchar_t[80];
	MultiByteToWideChar(CP_ACP, 0, line, -1, outfile, 80);

	char symbol_to_insert = char_to_insert[0];

	read_from_file_func read_from_file;
	save_to_file_func save_to_file;

	HINSTANCE dll_handle = LoadLibrary(TEXT("../DLL/debug/DLL.dll"));
	if (dll_handle == NULL) {
		std::cout << "Cannot load library\n";
		return 1;
	}

	read_from_file = (read_from_file_func)GetProcAddress(dll_handle, "read_from_file");
	if (read_from_file == nullptr)
		return 1;
	save_to_file = (save_to_file_func)GetProcAddress(dll_handle, "write_to_file");
	if (save_to_file == nullptr)
		return 1;

	char* buffer = nullptr;
	unsigned int size = 0;
	if (!read_from_file(inpfile, buffer, size))
	{
		std::cout << "error reading file\n";
		FreeLibrary(dll_handle);
		return 1;
	}

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

	delete[] buffer;

	if (!save_to_file(outfile, modified_buffer, modified_size - 1))
	{
		std::cout << "error writing to file\n";
		FreeLibrary(dll_handle);
		delete[] modified_buffer;
		return 1;
	}

	delete[] modified_buffer;

	FreeLibrary(dll_handle);

	return 0;
}
