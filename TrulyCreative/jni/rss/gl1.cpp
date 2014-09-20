#include <jni.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "driver.h"

#undef PI
#define PI 3.1415926535897932f

void drawRss();

int sWindowWidth = 0;
int sWindowHeight = 0;

int currentFlip = 1;

xstuff_t *XStuff;
struct timeval cycleStart, now, fps_time;
int frameTimeSoFar;
pthread_mutex_t mutg = PTHREAD_MUTEX_INITIALIZER;

int chosen;	//
// 0 hufo's tunnel
// 1 sundancer
// 2 lattice

extern "C" {
    JNIEXPORT void JNICALL
	Java_com_unwrappedapps_android_wallpaper_creative_CallNative_nativeInit
	(JNIEnv * env);
    JNIEXPORT void JNICALL
	Java_com_unwrappedapps_android_wallpaper_creative_CallNative_nativeResize
	(JNIEnv * env, jobject thiz, jint w, jint h);
    JNIEXPORT void JNICALL
	Java_com_unwrappedapps_android_wallpaper_creative_CallNative_nativeRender
	(JNIEnv * env);
    JNIEXPORT void JNICALL
	Java_com_unwrappedapps_android_wallpaper_creative_CallNative_nativeDone
	(JNIEnv * env);
    JNIEXPORT void JNICALL
	Java_com_unwrappedapps_android_wallpaper_creative_TunnelWallpaper_nativeSettings
	(JNIEnv * env, jobject thiz, jstring jhack, jstring hackPref,
	 jint draw);
    JNIEXPORT void JNICALL
	Java_com_unwrappedapps_android_wallpaper_creative_SundancerWallpaper_nativeSettings
	(JNIEnv * env, jobject thiz, jstring jhack, jstring hackPref,
	 jint draw);
    JNIEXPORT void JNICALL
	Java_com_unwrappedapps_android_wallpaper_creative_LatticeWallpaper_nativeSettings
	(JNIEnv * env, jobject thiz, jstring jhack, jstring hackPref,
	 jint draw);
};

void doinit()
{

    XStuff = (xstuff_t *) malloc(sizeof(xstuff_t));
    XStuff->commandLineName = NULL;
    XStuff->windowWidth = sWindowWidth;
    XStuff->windowHeight = sWindowHeight;
    if (chosen == 0) {
	ht_hack_init(XStuff);
    } else if (chosen == 1) {
	sd_hack_init(XStuff);
    } else {
	lt_hack_init(XStuff);
    }

    gettimeofday(&cycleStart, NULL);
    now = fps_time = cycleStart;
    frameTimeSoFar = 0;
}


JNIEXPORT void JNICALL
Java_com_unwrappedapps_android_wallpaper_creative_CallNative_nativeInit
(JNIEnv * env)
{

    doinit();
}

JNIEXPORT void JNICALL
Java_com_unwrappedapps_android_wallpaper_creative_CallNative_nativeResize
(JNIEnv * env, jobject thiz, jint w, jint h)
{
    sWindowWidth = w;
    sWindowHeight = h;
    XStuff->windowWidth = sWindowWidth;
    XStuff->windowHeight = sWindowHeight;
    if (chosen == 0) {
	ht_hack_reshape(XStuff);
    } else if (chosen == 1) {
	sd_hack_reshape(XStuff);
    } else {
	lt_hack_reshape(XStuff);
    }

}

JNIEXPORT void JNICALL
Java_com_unwrappedapps_android_wallpaper_creative_CallNative_nativeRender
(JNIEnv * env)
{
    drawRss();
}

JNIEXPORT void JNICALL
Java_com_unwrappedapps_android_wallpaper_creative_CallNative_nativeDone
(JNIEnv * env)
{
    if (chosen == 2) {
	lt_hack_cleanup(XStuff);
    }
}


JNIEXPORT void JNICALL
Java_com_unwrappedapps_android_wallpaper_creative_TunnelWallpaper_nativeSettings
(JNIEnv * env, jobject thiz, jstring jhack, jstring hackPref, jint draw)
{
    const char *chack = env->GetStringUTFChars(hackPref, 0);
    char *hck = (char *) chack;
    // bogus values passed.  This code must be cleaned up.
    char *zrgv[] = { "./hufo_tunnel", "-t", "2", "-c", hck, NULL };
    // texture: 1 (swirl), 2 (marble)
    // coarseness: 0 (none), 3 (very)
    int zrgc = sizeof(zrgv) / sizeof(char *) - 1;
    if (draw == 0) {
	ht_hack_handle_opts(zrgc, zrgv);
    }
    chosen = 0;
    env->ReleaseStringUTFChars(hackPref, chack);
}


JNIEXPORT void JNICALL
Java_com_unwrappedapps_android_wallpaper_creative_SundancerWallpaper_nativeSettings
(JNIEnv * env, jobject thiz, jstring jhack, jstring hackPref, jint draw)
{
    const char *chack = env->GetStringUTFChars(hackPref, 0);
    char *hck = (char *) chack;
    char *zrgv[] = { "./sundancer2", "-q", hck, NULL };
    int zrgc = sizeof(zrgv) / sizeof(char *) - 1;
    if (draw == 0) {
	sd_hack_handle_opts(zrgc, zrgv);
    }

    chosen = 1;

    env->ReleaseStringUTFChars(hackPref, chack);
}

JNIEXPORT void JNICALL
Java_com_unwrappedapps_android_wallpaper_creative_LatticeWallpaper_nativeSettings
(JNIEnv * env, jobject thiz, jstring jhack, jstring hackPref, jint draw)
{
    const char *chack = env->GetStringUTFChars(hackPref, 0);
    char *hck = (char *) chack;
    char *zrgv[] = { "./lattice", "-e", hck, NULL };
    int zrgc = sizeof(zrgv) / sizeof(char *) - 1;
    if (draw == 0) {
	lt_hack_handle_opts(zrgc, zrgv);
    }

    chosen = 2;

    env->ReleaseStringUTFChars(hackPref, chack);
}

int deltaus(const struct timeval now, const struct timeval then)
{
    return (now.tv_sec - then.tv_sec) * 1000000 + now.tv_usec -
	then.tv_usec;
}


void drawRss()
{
    pthread_mutex_lock(&mutg);
    gettimeofday(&now, NULL);
    frameTimeSoFar = deltaus(now, cycleStart);
    if (chosen == 0) {
	ht_hack_draw(XStuff,
		     (double) now.tv_sec + now.tv_usec / 1000000.0f,
		     frameTimeSoFar / 1000000.0f);
    } else if (chosen == 1) {
	sd_hack_draw(XStuff,
		     (double) now.tv_sec + now.tv_usec / 1000000.0f,
		     frameTimeSoFar / 1000000.0f);
    } else {
	lt_hack_draw(XStuff,
		     (double) now.tv_sec + now.tv_usec / 1000000.0f,
		     frameTimeSoFar / 1000000.0f);
    }
    pthread_mutex_unlock(&mutg);

}
