#ifndef TERASPACE_H
#define TERASPACE_H

#define MEMSIG (0x4c6e) // will show <> in the trace
//    define MEMDEBUG 1  // TODO can we change this to a runtime configuration with env vars?

PVOID  teraspace_alloc(UINT64 len);
PVOID  teraspace_calloc(UINT64 len);
void   teraspace_free(PVOID * p);
PUCHAR teraspace_strdup(PUCHAR s);
PUCHAR teraspace_strTrimDup(PUCHAR s);
PVOID  teraspace_realloc(PVOID * p, UINT64 len);
PVOID  teraspace_share(PUCHAR path, UINT64 len);
UINT64 teraspace_size(PVOID p);
void   teraspace_stat(void);
BOOL   teraspace_leak(void);
UINT64 teraspace_use(void);

#endif
