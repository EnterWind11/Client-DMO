#pragma once

// ini parser


#include "inipp.h"
#include <fstream>

class cProfile
{
public:
	cProfile(const char* ini);
	~cProfile();

	int GetInt(const char* section, const char* value);
	std::string GetStr(const char* section, const char* value);
	void Write(const char* section, const char* value, int data);

private:
	inipp::Ini<char> m_ini;
	std::string m_name;
};
