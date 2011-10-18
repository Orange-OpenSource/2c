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

    // Modify the offset target z to fit the fov
	virtual const bool interpolateLookAt(const CCVector3 &lookAtTarget, const CCVector3 &offsetTarget, const float delta, float speed);
    
    inline const CCInterpolatorCurveV3<CCInterpolatorX3Curve>& getLookAtInterpolator() { return lookAtInterpolator; }
    
    const CCVector3Target& getLookAtTarget() { return currentLookAtTarget; }
    
    void setOffset(const CCVector3 &offsetTarget);
    
protected:
    CCVector3Target currentLookAtTarget;
    CCVector3Target currentOffsetTarget;
    
    CCInterpolatorCurveV3<CCInterpolatorX3Curve> lookAtInterpolator;
    CCInterpolatorCurveV3<CCInterpolatorCurve> offsetInterpolator;
};

