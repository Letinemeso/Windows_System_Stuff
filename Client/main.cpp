#include "Mailbox_Manager.h"
#include "Process_Manager.h"
#include "Event.h"

#include <thread>
#include <chrono>

typedef bool(*read_from_file_func)(LPCWSTR, char*&, unsigned int&);
typedef bool(*save_to_file_func)(LPCWSTR, const char*, unsigned int);

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

	std::string file_name;
	std::cin >> file_name;
	std::string char_to_insert;
	std::cin >> char_to_insert;

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

//	event_from_client.activate();

	mailbox_manager.send_message(buffer);
	mailbox_manager.send_message(char_to_insert);

	delete[] buffer;

	event_from_client.activate();
	event_from_server.wait();

	std::string result = mailbox_manager.get_messages()[0];

	event_from_client.activate();

	mailbox_manager.disconnect();
	mailbox_manager.close_mailbox();

	process_manager.wait_for_process_end();

	event_from_client.abandon();
	event_from_server.abandon();

	if (!save_to_file(outfile, result.c_str(), result.size() - 1))
	{
		std::cout << "error writing to file\n";
		FreeLibrary(dll_handle);
		return 1;
	}

	FreeLibrary(dll_handle);

	return 0;
}

