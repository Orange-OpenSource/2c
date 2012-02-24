/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCTile3DButton.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCTextureFontPage.h"
#include "CCControllerMovement.h"


CCTile3DButton::CCTile3DButton(CCSceneBase *scene)
{
    construct( scene );
}


void CCTile3DButton::setupTile(const float width, const float height, const char *text)
{
    setTileSize( width, height );
    
    if( text )
    {
        textModel->setText( text, height );
    }
}


void CCTile3DButton::setupTextured(const float width, const char *textureName)
{
    const CCTextureBase *texture = gEngine->textureManager->getTexture( textureName, Resource_Packaged );
    const float aspectRatio = texture->getImageWidth() / texture->getImageHeight();
    const float height = width / aspectRatio;
    setTileSize( width, height );

    setTileTexture( textureName, Resource_Packaged );
}


void CCTile3DButton::setupTexturedHeight(const float height, const char *textureName)
{
    const CCTextureBase *texture = gEngine->textureManager->getTexture( textureName, Resource_Packaged );
    const float aspectRatio = texture->getImageWidth() / texture->getImageHeight();
    const float width = height * aspectRatio;
    setTileSize( width, height );
    
    setTileTexture( textureName, Resource_Packaged );
}


void CCTile3DButton::setupText(const char *text, const float height, const bool centered)
{
    textModel->setText( text, height );
    
    const float width = textModel->getWidth();
    
    setHCollisionBounds( width * 0.5f, height * 0.5f, CC_SMALLFLOAT );
    
    textModel->setCentered( centered );
    
    if( centered == false )
    {
        translate( collisionBounds.x, 0.0f, 0.0f );
    }
}


void CCTile3DButton::construct(CCSceneBase *scene)
{
    CCAddFlag( collideableType, collision_ui );

    if( scene != NULL )
    {
        setScene( scene );
    }
    
    renderDepth = false;
    setTransparent();
    
    model = new CCModelBase();
    
    tileModel = new CCModelBase();
    model->addModel( tileModel );
    setTileColour( CCColour(), true );
    tileColourInterpolator.setDuration( 0.5f );
    tileSquare = NULL;
    
    // Use an object to ensure the model is rendered in the transparent pass
    textObject = new CCSceneObject();
    textModel = new CCModelText( this );
    textObject->model = textModel;
    addChild( textObject );
    textObject->setTransparent( true );
    
    allowTouchRotation( false );
    touchRotationMagnitude = 0.0f;
    touchRotationSpeed = 1.0f;
    
    setTileScale( CCVector3( 1.0f ), false );

    allowTouchMovement( false );

    touchDepressInterpolator.setDuration( 0.125f );
    setTouchDepressRange( 1.0f );
    touchDepressDepth = 3.0f;

    CCUpdateCollisions( this );
}


void CCTile3DButton::destruct()
{
    super::destruct();
}

// CCRenderable
void CCTile3DButton::refreshModelMatrix()
{
	if( updateModelMatrix )
	{
        CCMatrixLoadIdentity( modelMatrix );	
        CCMatrixTranslate( modelMatrix, position.x, position.y, position.z );
        CCMatrixTranslate( modelMatrix, touchDepressPosition.x, touchDepressPosition.y, touchDepressPosition.z );
        
		if( scale != NULL )
		{	
			CCMatrixScale( modelMatrix, scale->x, scale->y, scale->z );
		}

        if( rotation.x != 0.0f )
        {
            CCMatrixRotate( modelMatrix, rotation.x, 1.0f, 0.0f, 0.0f );
        }

        if( rotation.y != 0.0f )
        {
            CCMatrixRotate( modelMatrix, rotation.y, 0.0f, 1.0f, 0.0f );
        }
        
        if( rotation.z != 0.0f )
        {
            CCMatrixRotate( modelMatrix, rotation.z, 0.0f, 0.0f, 1.0f );
        }
        
		if( touchRotationMagnitude != 0.0f )
        {
            CCMatrixRotate( modelMatrix, touchRotationMagnitude * 20.0f, 1.0f, 0.0f, 0.0f );
        }
        
		updateModelMatrix = false;
		dirtyWorldMatrix();
	}
}


// CCSceneObject
const bool CCTile3DButton::update(const CCTime &time)
{
    bool updated = super::update( time );

    if( scale != NULL )
    {
        const float speed = time.delta * 1.0f;
        if( scaleInterpolator.update( speed ) )
        {
            dirtyModelMatrix();
        }
        else
        {
            DELETE_POINTER( scale );
        }
        
        updated = true;
    }

    // Touch depress: On update
    if( touchDepressRange > 0.0f )
    {
        if( touchDepressInterpolator.update( time.delta ) )
        {
            dirtyModelMatrix();
            updated = true;
        }
    }

    if( touchRotationAllowed )
    {
        const float speed = touching || touchReleased ? 3.0f : touchRotationSpeed;
        if( touchRotationInterpolator.update( time.delta * speed ) )
        {
            const float magnitudeSquared = rotation.x * rotation.x + rotation.y * rotation.y;
            touchRotationMagnitude = sqrtf( magnitudeSquared );
            touchRotationMagnitude = MIN( touchRotationMagnitude, 1.0f );

            dirtyModelMatrix();
            updated = true;
        }
    }

    if( touching )
    {
        touchingTime += time.real;
    }
    else if( touchReleased )
    {
        if( touchDepressInterpolator.finished() && touchRotationInterpolator.finished() )
        {
            handleTouchRelease();
            updated = true;
        }
    }

    updated |= tileColourInterpolator.update( time.delta );

    if( textModel != NULL )
    {
        updated |= textModel->update( time.delta );
    }
    
    return updated;
}


void CCTile3DButton::render(const bool alpha)
{
    if( alpha )
    {
        if( renderDepth )
        {
            GLEnableDepth();
            super::render( alpha );
            GLDisableDepth();
        }
        else
        {
            super::render( alpha );
        }
    }
    else
    {
        if( renderDepth )
        {
            super::render( alpha );
        }
        else
        {
            GLDisableDepth();
            super::render( alpha );
            GLEnableDepth();
            
        }
    }
}


void CCTile3DButton::setTileSize(const float width, const float height)
{
    if( tileSquare == NULL )
    {
        tileSquare = new CCPrimitiveSquare();
        tileModel->addPrimitive( tileSquare );
    }

    const float hWidth = width * 0.5f;
    const float hHeight = height * 0.5f;
    setHCollisionBounds( hWidth, hHeight, CC_SMALLFLOAT );
    CCUpdateCollisions( this );
    
    tileSquare->setupZFacing( collisionBounds.x, collisionBounds.y );
}


void CCTile3DButton::setTileTexture(const char *name, const CCResourceType resourceType)
{
    tileSquare->setTexture( name, resourceType );
}


void CCTile3DButton::setTileColour(const CCColour &inColour, const bool immediatley)
{
    if( tileModel == NULL )
    {
        setTileSize( 0.0f, 0.0f );
    }
    
    if( immediatley )
    {
        tileModel->setColour( inColour );
        tileColourInterpolator.setup( tileModel->getColour(), *tileModel->getColour() );
    }
    else 
    {
        tileColourInterpolator.setup( tileModel->getColour(), inColour );
    }
}


void CCTile3DButton::setTileColourAlpha(const float inAlpha, const bool immediatley)
{
    if( immediatley )
    {
        tileModel->setColourAlpha( inAlpha );
        tileColourInterpolator.setup( tileModel->getColour(), *tileModel->getColour() );
    }
    else 
    {
        tileColourInterpolator.setTargetAlpha( inAlpha );
    }
}


void CCTile3DButton::setTileScale(const CCVector3 inScale, const bool immediatley)
{
    if( scale == NULL )
    {
        CCVector3FillPtr( &scale, 0.0f, 0.0f, 1.0f );
    }
    
    if( immediatley )
    {
        *scale = inScale;
        scaleInterpolator.setup( scale, *scale );
    }
    else 
    {
        scaleInterpolator.setup( scale, inScale );
    }
}


const bool CCTile3DButton::handleProjectedTouch(const CCCameraProjectionResults &cameraProjectionResults,
                                                const CCSceneCollideable *hitObject, 
                                                const CCVector3 &hitPosition,
                                                const CCScreenTouches &touch, 
                                                const CCTouchAction touchAction)
{
    if( enabled == false )
    {
        return false;
    }

    if( hitObject == this && 
        ( touchAction == touch_pressed || ( touchMovementAllowed && CCControls::TouchActionMoving( touchAction ) ) ) )
    {
        if( touching == false )
        {
            touching = true;
            touchingTime = 0.0f;
            onTouchPress();
        }
    }
    
    if( touching )
    {
        const float maxTimeHeld = 0.125f;
        
        if( touchAction == touch_pressed )
        {
            if( touch.timeHeld >= maxTimeHeld )
            {
                CCVector3 relativeHitPosition;
                relativeHitPosition.x = hitPosition.x - position.x;
                relativeHitPosition.y = hitPosition.y - position.y;
                float x = relativeHitPosition.x / collisionBounds.x;
                float y = relativeHitPosition.y / collisionBounds.y;
                touchActionPressed( x, y, touch, touchAction );
            }
        }
        else if( touchMovementAllowed && CCControls::TouchActionMoving( touchAction ) )
        {
            if( hitObject != this )
            {
                return handleProjectedTouch( cameraProjectionResults, hitObject, hitPosition, touch, touch_lost );
            }
            else
            {
                CCVector3 relativeHitPosition;
                relativeHitPosition.x = hitPosition.x - position.x;
                relativeHitPosition.y = hitPosition.y - position.y;
                float x = relativeHitPosition.x / collisionBounds.x;
                float y = relativeHitPosition.y / collisionBounds.y;
                touchActionPressed( x, y, touch, touchAction );
            }
        }
        else
        {   
            // Ensure we have a good touch release
            if( touchAction == touch_released )
            {
                if( touchMovementAllowed == false )
                {
                    const float absDeltaX = fabsf( touch.totalDelta.x );
                    const float absDeltaY = fabsf( touch.totalDelta.y );
                    if( absDeltaX > CCControls::GetTouchMovementThreashold().x || absDeltaY > CCControls::GetTouchMovementThreashold().y )
                    {
                        return handleProjectedTouch( cameraProjectionResults, hitObject, hitPosition, touch, touch_lost );
                    }
                }
                
                // If we have a good first touch, register it.
                CCVector3 relativeHitPosition;
                relativeHitPosition.x = hitPosition.x - position.x;
                relativeHitPosition.y = hitPosition.y - position.y;
                float x = relativeHitPosition.x / collisionBounds.x;
                float y = relativeHitPosition.y / collisionBounds.y;
                touchActionPressed( x, y, touch, touch_pressed );
            }

            touching = false;
            touchActionRelease( touchAction );
            
            if( touchAction == touch_released )
            {
                return true;
            }
        }
    }
    
    return false;
}


void CCTile3DButton::touchActionPressed(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    if( touchRotationAllowed )
    {
        float clampedX = x;
        float clampedY = y;
        CCClampFloat( clampedX, -1.0f, 1.0f );
        CCClampFloat( clampedY, -1.0f, 1.0f );
        touchRotationInterpolator.pushV3( &rotation, CCVector3( -clampedY, clampedX, 0.0f ), true );
    }
    
    // Touch depress: On pressed
    if( touchDepressRange > 0.0f )
    {
        // Touch depress range specifies the area which the button is pressed down
        // If it's out of range the button is pushed back up
        if( fabsf( x ) < touchDepressRange && fabsf( y ) < touchDepressRange )
        {
            touchDepressInterpolator.pushV3( &touchDepressPosition, CCVector3( 0.0f, 0.0f, -touchDepressDepth ), true );
        }
        else
        {
            touchDepressInterpolator.pushV3( &touchDepressPosition, CCVector3(), true );
        }
    }

    if( touchAction > touch_pressed && touchAction < touch_released )
    {
        touchActionMoved( x,  y,  touch, touchAction);
    }
}


void CCTile3DButton::touchActionMoved(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    onTouchMove();
}


void CCTile3DButton::touchActionRelease(const CCTouchAction touchAction)
{
    if( touchAction == touch_released )
    {
        touchReleased = true;
    }
    else 
    {
        handleTouchRelease();
        if( touchAction == touch_lost )
        {
            onTouchLoss();
        }
    }
}


void CCTile3DButton::handleTouchRelease()
{
    // If the touch has been released successfully we fire our callback
    if( touchReleased )
    {
        touchReleased = false;
        onTouchRelease();
    }

    if( touchRotationAllowed )
    {
        touchRotationInterpolator.pushV3( &rotation, CCVector3() );
    }

    // Touch depress: On releasse
    if( touchDepressRange > 0.0f )
    {
        touchDepressInterpolator.pushV3( &touchDepressPosition, CCVector3(), true );
    }
}
