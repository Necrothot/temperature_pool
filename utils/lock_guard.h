#ifndef LOCK_GUARD_H
#define LOCK_GUARD_H

#include <ucos.h>

#include "global_definitions/types.h"

// Analog of the std::lock_guard<T>
template <typename T,
          std::uint8_t (* CaptureFunc)(T *, std::uint16_t),
          std::uint8_t (* ReleaseFunc)(T *),
          std::uint16_t Timeout = 0>
class LockGuard
{
public:
    LockGuard() = delete;
    LockGuard(const LockGuard &) = delete;
    LockGuard & operator = (const LockGuard &) = delete;

    explicit LockGuard(T &os_struct) :
		os_struct_(os_struct)
    {
        CaptureFunc(&os_struct_, Timeout);
    }

    ~LockGuard()
    {
        ReleaseFunc(&os_struct_);
    }

private:
    T &os_struct_;

};

// Semaphore guard
template <WORD Timeout = 0>
using SemLockGuard = LockGuard<OS_SEM, OSSemPend, OSSemPost, Timeout>;

// Critical section guard
template <WORD Timeout = 0>
using CritLockGuard = LockGuard<OS_CRIT, OSCritEnter, OSCritLeave, Timeout>;

#endif // LOCK_GUARD_H
