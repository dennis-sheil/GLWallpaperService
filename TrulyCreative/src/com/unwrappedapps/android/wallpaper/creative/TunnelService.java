package com.unwrappedapps.android.wallpaper.creative;

import net.rbgrn.android.glwallpaperservice.*;
import android.content.SharedPreferences;

// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers
public class TunnelService extends GLWallpaperService {

    SharedPreferences sp;

    public TunnelService() {
        super();
    }

    @Override
    public void onCreate() {
        sp = ((TrulyApp)getApplication()).getThePrefs(TunnelWallpaper.SHARED_PREFS_NAME);
    }

    public Engine onCreateEngine() {
        MyEngine engine = new MyEngine();
        return engine;
    }

    class MyEngine extends GLEngine {
        //MyRenderer renderer;
        TunnelWallpaper renderer;
        public MyEngine() {
            super();
            // handle prefs, other initialization
            renderer = new TunnelWallpaper();
            setRenderer(renderer);
            setRenderMode(RENDERMODE_CONTINUOUSLY);
        }

        public void onDestroy() {
            super.onDestroy();
            if (renderer != null) {
                renderer.release(); // assuming yours has this method - it should!
            }
            renderer = null;
        }

        @Override
        public void onVisibilityChanged(boolean visible) {
            super.onVisibilityChanged(visible);
            if (visible) {
                renderer.doSP(sp);
            }
        }

    }

    static
    {
        System.loadLibrary ("optlist");
        System.loadLibrary ("bzip2");
        System.loadLibrary ("gl1");
    }

}
