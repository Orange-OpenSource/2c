/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCFBWebView.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCFBWebView.h"

#include "CCGLView.h"
#include "CCFBApi.h"


extern "C"
{
    JNIEXPORT void JNICALL Java_com_android2c_CCJNI_webViewURLLoaded(JNIEnv *jEnv, jobject jObj, jstring jUrl);
};

JNIEXPORT void JNICALL Java_com_android2c_CCJNI_webViewURLLoaded(JNIEnv *jEnv, jobject jObj, jstring jUrl)
{
	gView->jniEnv = jEnv;
	gView->jniObj = jObj;

	// Convert the strings
	jboolean isCopy;
	const char *cUrl = jEnv->GetStringUTFChars( jUrl, &isCopy );

	if( CCFBApi::ProcessLogin( cUrl ) )
	{
		CCFBWebView::closeWebBrowser();
	    CCFBApi::onBrowserClosed();
	}

	jEnv->ReleaseStringUTFChars( jUrl, cUrl );
}


void CCFBWebView::openWebBrowser()
{
	// JNI Java call
	JNIEnv *env = gView->jniEnv;
	jobject obj = gView->jniObj;
	jclass jniClass = env->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "urlRequest", which takes one parameter of type string, and returns void
	static jmethodID mid = env->GetMethodID( jniClass, "openWebBrowser", "(Ljava/lang/String;)V" );
	ASSERT( mid != 0 );

	// Call the function
	jstring javaURL = env->NewStringUTF( CCFBApi::GetAuthorizationURL().buffer );
	env->CallVoidMethod( obj, mid, javaURL );
}


void CCFBWebView::closeWebBrowser()
{
	// JNI Java call
	JNIEnv *env = gView->jniEnv;
	jobject obj = gView->jniObj;

	jclass jniClass = env->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "urlRequest", which takes one parameter of type string, and returns void
	static jmethodID mid = env->GetMethodID( jniClass, "closeWebBrowser", "()V" );
	ASSERT( mid != 0 );

	// Call the function
	env->CallVoidMethod( obj, mid );
}
