/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCCameraAppUI.h
 * Description : AppUI Scene camera functionality.
 *
 * Created     : 27/04/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class CCCameraAppUI : public CCCameraBase
{
public:
    typedef CCCameraBase super;

	CCCameraAppUI();
    
    virtual void update();

	virtual const bool interpolateCamera(const float delta, const float speed);
    
    inline const CCInterpolatorV3<CCInterpolatorX3Curve>& getLookAtInterpolator() { return lookAtInterpolator; }
    
    const CCVector3Target& getLookAtTarget() { return currentLookAtTarget; }
    
    void setOffset(const CCVector3 &offsetTarget);
    
protected:
    CCVector3Target currentLookAtTarget;
    CCVector3Target currentOffsetTarget;
    
    CCInterpolatorV3<CCInterpolatorX3Curve> lookAtInterpolator;
    CCInterpolatorV3<CCInterpolatorSin2Curve> offsetInterpolator;
    
public:
    void setCameraWidth(const float inWidth);
    void setCameraHeight(const float inHeight);
    void refreshCameraSize();
    
    const float calcCameraOffset(const float inWidth);
    const float calcCameraWidth(const float inOffset);
    const float calcCameraHeight(const float inOffset);
    
public:
    CCVector3 targetOffset;
    CCVector3 targetLookAt;
    float targetWidth;
    float cameraWidth, cameraHeight, cameraHWidth, cameraHHeight;
};

