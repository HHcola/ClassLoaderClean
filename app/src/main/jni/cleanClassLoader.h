//
// Created by hewei05 on 16/1/15.
//

#ifndef PLUGDEMO_CLEANCLASSLOADER_H
#define PLUGDEMO_CLEANCLASSLOADER_H

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Inlines.h"
#include "Hash.h"

#define HASH_TOMBSTONE ((void*) 0xcbcacccd)     // invalid ptr value
#define LOAD_NUMER  5       // 62.5%
#define LOAD_DENOM  8


/* search for these criteria in the Class hash table */
struct ClassMatchCriteria {
    const char* descriptor;
    Object*     loader;
};

/*
 * Compare a hash entry with a "loose" item after their hash values match.
 * Returns { <0, 0, >0 } depending on ordering of items (same semantics
 * as strcmp()).
 */
typedef int (*HashCompareFunc)(const void* tableItem, const void* looseItem);



jboolean cleanClassLoader(JNIEnv * env, const char *className, jobject classLoader);

// get descriptor
char* dvmDotToDescriptor(const char* str);
void* dvmHashTableLookup(HashTable* pHashTable, u4 itemHash, void* item,
                         HashCompareFunc cmpFunc, bool doAdd);
u4 dvmComputeUtf8Hash(const char* utf8Str);
#endif //PLUGDEMO_CLEANCLASSLOADER_H
