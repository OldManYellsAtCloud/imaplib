#include "imapcommand.h"
#include <format>

bool checkSuccess(const ImapResponse &result)
{
    return result.second.find(std::format("{} OK", result.first)) != std::string::npos;
}
