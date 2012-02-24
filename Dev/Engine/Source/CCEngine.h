/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCEngine.h
 * Description : Handles the update and render loop and feature managers.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCENGINE_H__
#define __CCENGINE_H__


struct CCTime
{
	CCTime()
	{
		real = 0.0f;
		delta = 0.0f;
        lifetime = 0.0f;
        lastUpdate = 0;
	}
	
    float real;
	float delta;
	float lifetime;

    double lastUpdate;
};


#include "CCControls.h"
#include "CCCameraBase.h"
#include "CCTextureManager.h"
#include "CCOctree.h"
#include "CCRenderer.h"
#include "CCURLManager.h"

class CCSceneBase;
class CCSceneCollideable;

class CCEngine
{
public:
	CCEngine();
	virtual ~CCEngine();
	
public:
	virtual void setupNativeThread();
    virtual void setupEngineThread();
	
    void addCamera(CCCameraBase *camera, const int index=-1);
	const bool removeCamera(CCCameraBase *camera);
    void refreshCameras();
	
	void addScene(CCSceneBase *scene, const int index=-1);
    void removeScene(CCSceneBase *scene);
	
	virtual const bool updateTime();
    
	virtual const bool updateNativeThread();
	virtual void updateEngineThread();
    virtual const bool updateJobsThread();
    
protected:
	virtual void start() = 0;
	virtual void updateLoop();
	void renderLoop();
    
public:
    void renderFrameBuffer(const int frameBufferId);
    
protected:
	// Finishes a job on the engine thread
	virtual void finishJobs();
    
public:
    virtual void restart();
	
	void addCollideable(CCSceneCollideable* collideable);
	void removeCollideable(CCSceneCollideable* collideable);
	
    virtual const bool serialize(const bool saving=false);
    
    // Run on another thread
    void engineToNativeThread(CCLambdaCallback *lambdaCallback);
    void nativeToEngineThread(CCLambdaCallback *lambdaCallback);
    void engineToJobsThread(CCLambdaCallback *lambdaCallback, const bool pushToFront=false);
    void jobsToEngineThread(CCLambdaCallback *lambdaCallback);
	
public:
	bool running, paused, engineThreadRunning;
    
	CCRenderer *renderer;
	CCControls *controls;
	CCTextureManager *textureManager;
	CCURLManager *urlManager;
	
    CCList<CCCameraBase> cameras;
    CCDestructList<CCSceneBase> scenes;
	
    // Our Octree collideables container
	CCCollisionManager collisionManager;

    // Engine level controls used for timers and such
    CCDestructList<CCUpdater> updaters;
	
	CCTime time;
	uint renderFlags;
	float fpsLimit;
	
protected:
    CCList<CCLambdaCallback> nativeThreadCallbacks;
    CCList<CCLambdaCallback> engineThreadCallbacks;
    CCList<CCLambdaCallback> jobsThreadCallbacks;
};


#define LAMBDA_RUN_NATIVETHREAD(FUNCTION_CALL)                      \
LAMBDA_UNSAFE( ThreadCallback,                                      \
    FUNCTION_CALL                                                   \
)                                                                   \
gEngine->engineToNativeThread( new ThreadCallback() );


#define LAMBDA_RUN_ENGINETHREAD(FUNCTION_CALL)                      \
LAMBDA_UNSAFE( ThreadCallback,                                      \
   FUNCTION_CALL                                                    \
)                                                                   \
gEngine->nativeToEngineThread( new ThreadCallback() );              \

#define LAMBDA_CONNECT_NATIVETHREAD(EVENT, FUNCTION_CALL)           \
LAMBDA_UNSAFE( EventCallback,                                       \
    LAMBDA_UNSAFE( ThreadCallback,                                  \
        FUNCTION_CALL                                               \
    )                                                               \
    gEngine->engineToNativeThread( new ThreadCallback() );          \
)                                                                   \
EVENT = new EventCallback();


#define LAMBDA_CONNECT_ENGINETHREAD(EVENT, FUNCTION_CALL)           \
LAMBDA_UNSAFE( EventCallback,                                       \
    LAMBDA_UNSAFE( ThreadCallback,                                  \
        FUNCTION_CALL                                               \
    )                                                               \
    gEngine->nativeToEngineThread( new ThreadCallback() );          \
)                                                                   \
EVENT = new EventCallback();


#endif // __CCENGINE_H__
