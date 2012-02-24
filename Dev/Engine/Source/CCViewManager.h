/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCViewManager.h
 * Description : Controls the different app views.
 *
 * Created     : 30/08/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCVIEWMANAGER_H__
#define __CCVIEWMANAGER_H__

#ifdef IOS
    #ifdef __OBJC__
        #include "CCGLView.h"
        #include "CCVideoView.h"
        #import "CCViewController.h"
        #import "CCARView.h"
    #else
        #define CCGLView void
        #define CCVideoView void
        #define CCARView void
        #define CCViewController void
        #define UIWindow void
    #endif
#else
    #include "CCGLView.h"
    #ifdef QT
        #include "CCVideoView.h"
    #endif
#endif

class CCViewManager
{
public:
	CCViewManager();
    ~CCViewManager();
    
    void startup();
    void shutdown();
    void pause();
    void resume();
    
    inline static const bool IsPortrait() { return orientation.target == 0.0f || orientation.target == 180.0f; }
    static void SetOrientation(const float targetOrientation, const bool immediatly=false);
    static void CorrectOrientation(float &x, float &y);
    static void UpdateOrientation(const float delta);
    inline static const CCTarget<float>& GetOrientation() { return orientation; }
    
    void toggleAdverts(const bool toggle);
    const float getAdvertHeight();

    void startVideoView(const char *file);
    void toggleVideoView(const bool toggle);
    void stopVideoView();
    
    void startARView();
    void stopARView();

protected:
    void toggleAdvertsNativeThread(const bool toggle);

    void startVideoViewNativeThread(const char *file);
    void toggleVideoViewNativeThread(const bool toggle);
    void stopVideoViewNativeThread();
    
    void startARViewNativeThread();
    void stopARViewNativeThread();
    
    void toggleBackgroundRender(const bool toggle);
    
public:
    static CCViewManager *instance;
    
protected:
    static CCTarget<float> orientation;
    enum OrientationState
    {
        Orientation_Set,
        Orientation_Updating,
        Orientation_Setting
    };
    static OrientationState orientationState;
    
    // Views
    CCGLView *glView;

#ifndef ANDROID
    CCVideoView *videoView;
#endif

#ifdef IOS
    CCARView *arView;
	CCViewController *viewController;
    UIWindow *window;
#endif
    
    bool opaqueOpenGLRendering;
};


#endif // __CCVIEWMANAGER_H__

