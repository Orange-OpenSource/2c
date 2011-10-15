/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCCameraFirstPerson.h
 * Description : First person view oriented camera.
 *
 * Created     : 22/08/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class CCCameraFirstPerson : public CCCameraBase
{
public:
    typedef CCCameraBase super;

	CCCameraFirstPerson();
    
    virtual void update();

    // Modify the offset target z to fit the fov
	virtual const bool interpolateLookAt(const CCVector3 &lookAtTarget, const CCVector3 &offsetTarget, const float delta, float speed);
    
protected:
    CCVector3Target currentOffsetTarget;
    CCInterpolatorCurveV3<CCInterpolatorCurve> offsetInterpolator;
};

