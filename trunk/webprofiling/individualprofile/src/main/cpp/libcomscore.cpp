/* ****************************************************************************
 *
 * FILE                     main_samson_comscore_directory_create.cpp
 *
 * AUTHOR                   Andreu Urrueka
 *
 * CREATION DATE            2012
 *
 */

#include <jni.h>
#include <signal.h>

#include "au/ThreadManager.h"
#include "comscore/common.h"
#include "comscore/SamsonComscoreDictionary.h"
#include "parseArgs/paConfig.h"

#include "es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface.h"

typedef samson::comscore::uint uint;

samson::comscore::SamsonComscoreDictionary samson_comscore_dictionary;

PaArgument paArgs[] =
{
    PA_END_OF_ARGS
};

JNIEXPORT jboolean JNICALL Java_es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface_init
  (JNIEnv *env, jclass jClassObject) {
    paConfig("builtin prefix",                (void *) "CS_DICT_");
    paConfig("usage and exit on any warning", (void *) true);
    paConfig("log to screen",                 (void *) true);
    paConfig("screen line format", (void *) "TYPE@TIME  EXEC: TEXT");
    paConfig("log to file", (void *) false);
    paConfig("log file line format",
	    (void *) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
    char *argv[] = { (char *) "cs_dict", NULL };
    return !paParse(paArgs, 1, argv, 1, false); 
}

JNIEXPORT jboolean JNICALL Java_es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface_loadCSDictionary
  (JNIEnv *env, jobject jobj, jstring jdictionary_name) {
    jboolean isCopy;
    const char *dictionary_file_name =
                env->GetStringUTFChars(jdictionary_name, &isCopy);
    samson_comscore_dictionary.read(dictionary_file_name);
    env->ReleaseStringUTFChars(jdictionary_name, dictionary_file_name);

    return true;
}

JNIEXPORT jintArray JNICALL Java_es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface_lookupCategories
  (JNIEnv *env, jobject jobj, jstring jurl) {
    jboolean isCopy;
    const char *url = env->GetStringUTFChars(jurl, &isCopy);
    std::vector<uint> categories =
            samson_comscore_dictionary.getCategories(url);
    env->ReleaseStringUTFChars(jurl, url);

    jintArray jcategories = env->NewIntArray(categories.size());
    jint *pjcat= env->GetIntArrayElements(jcategories, &isCopy);
    for (size_t i = 0; i < categories.size(); ++i) {
        pjcat[i] = categories[i];
    }
    env->ReleaseIntArrayElements(jcategories, pjcat, 0);

    return jcategories;
}

JNIEXPORT jstring JNICALL Java_es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface_getCategoryName
  (JNIEnv *env, jobject jobj, jint jcategory) {
    const char *name = samson_comscore_dictionary.getCategoryName(jcategory);
    return env->NewStringUTF(name);
}
