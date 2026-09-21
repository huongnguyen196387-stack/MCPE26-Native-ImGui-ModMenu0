#include "modmenu.h"
#include <jni.h>

extern "C" JNIEXPORT void JNICALL
Java_com_example_mcpeimgui_MainActivity_nativeInit(JNIEnv*, jclass) {
    modmenu::Init();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_mcpeimgui_MainActivity_nativeResize(
    JNIEnv*, jclass, jint width, jint height) {
    modmenu::Resize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_mcpeimgui_MainActivity_nativeRender(JNIEnv*, jclass) {
    modmenu::Render();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_mcpeimgui_MainActivity_nativeTouch(
    JNIEnv*, jclass, jint action, jfloat x, jfloat y) {
    modmenu::Touch(action, x, y);
}
