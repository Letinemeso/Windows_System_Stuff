#include "Shared_Memory.h"


Shared_Memory::Shared_Memory()
{

}

Shared_Memory::~Shared_Memory()
{
	stop_sharing();
}



void Shared_Memory::allocate(const std::string &_identificator, unsigned int _size)
{
	stop_sharing();

	m_identificator = _identificator;
	m_size = _size;

	wchar_t* name = WSS_Utility::convert_to_wide_char(m_identificator);
	m_file_mapping_handle = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, m_size, name);
	delete[] name;

	if(m_file_mapping_handle == INVALID_HANDLE_VALUE)
	{
		m_last_error = GetLastError();
		m_file_mapping_handle = nullptr;
		return;
	}

	m_shared_data = (char*)MapViewOfFile(m_file_mapping_handle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if(m_shared_data == nullptr)
	{
		m_last_error = GetLastError();
		CloseHandle(m_file_mapping_handle);
		m_file_mapping_handle = nullptr;
		return;
	}

	for(unsigned int i=0; i<m_size; ++i)
		m_shared_data[i] = 0;
}

void Shared_Memory::use_memory(const std::string &_identificator, unsigned int _expected_size)
{
	stop_sharing();

	m_identificator = _identificator;
	m_size = _expected_size;

	wchar_t* name = WSS_Utility::convert_to_wide_char(m_identificator);
	m_file_mapping_handle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, name);
	delete[] name;

	if(m_file_mapping_handle == INVALID_HANDLE_VALUE)
	{
		m_last_error = GetLastError();
		m_file_mapping_handle = nullptr;
		return;
	}

	m_shared_data = (char*)MapViewOfFile(m_file_mapping_handle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if(m_shared_data == nullptr)
	{
		m_last_error = GetLastError();
		CloseHandle(m_file_mapping_handle);
		m_file_mapping_handle = nullptr;
		return;
	}
}


void Shared_Memory::stop_sharing()
{
	UnmapViewOfFile(m_shared_data);
	CloseHandle(m_file_mapping_handle);
	m_identificator.clear();
	m_last_error = 0;
	m_size = 0;
}



std::string Shared_Memory::get() const
{
	if(!valid())
		return "";

	unsigned int real_size = strlen(m_shared_data);
	if(real_size == 0)
		return "";

	std::string result;
	result.resize(real_size);
	for(unsigned int i=0; i<real_size; ++i)
		result[i] = m_shared_data[i];

	return result;
}

void Shared_Memory::set(const std::string &_data)
{
	if(!valid())
		return;

	unsigned int bytes_to_write = m_size - 1;
	if(_data.size() < bytes_to_write)
		bytes_to_write = _data.size();

	m_shared_data[bytes_to_write] = 0;
	for(unsigned int i=0; i<bytes_to_write; ++i)
		m_shared_data[i] = _data[i];
}



bool Shared_Memory::valid() const
{
	return m_file_mapping_handle != nullptr;
}

unsigned int Shared_Memory::last_error() const
{
	return m_last_error;
}










































