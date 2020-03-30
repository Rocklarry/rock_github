#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_learn_app_1test_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


//JNI 创建
extern "C" JNIEXPORT jstring JNICALL
Java_com_learn_app_1test_MainActivity_stringTestJNI(JNIEnv *env, jobject instance) {

    const char *test = "JNI  TEST ";
    return env->NewStringUTF(test);

}