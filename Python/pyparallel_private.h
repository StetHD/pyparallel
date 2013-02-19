#ifndef PYPARALLEL_PRIVATE_H
#define PYPARALLEL_PRIVATE_H

#ifdef __cpplus
extern "C" {
#endif

#ifndef UNICODE
#define UNICODE
#endif

#include "../Modules/socketmodule.h"
#include <Windows.h>
#include "pyparallel.h"

#pragma comment(lib, "ws2_32.lib")

#if defined(_MSC_VER) && _MSC_VER>1201
  /* Do not include addrinfo.h for MSVC7 or greater. 'addrinfo' and
   * EAI_* constants are defined in (the already included) ws2tcpip.h.
   */
#else
#  include "../Modules/addrinfo.h"
#endif


#ifdef _WIN64
#define Px_PTR_ALIGN_SIZE 8U
#define Px_PTR_ALIGN_RAW 8
#define Px_UINTPTR unsigned long long
#define Px_INTPTR long long
#define Px_LARGE_PAGE_SIZE 2 * 1024 * 1024 /* 2MB on x64 */
#else
#define Px_LARGE_PAGE_SIZE 4 * 1024 * 1024 /* 4MB on x86 */
#define Px_PTR_ALIGN_SIZE 4U
#define Px_PTR_ALIGN_RAW 4
#define Px_UINTPTR unsigned long
#define Px_INTPTR long
#endif
#define Px_PAGE_SIZE (4096)
#define Px_SMALL_PAGE_SIZE Px_PAGE_SIZE
#define Px_PAGE_SHIFT 12ULL
#define Px_MEM_ALIGN_RAW MEMORY_ALLOCATION_ALIGNMENT
#define Px_MEM_ALIGN_SIZE ((Px_UINTPTR)MEMORY_ALLOCATION_ALIGNMENT)
#define Px_PAGE_ALIGN_SIZE ((Px_UINTPTR)Px_PAGE_SIZE)
#define Px_CACHE_ALIGN_SIZE ((Px_UINTPTR)SYSTEM_CACHE_ALIGNMENT_SIZE)

#define Px_ALIGN(n, a) (                             \
    (((Px_UINTPTR)(n)) + (((Px_UINTPTR)(a))-1ULL)) & \
    ~(((Px_UINTPTR)(a))-1ULL)                        \
)

#define Px_ALIGN_DOWN(n, a) (                        \
    (((Px_UINTPTR)(n)) & (-((Px_INTPTR)(a))))        \
)

#define Px_PTR_ALIGN(n)         (Px_ALIGN((n), Px_PTR_ALIGN_SIZE))
#define Px_MEM_ALIGN(n)         (Px_ALIGN((n), Px_MEM_ALIGN_SIZE))
#define Px_CACHE_ALIGN(n)       (Px_ALIGN((n), Px_CACHE_ALIGN_SIZE))
#define Px_PAGE_ALIGN(n)        (Px_ALIGN((n), Px_PAGE_ALIGN_SIZE))
#define Px_PAGE_ALIGN_DOWN(n)   (Px_ALIGN_DOWN((n), Px_PAGE_ALIGN_SIZE))

#define Px_PTR(p)           ((Px_UINTPTR)(p))
#define Px_PTR_ADD(p, n)    ((void *)((Px_PTR(p)) + (Px_PTR(n))))

#define Px_PTR_ALIGNED_ADD(p, n) \
    (Px_PTR_ALIGN(Px_PTR_ADD(p, Px_PTR_ALIGN(n))))

#define Px_ALIGNED_MALLOC(n)                                \
    (Py_PXCTX ? _PxHeap_Malloc(ctx, n, Px_MEM_ALIGN_SIZE) : \
                _aligned_malloc(n, MEMORY_ALLOCATION_ALIGNMENT))

#define Px_ALIGNED_FREE(n)                                  \
    (Py_PXCTX ? _PxHeap_Malloc(ctx, n, Px_MEM_ALIGN_SIZE) : \
                _aligned_malloc(n, MEMORY_ALLOCATION_ALIGNMENT))

#define Px_MAX(a, b) ((a > b) ? a : b)

#define Px_DEFAULT_HEAP_SIZE (Px_PAGE_SIZE) /* 4KB */
#define Px_DEFAULT_TLS_HEAP_SIZE (Px_LARGE_PAGE_SIZE) /* 2MB/4MB */
#define Px_MAX_SEM (32768)

#define Px_PTR_IN_HEAP(p, h) (!h ? 0 : (            \
    (Px_PTR((p)) >= Px_PTR(((Heap *)(h))->base)) && \
    (Px_PTR((p)) <= Px_PTR(                         \
        Px_PTR((((Heap *)(h))->base)) +             \
        Px_PTR((((Heap *)(h))->size))               \
    ))                                              \
))

static __inline
size_t
Px_GET_ALIGNMENT(void *p)
{
    register Px_UINTPTR c = Px_PTR(p);
    register unsigned int i = 0;
    if (!p)
        return 0;
    while (!((c >> i) & 1))
        i++;
    return (1ULL << i);
}

#define Py_ASPX(ob) ((PxObject *)(((PyObject*)(ob))->px))

#ifdef MS_WINDOWS
#define PyEvent     HANDLE
#define PyEventType HANDLE

#define Py_EVENT(o)         ((PyEventType)(((PyObject *)(o))->event))
#define PyEvent_CREATE(o)   (Py_EVENT(o) = CreateEvent(0, 0, 0, 0))
#define PyEvent_INIT(o)     /* N/A */
#define PyEvent_SIGNAL(o)   (SetEvent(Py_EVENT(o)))
#define PyEvent_DESTROY(o)  (CloseHandle(Py_EVENT(o)))

#define PyRWLock            SRWLOCK
#define Py_RWLOCK(o)        ((PyRWLock *)&(((PyObject *)(o))->srw_lock))

#define PyRWLock_CREATE(o)  /* N/A */
#define PyRWLock_INIT(o)    (InitializeSRWLock((PSRWLOCK)&(o->srw_lock)))
#define PyRWLock_DESTROY(o) /* N/A */
#endif

#define PyAsync_IO_READ      (1UL <<  1)
#define PyAsync_IO_WRITE     (1UL <<  2)

#define Px_CTXTYPE(c)      (((Context *)c)->context_type)

#define Px_CTXTYPE_WORK    (1)
#define Px_CTXTYPE_WAIT    (1UL <<  1)
#define Px_CTXTYPE_SOCK    (1UL <<  2)
#define Px_CTXTYPE_FILE    (1UL <<  3)

#include "pxlist.h"

#ifdef _WIN64
#define Px_NUM_TLS_WSABUFS 64
#else
#define Px_NUM_TLS_WSABUFS 32
#endif


typedef struct _PyParallelHeap PyParallelHeap, Heap;
typedef struct _PyParallelContext PyParallelContext, WorkContext, Context;
typedef struct _PyParallelIOContext PyParallelIOContext, IOContext;
typedef struct _PyParallelContextStats PyParallelContextStats, Stats;
typedef struct _PyParallelIOContextStats PyParallelIOContextStats, IOStats;
typedef struct _PyParallelCallback PyParallelCallback, Callback;

typedef struct _PxSocketBuf PxSocketBuf;
typedef struct _PxHeap PxHeap;

typedef struct _PxThreadLocalState TLS;

typedef struct _TLSBUF {
    char index;
    TLS *tls;
    WSABUF w;
} TLSBUF;

#define T2W(b) (_Py_CAST_FWD(b, WSABUF *, TLSBUF, w))
#define W2T(b) (_Py_CAST_BACK(b, TLSBUF *, TLSBUF, w))

#define usize_t unsigned size_t

typedef struct _cpuinfo {
    struct _core {
        int logical;
        int physical;
    } core;
    struct _cache {
        int l1;
        int l2;
    } cache;
} cpuinfo;

typedef struct _Object Object;

typedef struct _Object {
    Object   *prev;
    Object   *next;
    PyObject *op;
} Object;

typedef struct _Objects {
    Object *first;
    Object *last;
} Objects;

static __inline
void
append_object(Objects *list, Object *o)
{
    register Object *n;
    if (!list->first) {
        list->first = o;
        list->last = o;
        o->prev = NULL;
    } else {
        n = list->last;
        n->next = o;
        o->prev = n;
        list->last = o;
    }
    o->next = NULL;
}

static __inline
void
remove_object(Objects *list, Object *o)
{
    register Object *prev = o->prev;
    register Object *next = o->next;

    if (list->first == o)
        list->first = next;

    if (list->last == o)
        list->last = prev;

    if (prev)
        prev->next = next;

    if (next)
        next->prev = prev;
}

#define _PxHeap_HEAD_EXTRA  \
    Heap   *sle_prev;       \
    Heap   *sle_next;       \
    void   *base;           \
    void   *next;           \
    size_t  pages;          \
    size_t  next_alignment; \
    size_t  size;           \
    size_t  allocated;      \
    size_t  remaining;      \
    size_t  snapshot_id;    \
    int     index;

#define PxHeap_HEAD PxHeap heap_base;

typedef struct _PxHeap {
    _PxHeap_HEAD_EXTRA
} PxHeap;

typedef struct _PyParallelHeap {
    _PxHeap_HEAD_EXTRA
    size_t  mallocs;
    size_t  deallocs;
    size_t  mem_reallocs;
    size_t  obj_reallocs;
    size_t  resizes;
    size_t  frees;
    size_t  alignment_mismatches;
    size_t  bytes_wasted;
} PyParallelHeap, Heap;

typedef struct _PyParallelContextStats {
    unsigned __int64 submitted;
    unsigned __int64 entered;
    unsigned __int64 exited;
    unsigned __int64 start;
    unsigned __int64 end;
    double runtime;

    long thread_id;
    long process_id;

    int blocking_calls;

    size_t mallocs;
    size_t mem_reallocs;
    size_t obj_reallocs;
    size_t deallocs;
    size_t resizes;
    size_t frees;
    size_t alignment_mismatches;
    size_t bytes_wasted;

    size_t newrefs;
    size_t forgetrefs;

    size_t heaps;

    size_t size;
    size_t allocated;
    size_t remaining;

    size_t objects;
    size_t varobjs;

    size_t startup_size;
} PyParallelContextStats, Stats;

typedef struct _PxThreadLocalState {
    Heap       *h;
    Heap       *ctx_heap;
    Heap        heap;
    HANDLE      handle;
    DWORD       thread_id;
    Stats       stats;

    CRITICAL_SECTION    sbuf_cs;
    volatile Px_INTPTR  sbuf_bitmap;
    WSABUF             *sbufs[Px_NUM_TLS_WSABUFS];
    TLSBUF              sbuf[Px_NUM_TLS_WSABUFS];

    CRITICAL_SECTION    rbuf_cs;
    volatile Px_INTPTR  rbuf_bitmap;
    WSABUF             *rbufs[Px_NUM_TLS_WSABUFS];
    TLSBUF              rbuf[Px_NUM_TLS_WSABUFS];

    size_t              snapshot_id;
    CRITICAL_SECTION    snapshots_cs;
    volatile Px_INTPTR  snapshots_bitmap;
    Heap               *snapshots[Px_NUM_TLS_WSABUFS];
    Heap                snapshot[Px_NUM_TLS_WSABUFS];

} PxThreadLocalState, TLS;



#define _PX_TMPBUF_SIZE 1024

#ifdef Py_DEBUG
#define HASH_DEBUG
#include "uthash.h"

#define _PxPages_MAX_HEAPS 2
typedef struct _PxPages {
    Px_UINTPTR  base;
    Heap       *heaps[_PxPages_MAX_HEAPS];
    short       count;
    UT_hash_handle hh;
} PxPages;
#endif

#define PyAsync_IO_BUFSIZE (64 * 1024)

#define PyAsync_NUM_BUFS (32)

#define PxIO_PREALLOCATED (0)
#define PxIO_ONDEMAND     (1)

#define PxIO_FLAGS(i) (((PxIO *)i)->flags)
#define PxIO_IS_PREALLOC(i) (PxIO_FLAGS(i) == PxIO_PREALLOCATED)
#define PxIO_IS_ONDEMAND(i) (PxIO_FLAGS(i) == PxIO_ONDEMAND)

#define Px_IOTYPE_FILE      (1)
#define Px_IOTYPE_SOCKET    (1UL <<  1)

typedef struct _PxIO PxIO;

typedef struct _PxIO {
    __declspec(align(Px_MEM_ALIGN_RAW))
    PxListEntry entry;
    OVERLAPPED  overlapped;
    PyObject   *obj;
    ULONG       size;
    int         flags;
    __declspec(align(Px_PTR_ALIGN_RAW))
    ULONG       len;
    char FAR   *buf;
} PxIO;

#define PxIO2WSABUF(io) (_Py_CAST_FWD(io, LPWSABUF, PxIO, len))
#define OL2PxIO(ol)     (_Py_CAST_BACK(ol, PxIO *, PxIO, overlapped))

typedef struct _PxState {
    PxListHead *retired_contexts;
    PxListHead *errors;
    PxListHead *completed_callbacks;
    PxListHead *completed_errbacks;
    PxListHead *incoming;
    PxListHead *finished;
    PxListHead *finished_sockets;

    PxListHead *io_ondemand;
    PxListHead *io_free;
    HANDLE      io_free_wakeup;

    Context    *iob_ctx;

#ifdef Py_DEBUG
    SRWLOCK     pages_srwlock;
    PxPages    *pages;
#endif

    /*
    PxListHead *free_contexts_4096;
    PxListHead *free_contexts_8192;
    PxListHead *free_contexts_16384;
    short max_free_contexts;
    */

    Context *ctx_first;
    Context *ctx_last;
    unsigned short ctx_minfree;
    unsigned short ctx_curfree;
    unsigned short ctx_maxfree;
    unsigned short ctx_ttl;

    IOContext *ioctx_first;
    IOContext *ioctx_last;

    HANDLE wakeup;

    CRITICAL_SECTION cs;

    int processing_callback;

    long long contexts_created;
    long long contexts_destroyed;
    long contexts_active;
    long contexts_persisted;

    volatile long long io_stalls;

    volatile long active;
    volatile long persistent;

    //__declspec(align(SYSTEM_CACHE_ALIGNMENT_SIZE))
    volatile long long  submitted;
    volatile long       pending;
    volatile long       inflight;
    volatile long long  done;
    //volatile long long  failed;
    //volatile long long  succeeded;

    //__declspec(align(SYSTEM_CACHE_ALIGNMENT_SIZE))
    volatile long long  waits_submitted;
    volatile long       waits_pending;
    volatile long       waits_inflight;
    volatile long long  waits_done;
    //volatile long long  failed;
    //volatile long long  succeeded;

    //__declspec(align(SYSTEM_CACHE_ALIGNMENT_SIZE))
    volatile long long  timers_submitted;
    volatile long       timers_pending;
    volatile long       timers_inflight;
    volatile long long  timers_done;
    //volatile long long  failed;
    //volatile long long  succeeded;

    //__declspec(align(SYSTEM_CACHE_ALIGNMENT_SIZE))
    volatile long long  io_submitted;
    volatile long       io_pending;
    volatile long       io_inflight;
    volatile long long  io_done;

    volatile long long  async_writes_completed_synchronously;
    volatile long long  async_reads_completed_synchronously;

    //__declspec(align(SYSTEM_CACHE_ALIGNMENT_SIZE))
    volatile long long  sync_wait_submitted;
    volatile long       sync_wait_pending;
    volatile long       sync_wait_inflight;
    volatile long long  sync_wait_done;

    //__declspec(align(SYSTEM_CACHE_ALIGNMENT_SIZE))
    volatile long long  sync_nowait_submitted;
    volatile long       sync_nowait_pending;
    volatile long       sync_nowait_inflight;
    volatile long long  sync_nowait_done;

    //__declspec(align(SYSTEM_CACHE_ALIGNMENT_SIZE))
    long long last_done_count;
    long long last_submitted_count;

    long long last_sync_wait_done_count;
    long long last_sync_wait_submitted_count;

    long long last_sync_nowait_done_count;
    long long last_sync_nowait_submitted_count;

    volatile long tls_buf_mismatch;

} PxState;

#define _PxContext_HEAD_EXTRA       \
    __declspec(align(16))           \
    SLIST_ENTRY slist_entry;        \
    HANDLE  heap_handle;            \
    Heap    heap;                   \
    Heap    *h;                      \
    PxState *px;                    \
    PyThreadState *tstate;          \
    PyThreadState *pstate;          \
    PTP_CALLBACK_INSTANCE instance; \
    int flags;

#define PxContext_HEAD  PxContext ctx_base;

typedef struct _PxContext {
    _PxContext_HEAD_EXTRA
} PxContext;

typedef struct _PyParallelContext {
    _PxContext_HEAD_EXTRA
    Stats     stats;

    PyObject *waitobj;
    PyObject *waitobj_timeout;
    PyObject *func;
    PyObject *args;
    PyObject *kwds;
    PyObject *callback;
    PyObject *errback;
    PyObject *result;

    TP_WAIT        *tp_wait;
    TP_WAIT_RESULT  wait_result;
    PFILETIME       wait_timeout;

    int         io_type;
    TP_IO      *tp_io;
    DWORD       io_status;
    ULONG       io_result;
    ULONG_PTR   io_nbytes;
    PxIO       *io;
    PyObject   *io_obj;

    OVERLAPPED  overlapped;


    PxSocketBuf *rbuf_first;
    PxSocketBuf *rbuf_last;

    LARGE_INTEGER filesize;
    LARGE_INTEGER next_read_offset;

    TP_TIMER *tp_timer;

    PyObject    *exc_type;
    PyObject    *exc_value;
    PyObject    *exc_traceback;

    Context *prev;
    Context *next;

    PyObject *ob_first;
    PyObject *ob_last;

    PxListItem *error;
    PxListItem *callback_completed;
    PxListItem *errback_completed;

    PxListHead *outgoing;
    PxListHead *decrefs;
    PxListItem *decref;

    volatile long refcnt;

    Objects objects;
    Objects varobjs;
    Objects events;

    char  tbuf[_PX_TMPBUF_SIZE];
    void *tbuf_base;
    void *tbuf_next;
    size_t tbuf_mallocs;
    size_t tbuf_allocated;
    size_t tbuf_remaining;
    size_t tbuf_bytes_wasted;
    size_t tbuf_next_alignment;
    size_t tbuf_alignment_mismatches;


    size_t leaked_bytes;
    size_t leak_count;
    void *last_leak;

    PyObject *errors_tuple;
    int hijacked_for_errors_tuple;
    size_t size_before_hijack;

    short ttl;

    long done;

    int times_finished;
    char is_persisted;
    char was_persisted;
    int persisted_count;

} PyParallelContext, Context;

int PxContext_Snapshot(Context *c);
int PxContext_Restore(Context *c);

typedef struct _PyParallelIOContext {
    PyObject        *o;
    WorkContext     *work_ctx;

} PyParallelIOContext, IOContext;


typedef struct _PxObject {
    Context     *ctx;
    size_t       size;
    PyObject    *resized_to;
    PyObject    *resized_from;
    INIT_ONCE    persist;
    size_t       signature;
} PxObject;

#define Px_CTXFLAGS(c)      (((Context *)c)->flags)

#define Px_CTXFLAGS_IS_PERSISTED    (1)
#define Px_CTXFLAGS_WAS_PERSISTED   (1UL <<  1)
#define Px_CTXFLAGS_REUSED          (1UL <<  2)
#define Px_CTXFLAGS_IS_WORK_CTX     (1UL <<  3)
#define Px_CTXFLAGS_DISASSOCIATED   (1UL <<  4)
#define Px_CTXFLAGS_HAS_STATS       (1UL <<  5)
#define Px_CTXFLAGS_TLS_HEAP_ACTIVE (1UL <<  6)

#define Px_CTX_IS_PERSISTED(c)   (Px_CTXFLAGS(c) & Px_CTXFLAGS_IS_PERSISTED)
#define Px_CTX_WAS_PERSISTED(c)  (Px_CTXFLAGS(c) & Px_CTXFLAGS_WAS_PERSISTED)
#define Px_CTX_REUSED(c)         (Px_CTXFLAGS(c) & Px_CTXFLAGS_REUSED)
#define Px_IS_WORK_CTX(c)        (Px_CTXFLAGS(c) & Px_CTXFLAGS_IS_WORK_CTX)
#define Px_CTX_IS_DISASSOCIATED(c) (Px_CTXFLAGS(c) & Px_CTXFLAGS_DISASSOCIATED)
#define Px_CTX_HAS_STATS(c)      (Px_CTXFLAGS(c) & Px_CTXFLAGS_HAS_STATS)
#define Px_TLS_HEAP_ACTIVE(c)    (Px_CTXFLAGS(c) & Px_CTXFLAGS_TLS_HEAP_ACTIVE)

#define STATS(c) \
    (Px_CTX_HAS_STATS(c) ? ((Stats *)(&(((Context *)c)->stats))) : 0)

#define Px_SOCKFLAGS(s)     (((PxSocket *)s)->flags)

#define Px_SOCKFLAGS_CLIENT                     (1)
#define Px_SOCKFLAGS_SERVER                     (1UL <<  1)
#define Px_SOCKFLAGS_____________________       (1UL <<  2)
#define Px_SOCKFLAGS_RECV_MORE                  (1UL <<  3)
#define Px_SOCKFLAGS_CONNECTED                  (1UL <<  4)
#define Px_SOCKFLAGS_LONG_LIVED                 (1UL <<  5)
#define Px_SOCKFLAGS_THROUGHPUT                 (1UL <<  6)
#define Px_SOCKFLAGS_SERVERCLIENT               (1UL <<  7)
#define Px_SOCKFLAGS_INITIAL_BYTES              (1UL <<  8)
#define Px_SOCKFLAGS_INITIAL_BYTES_STATIC       (1UL <<  9)
#define Px_SOCKFLAGS_INITIAL_BYTES_CALLABLE     (1UL << 10)
#define Px_SOCKFLAGS_CONCURRENCY                (1UL << 11)
#define Px_SOCKFLAGS_CHECKED_DR_UNREACHABLE     (1UL << 12)
#define Px_SOCKFLAGS_SENDING_INITIAL_BYTES      (1UL << 13)
#define Px_SOCKFLAGS_HAS_CONNECTION_MADE        (1UL << 14)
#define Px_SOCKFLAGS_HAS_DATA_RECEIVED          (1UL << 15)
#define Px_SOCKFLAGS_HAS_SEND_FAILED            (1UL << 16)
#define Px_SOCKFLAGS_SEND_SHUTDOWN              (1UL << 17)
#define Px_SOCKFLAGS_RECV_SHUTDOWN              (1UL << 18)
#define Px_SOCKFLAGS_BOTH_SHUTDOWN              (1UL << 19)
#define Px_SOCKFLAGS_SEND_SCHEDULED             (1UL << 20)
#define Px_SOCKFLAGS_HAS_SEND_COMPLETE          (1UL << 21)
#define Px_SOCKFLAGS_CLOSE_SCHEDULED            (1UL << 22)
#define Px_SOCKFLAGS_CLOSED                     (1UL << 23)
#define Px_SOCKFLAGS_TIMEDOUT                   (1UL << 24)
#define Px_SOCKFLAGS_CALLED_CONNECTION_MADE     (1UL << 25)
#define Px_SOCKFLAGS_IS_WAITING_ON_FD_ACCEPT    (1UL << 26)
#define Px_SOCKFLAGS_HAS_SHUTDOWN_SEND          (1UL << 27)
#define Px_SOCKFLAGS_RELOAD_PROTOCOL            (1UL << 29)
#define Px_SOCKFLAGS_INITIAL_BYTES_PYBYTEARRAY  (1UL << 30)
#define Px_SOCKFLAGS_                           (1UL << 31)

#define PxSocket_IS_CLIENT(s)   (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_CLIENT)
#define PxSocket_IS_SERVER(s)   (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_SERVER)
#define PxSocket_IS_BOUND(s)    (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_BOUND)
#define PxSocket_IS_CONNECTED(s) (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_CONNECTED)
#define PxSocket_LONG_LIVED(s)  (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_LONG_LIVED)

#define PxSocket_IS_PERSISTENT(s) (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_PERSISTENT)

#define PxSocket_HAS_INITIAL_BYTES(s) \
    (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_INITIAL_BYTES)

#define PxSocket_HAS_SEND_COMPLETE(s) \
    (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_SEND_COMPLETE)

#define PxSocket_HAS_DATA_RECEIVED(s) \
    (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_DATA_RECEIVED)

#define PxSocket_IS_SERVERCLIENT(s) \
    (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_SERVERCLIENT)

#define PxSocket_IS_PENDING_DISCONNECT(s) \
    (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_PENDING_DISCONNECT)

#define PxSocket_IS_DISCONNECTED(s) \
    (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_DISCONNECTED)

#define PxSocket_RECV_MORE(s)   (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_RECV_MORE)

#define PxSocket_CB_CONNECTION_MADE     (1)
#define PxSocket_CB_DATA_RECEIVED       (1UL <<  1)
#define PxSocket_CB_LINES_RECEIVED      (1UL <<  2)
#define PxSocket_CB_EOF_RECEIVED        (1UL <<  3)
#define PxSocket_CB_CONNECTION_LOST     (1UL <<  4)

#define PxSocket_IO_CONNECT             (1)
#define PxSocket_IO_ACCEPT              (1UL << 1)
#define PxSocket_IO_RECV                (1UL << 2)
#define PxSocket_IO_RECV_SYNC           (1UL << 3)
#define PxSocket_IO_SEND                (1UL << 3)
#define PxSocket_IO_SEND_SYNC           (1UL << 4)
#define PxSocket_IO_DISCONNECT          (1UL << 5)
#define PxSocket_IO_CLOSE               (1UL << 6)

typedef struct _PxSocketBuf PxSocketBuf;
typedef struct _PxSocketBufList PxSocketBufList;

typedef struct _PxSocketBuf {
    /*WSAOVERLAPPED ol;*/
    WSABUF w;
    PxSocketBuf *prev;
    PxSocketBuf *next;
    size_t signature;
    /* mimic PyBytesObject herein */
    PyObject_VAR_HEAD
    Py_hash_t ob_shash;
    char ob_sval[1];
} PxSocketBuf;


typedef struct _PxSocketBufList {
    PyObject_VAR_HEAD
    /* mimic PyListObject */
    PyObject **ob_item;
    Py_ssize_t allocated;
    WSABUF **wsabufs;
    int nbufs;
    int flags;
} PxSocketBufList;

typedef void (*sockcb_t)(Context *c);

#define PxSocket2WSABUF(s) (_Py_CAST_FWD(s, LPWSABUF, PxSocket, len))
#define PxSocketBuf2PyBytesObject(s) \
    (_Py_CAST_FWD(s, PyBytesObject *, PxSocketBuf, ob_base))

#define PyBytesObject2PxSocketBuf(b)                                  \
    (PyBytesObject2PxSocketBufSignature(b) == _PxSocketBufSignature ? \
        (_Py_CAST_BACK(b, PxSocketBuf *, PxSocketBuf, ob_base)) :     \
        (PxSocketBuf *)NULL                                           \
    )

#define PyBytesObject2PxSocketBufSignature(b) \
    (_Py_CAST_BACK(b, size_t, PxSocketBuf, ob_base))

#define IS_SBUF(b)

typedef struct _PxSocketListItem {
    __declspec(align(16)) SLIST_ENTRY slist_entry;
    SOCKET_T sock_fd;
} PxSocketListItem;

typedef struct _PxSocket PxSocket;

typedef struct _PxSocket {
    PyObject_HEAD
    /* Mirror PySocketSockObject. */
    SOCKET_T sock_fd;           /* Socket file descriptor */
    int sock_family;            /* Address family, e.g., AF_INET */
    int sock_type;              /* Socket type, e.g., SOCK_STREAM */
    int sock_proto;             /* Protocol type, usually 0 */
    PyObject *(*errorhandler)(void); /* Error handler; checks
                                        errno, returns NULL and
                                        sets a Python exception */
    double sock_timeout;                 /* Operation timeout in seconds;
                                        0.0 means non-blocking */

    struct addrinfo local_addrinfo;
    struct addrinfo remote_addrinfo;

    sock_addr_t  local_addr;
    int          local_addr_len;
    sock_addr_t  remote_addr;
    int          remote_addr_len;

    int   flags;
    int   error_occurred;

    Context *ctx;

    /* endpoint */
    char *ip;
    char *host;
    int   port;

    CRITICAL_SECTION cs;

    int       recvbuf_size;
    int       sendbuf_size;

    size_t send_id;

    WSABUF **wbufs;
    DWORD    nbufs;

    PyObject   *send_list;
    Py_ssize_t  send_nbytes;

    PyObject *protocol_type;
    PyObject *protocol;
    PyObject *exception_handler;
    PyObject *initial_bytes_callable;
    WSABUF    initial_bytes;

    int     io_op;
    TP_IO  *tp_io;

    /* Server-specific stuff. */
    int preallocate;
    WSAEVENT  fd_accept;
    Context  *wait_ctx;
    PxSocket *first;
    PxSocket *last;

    PxListHead *freelist;

    DWORD     rbytes;
    TP_WORK  *acceptex;
    CRITICAL_SECTION acceptex_cs;
    volatile long num_accepts_wanted;
    HANDLE  more_accepts;
    HANDLE  shutdown;
    HANDLE  wait_handles[3];

    /* Server socket clients. */
    PxSocket *parent;
    PxSocket *prev;
    PxSocket *next;

    /*
    PyObject *connection_made;
    PyObject *data_received;
    PyObject *data_sent;
    PyObject *line_received;
    PyObject *eof_received;
    PyObject *connection_lost;
    PyObject *connection_error;
    PyObject *connection_closed;
    PyObject *connection_timeout;

    PyObject *connection_cleanup;

    PyObject *network_up;
    PyObject *network_down;

    PyObject *exception_handler;
    PyObject *initial_connection_error;

    PyObject *initial_bytes_to_send;
    PyObject *initial_words_to_expect;
    PyObject *initial_regex_to_expect;

    char      line_mode;
    char      wait_for_eol;
    char      auto_reconnect;
    char     *eol[2];
    int       max_line_length;


    short     bufsize;

    __declspec(align(64))

#ifndef _WIN64
#define _PxSocket_BUFSIZE (4096-512)
#else
#define _PxSocket_BUFSIZE (4096-576)
#endif

    char buf[_PxSocket_BUFSIZE];
    */
} PxSocket;

#define I2S(i) (_Py_CAST_BACK(i, PxSocket *, PyObject, slist_entry))

#define PxSocket_GET_ATTR(n)                     \
    (PyObject_HasAttrString(s->protocol, n) ?    \
        PyObject_GetAttrString(s->protocol, n) : \
        Py_None)


static __inline
void
PxList_PushSocket(PxListHead *head, PxSocket *s)
{
    SLIST_ENTRY *entry = (SLIST_ENTRY *)(&(s->ob_base.slist_entry));
    InterlockedPushEntrySList(head, entry);
}

static __inline
PxSocket *
PxList_PopSocket(PxListHead *head)
{
    PxSocket *s;
    SLIST_ENTRY *entry = InterlockedPopEntrySList(head);

    if (!entry)
        return NULL;

    s = I2S(entry);

    return s;
}


static __inline
int
PxSocket_HasAttr(PxSocket *s, const char *callback)
{
    return PyObject_HasAttrString(s->protocol, callback);
}

void PxSocket_TrySendScheduled(Context *c);

void PxSocket_HandleError(Context *c,
                          int op,
                          const char *syscall,
                          int errcode);

int PxSocket_ConnectionClosed(PxSocket *s, int op);
int PxSocket_ConnectionLost(PxSocket *s, int op, int errcode);
int PxSocket_ConnectionTimeout(PxSocket *s, int op);
int PxSocket_ConnectionError(PxSocket *s, int op, int errcode);
int PxSocket_ConnectionDone(PxSocket *s);

void PxSocket_TryRecv(Context *c);

void
PxSocket_HandleCallback(
    Context *c,
    const char *name,
    const char *format,
    ...
);

int PxSocket_ScheduleBufForSending(Context *c, PxSocketBuf *b);
PxSocketBuf *PxSocket_GetInitialBytes(PxSocket *);
PxSocketBuf *_try_extract_something_sendable_from_object(Context *c,
                                                         PyObject *o,
                                                         int depth);

void
NTAPI
PxSocketClient_Callback(
    PTP_CALLBACK_INSTANCE instance,
    void *context,
    void *overlapped,
    ULONG io_result,
    ULONG_PTR nbytes,
    TP_IO *tp_io
);

void PxServerSocket_ClientClosed(Context *x);


void PxSocket_HandleException(Context *c, const char *syscall, int fatal);

int PxSocket_LoadInitialBytes(PxSocket *s);

__inline
PyObject *
_read_lock(PyObject *obj)
{
    AcquireSRWLockShared((PSRWLOCK)&(obj->srw_lock));
    return obj;
}
#define READ_LOCK(o) (_read_lock((PyObject *)o))

__inline
PyObject *
_read_unlock(PyObject *obj)
{
    ReleaseSRWLockShared((PSRWLOCK)&(obj->srw_lock));
    return obj;
}
#define READ_UNLOCK(o) (_read_unlock((PyObject *)o))

__inline
char
_try_read_lock(PyObject *obj)
{
    return TryAcquireSRWLockShared((PSRWLOCK)&(obj->srw_lock));
}
#define TRY_READ_LOCK(o) (_try_read_lock((PyObject *)o))

__inline
PyObject *
_write_lock(PyObject *obj)
{
    AcquireSRWLockExclusive((PSRWLOCK)&(obj->srw_lock));
    return obj;
}
#define WRITE_LOCK(o) (_write_lock((PyObject *)o))

__inline
PyObject *
_write_unlock(PyObject *obj)
{
    ReleaseSRWLockExclusive((PSRWLOCK)&(obj->srw_lock));
    return obj;
}
#define WRITE_UNLOCK(o) (_write_unlock((PyObject *)o))

__inline
char
_try_write_lock(PyObject *obj)
{
    return TryAcquireSRWLockExclusive((PSRWLOCK)&(obj->srw_lock));
}
#define TRY_WRITE_LOCK(o) (_try_write_lock((PyObject *)o))


#define DO_SEND_COMPLETE() do {                                          \
    PxSocket_HandleCallback(c, "send_complete", "(On)", s, s->send_id);  \
    if (PyErr_Occurred())                                                \
        goto end;                                                        \
} while (0)

#define MAYBE_DO_SEND_COMPLETE() do {                                    \
    if (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_SEND_COMPLETE)                \
        DO_SEND_COMPLETE();                                              \
} while (0)

#define DO_CONNECTION_MADE() do {                                        \
    assert(!(Px_SOCKFLAGS(s) & Px_SOCKFLAGS_SEND_SCHEDULED));            \
    PxSocket_HandleCallback(c, "connection_made", "(O)", s);             \
    if (PyErr_Occurred())                                                \
        goto end;                                                        \
} while (0)

#define DO_DATA_RECEIVED() do {                                          \
    const char *f = PxSocket_GetRecvCallback(s);                         \
    PxSocketBuf   *sbuf;                                                 \
    PyBytesObject *pbuf;                                                 \
    sbuf = c->rbuf_first;                                                \
    sbuf->ob_base.ob_size = c->io_nbytes;                                \
    pbuf = PxSocketBuf2PyBytesObject(sbuf);                              \
    PxSocket_HandleCallback(c, f, "(OO)", s, pbuf);                      \
    if (PyErr_Occurred())                                                \
        goto end;                                                        \
} while (0)

#define MAYBE_DO_CONNECTION_MADE() do {                                  \
    if ((Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_CONNECTION_MADE) &&          \
       !(Px_SOCKFLAGS(s) & Px_SOCKFLAGS_CALLED_CONNECTION_MADE))         \
        DO_CONNECTION_MADE();                                            \
} while (0)

#define MAYBE_DO_SEND_FAILED() do {                                      \
    if ((s->io_op == PxSocket_IO_SEND) &&                                \
        (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_SEND_FAILED))                \
    {                                                                    \
        PyObject *args, *func;                                           \
        args = Py_BuildValue("(Oni)", s, s->send_id, c->io_result);      \
        if (!args)                                                       \
            PxSocket_EXCEPTION();                                        \
        READ_LOCK(s);                                                    \
        func = PxSocket_GET_ATTR("send_failed");                         \
        READ_UNLOCK(s);                                                  \
        assert(func);                                                    \
        result = PyObject_CallObject(func, args);                        \
        if (null_with_exc_or_non_none_return_type(result, c->pstate))    \
            PxSocket_EXCEPTION();                                        \
    }                                                                    \
} while (0)

#define _m_MAYBE_CLOSE() do {                                            \
    if ((Px_SOCKFLAGS(s) & Px_SOCKFLAGS_CLOSE_SCHEDULED) ||              \
       !(Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_DATA_RECEIVED))              \
    {                                                                    \
        char error = 0;                                                  \
                                                                         \
        assert(!(Px_SOCKFLAGS(s) & Px_SOCKFLAGS_CLOSED));                \
                                                                         \
        s->io_op = PxSocket_IO_CLOSE;                                    \
                                                                         \
        if (closesocket(s->sock_fd) == SOCKET_ERROR) {                   \
            if (WSAGetLastError() == WSAEWOULDBLOCK)                     \
                Py_FatalError("closesocket() -> WSAEWOULDBLOCK!");       \
            else                                                         \
                error = 1;                                               \
        }                                                                \
                                                                         \
        Px_SOCKFLAGS(s) &= ~Px_SOCKFLAGS_CLOSE_SCHEDULED;                \
        Px_SOCKFLAGS(s) &= ~Px_SOCKFLAGS_CONNECTED;                      \
        Px_SOCKFLAGS(s) |=  Px_SOCKFLAGS_CLOSED;                         \
                                                                         \
        if (error)                                                       \
            PxSocket_HandleException(c, "closesocket", 0);               \
        else                                                             \
            PxSocket_HandleCallback(c, "connection_closed", "(O)", s);   \
        goto end;                                                        \
    }                                                                    \
} while (0)

#define MAYBE_SEND() do {                                                \
    if (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_SEND_SCHEDULED) {                 \
        PxSocket_TrySendScheduled(c);                                    \
        goto end;                                                        \
    }                                                                    \
} while (0)

#define MAYBE_RECV() do {                                                \
    if (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_DATA_RECEIVED) {              \
        PxSocket_TryRecv(c);                                             \
        goto end;                                                        \
    }                                                                    \
} while (0)

#define MAYBE_SHUTDOWN_SEND_OR_RECV() do {                               \
    if (!(Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_DATA_RECEIVED)) {           \
        if (shutdown(s->sock_fd, SD_RECEIVE) == SOCKET_ERROR)            \
            PxSocket_WSAERROR("shutdown(SD_RECEIVE)");                   \
    } else if (Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_SHUTDOWN_SEND) {       \
        if (shutdown(s->sock_fd, SD_SEND) == SOCKET_ERROR)               \
            PxSocket_WSAERROR("shutdown(SD_SEND)");                      \
    }                                                                    \
} while (0)

#ifdef Py_DEBUG
#define CHECK_SEND_RECV_CALLBACK_INVARIANTS() do {                       \
    if (!c->io_nbytes)                                                   \
        assert(c->io_result != NO_ERROR);                                \
                                                                         \
    if (c->io_result == NO_ERROR)                                        \
        assert(c->io_nbytes > 0);                                        \
    else                                                                 \
        assert(!c->io_nbytes);                                           \
} while (0)
#else
#define CHECK_SEND_RECV_CALLBACK_INVARIANTS() /* no-op */
#endif


#define PxSocket_FATAL() do {                                            \
    assert(PyErr_Occurred());                                            \
    PxSocket_HandleException(c, "", 1);                                  \
    goto end;                                                            \
} while (0)


#define PxSocket_EXCEPTION() do {                                        \
    assert(PyErr_Occurred());                                            \
    PxSocket_HandleException(c, "", 0);                                  \
    goto end;                                                            \
} while (0)

#define PxSocket_SYSERROR(n) do {                                        \
    PyErr_SetFromWindowsErr(0);                                          \
    PxSocket_HandleException(c, n, 1);                                   \
    goto end;                                                            \
} while (0)

#define PxSocket_WSAERROR(n) do {                                        \
    PyErr_SetFromWindowsErr(WSAGetLastError());                          \
    PxSocket_HandleException(c, n, 1);                                   \
    goto end;                                                            \
} while (0)

#define PxSocket_SOCKERROR(n) do {                                       \
    PxSocket_HandleError(c, op, n, WSAGetLastError());                   \
    goto end;                                                            \
} while (0)

#define PxSocket2WSABUF(s) (_Py_CAST_FWD(s, LPWSABUF, PxSocket, len))

#define OL2PxSocket(ol) (_Py_CAST_BACK(ol, PxSocket *, PxSocket, overlapped))

#define PxSocket_SET_DISCONNECTED(s) do {                                \
    Px_SOCKFLAGS(s) |= Px_SOCKFLAGS_DISCONNECTED;                        \
    Px_SOCKFLAGS(s) &= ~Px_SOCKFLAGS_CONNECTED;                          \
} while (0)

#define PxSocket_CLOSE(s) do {                                           \
    (void)closesocket((SOCKET)s->sock_fd);                               \
} while (0)

static PyTypeObject PxSocket_Type;
static PyTypeObject PxSocketBuf_Type;
static PyTypeObject PxClientSocket_Type;
static PyTypeObject PxServerSocket_Type;

static PySocketModule_APIObject PySocketModule;

#define PySocket_Type           PySocketModule.Sock_Type
#define getsockaddrarg          PySocketModule.getsockaddrarg
#define getsockaddrlen          PySocketModule.getsockaddrlen
#define makesockaddr            PySocketModule.makesockaddr
#define AcceptEx                PySocketModule.AcceptEx
#define ConnectEx               PySocketModule.ConnectEx
#define WSARecvMsg              PySocketModule.WSARecvMsg
#define WSASendMsg              PySocketModule.WSASendMsg
#define DisconnectEx            PySocketModule.DisconnectEx
#define TransmitFile            PySocketModule.TransmitFile
#define TransmitPackets         PySocketModule.TransmitPackets
#define GetAcceptExSockaddrs    PySocketModule.GetAcceptExSockaddrs

#define PxSocket_Check(v)         (Py_ORIG_TYPE(v) == &PxSocket_Type)
#define PxClientSocket_Check(v)   (Py_TYPE(v) == &PxClientSocket_Type)
#define PxServerSocket_Check(v)   (Py_TYPE(v) == &PxServerSocket_Type)

#define PXS2S(s) ((PySocketSockObject *)s)

#define Py_RETURN_BOOL(expr) return (              \
    ((expr) ? (Py_INCREF(Py_True), Py_True) :      \
              (Py_INCREF(Py_False), Py_False))     \
)

#define Px_PROTECTION_GUARD(o)                     \
    do {                                           \
        if (!_protected(o)) {                      \
            PyErr_SetNone(PyExc_ProtectionError);  \
            return NULL;                           \
        }                                          \
    } while (0)

#define Px_PERSISTENCE_GUARD(o)                    \
    do {                                           \
        if (!_persistent(o)) {                     \
            PyErr_SetNone(PyExc_PersistenceError); \
            return NULL;                           \
        }                                          \
    } while (0)

#define ENTERED_IO_CALLBACK()                 \
    _PyParallel_EnteredIOCallback(c,          \
                                  instance,   \
                                  overlapped, \
                                  io_result,  \
                                  nbytes,     \
                                  tp_io)

#define ENTERED_CALLBACK() _PyParallel_EnteredCallback(c, instance)

static __inline
int
_i_MAYBE_CLOSE_old(Context *c)
{
    PxSocket *s = (PxSocket *)c->io_obj;
    if ((Px_SOCKFLAGS(s) & Px_SOCKFLAGS_CLOSE_SCHEDULED) ||
       !(Px_SOCKFLAGS(s) & Px_SOCKFLAGS_HAS_DATA_RECEIVED))
    {
        char error = 0;

        assert(!(Px_SOCKFLAGS(s) & Px_SOCKFLAGS_CLOSED));

        s->io_op = PxSocket_IO_CLOSE;

        if (closesocket(s->sock_fd) == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
                Py_FatalError("closesocket() -> WSAEWOULDBLOCK!");
            else
                error = 1;
        }

        Px_SOCKFLAGS(s) &= ~Px_SOCKFLAGS_CLOSE_SCHEDULED;
        Px_SOCKFLAGS(s) &= ~Px_SOCKFLAGS_CONNECTED;
        Px_SOCKFLAGS(s) |=  Px_SOCKFLAGS_CLOSED;

        if (error)
            PxSocket_HandleException(c, "closesocket", 0);
        else
            PxSocket_HandleCallback(c, "connection_closed", "(O)", s);

        if (PxSocket_IS_SERVERCLIENT(s))
            PxServerSocket_ClientClosed(c);

        return 1;
    }
    return 0;
}

static const char *pxsocket_kwlist[] = {
    "host",
    "port",

    /* inherited from socket */
    "family",
    "type",
    "proto",

    /*
    "connection_made",
    "data_received",
    "line_received",
    "eof_received",
    "connection_lost",
    "connection_closed",
    "connection_timeout",
    "connection_done",

    "exception_handler",
    "initial_connection_error",

    "initial_bytes_to_send",
    "initial_words_to_expect",
    "initial_regex_to_expect",

    "duplex",
    "line_mode",
    "wait_for_eol",
    "auto_reconnect",
    "max_line_length",
    */

    NULL
};

static const char *pxsocket_protocol_attrs[] = {
    "connection_made",
    "data_received",
    "line_received",
    "eof_received",
    "connection_lost",
    "connection_closed",
    "connection_timeout",
    "connection_done",

    "exception_handler",
    "initial_connection_error",

    "initial_bytes_to_send",
    "initial_words_to_expect",
    "initial_regex_to_expect",

    "line_mode",
    "wait_for_eol",
    "auto_reconnect",
    "max_line_length",
    NULL
};

static const char *pxsocket_kwlist_formatstring = \
    /* optional below */
    "|"

    /* endpoint */
    "s#"    /* host + len */
    "i"     /* port */

    /* base */
    "i"     /* family */
    "i"     /* type */
    "i"     /* proto */

    ":socket";

    /* extensions */

//    "O"     /* connection_made */
//    "O"     /* data_received */
//    "O"     /* line_received */
//    "O"     /* eof_received */
//    "O"     /* connection_lost */
//    "O"     /* connection_closed */
//    "O"     /* connection_timeout */
//    "O"     /* connection_done */
//
//    "O"     /* exception_handler */
//    "O"     /* initial_connection_error */
//
//    "O"     /* initial_bytes_to_send */
//    "O"     /* initial_words_to_expect */
//    "O"     /* initial_regex_to_expect */
//
//    "p"     /* duplex */
//    "p"     /* line_mode */
//    "p"     /* wait_for_eol */
//    "p"     /* auto_reconnect */
//    "i"     /* max_line_length */
//
//    ":socket";

#define PxSocket_PARSE_ARGS                  \
    args,                                    \
    kwds,                                    \
    pxsocket_kwlist_formatstring,            \
    (char **)pxsocket_kwlist,                \
    &host,                                   \
    &hostlen,                                \
    &(s->port),                              \
    &(s->sock_family),                       \
    &(s->sock_type),                         \
    &(s->sock_proto)
    /*
    &(s->handler)
    &(s->connection_made),                   \
    &(s->data_received),                     \
    &(s->data_sent),                         \
    &(s->send_failed),                       \
    &(s->line_received),                     \
    &(s->eof_received),                      \
    &(s->connection_lost),                   \
    &(s->exception_handler),                 \
    &(s->initial_connection_error),          \
    &(s->initial_bytes_to_send),             \
    &(s->initial_words_to_expect),           \
    &(s->initial_regex_to_expect),           \
    &(s->line_mode),                         \
    &(s->wait_for_eol),                      \
    &(s->auto_reconnect),                    \
    &(s->max_line_length)
    */

#define PxSocket_XINCREF(s) do {             \
    if (Py_PXCTX)                            \
        break;                               \
    Py_XINCREF(s->protocol);                 \
    Py_XINCREF(s->connection_made);          \
    Py_XINCREF(s->data_received);            \
    Py_XINCREF(s->data_sent);                \
    Py_XINCREF(s->line_received);            \
    Py_XINCREF(s->eof_received);             \
    Py_XINCREF(s->connection_lost);          \
    Py_XINCREF(s->exception_handler);        \
    Py_XINCREF(s->initial_connection_error); \
    Py_XINCREF(s->initial_bytes_to_send);    \
    Py_XINCREF(s->initial_words_to_expect);  \
    Py_XINCREF(s->initial_regex_to_expect);  \
} while (0)

#define PxSocket_XDECREF(s) do {             \
    if (Py_PXCTX)                            \
        break;                               \
    Py_XDECREF(s->protocol);                 \
    Py_XDECREF(s->connection_made);          \
    Py_XDECREF(s->data_received);            \
    Py_XDECREF(s->data_sent);                \
    Py_XDECREF(s->line_received);            \
    Py_XDECREF(s->eof_received);             \
    Py_XDECREF(s->connection_lost);          \
    Py_XDECREF(s->exception_handler);        \
    Py_XDECREF(s->initial_connection_error); \
    Py_XDECREF(s->initial_bytes_to_send);    \
    Py_XDECREF(s->initial_words_to_expect);  \
    Py_XDECREF(s->initial_regex_to_expect);  \
} while (0)

//C_ASSERT(sizeof(PxSocket) == Px_PAGE_SIZE);

typedef struct _PxClientSocket {
    PxSocket _pxsocket;

    /* attributes */
    int auto_reconnect;
} PxClientSocket;

typedef struct _PxServerSocket {
    PxSocket _pxsocket;

    /* attributes */
    int auto_reconnect;
} PxServerSocket;

typedef struct _PxAddrInfo {
    PyObject_HEAD


} PxAddrInfo;


#ifdef __cpplus
}
#endif

#endif /* PYPARALLEL_PRIVATE_H */

/* vim:set ts=8 sw=4 sts=4 tw=78 et nospell: */
