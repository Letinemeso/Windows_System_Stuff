#include <stdio.h>
#include <iostream>

#include <windows.h>

#include "Pipe_Manager.h"

typedef bool(*read_from_file_func)(LPCWSTR, char*&, unsigned int&);
typedef bool(*save_to_file_func)(LPCWSTR, const char*, unsigned int);


int main()
{
	Pipe_Manager pm;

	pm.connect_to_pipe("\\\\.\\pipe\\$MyPipe$");

	if (pm.last_error())
		return 1;

	std::string file_name = pm.get_message();
	if (pm.last_error())
		return 1;
	std::string char_to_insert = pm.get_message();
	if (pm.last_error())
		return 1;

	pm.close_pipe();

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
