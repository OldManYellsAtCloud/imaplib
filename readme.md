This is mostly for my own notes, so when I stumble upon this repo, I will
also remember what I wanted to do... this is not meant to be used by others.
If you do, there is no support, nor anything else.

So the idea is to have an imap library that does only what I want, when I want.
My 1st-gen (read-only) email client backed by curl works, but it is very limited.

The main limitation comes from curl: while it can perform IMAP requests fine, it
wasn't meant to be an IMAP client. Most notably curl disconnects after peforming
a request, so it isn't possible to do meaningful `IDLE` in IMAP.

Anyway. This is a permanent connection library, at this time with only basic
functionality: `AUTHENTICATE`, `LIST`, `CAPABILITY` and `LOGOUT` commands are
implemented so far - where applicable, the arguments are hardcoded. For example
I can't imagine myself querying only a subset of labels with `LIST` - it is 
hardcoded to return all. I expect to have further similarly opinionated decisions.

