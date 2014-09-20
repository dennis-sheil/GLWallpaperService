package com.unwrappedapps.android.wallpaper.creative;

class CallNative {

    void onSurfaceCreated() {
        nativeInit();
    }

    void onSurfaceChanged(int w, int h) {
        nativeResize(w, h);
    }

    void onDrawFrame() {
        nativeRender();
    }

    void onDone() {
        nativeDone();
    }

    static native void nativeInit();
    static native void nativeResize(int w, int h);
    static native void nativeRender();
    static native void nativeDone();
}
