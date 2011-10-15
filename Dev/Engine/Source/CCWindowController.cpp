/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCWindowController.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCWindowController.h"
#include "CCFileManager.h"

#ifdef QT
#include "CCMainWindow.h"
#endif


CCGLView *gView = NULL;
CCAppEngine *gEngine = NULL;

CCWindowController *CCWindowController::instance = NULL;


CCWindowController::CCWindowController()
{
    instance = this;
    
    glView = NULL;
    videoView = NULL;

#ifdef IOS

    arView = NULL;

#endif

    opaqueOpenGLRendering = true;
}


CCWindowController::~CCWindowController()
{
    if( videoView != NULL )
    {
        stopVideoView();
    }

#ifdef IOS

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


void CCWindowController::startup()
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
    glView = [[CCGLView alloc] initWithFrame:rect];
#elif defined QT

 //   QGraphicsScene scene;
  //  scene.setSceneRect( QRectF( 0, 0, 300, 300 ) );

//    glView = new CCGLView( NULL );
//    CCGraphicsView *graphicsView = new CCGraphicsView( &scene, CCMainWindow::instance );
//    CCMainWindow::instance->addChild( graphicsView );
//    graphicsView->setViewport( glView );

    glView = new CCGLView( CCMainWindow::instance );
    CCMainWindow::instance->addChild( glView );

#elif defined ANDROID

	// Create our game engine system
	glView = new CCGLView();

#endif

#ifdef IOS
    [window addSubview:glView];
    [window makeKeyAndVisible];
#endif
}


void CCWindowController::shutdown()
{
    glView->runningGame = false;

    // Qt isn't multi-threaded yet, on Android this get's called from the rendering thread.
#ifndef IOS
    glView->gameThreadRunning = false;
#endif

    while( glView->gameThreadRunning )
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


void CCWindowController::pause()
{
    if( glView != NULL )
    {
        glView->paused = true;
    }
}


void CCWindowController::resume()
{
    if( glView != NULL )
    {
        glView->paused = false;
    }
}


void CCWindowController::toggleAdverts(const bool toggle)
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
            CCWindowController::instance->toggleAdvertsNativeThread( toggle );
        }                        
    private:
        bool toggle;
    };
    gEngine->runOnNativeThread( new ThreadCallback( toggle ) );
}


void CCWindowController::startVideoView(const char *file)
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
            CCWindowController::instance->startVideoViewNativeThread( this->file.buffer );
        }                        
    private:
        CCText file;
    };
    gEngine->runOnNativeThread( new ThreadCallback( file ) );
}


void CCWindowController::toggleVideoView(const bool toggle)
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
            CCWindowController::instance->toggleVideoViewNativeThread( toggle );
        }                        
    private:
        bool toggle;
    };
    gEngine->runOnNativeThread( new ThreadCallback( toggle ) );
}


void CCWindowController::stopVideoView()
{
    LAMBDA_UNSAFE( ThreadCallback, CCWindowController::instance->stopVideoViewNativeThread(); );
    gEngine->runOnNativeThread( new ThreadCallback() );
}


void CCWindowController::startARView()
{
    LAMBDA_UNSAFE( ThreadCallback, CCWindowController::instance->startARViewNativeThread(); );
    gEngine->runOnNativeThread( new ThreadCallback() );
}


void CCWindowController::stopARView()
{
    LAMBDA_UNSAFE( ThreadCallback, CCWindowController::instance->stopARViewNativeThread(); );
    gEngine->runOnNativeThread( new ThreadCallback() );
}


void CCWindowController::toggleAdvertsNativeThread(const bool toggle)
{
#ifdef IOS
    
    [viewController toggleAdverts:toggle];
    
#endif
}


void CCWindowController::startVideoViewNativeThread(const char *file)
{
#ifdef QT
    return;
#endif
    
    ASSERT( videoView == NULL );
    
    toggleBackgroundRender( true );
    
    CCText fullFilePath;
    CCFileManager::getFilePath( fullFilePath, file, Resource_Packaged );
    
#ifdef IOS
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    videoView = [[CCVideoView alloc] initWithFrame:rect];
    [window insertSubview:videoView belowSubview:glView];
    
    [videoView playVideo:fullFilePath.buffer];
    
#elif defined ANDROID

    CCVideoView::startVideoView( fullFilePath.buffer );

#elif defined QT

    videoView = new CCVideoView( CCMainWindow::instance );
    CCMainWindow::instance->addChild( videoView );
    videoView->playVideo( fullFilePath.buffer );
    videoView->lower();
    
#endif
}


void CCWindowController::toggleVideoViewNativeThread(const bool toggle)
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
    
    CCVideoView::stopVideoView();
    
#endif
}



void CCWindowController::stopVideoViewNativeThread()
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

    CCVideoView::stopVideoView();

#endif
}


void CCWindowController::startARViewNativeThread()
{
#ifdef IOS
    
    ASSERT( arView == NULL );
    
    toggleBackgroundRender( true );
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    arView = [[CCARView alloc] initWithFrame:rect];
    [window insertSubview:arView belowSubview:glView];
    
#endif
}


void CCWindowController::stopARViewNativeThread()
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


void CCWindowController::toggleBackgroundRender(const bool toggle)
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
