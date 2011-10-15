/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCSceneMoveable.h
 * Description : A scene managed moveable object.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class CCSceneMoveable : public CCSceneCollideable
{
public:
	typedef CCSceneCollideable super;

	CCSceneMoveable();
	
	// CCSceneObject
    virtual void update(const CCGameTime &gameTime);
	
	// CCSceneCollideable
	virtual CCSceneCollideable* requestCollisionReport(CCSceneCollideable *collidedWith);
	virtual const bool isMoveable() { return true; }
	
	virtual void updateMovement(const float delta);
	virtual const float applyMovementDirection(const float delta);
	
	virtual void applyVelocity(const float delta, const float directionMagnitude);
	float getCollisionPosition(float *thisObjectPosition, float *thisObjectBounds, float *collidedObjectPosition, float *collidedObjectBounds);
	CCSceneCollideable* applyHorizontalVelocity(const float velocityX, const float velocityZ);
	CCSceneCollideable* applyVerticalVelocity(const float increment);
	
	virtual void reportVerticalCollision(const CCSceneCollideable *collidedWith);

	static void setGravityForce(const float force) { gravityForce = force; }

	inline void setVelocity(const float x, const float y, const float z) { movementVelocity.x = x; movementVelocity.y = y; movementVelocity.z = z; }
	inline void setAdditionalVelocity(const float x, const float y, const float z) { additionalVelocity.x = x; additionalVelocity.y = y; additionalVelocity.z = z; }
	inline void incrementAdditionalVelocity(const float x, const float y, const float z) { additionalVelocity.x += x; additionalVelocity.y += y; additionalVelocity.z += z; }

	bool moveable;
	float movementSpeed;
	CCVector3 movementDirection;

protected:
	CCVector3 velocity;
	CCVector3 movementVelocity;
	CCVector3 additionalVelocity;
	float decelerationSpeed;
	
	bool gravity;
	static float gravityForce;
};
