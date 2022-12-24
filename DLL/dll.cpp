#include <iostream>
#include <windows.h> 
#include <stdio.h>

extern "C"
{
	__declspec(dllexport) bool read_from_file(LPCWSTR _file_name, char*& _buffer_out, unsigned int& _size_out)
	{
		HANDLE file = CreateFile(_file_name, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
		if (file == INVALID_HANDLE_VALUE)
		{
			_buffer_out = nullptr;
			_size_out = 0;
			return false;
		}

		unsigned long size = GetFileSize(file, 0);
		char* buffer = new char[size + 1];

		unsigned long real_size = 0;

		if (!ReadFile(file, buffer, size, &real_size, 0))
		{
			delete[] buffer;
			_buffer_out = nullptr;
			_size_out = 0;
			CloseHandle(file);

			return false;
		}
		buffer[size] = 0;

		_buffer_out = buffer;
		_size_out = size + 1;

		return true;
	}

	__declspec(dllexport) bool write_to_file(LPCWSTR _file_name, const char* _buffer, unsigned int _size)
	{
		HANDLE file = CreateFile(_file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
			return false;

		unsigned long real_size = 0;

		WriteFile(file, _buffer, _size, &real_size, NULL);

		CloseHandle(file);

		return true;
	}

}