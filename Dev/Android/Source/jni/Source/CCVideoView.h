/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCVideoView.h
 * Description : JNI interface class to trigger video playback.
 *
 * Created     : 08/10/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCVIDEVIEW_H__
#define __CCVIDEVIEW_H__


class CCVideoView
{
public:
	static void startVideoView(const char *file);
	static void stopVideoView();
};


#endif // __CCVIDEVIEW_H__
