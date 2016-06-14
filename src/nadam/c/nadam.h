// FIXME temporary copy which will be replaced with C++ implementation
/*
Copyright:  Copyright Johannes Teichrieb 2015
License:    opensource.org/licenses/MIT
*/
#pragma once

#include <cstdlib>
#include <cstdint>

#include <nadam/types.h>

namespace nadam {
namespace c {

typedef struct {
    bool isVariable;
    union {
        uint32_t total;
        uint32_t max;
    };
} messageSize_t;

typedef struct {
    const char *name;
    size_t nameLength;
    messageSize_t size;
    uint8_t hash[20];
} messageInfo_t;

// errors returned by interface functions
#define NADAM_ERROR_UNKNOWN_NAME 300
#define NADAM_ERROR_EMPTY_MESSAGE_INFOS 301
#define NADAM_ERROR_MIN_HASH_LENGTH 302
#define NADAM_ERROR_NAME_COLLISION 303
#define NADAM_ERROR_ALLOC_FAILED 304
#define NADAM_ERROR_HANDSHAKE_SEND 305
#define NADAM_ERROR_HANDSHAKE_RECV 306
#define NADAM_ERROR_HANDSHAKE_HASH_LENGTH 307
#define NADAM_ERROR_DELEGATE_BUFFER 308
#define NADAM_ERROR_NULL_POINTER 309
#define NADAM_ERROR_SEND 310
#define NADAM_ERROR_SIZE_ARG 311
// errors passed to the error delegate
#define NADAM_ERROR_RECV 500
#define NADAM_ERROR_UNKNOWN_HASH 501
#define NADAM_ERROR_VARIABLE_SIZE 502

typedef int (*send_t)(const void *src, uint32_t n);
typedef int (*recv_t)(void *dest, uint32_t n);
/* If a delegate was set with setDelegate()
   memory pointed to by msg should be considered invalid after it returns.
   Size parmeter will provide the actual size of a variable size message.  */
typedef void (*recvDelegate_t)(void *msg, uint32_t size, const messageInfo_t *messageInfo);

// if the error delegate gets called, no new messages will be received (the connection should be closed)
// errno won't be overwritten (check error argument instead)
typedef void (*errorDelegate_t)(int error);

/* Functions return 0 on success and -1 on error.
   If -1 is returned, errno will contain the specific value.  */

/* Modern languages shouldn't care about a string being zero terminated.
   To make C implementation fully compliant, one would need to pass the length with every name.
   That would be painful, therefore the follwing compromise was made:
   name's length is assumed to be the index of first '\0'.
   The limitation of this is that all names truncated at first '\0' have to be unique.
   init() will test for it. Barring that, messageInfos argument is expected to be correct.
   Otherwise, the behaviour is undefined.

   nameLength in messageInfo_t is endowed
   by message info generator utility, but is ignored in this implementation.  */

// messageInfos will be used continuously - it should be unlimited lifetime const
int init(const messageInfo_t *messageInfos, size_t messageInfoCount, size_t hashLengthMin);

/* If the delegate for a message type is not set, messages of this type are ignored (dumped).
   Passing NULL as second argument removes the delegate.
   The simplier interface version uses a shared "stock" buffer to receive messages.
   This buffer could be overwritten at any time after a delegate returns.  */
int setDelegate(const char *name, recvDelegate_t delegate);
int setDelegateWithRecvBuffer(const char *name, recvDelegate_t delegate,
        void *buffer, volatile bool *recvStart);

int initiate(send_t send, recv_t recv, errorDelegate_t errorDelegate);

/* send() can only be used after a successful initiate() call.
   Size argument is ignored for constant size messages.  */
int send(const char *name, const void *msg, uint32_t size);
int sendUmi(const messageInfo_t *messageInfo, const void *msg, uint32_t size);

// stops receiving - connection should be closed after this
void stop(void);

} // namespace c
} // namespace nadam
