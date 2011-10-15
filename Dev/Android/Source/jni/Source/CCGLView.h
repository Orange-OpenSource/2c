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

    	width = height = 0.0f;
    	runningGame = paused = gameThreadRunning = false;
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

protected:
    float width, height;
};


#endif // __CCGLVIEW_H__
