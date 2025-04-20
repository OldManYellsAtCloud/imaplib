#ifndef INBOXLABEL_H
#define INBOXLABEL_H

#include <vector>
#include <string>
#include "imapconnection.h"
#include "imapcommand.h"

#define INBOX_LABEL_COMMAND "LIST \"/\" \"*\""

#define INBOX_LABEL_HAS_NO_CHILDREN 1
#define INBOX_LABEL_HAS_CHILDREN 2
#define INBOX_LABEL_NO_SELECT 4
#define INBOX_LABEL_TRASH 8
#define INBOX_LABEL_DRAFTS 16
#define INBOX_LABEL_IMPORTANT 32
#define INBOX_LABEL_SENT 64
#define INBOX_LABEL_JUNK 128
#define INBOX_LABEL_FLAGGED 256

namespace InboxLabels {

struct InboxLabel {
    std::string name;
    int flags;
};

ImapResponse inboxLabelRaw(ImapConnection *imap);
std::vector<InboxLabel> inboxLabelParse(ImapResponse imap);

} // namespace InboxLabel
#endif // INBOXLABEL_H
