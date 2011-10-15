0a 1b *2c* 3d

Mission statement
=================
We want to make writing cross-platform applications easier.
We want to make those apps more 'je ne sais quoi'; adding a touch of 3D, adding augmented reality, playing videos should be easy.
We want to allow you to add this framework as a component (supplimentary view) to your existening app.


Getting started
===============
Clone the repo.

Android - Import all projects in Dev/ folder into Eclipse, then run ndk-build in Dev/Android/Source folder.
iOS - Open Dev/iOS/2c.xcodeproj in xCode
Qt - Open Dev/Qt/Project/2c.pro in Qt Creator

Look at source file Dev/App/Source/SceneSample1.cpp
for the hook into the first scene that launches.

More detailed instructions and samples coming soon.


External components used
========================
3dsloader (BSD)
http://www.spacesimulator.net/wiki/index.php?title=3d_Engine_Programming_Tutorials
Loads the 3ds model format.
Found in Dev/External/3dsloader
*Modified - stripped down tutorial 5 and provided hooks into engine's cross platform file loader.

AdMob SDK
http://admob.com
An advertising provider.
Found in Dev/External/AdMob
Library file linked to.

Android NDK r5b (Apache 2.0)
http://developer.android.com/sdk/ndk/
Android NDK samples used for guidance to set u Android port.
Source file CCGLViewJNI.java found in Dev/Android/Source/src/com/android2c/ based off hello-gl2 sample.

Facebook iOS SDK (Apache 2.0)
https://developers.facebook.com/
Provides login to Facebook API.
Found in Dev/External/facebook-ios-sdk
*Modified - stripped out example files and provided direct callbacks into C++ engine classes rather than using their Objective C interface callbacks.

Jansson (MIT)
http://www.digip.org/jansson/
Jansson is a C library for encoding, decoding and manipulating JSON data.
Found in Dev/External/jansson-2.1
*Modified - stripped out example files.

libzip (libzip)
http://nih.at/libzip/
Used for Android port to open up packaged resources.
Found in Dev/Android/Source/jni/libzip

libpng (libpng)
http://libpng.org/
Contained in Android port for potential use to open png textures.
Found in Dev/Android/Source/jni/libpng

UIDevice-Hardware (BSD)
https://github.com/erica/uidevice-extension
Describes iOS hardware types.
Found in Dev/External/UIDevice-Hardware


External tools used
===================
RevanTech Bitmap Font Generator (BSD)
Used to generate font pages.
Found in Tools/RTBFG
*Modified - outputs fonts to fill 1024x1024 page.