#pragma once

#include <string>
#include <tuple>
#include <vector>

enum class LogLevel { Status, Warning, Error, CriticalError };

std::string output_string(LogLevel l, bool colors);

class Log
{
    std::vector<std::tuple<LogLevel, std::string, std::string>> log;

public:
    ~Log();

    void add(LogLevel l, std::string&& where,  std::string&& msg);
    void output_to_stream(std::ostream& os, bool colors);
};
