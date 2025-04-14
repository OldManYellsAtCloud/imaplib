/**
 * @file    imapcommand.cpp
 * @brief   Implementation of ImapCommand
 *
 * ImapCommand is expected to be the base class to
 * derive actual IMAP command implementations. Beside
 * `checkSuccess` all functions are expected to be
 * implemented by the derived classes.
 *
 */
#include "imapcommand.h"
#include <loglib/loglib.h>

/**
 * @brief ImapCommand::ImapCommand
 *
 * Constructor of ImapCommand base class.
 * It doesn't do much on its own - derive actual
 * IMAP command implementations from it.
 */
ImapCommand::ImapCommand() {}

/**
 * @brief Check if the result content refers to a
 *        successful command execution.
 * @param result : std::pair<std::string, std::string>, containing
 *        the result of the command execution.
 * @return std::string::npos, if the command failed. The position of
 *         "$serialID OK" is returned otherwise.
 *
 * This method can be overriden in case the response needs special
 * handling, because it has some custom format.
 */
size_t ImapCommand::checkSuccess(ImapResult result)
{
    return result.second.find(std::format("{} OK", result.first));
}

/**
 * @brief Set optional extra property
 * @param property
 * @param value
 *
 * Set an optional extra property that maybe used to construct the IMAP
 * command. These should be called before the command would be performed.
 */
void ImapCommand::setProperty(const std::string &property, const std::string &value)
{
    LOG_WARNING("ImapCommand - setProperty - used without implementation");
}

/**
 * @brief Set optional extra property
 * @param property
 * @param value
 *
 * Set an optional extra property that maybe used to construct the IMAP
 * command. These should be called before the command would be performed.
 */
void ImapCommand::setProperty(const int &property, const std::string &value)
{
    LOG_WARNING("ImapCommand - setProperty - used without implementation");
}
