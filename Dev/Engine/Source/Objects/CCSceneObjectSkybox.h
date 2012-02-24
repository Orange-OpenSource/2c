/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCSceneObjectSkybox.h
 * Description : A square textured box around the camera.
 *
 * Created     : 02/05/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class CCSceneObjectSkybox : public CCSceneObject
{
public:
    typedef CCSceneObject super;

	CCSceneObjectSkybox();
    void setup(const float size, const char *name);
    
	void setup(const float size, 
               const char *top, 
               const char *bottom, 
               const char *east, 
               const char *west, 
               const char *north,
               const char *south);

	// ObjectBase
	virtual const bool update(const CCTime &time);
	virtual void render(const bool alpha);
	void animateScale(const float startScale);

protected:
    bool interpolating;
    CCInterpolatorV3<CCInterpolatorSinCurve> scaleInterpolator;
};