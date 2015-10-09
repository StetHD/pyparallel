#ifndef PXTIMEROBJECT_H
#define PXTIMEROBJECT_H

#ifdef __cpplus
extern "C" {
#endif

#include "Python.h"
#include "../Python/pyparallel_private.h"
#include <Windows.h>

void PxTimer_HandleException(Context *c, const char *syscall, int fatal);

#define PxTimer_FATAL() do {                                                   \
    assert(PyErr_Occurred());                                                  \
    PxTimer_HandleException(c, "", 1);                                         \
    goto end;                                                                  \
} while (0)


#define PxTimer_EXCEPTION() do {                                               \
    assert(PyErr_Occurred());                                                  \
    PxTimer_HandleException(c, "", 0);                                         \
    goto end;                                                                  \
} while (0)

#define PxTimer_SYSERROR(n) do {                                               \
    PyErr_SetFromWindowsErr(0);                                                \
    PxTimer_HandleException(c, n, 1);                                          \
    goto end;                                                                  \
} while (0)

#define Px_TIMERFLAGS(t) (((PxTimerObject *)(t))->flags)
#define Px_TIMERFLAGS_VALID                             (1)
#define Px_TIMERFLAGS_START_REQUESTED                   (1ULL <<  1)
#define Px_TIMERFLAGS_STARTED                           (1ULL <<  2)
#define Px_TIMERFLAGS_STOP_REQUESTED                    (1ULL <<  3)
#define Px_TIMERFLAGS_STOPPED                           (1ULL <<  4)
#define Px_TIMERFLAGS_RUNNING                           (1ULL <<  5)
#define Px_TIMERFLAGS_SNAPSHOT_UPDATE_SCHEDULED         (1ULL <<  6)
#define Px_TIMERFLAGS_                                  (1ULL << 63)

#define PxTimer_IS_VALID(t)                                                    \
    (Px_TIMERFLAGS(t) & Px_TIMERFLAGS_VALID)

#define PxTimer_START_REQUESTED(t)                                             \
    (Px_TIMERFLAGS(t) & Px_TIMERFLAGS_START_REQUESTED)

#define PxTimer_STARTED(t)                                                     \
    (Px_TIMERFLAGS(t) & Px_TIMERFLAGS_STARTED)

#define PxTimer_STOP_REQUESTED(t)                                              \
    (Px_TIMERFLAGS(t) & Px_TIMERFLAGS_STOP_REQUESTED)

#define PxTimer_STOPPED(t)                                                     \
    (Px_TIMERFLAGS(t) & Px_TIMERFLAGS_STOPPED)

#define PxTimer_RUNNING(t)                                                     \
    (Px_TIMERFLAGS(t) & Px_TIMERFLAGS_RUNNING)

#define PxTimer_XSET(timer, flag)                                              \
    InterlockedExchange(timer->flags, timer->flags | flag)

#define PxTimer_XUNSET(timer, flag)                                            \
    InterlockedExchange(timer->flags, timer->flags & ~flag)

#define PxTimer_SET(timer, flag)   Px_TIMERFLAGS(t) |= flag
#define PxTimer_UNSET(timer, flag) Px_TIMERFLAGS(t) &= ~flag

#define PxTimer_SET_START_REQUESTED(t)                                         \
    PxTimer_SET_FLAG(t, Px_TIMERFLAGS_START_REQUESTED)

#define PxTimer_SET_STARTED(t)                                                 \
    PxTimer_SET_FLAG(t, Px_TIMERFLAGS_STARTED)

#define PxTimer_SET_STOPPED(t)                                                 \
    PxTimer_SET_FLAG(t, Px_TIMERFLAGS_STOPPED)

#define PxTimer_SET_STOP_REQUESTED(t)                                          \
    PxTimer_SET_FLAG(t, Px_TIMERFLAGS_STOP_REQUESTED)

#define PxTimer_SET_STOPPED(t)                                                 \
    PxTimer_SET_FLAG(t, Px_TIMERFLAGS_STOPPED)

#define PxTimer_SET_RUNNING(t)                                                 \
    PxTimer_SET_FLAG(t, Px_TIMERFLAGS_RUNNING)

#define PxTimer_UNSET_RUNNING(t)                                               \
    PxTimer_UNSET_FLAG(t, Px_TIMERFLAGS_RUNNING)

typedef struct _PxTimerObject {
    PyObject_HEAD
    Context *ctx;
    HANDLE heap_override;
    volatile LONG flags;
    FILETIME duetime;
    DWORD period;
    DWORD window_length;
    PTP_TIMER ptp_timer;
    PTP_CALLBACK_ENVIRON ptp_cbe;
    LIST_ENTRY px_link;
    CRITICAL_SECTION cs;
    INIT_ONCE start_once;
    INIT_ONCE stop_once;
    INIT_ONCE shutdown_once;
    TP_WORK shutdown;
    PyObject *data;
    SRWLOCK data_srwlock;
} PxTimerObject;

PyAPI_FUNC(PyObject *) PxTimer_StartTimers(void);

PyAPI_FUNC(PyObject *) PxTimer_New(
    PyObject *duetime,
    PyObject *period,
    PyObject *window_length,
    PyObject *func,
    PyObject *args,
    PyObject *kwds,
    PyObject *errback
);

PyAPI_FUNC(int) PxTimer_Valid(PyObject *o);
#define PxTimer_VALID(t) PxTimer_Valid((PyObject *)t)

PyAPI_FUNC(PxTimerObject *) PxTimer_GetActive(void);

PyObject *pxtimer_set_data(PxTimerObject *t, PyObject *data);
PyObject *pxtimer_get_data(PxTimerObject *t);

PyObject *pxtimer_shutdown(PyObject *self);

#ifdef __cpplus
}
#endif

#endif /* PXTIMEROBJECT_H */

/* vim:set ts=8 sw=4 sts=4 tw=80 et nospell:                                  */
