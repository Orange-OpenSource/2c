/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCCameraFirstPerson.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCCameraFirstPerson.h"


CCCameraFirstPerson::CCCameraFirstPerson()
{
}


void CCCameraFirstPerson::update()
{
	CCMatrixLoadIdentity( modelViewMatrix );
    
    GluLookAt( modelViewMatrix,
               rotatedPosition.x, rotatedPosition.y, rotatedPosition.z,	// Position
               lookAt.x, lookAt.y, lookAt.z,                            // Looking At
               0.0f, 1.0f, 0.0f );										// Up
    
    GLSetPushMatrix( modelViewMatrix );
	
    ExtractFrustum();
	
    updateVisibleObjects();
}


const bool CCCameraFirstPerson::interpolateLookAt(const CCVector3 &lookAtTarget, const CCVector3 &offsetTarget, const float delta, float speed)
{
	if( updating )
	{   
        updating = false;
        
        // We don't actually use the FOV to figure out the screen view,
        // so no need to trigger this scan
        //updateFOV = true;
        
        lookAt = lookAtTarget;
        
        if( currentOffsetTarget.target != offsetTarget )
        {
            currentOffsetTarget.target = offsetTarget;
            offsetInterpolator.setup( &currentOffsetTarget.current, currentOffsetTarget.target );
        }
        
        if( offsetInterpolator.update( delta * 3.0f ) )
        {
            updating = true;
        }
        
        updateOffset( offsetTarget );
        
		return true;
	}
	
	return false;
}