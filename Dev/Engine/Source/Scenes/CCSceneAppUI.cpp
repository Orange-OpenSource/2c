/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCSceneAppUI.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"


CCSceneAppUI::CCSceneAppUI()
{
    camera = NULL;
    cameraScrolling = false;
    // s = ( ( u+v ) / 2 ) * t;
    // u+v = ( s / t ) * 2
    //float distance = ( velocity * time ) + ( ( -velocity * pow( time, 2 ) ) / 2 );
    //    if( cameraVelocity != 0.0f )
    //    {
    //        cameraLookAt.y += cameraVelocity * gameTime.delta;
    //        ToTarget( &cameraVelocity, 0.0f, cameraReleaseVelocity * gameTime.delta * 2.0f );
    //        camera->flagUpdate();
    //        
    //        if( cameraScrolling && fabsf( cameraVelocity ) < 10.0f )
    //        {
    //            cameraScrolling = false;
    //            lockCameraView();
    //        }
    //    }

    controlsUITouch = NULL;
    controlsMoving = false;

    orientationCallback = NULL;
    updatingOrientation = false;

    scrollBar = NULL;
    cameraWidth = 100.0f;
}


void CCSceneAppUI::setup()
{
    if( camera == NULL )
    {
        camera = new CCCameraAppUI();
        gEngine->addCamera( camera );
        camera->setupViewport( 0.0f, 0.0f, 1.0f, 1.0f );
    }

    setCameraWidth( cameraWidth );
    refreshCameraView();
    lockCameraView();
}


void CCSceneAppUI::destruct()
{
    orientationCallback.deleteObjectsAndList();

    if( camera != NULL )
    {
        gEngine->removeCamera( camera );
        delete camera;
    }
    
    super::destruct();
}


// CCSceneBase
const bool CCSceneAppUI::handleControls(const CCGameTime &gameTime)
{	
    if( updatingOrientation )
    {
        return false;
    }

    bool usingControls = false;
    CCScreenTouches *touches = (CCScreenTouches*)&camera->cameraTouches;

    // Handles two touches pressed
	if( touches[0].usingTouch != NULL && touches[1].usingTouch != NULL )
	{
        if( controlsUITouch == touches[0].usingTouch )
        {
            handleTilesTouch( touches[0], touch_lost );
        }
        
        if( controlsUITouch != touches[1].usingTouch )
        {
            controlsUITouch = touches[1].usingTouch;
        }
        
        usingControls = touchCameraRotating( touches[0].delta.x, touches[0].delta.y );
	}
	else
	{
		const CCScreenTouches &touch = touches[0];
		
		// Handles one touch pressed
		if( touch.usingTouch != NULL && 
             touches[1].lastTimeReleased > 0.0f &&
             touch.position.x > 0.0f && touch.position.x < 1.0f &&
             touch.position.y > 0.0f && touch.position.y < 1.0f )
		{
			if( controlsUITouch != touch.usingTouch )
			{
                controlsUITouch = touch.usingTouch;
                touchPressed( touch );
			}
			
            CCPoint touchDelta = touch.delta;
            if( controlsMoving == false )
            {
                if( touchMovementAllowed( touch, touchDelta ) )
                {
                }
                else
                {   
                    usingControls = handleTilesTouch( touch, touch_pressed );
                }
            }
            
            if( controlsMoving )
            {   
                usingControls = touchMoving( touch, touchDelta );
            }
		}
		
		// On touch release
		else 
        {
            if( controlsUITouch != NULL )
            {
                if( controlsUITouch == touch.lastTouch )
                {	
                    usingControls = touchReleased( touch );
                }
                else
                {
                    handleTilesTouch( touch, touch_lost );
                }
                
                lockCameraView();
                
                // Reset state
                if( controlsMoving )
                {
                    controlsMoving = false;
                }
				controlsUITouch = NULL;
                
                return usingControls;
			}
		}
	}

    if( usingControls == false )
    {
        return super::handleControls( gameTime );
    }
    
    return usingControls;
}


void CCSceneAppUI::updateScene(const CCGameTime &gameTime)
{
    super::updateScene( gameTime );
}


void CCSceneAppUI::updateCamera(const CCGameTime &gameTime)
{
    const float lookAtSpeed = controlsUITouch && cameraScrolling == false ? 20.0f : 1.5f;
	if( camera->interpolateLookAt( cameraLookAt, cameraOffset, gameTime.delta, lookAtSpeed ) )
    {
        // Tell the scroll bar where to go
        if( scrollBar != NULL )
        {
            scrollBar->reposition( camera->getLookAt().y, cameraWidth, cameraHeight );
        }
	}
    else
    {
        if( cameraScrolling )
        {
            cameraScrolling = false;
            lockCameraView();
        }
        
        if( updatingOrientation )
        {
            updatingOrientation = false;
            refreshCameraView();
            lockCameraView();
        }
    }
}


const bool CCSceneAppUI::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( camera == inCamera )
    {
        renderObjects( pass, alpha );
        return true;
    }
    return false;
}


void CCSceneAppUI::renderOctreeObject(CCSceneObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( camera == inCamera )
	{
        object->render( alpha );
	}
}


void CCSceneAppUI::render2DBackground(const uint inCameraIndex)
{
    if( inCameraIndex == camera->getIndex() )
    {
        for( int i=0; i<childScenes.length; ++i )
        {
            childScenes.list[i]->render2DBackground( inCameraIndex );
        }
    }
}


void CCSceneAppUI::render2DForeground(const uint inCameraIndex)
{
    if( inCameraIndex == camera->getIndex() )
    {
        for( int i=0; i<childScenes.length; ++i )
        {
            childScenes.list[i]->render2DForeground( inCameraIndex );
        }
    }
}


void CCSceneAppUI::beginOrientationUpdate()
{
    setCameraWidth( cameraWidth );

    for( int i=0; i<orientationCallback.length; ++i )
    {
        orientationCallback.list[i]->run();
    }
    
    updatingOrientation = true;
}


void CCSceneAppUI::finishOrientationUpdate()
{
    updatingOrientation = true;
}


void CCSceneAppUI::setCameraWidth(const float inWidth)
{
    cameraWidth = inWidth;
    cameraOffset.z = cameraWidth;
    if( gEngine->renderer->isPortrait() )
    {
        cameraOffset.z /= camera->getFrustumSize().width;
        cameraHeight = cameraOffset.z * camera->getFrustumSize().height;
    }
    else
    {
        cameraOffset.z /= camera->getFrustumSize().height;
        cameraHeight = cameraOffset.z * camera->getFrustumSize().width;
    }

    cameraHWidth = cameraWidth * 0.5f;
    cameraHHeight = cameraHeight * 0.5f;
}


void CCSceneAppUI::setCameraHeight(const float inHeight)
{
    cameraHeight = inHeight;
    cameraOffset.z = cameraHeight;
    if( gEngine->renderer->isPortrait() )
    {
        cameraOffset.z /= camera->getFrustumSize().height;
    }
    else
    {
        cameraOffset.z /= camera->getFrustumSize().width;
    }

    cameraWidth = cameraOffset.z * camera->getFrustumSize().height;
    cameraHWidth = cameraWidth * 0.5f;
    cameraHHeight = cameraHeight * 0.5f;
}


void CCSceneAppUI::touchPressed(const CCScreenTouches &touch)
{

}


const bool CCSceneAppUI::touchMovementAllowed(const CCScreenTouches &touch, CCPoint &touchDelta)
{
    const float absDeltaX = fabsf( touch.totalDelta.x );
    const float absDeltaY = fabsf( touch.totalDelta.y );
    if( absDeltaX > CC_TOUCH_TO_MOVEMENT_THRESHOLD || absDeltaY > CC_TOUCH_TO_MOVEMENT_THRESHOLD )
    {
        controlsMoving = true;
        touchDelta.x += touch.totalDelta.x;
        touchDelta.y += touch.totalDelta.y;
        
        controlsMovingVertical = absDeltaY > absDeltaX;
        return true;
    }
    return false;
}


const bool CCSceneAppUI::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    // Run through all the tiles
    if( handleTilesTouch( touch, controlsMovingVertical ? touch_movingVertical : touch_movingHorizontal ) )
    {
        return true;
    }

    return touchCameraMoving( touch, touchDelta.x, touchDelta.y );
}


const bool CCSceneAppUI::touchReleased(const CCScreenTouches &touch)
{
    // Find pressed tile
    bool usingControls = handleTilesTouch( touch, touch_released );
    if( usingControls == false )
    {
        usingControls = touchReleaseSwipe( touch );
    }
    return usingControls;
}


const bool CCSceneAppUI::handleTilesTouch(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    if( camera->project3D( touch.position.x, touch.position.y ) )
    {
        // Scan to see if we're blocked by a collision
        static CCVector3 hitPosition;
        CCSceneCollideable *hitObject = CCBasicLineCollisionCheck( (CCSceneCollideable**)tiles.list,
                                                                   tiles.length,
                                                                   NULL,
                                                                   camera->projection.vNear, camera->projection.vFar,
                                                                   &hitPosition,
                                                                   true,
                                                                   collision_ui,
                                                                   false );

        for( int i=0; i<tiles.length; ++i )
        {
            CCTile3D *tile = tiles.list[i];
            if( tile->handleProjectedTouch( camera->projection, hitObject, hitPosition, touch, touchAction ) == 2 )
            {
                return true;
            }
        }
    }
    return false;
}


const bool CCSceneAppUI::touchCameraMoving(const CCScreenTouches &touch, const float x, const float y)
{
    if( controlsMovingVertical )
    {
        float delta = y * cameraHeight;
        if( cameraLookAt.y > sceneTopY || cameraLookAt.y < sceneBottomY )
        {
            delta *= 0.5f;
        }
        cameraLookAt.y += delta;
    }
    else
    {
        float delta = x * cameraWidth;
        if( cameraLookAt.x != 0.0f )
        {
            delta *= 0.5f;
        }
        cameraLookAt.x -= delta;
    }

    camera->flagUpdate();
    return true;
}


const bool CCSceneAppUI::touchCameraRotating(const float x, const float y)
{
    CCVector3 rotation = camera->getRotationVector();
    
    rotation.y += -x * 180.0f;
    CCClampFloat( rotation.y, -45.0f, 45.0f );
    camera->setRotationY( rotation.y );
    
    rotation.x += -y * 180.0f;
    CCClampFloat( rotation.x, -45.0f, 45.0f );
    camera->setRotationX( rotation.x );
    
    camera->flagUpdate();
    return true;
}


const bool CCSceneAppUI::touchReleaseSwipe(const CCScreenTouches &touch)
{
    const float maxTimeHeld = 0.5f;
    if( touch.timeHeld < maxTimeHeld )
    {
        if( controlsMovingVertical )
        {
            cameraScrolling = true;
            const CCPoint averageLastDelta = touch.averageLastDeltas();
            const float displacement = averageLastDelta.y * cameraHeight * 5.0f;
            float cameraLookAtTarget = cameraLookAt.y + displacement;
            if( cameraLookAtTarget > sceneTopY )
            {
                const float distance = cameraLookAtTarget - sceneTopY;
                cameraLookAtTarget -= distance * 0.25f;
                cameraLookAtTarget = MIN( cameraLookAtTarget, sceneTopY + cameraHHeight * 0.5f );
            }
            else if( cameraLookAtTarget < sceneBottomY )
            {
                const float distance = cameraLookAtTarget - sceneBottomY;
                cameraLookAtTarget -= distance * 0.25f;
                cameraLookAtTarget = MAX( cameraLookAtTarget, sceneBottomY - cameraHHeight * 0.5f );
            }
            cameraLookAt.y = cameraLookAtTarget;
            camera->flagUpdate();
        }
        else
        {
            const float minMovementThreashold = 0.1f;
            if( touch.totalDelta.x < -minMovementThreashold )
            {
                cameraLookAt.x += cameraHWidth;
                camera->flagUpdate();
            }
            else if( touch.totalDelta.x > minMovementThreashold )
            {
                cameraLookAt.x -= cameraHWidth;
                camera->flagUpdate();
            }
        }
    }
    return true;
}


void CCSceneAppUI::scrollCameraToTop()
{
    cameraLookAt.y = sceneTopY;
    camera->flagUpdate();
}


void CCSceneAppUI::refreshCameraView()
{
    sceneTopY = 0.0f;
    sceneBottomY = 0.0f;
}


// Lock the camera on something interesting
void CCSceneAppUI::lockCameraView()
{
    if( cameraScrolling )
    {
        return;
    }
    
    camera->flagUpdate();
    cameraLookAt.x = 0.0f;
    cameraLookAt.y = 0.0f;
}
