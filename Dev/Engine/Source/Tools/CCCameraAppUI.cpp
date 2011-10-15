/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCCameraAppUI.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"


CCCameraAppUI::CCCameraAppUI()
{
    offsetInterpolator.setDuration( 0.3f );
}


void CCCameraAppUI::update()
{
    super::update();
}


const bool CCCameraAppUI::interpolateLookAt(const CCVector3 &lookAtTarget, const CCVector3 &offsetTarget, const float delta, float speed)
{
	if( updating )
	{   
        updating = false;
        
        if( currentOffsetTarget.target != offsetTarget )
        {
            currentOffsetTarget.target = offsetTarget;
            offsetInterpolator.setup( &currentOffsetTarget.current, currentOffsetTarget.target );
        }
        
        if( currentLookAtTarget.target != lookAtTarget )
        {
            currentLookAtTarget.target = lookAtTarget;
            lookAtInterpolator.setup( &currentLookAtTarget.current, currentLookAtTarget.target );
        }
        
        if( offsetInterpolator.update( delta ) )
        {
            updating = true;
        }
        
        if( lookAtInterpolator.update( delta * speed ) )
        {
            updating = true;
            updatedPosition = true;
        }
        
        // Now update our camera
        {
            lookAt = currentLookAtTarget.current;
            updateOffset( currentOffsetTarget.current );
        }
        return true;
	}

	return false;
}


void CCCameraAppUI::setOffset(const CCVector3 &offsetTarget)
{
    currentOffsetTarget.current = currentOffsetTarget.target = offsetTarget;
    updateOffset( offsetTarget );
}
