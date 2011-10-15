/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 Ð 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCDeviceURLManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCDeviceURLManager.h"

#include <CCGLView.h>


extern "C"
{
    JNIEXPORT void JNICALL Java_com_android2c_CCJNI_urlManagerDownloadFinished(JNIEnv *jEnv, jobject jObj,
    		jstring jUrl, jboolean jSuccess, jbyteArray jData, jint jDataLength,
    		jobjectArray jHeaderNames, jobjectArray jHeaderValues, jint jHeaderLength);
};

JNIEXPORT void JNICALL Java_com_android2c_CCJNI_urlManagerDownloadFinished(JNIEnv *jEnv, jobject jObj,
		jstring jUrl, jboolean jSuccess, jbyteArray jData, jint jDataLength,
		jobjectArray jHeaderNames, jobjectArray jHeaderValues, jint jHeaderLength)
{
	// Convert the strings
	jboolean isCopy;
	const char *cUrl = jEnv->GetStringUTFChars( jUrl, &isCopy );
	CCText textURL = cUrl;
	jEnv->ReleaseStringUTFChars( jUrl, cUrl );

	jbyte *jByteData = NULL;
	int cLength = 0;
	if( jData != NULL )
	{
		cLength = jEnv->GetArrayLength( jData );
		jByteData = jEnv->GetByteArrayElements( jData, &isCopy );
	}

	// Parse headers
	CCList<CCText> headerNames;
	CCList<CCText> headerValues;
	for( int i=0; i<jHeaderLength; ++i )
	{
		jstring jHeaderName = (jstring)jEnv->GetObjectArrayElement( jHeaderNames, i );
		const char *cHeaderName = jEnv->GetStringUTFChars( jHeaderName, &isCopy );
		CCText *headerName = new CCText( cHeaderName );
		headerNames.add( headerName );
		jEnv->ReleaseStringUTFChars( jHeaderName, cHeaderName );

		jstring jHeaderValue = (jstring)jEnv->GetObjectArrayElement( jHeaderValues, i );
		const char *cHeaderValue = jEnv->GetStringUTFChars( jHeaderValue, &isCopy );
		CCText *headerValue = new CCText( cHeaderValue );
		headerValues.add( headerValue );
		jEnv->ReleaseStringUTFChars( jHeaderValue, cHeaderValue );
	}

	// Call the relevant function in DeviceURLManager
	gEngine->urlManager->deviceURLManager->downloadFinished( textURL.buffer, jSuccess, (char*)jByteData, jDataLength, headerNames, headerValues );

	jEnv->ReleaseByteArrayElements( jData, jByteData, 0 );
}


CCDeviceURLManager::CCDeviceURLManager()
{
	currentURLRequest = NULL;
}


void CCDeviceURLManager::processRequest(CCURLRequest *inRequest)
{
	currentURLRequest = inRequest;

	// JNI Java call
	JNIEnv *env = gView->jniEnv;
	jobject obj = gView->jniObj;
	jclass jniClass = env->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	// Get the method ID of our method "urlRequest", which takes one parameter of type string, and returns void
	static jmethodID mid = env->GetMethodID( jniClass, "urlManagerProcessRequest", "(Ljava/lang/String;)V" );
	ASSERT_MESSAGE( mid != 0, "Could not get method ID for urlRequest." );

	if( mid == 0 )
	{
		// Can't find the method
		DEBUGLOG( "Can't find the \"urlRequest\" method in Java." );
	}
	else
	{
		// Call the function
		jstring javaURL = env->NewStringUTF( inRequest->url.buffer );
		env->CallVoidMethod( obj, mid, javaURL );
	}
}


void CCDeviceURLManager::downloadFinished(const char *url, const bool success,
		const char *data, const int dataLength,
		CCList<CCText> &headerNames, CCList<CCText> &headerValues)
{
	// Transfer over the headers
	for( int i=0; i<headerNames.length; ++i )
	{
		currentURLRequest->header.names.add( headerNames.list[i] );
		currentURLRequest->header.values.add( headerValues.list[i] );
	}

	if( !success )
	{
		DEBUGLOG( "Download of %s failed.", url );
		currentURLRequest->state = CCURLRequest::failed;
	}
	else
	{
		currentURLRequest->state = CCURLRequest::succeeded;
		currentURLRequest->data.set( data, dataLength );
	}
	currentURLRequest = NULL;
}

const bool CCDeviceURLManager::readyToRequest()
{
	return currentURLRequest == NULL;
}
