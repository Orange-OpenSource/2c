/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCControls.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCControls.h"
#include "CCDeviceControls.h"
#include "CCViewManager.h"


const CCPoint CCScreenTouches::averageLastDeltas() const
{
    CCPoint averageDeltas;
    int numberOfDeltas = 0;
    const float lifetime = gEngine->time.lifetime;
    for( int i=0; i<max_last_deltas; ++i )
    {
        const TimedDelta &lastDelta = lastDeltas[i];
        const float difference = lifetime - lastDelta.time;
        if( difference < 0.25f )
        {
            averageDeltas.x += lastDelta.delta.x;
            averageDeltas.y += lastDelta.delta.y;
            numberOfDeltas++;
        }
        else
        {
            break;
        }
    }
    if( numberOfDeltas > 1 )
    {
        averageDeltas.x /= numberOfDeltas;
        averageDeltas.y /= numberOfDeltas;
    }
    return averageDeltas;
}



CCPoint CCControls::touchMovementThreashold;


CCControls::CCControls()
{
    inUse = false;
    
    RefreshTouchMovementThreashold();
}


void CCControls::render()
{
}


void CCControls::update(const CCTime &time)
{
	for( uint i=0; i<numberOfTouches; ++i )
	{
        UpdateTouch( screenTouches[i], time );
	}
}


void CCControls::UpdateTouch(CCScreenTouches &touch, const CCTime &time)
{
    if( touch.usingTouch != NULL )
    {
        touch.timeHeld += time.real;
    }
    
    if( touch.lastTouch != NULL )
    {
        if( touch.usingTouch != NULL )
        {
            touch.usingTouch = NULL;
        }
        else
        {
            touch.lastTouch = NULL;
        }
    }
    else
    {
        touch.lastTimeReleased += time.real;
    }
    
    for( uint j=1; j<CCScreenTouches::max_last_deltas; ++j )
    {
        touch.lastDeltas[j] = touch.lastDeltas[j-1];
    }
    touch.lastDeltas[0].time = time.lifetime;
    touch.lastDeltas[0].delta = touch.delta;
    
    touch.delta.x = 0.0f;
    touch.delta.y = 0.0f;
}


void CCControls::unTouch(void *touch)
{
    UITouch *uiTouch = (UITouch*)touch;
	for( uint i=0; i<numberOfTouches; ++i )
	{
        CCScreenTouches &screenTouch = screenTouches[i];
		if( screenTouch.usingTouch == uiTouch )
        {
            CCNativeThreadLock();
            screenTouch.lastTouch = uiTouch;
			
			screenTouch.lastTimeReleased = 0.0f;
			screenTouch.lastTotalDelta = screenTouch.totalDelta;
            CCNativeThreadUnlock();
		}
	}
}


const bool CCControls::DetectZoomGesture(const CCScreenTouches &touch1, const CCScreenTouches &touch2)
{
    if( touch1.delta.x != 0.0f || touch1.delta.y != 0.0f ||
        touch2.delta.x != 0.0f || touch2.delta.y != 0.0f )
    {
        const CCScreenTouches *topTouch = &touch1,
        *bottomTouch = &touch2;
        if( touch1.position.y < touch2.position.y )
        {
            topTouch = &touch2;
            bottomTouch = &touch1;
        }
        const CCScreenTouches *rightTouch = &touch1,
        *leftTouch = &touch2;
        if( touch1.position.x < touch2.position.x )
        {
            rightTouch = &touch2;
            leftTouch = &touch1;
        }
        
        if( leftTouch->totalDelta.x < 0.0f && rightTouch->totalDelta.x > 0.0f )
        {
            const float total = -leftTouch->totalDelta.x + rightTouch->totalDelta.x;
            if( total > touchMovementThreashold.x * 2.0f )
            {
                return true;
            }
        }
        else if( leftTouch->totalDelta.x > 0.0f && rightTouch->totalDelta.x < 0.0f )
        {
            const float total = leftTouch->totalDelta.x + -rightTouch->totalDelta.x;
            if( total > touchMovementThreashold.x * 2.0f )
            {
                return true;
            }
        }
        
        if( bottomTouch->totalDelta.y < 0.0f && topTouch->totalDelta.y > 0.0f )
        {
            const float total = -bottomTouch->totalDelta.x + topTouch->totalDelta.x;
            if( total > touchMovementThreashold.y * 2.0f )
            {
                return true;
            }
        }
        else if( bottomTouch->totalDelta.y > 0.0f && topTouch->totalDelta.y < 0.0f )
        {
            const float total = bottomTouch->totalDelta.y + -topTouch->totalDelta.y;
            if( total > touchMovementThreashold.y * 2.0f )
            {
                return true;
            }
        }
    }
	
    return false;
}


const bool CCControls::DetectRotateGesture(const CCScreenTouches &touch1, const CCScreenTouches &touch2)
{
    if( touch1.delta.x != 0.0f || touch1.delta.y != 0.0f ||
        touch2.delta.x != 0.0f || touch2.delta.y != 0.0f )
    {
        const float absX1 = fabsf( touch1.totalDelta.x );
        const float absX2 = fabsf( touch2.totalDelta.x );
        const float absY1 = fabsf( touch1.totalDelta.y );
        const float absY2 = fabsf( touch2.totalDelta.y );
        if( absX1 > absY1 && absX2 > absY2 )
        {
            if( touch1.totalDelta.x > touchMovementThreashold.x && touch2.totalDelta.x > touchMovementThreashold.x )
            {
                return true;
            }

            if( touch1.totalDelta.x < -touchMovementThreashold.x && touch2.totalDelta.x < -touchMovementThreashold.x )
            {
                return true;
            }
        }
        else if( absY1 > absX1 && absY2 > absX2 )
        {
            if( touch1.totalDelta.y > touchMovementThreashold.y && touch2.totalDelta.y > touchMovementThreashold.y )
            {
                return true;
            }
            
            if( touch1.totalDelta.y < -touchMovementThreashold.y && touch2.totalDelta.y < -touchMovementThreashold.y )
            {
                return true;
            }
        }
    }
    
    return false;
}


const bool CCControls::TouchActionMoving(const CCTouchAction touchAction)
{
    if( touchAction >= touch_movingHorizontal && touchAction <= touch_moving )
    {
        return true;
    }
    return false;
}


void CCControls::RefreshTouchMovementThreashold()
{
    const float pixels = 3.0f;
    if( CCViewManager::IsPortrait() )
    {
        touchMovementThreashold.x = pixels * gEngine->renderer->getInverseScreenSize().width;
        touchMovementThreashold.y = pixels * gEngine->renderer->getInverseScreenSize().height;
    }
    else
    {
        touchMovementThreashold.x = pixels * gEngine->renderer->getInverseScreenSize().height;
        touchMovementThreashold.y = pixels * gEngine->renderer->getInverseScreenSize().width;
    }
}