//
// Created by hewei05 on 16/1/15.
//
#include "com_mobovee_plugin_classloader_MainActivity.h"
#include "cleanClassLoader.h"

JNIEXPORT jboolean JNICALL Java_com_mobovee_plugin_pluginmgr_DyJniProxy_OnClearClassLoader
        (JNIEnv * env, jobject instance, jstring className, jobject classLoader) {

    const char * pClassName = env->GetStringUTFChars(className, 0);
    jboolean result = cleanClassLoader(env, pClassName, classLoader);
    env->ReleaseStringUTFChars(className, pClassName);

    if (result) {
        return JNI_TRUE;
    }

    return JNI_FALSE;
}