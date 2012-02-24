/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCGLView.h
 * Description : OpenGL view.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCGLVIEW_H__
#define __CCGLVIEW_H__


#import "CCDefines.h"

struct ThreadMutex
{
    ThreadMutex();
    ~ThreadMutex();
    
	pthread_mutex_t mutex;
    pthread_mutexattr_t type;
};

@class CCGLView;
extern CCGLView *gView;

@interface CCGLView : UIView 
{	
@public
    ThreadMutex *nativeThreadMutex;
    ThreadMutex *jobsThreadMutex;
	NSTimer *updateTimer;
}

-(void)shutdown;

-(void)setup;

-(void)updateNativeThread;

@end


#endif // __CCGLVIEW_H__
