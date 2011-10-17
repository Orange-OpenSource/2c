/*----------------------------------------------------------
* Copyright © 2011 - 2011 France Telecom
* This software is distributed under the Apache 2.0 license,
* see the "license.txt" file for more details.
*-----------------------------------------------------------
* File Name   : GIGLView.h
* Description : OpenGL view handler.
*
* Created     : 09/06/11
* Author(s)   : Ashraf Samy Hegab, Chris Bowers
*-----------------------------------------------------------
*/

#ifndef __CCGLVIEW_H__
#define __CCGLVIEW_H__


#include <jni.h>

extern class CCGLView *gView;

class CCGLView
{
public:
    CCGLView()
    {
        gView = this;

    	jniEnv = NULL;
    	jniObj = NULL;

    	pthread_mutexattr_init( &gameThreadMutexType );
		pthread_mutexattr_settype( &gameThreadMutexType, PTHREAD_MUTEX_RECURSIVE );
		pthread_mutex_init( &gameThreadMutex, &gameThreadMutexType );

    	width = height = 0.0f;
    	runningGame = paused = gameThreadRunning = false;
    }

    ~CCGLView()
    {
        ASSERT( runningGame == false );
    	pthread_mutex_destroy( &gameThreadMutex );
        pthread_mutexattr_destroy( &gameThreadMutexType );
    }

    void resizeView(const float inWidth, const float inHeight)
	{
    	width = inWidth;
    	height = inHeight;
	}

    const float getWidth() { return width; }
    const float getHeight() { return height; }

public:
    bool runningGame, paused, gameThreadRunning;
    JNIEnv *jniEnv;
    jobject jniObj;

    pthread_mutex_t gameThreadMutex;
	pthread_mutexattr_t gameThreadMutexType;

protected:
    float width, height;
};


#endif // __CCGLVIEW_H__
