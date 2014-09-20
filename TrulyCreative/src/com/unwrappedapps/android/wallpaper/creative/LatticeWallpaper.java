package com.unwrappedapps.android.wallpaper.creative;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import net.rbgrn.android.glwallpaperservice.*;
import android.opengl.GLU;

import android.content.Context;
import android.content.SharedPreferences;

public class LatticeWallpaper extends ARenderer {

    private static native void nativeSettings(String hack, String hackPref, int draw);
    public static final String SHARED_PREFS_NAME="latticesettings";
    CallNative cn;
    boolean initRun = false;

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        super.onSurfaceCreated(gl, config);

        cn = new CallNative();

        if (initRun) {
            NonSurfaceCreated();
        }
    }

    public void onDrawFrame(GL10 gl) {
        super.onDrawFrame(gl);
        if (initRun) {
            nativeSettings("bogus", "bogus", 1);
            NonDrawFrame();
        }
        initRun = true;
    }

    void NonDrawFrame() {
        cn.nativeRender();

    }

    void doSP(SharedPreferences sspp) {
        String hackPref = sspp.getString("lattice_speed", "15"); // key
        String hack = "lattice";
        nativeSettings(hack, hackPref, 0);
    }


    static
    {
        System.loadLibrary ("gl1");
        System.loadLibrary ("optlist");
        System.loadLibrary ("bzip2");
    }

}
