#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <string>

#include <windows.h>

#include "Utility.h"


class Shared_Memory
{
private:
	HANDLE m_file_mapping_handle = nullptr;
	char* m_shared_data = nullptr;
	unsigned int m_size = 0;

	std::string m_identificator;

	unsigned int m_last_error = 0;

public:
	Shared_Memory();
	~Shared_Memory();

public:
	void allocate(const std::string& _identificator, unsigned int _size);
	void use_memory(const std::string& _identificator, unsigned int _expected_size);

	void stop_sharing();

public:
	std::string get() const;
	void set(const std::string& _data);

public:
	bool valid() const;
	unsigned int last_error() const;

};

#endif // SHARED_MEMORY_H
