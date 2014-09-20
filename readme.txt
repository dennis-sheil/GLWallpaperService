Truly Creative Live Wallpaper - Version 1.1


Upstream codebases
------------------

This is a port of some of the Really Slick Screensavers to Android.  It 
uses the RSS-GLX version of the Really Slick Screensavers.

On the Android side, it uses Ben Gruver's version of Mark Guerra's GL 
WallpaperService for Android.

RSS-GLX is OpenGL, and Android is OpenGL ES, 
so we use the OpenGL to OpenGL ES translation shim that is in Xscreensaver.

As optlist is more thread-safe, we use that instead of getopt.  Some of 
the screensavers use bzip so we include that library as well.


Compiling
---------

We are compiling our APK with ant.  If you have a problem getting this 
to work with Eclipse, Android Studio, or some other IDE, let us know.  
TrulyCreative is the Service, and GLWallpaperService is the library 
which TrulyCreative uses.  Also don't forget this uses C/C++ code via 
the NDK, so you have to build both the C/C++ and Java/Dalvik code.

This app can be downloaded on Google Play here -

https://play.google.com/store/apps/details?id=com.unwrappedapps.android.wallpaper.creative


Licenses
--------

Some code in jni/rss/gl1.cpp is based off code from Jetro Lauha's San 
Angeles Observation project which is under a BSD-style license.

Some other code in jni/rss is based off code from "Really Slick 
Screensavers".  Most of that is released under the GPL.

Much of the OpenGL to OpenGL ES code conversion is done by code written 
by Jamie Zawinski for XScreensaver.  That is under the X11 license.  It 
covers the three files in jni/rss with the prefix "jwzgles".

The bzip2 package (jni/bzip2) is released under a BSD-like license.

Michael Dipperstein's Optlist library (jni/optlist) is released under 
the GPL.

GLWallpaperService is under an Apache License, Version 2.0.

The rest of the code is released under Apache License, Version 2.0.  
Some of that code is from the Android Open Source Project, some is from 
Dennis Sheil.
