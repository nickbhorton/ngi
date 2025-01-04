#include "log.h"

#include <iostream>

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

std::string output_string(LogLevel l, bool colors)
{
    switch (l) {
    case LogLevel::Status:
        return colors ? std::string(GREEN) + std::string("[") +
                            std::string(BOLDGREEN) + "Status" +
                            std::string(GREEN) + std::string("]") +
                            std::string(RESET)
                      : "[Status]";
    case LogLevel::Warning:
        return colors ? std::string(YELLOW) + std::string("[") +
                            std::string(BOLDYELLOW) + "Warning" +
                            std::string(YELLOW) + std::string("]") +
                            std::string(RESET)
                      : "[Warning]";
    case LogLevel::Error:
        return colors ? std::string(RED) + std::string("[") +
                            std::string(BOLDRED) + "Error" + std::string(RED) +
                            std::string("]") + std::string(RESET)
                      : "[Error]";
    case LogLevel::CriticalError:
        return colors ? std::string(MAGENTA) + std::string("[") +
                            std::string(BOLDMAGENTA) + "Critical" +
                            std::string(MAGENTA) + std::string("]") +
                            std::string(RESET)
                      : "[Critical]";
    }
    return "";
}

void Log::add(LogLevel l, std::string&& where, std::string&& msg)
{
    log.push_back({l, std::move(where), std::move(msg)});
}

void Log::output_to_stream(std::ostream& os, bool colors)
{
    for (auto const& [level, where, msg] : this->log) {
        os << output_string(level, colors) << ": " << BOLDWHITE << where
           << RESET;
        os << ": " << msg << "\n";
    }
}

Log::~Log() { output_to_stream(std::cout, true); }
