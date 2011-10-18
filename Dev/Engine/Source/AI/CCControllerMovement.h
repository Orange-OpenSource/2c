/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCControllerMovement.h
 * Description : Handles the movement and rotation of an object.
 *
 * Created     : 27/06/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCCONTROLLERMOVEMENT_H__
#define __CCCONTROLLERMOVEMENT_H__


class CCControllerMovement : public CCUpdater
{
public:
    typedef CCUpdater super;

    CCControllerMovement(CCSceneCollideable *inObject);

    virtual const bool update(const float delta);

    const CCVector3& getTarget() { return positionTarget; }
    void setTarget(const CCVector3 &target);

protected:
    const float applyMovementDirection();
    void applyVelocity(const float delta, const float movementMagnitude);

protected:
    CCSceneCollideable *object;

    CCVector3 movementVelocity;
    CCVector3 movementRotation;

    float movementSpeed;

    float movementMaxSpeed;
    float movementMinSpeed;
    float movementAcceleration;
    float movementDeceleration;
    float movementRotationAngle;
    float rotationSpeed;

    bool findingTarget;
    CCVector3 positionTarget;
    float rotationDistance;
};


#endif // __CCCONTROLLERMOVEMENT_H__

