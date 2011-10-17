/*----------------------------------------------------------
* Copyright © 2011 - 2011 France Telecom
* This software is distributed under the Apache 2.0 license,
* see the "license.txt" file for more details.
*-----------------------------------------------------------
* File Name   : main.cpp
* Description : JNI entry and interface point.
*
* Created     : 15/05/11
* Author(s)   : Ashraf Samy Hegab, Chris Bowers
*-----------------------------------------------------------
*/

#include "CCDefines.h"
#include "CCAppEngine.h"
#include "CCGLView.h"
#include "CCDeviceControls.h"
#include "CCDeviceURLManager.h"

#include "CCWindowController.h"
static CCWindowController *windowController = NULL;

#include <jni.h>


static void printGLString(const char *name, GLenum s)
{
    const char *v = (const char *)glGetString( s );
    DEBUGLOG( "GL %s = %s\n", name, v );
}


extern "C"
{
    JNIEXPORT void JNICALL Java_com_android2c_CCJNI_onSurfaceChanged(JNIEnv *jEnv, jobject jObj,  jint jWidth, jint jHeight);
    JNIEXPORT void JNICALL Java_com_android2c_CCJNI_onDrawFrame(JNIEnv *jEnv, jobject jObj);
};

JNIEXPORT void JNICALL Java_com_android2c_CCJNI_onSurfaceChanged(JNIEnv *jEnv, jobject jObj, jint jWidth, jint jHeight)
{	
#ifdef DEBUGON
	// Now we can do what we intended
	printGLString( "Version", GL_VERSION );
	printGLString( "Vendor", GL_VENDOR );
	printGLString( "Renderer", GL_RENDERER );
	printGLString( "Extensions", GL_EXTENSIONS );
#endif
	
	if( windowController != NULL )
	{
		windowController->shutdown();
		delete windowController;
	}
	windowController = new CCWindowController();

	// Get the window controller to create the glView
	windowController->startup();

	// Then feed it in it's appropriate settings here
	gView->jniEnv = jEnv;
	gView->jniObj = jObj;
	gView->resizeView( jWidth, jHeight );

	if( gEngine != NULL )
	{
		delete gEngine;
	}

	gEngine = new CCAppEngine();
	gEngine->setupNativeThread();
	gEngine->setupGameThread();

	gView->runningGame = true;
	gView->gameThreadRunning = true;

#if defined PROFILEON
    CCProfiler::open();
#endif
}


JNIEXPORT void JNICALL Java_com_android2c_CCJNI_onDrawFrame(JNIEnv *jEnv, jobject jObj)
{
	if( gView->runningGame )
	{
		gView->jniEnv = jEnv;
		gView->jniObj = jObj;

		gEngine->updateNativeThread();
		gEngine->updateGameThread();
	}

#if defined PROFILEON
    CCProfiler::save();
#endif
}
