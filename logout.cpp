#include "logout.h"

ImapResponse logoutRaw(ImapConnection* imap)
{
    return imap->sendCommand(LOGOUT_COMMAND);
}

std::string logoutParse(ImapResponse response)
{
    return checkSuccess(response) ? "success" : "fail";
}


