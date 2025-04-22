#ifndef IMAP_H
#define IMAP_H

#include <map>
#include <vector>
#include <functional>
#include <variant>
#include "imapconnection.h"
#include "imapcommand.h"
#include "inboxlabel.h"

#define POSSIBLE_RESPONSE_TYPES  std::string, \
                                  std::vector<std::string>, \
                                  std::vector<InboxLabels::InboxLabel>, \
                                  std::map<std::string, std::string>


typedef std::function<ImapResponse(ImapConnection*)> ImapRequest;
typedef std::function<ImapResponse(ImapConnection*, std::map<std::string, std::string>)> ImapRequestWithArgs;
//typedef std::function<std::string(const ImapResponse&)> Parser;
//typedef std::function<std::vector<std::string>(const ImapResponse&)> VectorParser;

typedef std::function<std::variant<POSSIBLE_RESPONSE_TYPES>(const ImapResponse&)> ResponseParser;

enum REQUEST {
    AUTHENTICATE = 0,
    LOGOUT = 1,
    CAPABILITY = 2,
    INBOX_LABEL = 3,
    LIST = 3,
    EXAMINE = 4,
    SEARCH = 5,
    UID_SEARCH = 6,
    SELECT = 7
};

class Imap
{
private:
    std::string name_;
    std::map<REQUEST, ImapRequest> imapRequestDirectory;
    std::map<REQUEST, ImapRequestWithArgs> imapRequestWithArgsDirectory;

    std::map<REQUEST, ResponseParser> parserDirectory;
    //std::map<REQUEST, VectorParser> vectorParserDirectory;

    std::variant<POSSIBLE_RESPONSE_TYPES> getResponse(REQUEST request, const std::optional<std::map<std::string, std::string>> &params);


    ImapConnection *imapConnection_;

    void init();

public:
    Imap(const std::string &name, ImapConnection *imapConnection);
    void runAsync();
    void runAsyncRaw();

    std::string getString(REQUEST request, const std::optional<std::map<std::string, std::string>> &params = {});
    std::vector<std::string> getStringVector(REQUEST request, const std::optional<std::map<std::string, std::string>> &params = {});
    std::vector<InboxLabels::InboxLabel> getInboxLabelVector(REQUEST request, const std::optional<std::map<std::string, std::string>> &params = {});
    std::map<std::string, std::string> getStringMap(REQUEST request, const std::optional<std::map<std::string, std::string>> &params = {});


/*    std::string getString(REQUEST request);
    std::string getString(REQUEST request, const std::map<std::string, std::string>& params);

    std::vector<std::string> getStringVector(REQUEST request);
    std::vector<std::string> getStringVector(REQUEST request, const std::map<std::string, std::string>& params);*/
};

#endif // IMAP_H
