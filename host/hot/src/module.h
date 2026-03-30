#ifndef MODULE_H
#define MODULE_H

struct HostContext;

int init(struct HostContext *host);
void deinit(struct HostContext *host);
void update(struct HostContext *host);

#endif // MODULE_H
