/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCViewManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCViewManager.h"

#ifdef QT
#include "CCMainWindow.h"
#elif ANDROID
#include "CCJNI.h"
#endif

#include "CCFileManager.h"


CCGLView *gView = NULL;
CCAppEngine *gEngine = NULL;

CCViewManager *CCViewManager::instance = NULL;
CCTarget<float> CCViewManager::orientation;
CCViewManager::OrientationState CCViewManager::orientationState = Orientation_Set;


CCViewManager::CCViewManager()
{
    instance = this;
    
    glView = NULL;

#ifdef IOS

    videoView = NULL;
    arView = NULL;

#endif

    opaqueOpenGLRendering = true;
}


CCViewManager::~CCViewManager()
{

#ifdef IOS

    if( videoView != NULL )
    {
        stopVideoView();
    }

    if( arView != NULL )
    {
        stopARView();
    }

    [glView release];

    [viewController release];
	[window release];

#else

    delete glView;

#endif

    instance = NULL;
}


void CCViewManager::startup()
{
#ifdef IOS
    // Create a full screen window
    CGRect rect = [[UIScreen mainScreen] bounds];
    
    CGRect statusBarRect = [[UIApplication sharedApplication] statusBarFrame];
    rect.size.height -= statusBarRect.size.height;
    rect.origin.y += statusBarRect.size.height * 0.5f;
    
    window = [[UIWindow alloc] initWithFrame:rect];
    viewController = [[CCViewController alloc] initWithNibName:NULL bundle:NULL];
    
    // Create OpenGL view and add to window
    gEngine = new CCAppEngine();
    glView = [[CCGLView alloc] initWithFrame:rect];
#elif defined QT

 //   QGraphicsScene scene;
  //  scene.setSceneRect( QRectF( 0, 0, 300, 300 ) );

//    glView = new CCGLView( NULL );
//    CCGraphicsView *graphicsView = new CCGraphicsView( &scene, CCMainWindow::instance );
//    CCMainWindow::instance->addChild( graphicsView );
//    graphicsView->setViewport( glView );

    glView = new CCGLView( CCMainWindow::instance );
    gEngine = new CCAppEngine();
    CCMainWindow::instance->addChild( glView );

#elif defined ANDROID

	// Create our game engine system
	glView = new CCGLView();
    gEngine = new CCAppEngine();

#endif

#ifdef IOS
    [window addSubview:glView];
    [window makeKeyAndVisible];
#endif
}


void CCViewManager::shutdown()
{
    gEngine->running = false;

    // Qt isn't multi-threaded yet, on Android this get's called from the rendering thread.
#ifndef IOS
    gEngine->engineThreadRunning = false;
#endif

    while( gEngine->engineThreadRunning )
    {
        usleep( 0 );
    }

    if( gEngine != NULL )
    {
        gEngine->serialize( true );
    }

#ifdef IOS

    [glView shutdown];

#elif defined QT

    glView->shutdown();

#endif
}


void CCViewManager::pause()
{
    if( gEngine != NULL )
    {
        gEngine->paused = true;
    }
}


void CCViewManager::resume()
{
    if( gEngine != NULL )
    {
        gEngine->paused = false;
    }
}


void CCViewManager::SetOrientation(const float targetOrientation, const bool immediatly)
{
    orientation.target = targetOrientation;
    //if( orientation.current != orientation.target )
    {
        orientationState = immediatly ? Orientation_Setting : Orientation_Updating;
        
        // Update our controls
        CCControls::RefreshTouchMovementThreashold();
    }
}


void CCViewManager::CorrectOrientation(float &x, float &y)
{
    if( orientation.target == 270.0f )
    {
        CCSwapFloat( x, y );
        x = 1.0f - x;
        y = 1.0f - y;
    }
    else if( orientation.target == 90.0f )
    {
        CCSwapFloat( x, y );
    }
    else if( orientation.target == 180.0f )
    {
        x = 1.0f - x;
    }
    else
    {
        y = 1.0f - y;
    }
}


void CCViewManager::UpdateOrientation(const float delta)
{
    if( orientationState != Orientation_Set )
    {
        CCDestructList<CCSceneBase> &scenes = gEngine->scenes;
        for( int i=0; i<scenes.length; ++i )
        {
            CCSceneBase *scene = scenes.list[i];
            scene->beginOrientationUpdate();
        }
        
        // Immediate update?
        if( instance->orientationState == Orientation_Setting )
        {
            orientation.current = orientation.target - CC_SMALLFLOAT;
        }
        orientationState = Orientation_Set;
    }
    
    if( orientation.current != orientation.target )
    {
        if( CCToRotation( orientation.current, orientation.target, delta * 360.0f ) )
        {
            for( int i=0; i<gEngine->cameras.length; ++i )
            {
                CCCameraBase *camera = gEngine->cameras.list[i];
                camera->flagUpdate();
            }
            
            for( int i=0; i<gEngine->scenes.length; ++i )
            {
                gEngine->scenes.list[i]->finishOrientationUpdate();
            }
        }
    }
}


void CCViewManager::toggleAdverts(const bool toggle)
{
    class ThreadCallback : public CCLambdaCallback     
    {
    public:                                                             
        ThreadCallback(const bool toggle)
        {    
            this->toggle = toggle;
        }
        void run()
        {
            CCViewManager::instance->toggleAdvertsNativeThread( toggle );
        }                        
    private:
        bool toggle;
    };
    gEngine->engineToNativeThread( new ThreadCallback( toggle ) );
}


const float CCViewManager::getAdvertHeight()
{
#ifdef IOS
    return 50.0f/gEngine->renderer->getScreenSize().height;
#elif ANDROID
    
    const float bannerWidth = 320.0f;
    const float bannerHeight = 50.0f;
    const float screenWidth = gEngine->renderer->getScreenSize().width;
    const float screenHeight = gEngine->renderer->getScreenSize().height;
    
    float scale = screenWidth / bannerWidth;
    float scaledHeight = bannerHeight * scale;
    return scaledHeight / screenHeight;
    
#endif
    
    return 0.1f;
}


void CCViewManager::startVideoView(const char *file)
{
    class ThreadCallback : public CCLambdaCallback     
    {
    public:                                                             
        ThreadCallback(const char *file)
        {    
            this->file = file;
        }
        void run()
        {
            CCViewManager::instance->startVideoViewNativeThread( this->file.buffer );
        }                        
    private:
        CCText file;
    };
    gEngine->engineToNativeThread( new ThreadCallback( file ) );
}


void CCViewManager::toggleVideoView(const bool toggle)
{
    class ThreadCallback : public CCLambdaCallback     
    {
    public:                                                             
        ThreadCallback(const bool toggle)
        {    
            this->toggle = toggle;
        }
        void run()
        {
            CCViewManager::instance->toggleVideoViewNativeThread( toggle );
        }                        
    private:
        bool toggle;
    };
    gEngine->engineToNativeThread( new ThreadCallback( toggle ) );
}


void CCViewManager::stopVideoView()
{
    LAMBDA_UNSAFE( ThreadCallback, CCViewManager::instance->stopVideoViewNativeThread(); );
    gEngine->engineToNativeThread( new ThreadCallback() );
}


void CCViewManager::startARView()
{
    LAMBDA_UNSAFE( ThreadCallback, CCViewManager::instance->startARViewNativeThread(); );
    gEngine->engineToNativeThread( new ThreadCallback() );
}


void CCViewManager::stopARView()
{
    LAMBDA_UNSAFE( ThreadCallback, CCViewManager::instance->stopARViewNativeThread(); );
    gEngine->engineToNativeThread( new ThreadCallback() );
}


void CCViewManager::toggleAdvertsNativeThread(const bool toggle)
{
#ifdef IOS
    
    [viewController toggleAdverts:toggle];
    
#elif ANDROID
    
    CCJNI::AdvertsToggle( toggle );
    
#endif
}


void CCViewManager::startVideoViewNativeThread(const char *file)
{
#ifdef QT
    return;
#endif
    
    toggleBackgroundRender( true );
    
    CCText fullFilePath;
    CCFileManager::getFilePath( fullFilePath, file, Resource_Packaged );
    
#ifdef IOS

    ASSERT( videoView == NULL );
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    videoView = [[CCVideoView alloc] initWithFrame:rect];
    [window insertSubview:videoView belowSubview:glView];
    
    [videoView playVideo:fullFilePath.buffer];
    
#elif defined ANDROID

    CCJNI::VideoViewStart( fullFilePath.buffer );

#elif defined QT

    videoView = new CCVideoView( CCMainWindow::instance );
    CCMainWindow::instance->addChild( videoView );
    videoView->playVideo( fullFilePath.buffer );
    videoView->lower();
    
#endif
}


void CCViewManager::toggleVideoViewNativeThread(const bool toggle)
{
#ifdef IOS
    
    ASSERT( videoView != NULL );
    if( videoView->playing && toggle == false )
    {
        [videoView pause];
    }
    else if( videoView->playing == false && toggle )
    {
        [videoView play];
    }
    
#elif defined ANDROID
    
    CCJNI::VideoViewStop();
    
#endif
}



void CCViewManager::stopVideoViewNativeThread()
{
#ifdef IOS

    ASSERT( videoView != NULL );
    [videoView stop];
    [videoView remove];
    const int count = [videoView retainCount];
    for( int i=0; i<count; ++i )
    {
        [videoView release];
    }
    videoView = NULL;
    
    if( arView == NULL )
    {
        toggleBackgroundRender( false );
    }

#elif defined ANDROID

    CCJNI::VideoViewStop();

#endif
}


void CCViewManager::startARViewNativeThread()
{
#ifdef IOS
    
    ASSERT( arView == NULL );
    
    toggleBackgroundRender( true );
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    arView = [[CCARView alloc] initWithFrame:rect];
    [window insertSubview:arView belowSubview:glView];
    
#endif
}


void CCViewManager::stopARViewNativeThread()
{
#ifdef IOS
    
    ASSERT( arView != NULL );
    [arView remove];
    //const int count = [videoView retainCount];
    [arView release];
    arView = NULL;
    
    if( videoView == NULL )
    {
        toggleBackgroundRender( false );
    }
    
#endif
}


void CCViewManager::toggleBackgroundRender(const bool toggle)
{
    if( opaqueOpenGLRendering == toggle )
    {
        opaqueOpenGLRendering = !toggle;
#ifdef IOS
        CAEAGLLayer *eaglLayer = (CAEAGLLayer*)glView.layer;
        eaglLayer.opaque = !toggle;
#endif
    }
}
