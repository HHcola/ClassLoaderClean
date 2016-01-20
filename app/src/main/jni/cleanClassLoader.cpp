//
// Created by hewei05 on 16/1/15.
//

#include "cleanClassLoader.h"
#include "Globals-4.1.2.h"
#include <dlfcn.h>

#define  PLUGIN_LOG_TAG "JNI-PLUGIN"
#define  LOGD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG,PLUGIN_LOG_TAG, fmt, ##args)
#define  LOGE(fmt, args...)  __android_log_print(ANDROID_LOG_ERROR,PLUGIN_LOG_TAG, fmt, ##args)



static int hashcmpClassByCrit(const void* vclazz, const void* vcrit)
{

    const ClassObject* clazz = (const ClassObject*) vclazz;
    const ClassMatchCriteria* pCrit = (const ClassMatchCriteria*) vcrit;
    bool match;

    // just match by descriptor
    match = (strcmp(clazz->descriptor, pCrit->descriptor) == 0 );

//             &&(clazz->classLoader == pCrit->loader));
    //if (match)
    //    ALOGI("+++ %s %p matches existing %s %p",
    //        pCrit->descriptor, pCrit->loader,
    //        clazz->descriptor, clazz->classLoader);
    return !match;
}

char* dvmDotToDescriptor(const char* str) {
    size_t length = strlen(str);
    int wrapElSemi = 0;
    char *newStr;
    char *at;

    if (str[0] != '[') {
        length += 2; /* for "L" and ";" */
        wrapElSemi = 1;
    }

    newStr = at = (char *) malloc(length + 1); /* + 1 for the '\0' */

    if (newStr == NULL) {
        return NULL;
    }

    if (wrapElSemi) {
        *(at++) = 'L';
    }

    while (*str) {
        char c = *(str++);
        if (c == '.') {
            c = '/';
        }
        *(at++) = c;
    }

    if (wrapElSemi) {
        *(at++) = ';';
    }

    *at = '\0';
    return newStr;
}

/*
 * Look up an entry.
 *
 * We probe on collisions, wrapping around the table.
 */
void* dvmHashTableLookup(HashTable* pHashTable, u4 itemHash, void* item,
                         HashCompareFunc cmpFunc, bool doAdd)
{
    LOGD("cleanClassLoader dvmHashTableLookup begin itemHash = %u , item = 0X%X", itemHash, item);
    HashEntry* pEntry;
    HashEntry* pEnd;
    void* result = NULL;

    assert(pHashTable->tableSize > 0);
    assert(item != HASH_TOMBSTONE);
    assert(item != NULL);

    LOGD("cleanClassLoader dvmHashTableLookup begin find data");
    /* jump to the first entry and probe for a match */
    int hashSize = itemHash & (pHashTable->tableSize-1);
    pEntry = &pHashTable->pEntries[itemHash & (pHashTable->tableSize-1)];
    pEnd = &pHashTable->pEntries[pHashTable->tableSize];
    while (pEntry->data != NULL) {
        const ClassObject* clazz = (const ClassObject*) pEntry->data;
        if (clazz != NULL) {
            LOGD("cleanClassLoader descriptor %s", clazz->descriptor);
        }
        if (pEntry->data != HASH_TOMBSTONE &&
            pEntry->hashValue == itemHash &&
            (*cmpFunc)(pEntry->data, item) == 0)
        {
            /* match */
            LOGD("cleanClassLoader +++ match on entry %d", pEntry - pHashTable->pEntries);
            break;
        }

        pEntry++;
        if (pEntry == pEnd) {     /* wrap around to start */
            if (pHashTable->tableSize == 1)
                break;      /* edge case - single-entry table */
            pEntry = pHashTable->pEntries;
        }

        LOGD("+++ look probing %d...", pEntry - pHashTable->pEntries);
    }

    if (pEntry->data == NULL) {
        LOGD("cleanClassLoader dvmHashTableLookup can't find data");
    } else {
        LOGD("cleanClassLoader dvmHashTableLookup find data");
        result = pEntry->data;
    }

    LOGD("cleanClassLoader dvmHashTableLookup end");
    return result;
}


/*
 * Compute a hash code on a UTF-8 string, for use with internal hash tables.
 *
 * This may or may not yield the same results as the java/lang/String
 * computeHashCode() function.  (To make sure this doesn't get abused,
 * I'm initializing the hash code to 1 so they *don't* match up.)
 *
 * It would be more correct to invoke dexGetUtf16FromUtf8() here and compute
 * the hash with the result.  That way, if something encoded the same
 * character in two different ways, the hash value would be the same.  For
 * our purposes that isn't necessary.
 */
u4 dvmComputeUtf8Hash(const char* utf8Str)
{
    u4 hash = 1;

    while (*utf8Str != '\0')
        hash = hash * 31 + *utf8Str++;

    return hash;
}


jboolean cleanClassLoader(JNIEnv *env, const char *className, jobject classLoader) {
    // Get globals gvm
    LOGD("cleanClassLoader begin");
    void* so = dlopen("/system/lib/libdvm.so", 0);
    void* gDvm_addr = dlsym(so, "gDvm");
    DvmGlobals *gDvm = (DvmGlobals*)gDvm_addr;
    LOGE("cleanClassLoader begin gDvm = 0X%X classLoader = 0X%X", gDvm, classLoader);

    HashTable *hash_table = gDvm->loadedClasses;
    int table_size = hash_table->tableSize;

    LOGE("cleanClassLoader begin table size = %d ", table_size);
    // get hash table
    char* descriptor = NULL;
    descriptor = dvmDotToDescriptor(className);
    LOGD("cleanClassLoader descriptor = %s", descriptor);

    ClassMatchCriteria crit;
    void* found;
    u4 hash;

    crit.descriptor = descriptor;
    crit.loader = (Object * )classLoader;
    hash = dvmComputeUtf8Hash(descriptor);


    jboolean result = JNI_FALSE;
    dvmHashTableLock(gDvm->loadedClasses);
    found = dvmHashTableLookup(gDvm->loadedClasses, hash, &crit,
                                   hashcmpClassByCrit, false);
    dvmHashTableUnlock(gDvm->loadedClasses);



    // clean classloader
    if (found != NULL) {
        LOGD("cleanClassLoader found 0X%X", found);
//        free(found);
        found = NULL;
        result = JNI_TRUE;
    }

    LOGD("cleanClassLoader end");
    return result;
}
