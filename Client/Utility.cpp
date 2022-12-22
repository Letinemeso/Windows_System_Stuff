#include "Utility.h"


wchar_t* WSS_Utility::convert_to_wide_char(const std::string &_str)
{
	wchar_t* wstr = new wchar_t[_str.size() + 1];
	wstr[_str.size()] = 0;
	for (unsigned int i = 0; i < _str.size(); ++i)
		wstr[i] = _str[i];
	return wstr;
}
