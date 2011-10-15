/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCWindowController.h
 * Description : Controls the different app views.
 *
 * Created     : 30/08/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCWINDOWCONTROLLER_H__
#define __CCWINDOWCONTROLLER_H__


#include "CCGLView.h"
#include "CCVideoView.h"

#ifdef IOS
#import "CCViewController.h"
#import "CCARView.h"
#endif

class CCWindowController
{
public:
	CCWindowController();
    ~CCWindowController();
    
    void startup();
    void shutdown();
    void pause();
    void resume();
    
    void toggleAdverts(const bool toggle);

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
    
protected:
    CCGLView *glView;
    CCVideoView *videoView;

#ifdef IOS
    CCARView *arView;
	CCViewController *viewController;
    UIWindow *window;
#endif
    
    bool opaqueOpenGLRendering;
    
public:
    static CCWindowController *instance;
};


#endif // __CCWINDOWCONTROLLER_H__

