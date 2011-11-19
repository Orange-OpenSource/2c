/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCGameEngine.h
 * Description : Handles the engine loop and feature managers.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCGAMEENCCNE_H__
#define __CCGAMEENCCNE_H__


struct CCGameTime
{
	CCGameTime()
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

class CCGameEngine
{
public:
	CCGameEngine();
	~CCGameEngine();
	
public:
	virtual void setupNativeThread();
    virtual void setupGameThread();
	
    void addCamera(CCCameraBase *camera, const int index=-1);
	const bool removeCamera(CCCameraBase *camera);
    void refreshCameras();
	
	void addScene(CCSceneBase *scene, const int index=-1);
    void removeScene(CCSceneBase *scene);
	
	virtual const bool updateTime();
    
	virtual const bool updateNativeThread();
	void updateGameThread();
    
protected:
	virtual void startGame() = 0;
	virtual void updateGame();
	void render();
    
	// Finishes a job on the game thread
	virtual void finishJobs();
    
public:
    virtual void restartGame();
	
	void addCollideable(CCSceneCollideable* collideable);
	void removeCollideable(CCSceneCollideable* collideable);
	
    virtual const bool serialize(const bool saving=false);
    
    // Run on another thread
    void runOnNativeThread(CCLambdaCallback *lambdaCallback);
    void runOnGameThread(CCLambdaCallback *lambdaCallback);
	
public:
	CCRenderer *renderer;
	CCControls *controls;
	CCTextureManager *textureManager;
	CCURLManager *urlManager;
	
    CCList<CCCameraBase> cameras;
	CCCameraBase *currentCamera;
    CCDestructList<CCSceneBase> scenes;
	
    // Our Octree collideables container
	CCCollisionManager *collisionManager;

    // Engine level controls used for timers and such
    CCDestructList<CCUpdater> updaters;
	
	CCGameTime gameTime;
	uint renderFlags;
	float fpsLimit;
	
protected:
    CCList<CCLambdaCallback> nativeThreadCallbacks;
    CCList<CCLambdaCallback> gameThreadCallbacks;
};


#define LAMBDA_RUN_NATIVETHREAD(FUNCTION_CALL)                      \
LAMBDA_UNSAFE( ThreadCallback,                                      \
    FUNCTION_CALL                                                   \
)                                                                   \
gEngine->runOnNativeThread( new ThreadCallback() );


#define LAMBDA_RUN_GAMETHREAD(FUNCTION_CALL)                        \
LAMBDA_UNSAFE( ThreadCallback,                                      \
   FUNCTION_CALL                                                    \
)                                                                   \
gEngine->runOnGameThread( new ThreadCallback() );                   \

#define LAMBDA_CONNECT_NATIVETHREAD(EVENT, FUNCTION_CALL)           \
LAMBDA_UNSAFE( EventCallback,                                       \
    LAMBDA_UNSAFE( ThreadCallback,                                  \
        FUNCTION_CALL                                               \
    )                                                               \
    gEngine->runOnNativeThread( new ThreadCallback() );             \
)                                                                   \
EVENT = new EventCallback();


#define LAMBDA_CONNECT_GAMETHREAD(EVENT, FUNCTION_CALL)             \
LAMBDA_UNSAFE( EventCallback,                                       \
    LAMBDA_UNSAFE( ThreadCallback,                                  \
        FUNCTION_CALL                                               \
    )                                                               \
    gEngine->runOnGameThread( new ThreadCallback() );               \
)                                                                   \
EVENT = new EventCallback();


#endif // __CCGAMEENCCNE_H__
