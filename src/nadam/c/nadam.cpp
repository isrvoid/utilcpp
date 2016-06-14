// FIXME temporary copy which will be replaced with C++ implementation
/*
Copyright:  Copyright Johannes Teichrieb 2015
License:    opensource.org/licenses/MIT
*/
#include "nadam.h"

#include <pthread.h>
#include <cerrno>
#include <cassert>

#include "khash.h"

namespace nadam {
namespace c {

KHASH_MAP_INIT_INT(m32, size_t)
KHASH_MAP_INIT_STR(mStr, size_t)

/* For initial implementation let's assume that
   all hashes can be uniquely distinguished by the first 4 bytes
   TODO extension for lengths (4,8] with uint64_t keys  */
#define HASH_LENGTH_MAX 4

typedef struct {
    nadam_recvDelegate_t delegate;
    void *buffer;
    volatile bool *recvStart;
} recvDelegateRelated_t;

typedef struct {
    khash_t(mStr) *nameKeyMap;
    khash_t(m32) *hashKeyMap;

    void *commonRecvBuffer;
    recvDelegateRelated_t *delegates;
    bool nullRecvStart;

    const nadam_messageInfo_t *messageInfos;
    size_t messageCount;
    size_t hashLength;

    nadam_send_t send;
    nadam_recv_t recv;

    nadam_errorDelegate_t errorDelegate;

    pthread_t threadId;
    bool isThreadRunning;
} nadamMembers_t;

// private declarations
// -----------------------------------------------------------------------------
static int testInitIn(size_t infoCount, size_t hashLengthMin);
static void freeMembers(void);
static int allocateMembers(void);
static int allocate(void **dest, size_t size);
static uint32_t getMaxMessageSize(void);
static void initDelegates(void);
static recvDelegateRelated_t getDelegateInit(void);
static void initMaps(void);
static int fillNameMap(void);
static void fillHashMap(void);
static int getIndexForName(const char *name, size_t *index);
static void nullDelegate(void *msg, uint32_t size, const nadam_messageInfo_t *messageInfo);
static int handshakeSendHashLength(void);
static int handshakeHandleHashLengthRecv(void);
static int sendFixedSize(const nadam_messageInfo_t *mi, const void *msg);
static int sendVariableSize(const nadam_messageInfo_t *mi, const void *msg, uint32_t size);
// recv group -- errors are reported via error delegate
static void *recvWorker(void *arg);
static int getIndexForHash(const uint8_t *hash, size_t *index);
static uint32_t truncateHash(const uint8_t *hash);
static int getMessageSize(const nadam_messageInfo_t *mi, uint32_t *size);
static void createRecvThread(void);
static void cancelRecvThread(void);

static nadamMembers_t mbr;

// interface functions
// -----------------------------------------------------------------------------
int nadam_init(const nadam_messageInfo_t *messageInfos, size_t messageCount, size_t hashLengthMin) {
    if (testInitIn(messageCount, hashLengthMin))
        return -1;

    cancelRecvThread();
    freeMembers();
    memset(&mbr, 0, sizeof(nadamMembers_t));

    mbr.messageInfos = messageInfos;
    mbr.messageCount = messageCount;
    mbr.hashLength = hashLengthMin;

    if (allocateMembers())
        return -1;

    initDelegates();
    initMaps();
    return fillNameMap();
}

int nadam_setDelegate(const char *name, nadam_recvDelegate_t delegate) {
    return nadam_setDelegateWithRecvBuffer(name, delegate, mbr.commonRecvBuffer, nullptr);
}

int nadam_setDelegateWithRecvBuffer(const char *name, nadam_recvDelegate_t delegate,
        void *buffer, volatile bool *recvStart) {
    size_t index;
    if (getIndexForName(name, &index))
        return -1;

    recvDelegateRelated_t *dp = mbr.delegates + index;

    if (delegate == NULL) {
        *dp = getDelegateInit();
        return 0;
    }

    if (buffer == NULL) {
        errno = NADAM_ERROR_DELEGATE_BUFFER;
        return -1;
    }

    dp->recvStart = recvStart ? recvStart : &mbr.nullRecvStart;
    dp->buffer = buffer;
    dp->delegate = delegate;
    return 0;
}

int nadam_initiate(nadam_send_t send, nadam_recv_t recv, nadam_errorDelegate_t errorDelegate) {
    if (send == NULL || recv == NULL || errorDelegate == NULL) {
        errno = NADAM_ERROR_NULL_POINTER;
        return -1;
    }

    mbr.send = send;
    mbr.recv = recv;
    mbr.errorDelegate = errorDelegate;

    if (handshakeSendHashLength())
        return -1;

    if (handshakeHandleHashLengthRecv())
        return -1;

    kh_clear(m32, mbr.hashKeyMap);
    fillHashMap();
    cancelRecvThread();
    createRecvThread();
    return 0;
}

int nadam_send(const char *name, const void *msg, uint32_t size) {
    size_t index;
    if (getIndexForName(name, &index))
        return -1;

    const nadam_messageInfo_t *mi = mbr.messageInfos + index;
    bool isFixedSize = !mi->size.isVariable;
    if(isFixedSize)
        return sendFixedSize(mi, msg);
    else
        return sendVariableSize(mi, msg, size);
}

int nadam_sendUmi(const nadam_messageInfo_t *mi, const void *msg, uint32_t size) {
	assert(mi);
    bool isFixedSize = !mi->size.isVariable;
    if(isFixedSize)
        return sendFixedSize(mi, msg);
    else
        return sendVariableSize(mi, msg, size);
}

void nadam_stop(void) {
    cancelRecvThread();
}

// private functions
// -----------------------------------------------------------------------------
static int testInitIn(size_t infoCount, size_t hashLengthMin) {
    if (infoCount == 0) {
        errno = NADAM_ERROR_EMPTY_MESSAGE_INFOS;
        return -1;
    }

    if (hashLengthMin == 0 || hashLengthMin > HASH_LENGTH_MAX) {
        errno = NADAM_ERROR_MIN_HASH_LENGTH;
        return -1;
    }
    return 0;
}

static void freeMembers(void) {
    kh_destroy(mStr, mbr.nameKeyMap);
    kh_destroy(m32, mbr.hashKeyMap);
    free(mbr.commonRecvBuffer);
    free(mbr.delegates);
    // messageInfos are not ours to free
}

static int allocateMembers(void) {
    // + 1 allows a delegate that uses common buffer to safely do msg[size] = '\0';
    if (allocate(&mbr.commonRecvBuffer, getMaxMessageSize() + 1))
        return -1;

    if (allocate((void **) &mbr.delegates, sizeof(recvDelegateRelated_t) * mbr.messageCount))
        return -1;

    return 0;
}

static int allocate(void **dest, size_t size) {
    *dest = malloc(size);
    if (*dest == NULL) {
        errno = NADAM_ERROR_ALLOC_FAILED;
        return -1;
    }
    memset(*dest, 0, size);
    return 0;
}

static uint32_t getMaxMessageSize(void) {
    uint32_t maxSize = 0;
    for (size_t i = 0; i < mbr.messageCount; ++i) {
        uint32_t currentSize = mbr.messageInfos[i].size.total;
        if (currentSize > maxSize)
            maxSize = currentSize;
    }
    return maxSize;
}

static void initDelegates(void) {
    for (size_t i = 0; i < mbr.messageCount; ++i)
        mbr.delegates[i] = getDelegateInit();
}

static recvDelegateRelated_t getDelegateInit(void) {
    recvDelegateRelated_t init{nullDelegate, mbr.commonRecvBuffer, &mbr.nullRecvStart};
    return init;
}

static void initMaps(void) {
    mbr.nameKeyMap = kh_init(mStr);
    mbr.hashKeyMap = kh_init(m32);
}

static int fillNameMap(void) {
    for (size_t i = 0; i < mbr.messageCount; ++i) {
        int ret;
        khiter_t k = kh_put(mStr, mbr.nameKeyMap, mbr.messageInfos[i].name, &ret);
        assert(ret != -1);

        bool keyWasPresent = !ret;
        if (keyWasPresent) {
            errno = NADAM_ERROR_NAME_COLLISION;
            return -1;
        }

        kh_val(mbr.nameKeyMap, k) = i;
    }
    return 0;
}

static void fillHashMap(void) {
    for (size_t i = 0; i < mbr.messageCount; ++i) {
        int ret;
        uint32_t hash = truncateHash(mbr.messageInfos[i].hash);
        khiter_t k = kh_put(m32, mbr.hashKeyMap, hash, &ret);
        assert(ret != -1);

        kh_val(mbr.hashKeyMap, k) = i;
    }
}

static int getIndexForName(const char *name, size_t *index) {
    khiter_t k = kh_get(mStr, mbr.nameKeyMap, name);

    bool nameNotFound = (k == kh_end(mbr.nameKeyMap));
    if (nameNotFound) {
        errno = NADAM_ERROR_UNKNOWN_NAME;
        return -1;
    }

    *index = kh_val(mbr.nameKeyMap, k);
    return 0;
}

static void nullDelegate(void*, uint32_t, const nadam_messageInfo_t*) { }

static int handshakeSendHashLength(void) {
    const uint8_t hashLength = (uint8_t) mbr.hashLength;
    if (mbr.send(&hashLength, 1)) {
        errno = NADAM_ERROR_HANDSHAKE_SEND;
        return -1;
    }
    return 0;
}

static int handshakeHandleHashLengthRecv(void) {
    uint8_t hashLength;
    if (mbr.recv(&hashLength, 1)) {
        errno = NADAM_ERROR_HANDSHAKE_RECV;
        return -1;
    }

    if (hashLength > HASH_LENGTH_MAX) {
        errno = NADAM_ERROR_HANDSHAKE_HASH_LENGTH;
        return -1;
    }

    if (hashLength > mbr.hashLength)
        mbr.hashLength = hashLength;

    return 0;
}

static int sendFixedSize(const nadam_messageInfo_t *mi, const void *msg) {
    int errorCollector = mbr.send(mi->hash, (uint32_t) mbr.hashLength);
    errorCollector |= mbr.send(msg, mi->size.total);

    if (errorCollector) {
        errno = NADAM_ERROR_SEND;
        return -1;
    }
    return 0;
}

static int sendVariableSize(const nadam_messageInfo_t *mi, const void *msg, uint32_t size) {
    if (size > mi->size.max) {
        errno = NADAM_ERROR_SIZE_ARG;
        return -1;
    }
    int errorCollector = mbr.send(mi->hash, (uint32_t) mbr.hashLength);
    errorCollector |= mbr.send(&size, 4);
    errorCollector |= mbr.send(msg, size);

    if (errorCollector) {
        errno = NADAM_ERROR_SEND;
        return -1;
    }
    return 0;
}

// recv
static void *recvWorker(void*) {
    uint8_t hash[HASH_LENGTH_MAX];
    while (true) {
        if (mbr.recv(hash, (uint32_t) mbr.hashLength)) {
            mbr.errorDelegate(NADAM_ERROR_RECV);
            return NULL;
        }

        size_t index;
        int error = getIndexForHash(hash, &index);
        if (error) {
            mbr.errorDelegate(error);
            return NULL;
        }

        const nadam_messageInfo_t *messageInfo = mbr.messageInfos + index;
        uint32_t size;
        error = getMessageSize(messageInfo, &size);
        if (error) {
            mbr.errorDelegate(error);
            return NULL;
        }

        const recvDelegateRelated_t *delegate = mbr.delegates + index;
        void *buffer = delegate->buffer;
        *delegate->recvStart = true;
        if (mbr.recv(buffer, size)) {
            mbr.errorDelegate(NADAM_ERROR_RECV);
            return NULL;
        }

        delegate->delegate(buffer, size, messageInfo);
    }
}

static int getIndexForHash(const uint8_t *hash, size_t *index) {
    khiter_t k = kh_get(m32, mbr.hashKeyMap, truncateHash(hash));

    bool hashNotFound = (k == kh_end(mbr.hashKeyMap));
    if (hashNotFound)
        return NADAM_ERROR_UNKNOWN_HASH;

    *index = kh_val(mbr.hashKeyMap, k);
    return 0;
}

static uint32_t truncateHash(const uint8_t *hash) {
    uint32_t res = 0;
    memcpy(&res, hash, mbr.hashLength);
    return res;
}

static int getMessageSize(const nadam_messageInfo_t *mi, uint32_t *size) {
    nadam_messageSize_t ms = mi->size;
    uint32_t s;
    if (ms.isVariable) {
        if (mbr.recv(&s, 4))
            return NADAM_ERROR_RECV;

        if (s > ms.max)
            return NADAM_ERROR_VARIABLE_SIZE;
    } else {
        s = ms.total;
    }

    *size = s;
    return 0;
}

static void createRecvThread(void) {
    assert(!mbr.isThreadRunning);

    int error = pthread_create(&mbr.threadId, nullptr, recvWorker, nullptr);
    assert(!error);
    mbr.isThreadRunning = true;
}

static void cancelRecvThread(void) {
    if (!mbr.isThreadRunning)
        return;

    int error = pthread_cancel(mbr.threadId);
    assert(!error);
    error = pthread_join(mbr.threadId, nullptr);
    assert(!error);
    mbr.isThreadRunning = false;
}

} // namepace c
} // namespace nadam
