#ifndef INJECT_H
#define INJECT_H

#include <string>

void InjectAndChangeIP(const std::string& processName, const std::string& dllPath);

#endif // INJECT_H