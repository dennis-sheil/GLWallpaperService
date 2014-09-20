package com.unwrappedapps.android.wallpaper.creative;

import android.app.Application;
import android.content.SharedPreferences;

public class TrulyApp extends Application {

    public TrulyApp() {
        super();
    }


    SharedPreferences getThePrefs(String p) {
        SharedPreferences prefs = getApplicationContext().getSharedPreferences(p, 0);
        return prefs;
    }

}
