#ifndef INBOXLABEL_H
#define INBOXLABEL_H

#include <vector>
#include <string>
#include "imapcommand.h"

#define INBOX_LABEL_HAS_NO_CHILDREN 1
#define INBOX_LABEL_HAS_CHILDREN 2
#define INBOX_LABEL_NO_SELECT 4
#define INBOX_LABEL_TRASH 8
#define INBOX_LABEL_DRAFTS 16
#define INBOX_LABEL_IMPORTANT 32
#define INBOX_LABEL_SENT 64
#define INBOX_LABEL_JUNK 128
#define INBOX_LABEL_FLAGGED 256

struct InboxLabel {
    std::string name;
    int flags;
};

class InboxLabels : ImapCommand {
private:
    std::string cleanResponse(const std::string& s, size_t successStart);
public:
    InboxLabels();
    std::pair<std::string, std::string> rawPerform(ImapConnection *imap) override;
    std::vector<InboxLabel> perform(ImapConnection *imap);
};
#endif // INBOXLABEL_H
