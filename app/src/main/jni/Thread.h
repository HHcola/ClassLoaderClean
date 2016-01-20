//
// Created by hewei05 on 16/1/17.
//

#ifndef PLUGDEMO_THREAD_H
#define PLUGDEMO_THREAD_H


#include <pthread.h>
#include <assert.h>
#include "Inlines.h"
// only get need fun

/*
 * Grab a plain mutex.
 */
INLINE void dvmLockMutex(pthread_mutex_t* pMutex)
{
    int cc __attribute__ ((__unused__)) = pthread_mutex_lock(pMutex);
    assert(cc == 0);
}

/*
 * Try grabbing a plain mutex.  Returns 0 if successful.
 */
INLINE int dvmTryLockMutex(pthread_mutex_t* pMutex)
{
    int cc = pthread_mutex_trylock(pMutex);
    assert(cc == 0 || cc == EBUSY);
    return cc;
}

/*
 * Unlock pthread mutex.
 */
INLINE void dvmUnlockMutex(pthread_mutex_t* pMutex)
{
    int cc __attribute__ ((__unused__)) = pthread_mutex_unlock(pMutex);
    assert(cc == 0);
}


#endif //PLUGDEMO_THREAD_H
