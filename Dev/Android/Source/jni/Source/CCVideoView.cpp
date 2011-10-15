/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCVideoView.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCVideoView.h"

#include "CCGLView.h"


void CCVideoView::startVideoView(const char *file)
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;
	jobject jObj = gView->jniObj;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "startVideoView", which takes one parameter of type string, and returns void
	static jmethodID mid = jEnv->GetMethodID( jniClass, "startVideoView", "(Ljava/lang/String;)V" );
	ASSERT( mid != 0 );

	// Call the function
	jstring javaURL = jEnv->NewStringUTF( file );
	jEnv->CallVoidMethod( jObj, mid, javaURL );
}


void CCVideoView::stopVideoView()
{
	// JNI Java call
	JNIEnv *env = gView->jniEnv;
	jobject obj = gView->jniObj;
	jclass jniClass = env->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "stopVideoView", which takes 0 parameters, and returns void
	static jmethodID mid = env->GetMethodID( jniClass, "stopVideoView", "()V" );
	ASSERT( mid != 0 );

	// Call the function
	env->CallVoidMethod( obj, mid );
}
