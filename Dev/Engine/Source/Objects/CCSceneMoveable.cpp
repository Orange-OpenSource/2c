/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCSceneMoveable.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCSceneBase.h"
#include "CCOctree.h"


float CCSceneMoveable::gravityForce = 200.0f;


CCSceneMoveable::CCSceneMoveable()
{	
	moveable = true;
	movementSpeed = 75.0f;
	decelerationSpeed = movementSpeed * 2.0f;
	
	gravity = true;
}


// CCSceneObject
void CCSceneMoveable::update(const CCTime &gameTime)
{
	super::update( gameTime );
	
    updateMovement( gameTime.delta );
}


// CCSceneCollideable
CCSceneCollideable* CCSceneMoveable::requestCollisionReport(CCSceneCollideable *collidedWith)
{
	return collidedWith->reportCollision( this, velocity.x, velocity.y, velocity.z );
}


void CCSceneMoveable::updateMovement(const float delta)
{	
    if( moveable )
    {
        const float movementMagnitude = applyMovementDirection( delta );
        velocity = movementVelocity;

        const float additionalMagnitude = CCVector3LengthSquared( additionalVelocity );
        if( additionalMagnitude > 0.0f )
        {
            velocity.add( additionalVelocity );
            const float deceleration = decelerationSpeed * delta;
            CCToTarget( additionalVelocity.x, 0.0f, deceleration );
            CCToTarget( additionalVelocity.y, 0.0f, deceleration );
            CCToTarget( additionalVelocity.z, 0.0f, deceleration );
        }

        const float velocityMagnitude = CCVector3LengthSquared( velocity );
        if( velocityMagnitude > 0.0f )
        {
            applyVelocity( delta, movementMagnitude );

            dirtyModelMatrix();
            updateCollisions = true;
            CCOctreeRefreshObject( this );
        }
    }
}


const float CCSceneMoveable::applyMovementDirection(const float delta)
{
	const float movementMagnitude = CCLengthSquared( movementDirection.x, movementDirection.z );
	if( movementMagnitude > 0.0f )
	{
		// Z movement
		const float forwardSpeed = movementSpeed;
		const float zMovementSpeed = movementDirection.z * forwardSpeed;
		float rotationInRadians = CC_DEGREES_TO_RADIANS( rotation.y );
		float xAmount = sinf( rotationInRadians ) * zMovementSpeed;
		float zAmount = cosf( rotationInRadians ) * zMovementSpeed;
		
		// X Movement
		if( movementDirection.x != 0.0f )
		{
			const float xMovementSpeed = movementDirection.x * movementSpeed;
			rotationInRadians = CC_DEGREES_TO_RADIANS( rotation.y + 90.0f );
			xAmount += sinf( rotationInRadians ) * xMovementSpeed;
			zAmount += cosf( rotationInRadians ) * xMovementSpeed;
		}
		
		movementVelocity.x = xAmount;			
		movementVelocity.z = zAmount;
	}
	
	return movementMagnitude;
}


void CCSceneMoveable::applyVelocity(const float delta, const float movementMagnitude)
{
	const float velocityX = velocity.x * delta;
	const float velocityZ = velocity.z * delta;
	if( velocityX != 0.0f || velocityZ != 0.0f )
	{
		const CCSceneCollideable *collidedWith = NULL;
		
		const float velocityVsBoundingX = velocityX * inverseBoundsLength.x;
		const float velocityVsBoundingZ = velocityZ * inverseBoundsLength.x;
		const float absVelocityVsBoundingX = fabsf( velocityVsBoundingX );
		const float absVelocityVsBoundingZ = fabsf( velocityVsBoundingZ );
		if( absVelocityVsBoundingX > 1.0f || absVelocityVsBoundingZ > 1.0f )
		{
			const float furthestIncrement = absVelocityVsBoundingX > absVelocityVsBoundingZ ? absVelocityVsBoundingX : absVelocityVsBoundingZ;
			const uint numberOfIncrements = roundf( furthestIncrement + 0.5f );
			
			const float inverseNumberOfIncrements = 1.0f / numberOfIncrements;
			const float incrementsX = velocityX * inverseNumberOfIncrements;
			const float incrementsZ = velocityZ * inverseNumberOfIncrements;
			uint i = 0;
			do
			{
				collidedWith = applyHorizontalVelocity( incrementsX, incrementsZ );
				i++;
			} while( i < numberOfIncrements && collidedWith == NULL );
		}
		else
		{
			collidedWith = applyHorizontalVelocity( velocityX, velocityZ );
		}
		
		// Deceleration
		if( movementMagnitude == 0.0f )
		{
			const float movementDeceleration = decelerationSpeed * delta;
            CCToTarget( movementVelocity.x, 0.0f, movementDeceleration );
            CCToTarget( movementVelocity.z, 0.0f, movementDeceleration );
		}
	}
		
	// Gravity
	if( gravity )
	{
		const CCSceneCollideable *collidedWith = NULL;
		movementVelocity.y -= gravityForce * delta;
		const float velocityY = velocity.y * delta;
		
		if( velocityY > 0.0f )
		{
			const float velocityVsBoundingY = velocityY * inverseBoundsLength.y;
			const float absVelocityVsBoundingY = fabsf( velocityVsBoundingY );
			if( absVelocityVsBoundingY > 1.0f )
			{
				uint numberOfIncrements = roundf( absVelocityVsBoundingY + 0.5f );
				const float velocityIncrements = velocityY / numberOfIncrements;
				uint i = 0;
				do
				{
					collidedWith = applyVerticalVelocity( velocityIncrements );
					i++;
				} while( i < numberOfIncrements && collidedWith == NULL );
			}
			else
			{
				collidedWith = applyVerticalVelocity( velocityY );
			}
		}
		else
		{
			const float velocityVsBoundingY = velocityY * inverseBoundsLength.y;
			const float absVelocityVsBoundingY = fabsf( velocityVsBoundingY );
			if( absVelocityVsBoundingY > 1.0f )
			{
				uint numberOfIncrements = roundf( absVelocityVsBoundingY + 0.5f );
				const float velocityIncrements = velocityY / numberOfIncrements;
				uint i = 0;
				do
				{
					collidedWith = applyVerticalVelocity( velocityIncrements );
					i++;
				} while( i < numberOfIncrements && collidedWith == NULL );
			}
			else
			{
				collidedWith = applyVerticalVelocity( velocityY );
			}
		}
		
		reportVerticalCollision( collidedWith );
		
		// Ensure we have a velocity so we're checked for movement next frame
		if( collidedWith == NULL && movementVelocity.y == 0.0f )
		{
			movementVelocity.y = CC_SMALLFLOAT;
		}
	}
}


float CCSceneMoveable::getCollisionPosition(float *thisObjectPosition, float *thisObjectBounds, float *collidedObjectPosition, float *collidedObjectBounds)
{
	float collisionPosition = *collidedObjectPosition;
	if( collisionPosition < *thisObjectPosition )
	{
		collisionPosition += *collidedObjectBounds;
		collisionPosition += *thisObjectBounds;
		collisionPosition += CC_SMALLFLOAT;
		
		if( collisionPosition > *thisObjectPosition )
		{
			collisionPosition = *thisObjectPosition;
		}
	}
	else
	{
		collisionPosition -= *collidedObjectBounds;
		collisionPosition -= *thisObjectBounds;
		collisionPosition -= CC_SMALLFLOAT;
		
		if( collisionPosition < *thisObjectPosition )
		{
			collisionPosition = *thisObjectPosition;
		}
	}
	
	return collisionPosition;
}


CCSceneCollideable* CCSceneMoveable::applyHorizontalVelocity(const float velocityX, const float velocityZ)
{
	position.x += velocityX;
	position.z += velocityZ;
	
	CCSceneCollideable *collidedWith = CCBasicOctreeCollisionCheck( this, &position, true );
	if( collidedWith != NULL )
	{
		position.x -= velocityX;
		
		CCSceneCollideable *collidedWithZ = collidedWith;
		if( velocityZ != 0.0f )
		{
			collidedWithZ = CCBasicOctreeCollisionCheck( this, &position );
		}
		
		if( collidedWithZ != NULL )
		{
			position.z -= velocityZ;
			position.x += velocityX;
			
			CCSceneCollideable *collidedWithX = collidedWith;
			if( velocityX != 0.0f )
			{
				collidedWithX = CCBasicOctreeCollisionCheck( this, &position );
			}
			
			if( collidedWithX != NULL )
			{
				position.x -= velocityX;
				
				// Moving failed
				
				// Kill the additional velocity
				additionalVelocity.x = 0.0f;
				additionalVelocity.z = 0.0f;
			}
			
			// Moving X passed
			else
			{
				float collisionZ = getCollisionPosition( &position.z, &collisionBounds.z, &collidedWith->positionPtr->z, &collidedWith->collisionBounds.z );
				position.z = collisionZ;
				
				// Kill the z additional velocity
				additionalVelocity.z = 0.0f;
			}
		}	
		// Moving Z passed
		else
		{
			float collisionX = getCollisionPosition( &position.x, &collisionBounds.x, &collidedWith->positionPtr->x, &collidedWith->collisionBounds.x );
			position.x = collisionX;
			
			// Kill the x additional velocity
			additionalVelocity.x = 0.0f;
		}
	}
	
	return collidedWith;
}


CCSceneCollideable* CCSceneMoveable::applyVerticalVelocity(const float increment)
{
	position.y += increment;

	CCSceneCollideable *collidedWith = CCBasicOctreeCollisionCheck( this, &position, true );
	if( collidedWith != NULL )
	{
		position.y -= increment;
		const float originalPosition = position.y;
		
		// Since basic collision check doesn't return the actual closest collision, keep re-running the check until we're collision free
		CCSceneCollideable *currentlyCollidingWith = collidedWith;
		while( currentlyCollidingWith != NULL )
		{
			position.y = originalPosition;
			collidedWith = currentlyCollidingWith;
			position.y = getCollisionPosition( &position.y, &collisionBounds.y, &collidedWith->positionPtr->y, &collidedWith->collisionBounds.y );
			if( position.y == originalPosition ) 
			{
				currentlyCollidingWith = NULL;
			}
			else
			{
				currentlyCollidingWith = CCBasicOctreeCollisionCheck( this, &position );
			}
		}
	}
	
	return collidedWith;
}


void CCSceneMoveable::reportVerticalCollision(const CCSceneCollideable *collidedWith)
{
	if( collidedWith )
	{
		movementVelocity.y = 0.0f;
	}
}
