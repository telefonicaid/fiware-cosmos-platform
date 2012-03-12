#include <jni.h>
#include <signal.h>

#include "au/ThreadManager.h"
#include "comscore/common.h"
#include "comscore/SamsonComscoreDictionary.h"
#include "logMsg/logMsg.h"

#include "es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface.h"

using namespace std;

typedef samson::comscore::uint uint;

class CSDict : public samson::comscore::SamsonComscoreDictionary {
    public:
	vector<string> getAllCategoryNames() {
	    vector<string> names;
	    samson::comscore::Id2Id* category = categories.v;
	    for (size_t i = 0; i < categories.size; i++) {
		names.push_back(getCategoryName(category->first));
		category++;
	    }
	    return names;
	}
};

CSDict dictionary;

// TODO: Bug workaround remove when its fixed in Samson
extern char *progName;

JNIEXPORT jboolean JNICALL Java_es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface_loadCSDictionary
  (JNIEnv *env, jobject jobj, jstring jdictionary_name) {
    if (progName == NULL) {
        LmStatus status = lmInitX((char *)"ComscoreDict", NULL, NULL, NULL);
        // TODO: Bug workaround remove when its fixed in Samson
        progName = strdup(progName);
        if (status != LmsOk) {
            std::cerr << "LM failed with status " << status << std::endl;
            return false;
        }
    }

    jboolean isCopy;
    const char *dictionary_file_name =
                env->GetStringUTFChars(jdictionary_name, &isCopy);
    dictionary.read(dictionary_file_name);
    env->ReleaseStringUTFChars(jdictionary_name, dictionary_file_name);

    return true;
}

JNIEXPORT jintArray JNICALL Java_es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface_lookupCategories
  (JNIEnv *env, jobject jobj, jstring jurl) {
    jboolean isCopy;
    const char *url = env->GetStringUTFChars(jurl, &isCopy);
    std::vector<uint> categories =
            dictionary.getCategories(url);
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
    const char *name = dictionary.getCategoryName(jcategory);
    return env->NewStringUTF(name);
}

JNIEXPORT jobjectArray JNICALL Java_es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface_getAllCategoryNames
  (JNIEnv *env, jobject jobj) {
    vector<string> names = dictionary.getAllCategoryNames();
    jobjectArray jnames = env->NewObjectArray(names.size(),
	    env->FindClass("java/lang/String"), NULL);
    for (size_t i = 0; i < names.size(); ++i) {
	env->SetObjectArrayElement(jnames, i,
		env->NewStringUTF(names[i].c_str()));
    }
    return jnames;
}
