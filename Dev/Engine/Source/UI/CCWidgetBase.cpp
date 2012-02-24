/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCWidgetBase.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCWidgetBase.h"
#include "CCPrimitives.h"
#include "CCRenderTools.h"


void callback_widgetDisableOnInterpolated(void *parameter1, void *parameter2)
{
	CCWidgetBase *widget = (CCWidgetBase*)parameter1;
	widget->enabled = false;
	widget->onInterpolated.routine = NULL;
}


CCWidgetBase::CCWidgetBase()
{
	customUVs = NULL;
	
	inView = false;
	
	setSize( 0.05f, 0.05f );
	setPosition( 0.5f, 0.5f );
	
	colour = colourTarget = NULL;
	textureIndex = 0;
	
	enabled = true;
	renderButton = true;
	renderOutline = false;
	renderForeground = true;
	movementSpeed = 1.0f;
	resizeSpeed = 0.5f;
	
	distortEffect = false;
    distort = distortTarget = CCPoint( 0.0f, 0.0f );
	distortAmount = 0.005f;
	distortSpeed = 0.05f;
	
	colourSpeed = 1.0f;
}


void CCWidgetBase::destruct()
{
	if( customUVs != NULL )
	{
		delete customUVs;
	}
	
	if( colour != NULL )
	{
		delete colour;
	}
	
	if( colourTarget != NULL )
	{
		delete colourTarget;
	}
}


const bool CCWidgetBase::inActiveSpace(const CCPoint &inPosition)
{
	if( inPosition.x >= min.x && inPosition.x <= max.x )
	{
		if( inPosition.y >= min.y && inPosition.y <= max.y )
		{
			return true;
		}
	}
	
	return false;
}


const bool CCWidgetBase::touchInActiveSpace(const CCScreenTouches &screenTouch)
{
    return inActiveSpace( screenTouch.position );
}


const bool CCWidgetBase::handleControls(const CCScreenTouches &screenTouch, const bool released)
{
	return false;
}


const bool CCWidgetBase::update(const CCTime &time)
{	
	if( enabled )
	{
		const bool positionInterpolation = position.x != positionTarget.x || position.y != positionTarget.y;
		const bool sizeInterpolation = size.width != sizeTarget.width || size.height != sizeTarget.height;
		const bool colourInterpolation = colour != NULL && colourTarget != NULL && colour->equals( *colourTarget ) == false;
		if( positionInterpolation || sizeInterpolation || colourInterpolation )
		{
			if( positionInterpolation || sizeInterpolation )
			{
				if( positionInterpolation )
				{
                    const float deltaSpeed = time.delta * movementSpeed;
                    CCToTarget( position.x, positionTarget.x, deltaSpeed );
                    CCToTarget( position.y, positionTarget.y, deltaSpeed );
				}
		
				if( sizeInterpolation )
				{
                    const float deltaSpeed = time.delta * resizeSpeed;
                    CCToTarget( size.width, sizeTarget.width, deltaSpeed );
                    CCToTarget( size.height, sizeTarget.height, deltaSpeed );
				}
				
				refreshDimensions();
			}
			
			if( colourInterpolation )
			{
                colour->toTarget( *colourTarget, time.delta * colourSpeed );
			}
			
			if( interpolating() == false )
			{
				if( onInterpolated.routine != NULL )
				{
					onInterpolated.routine( onInterpolated.parameter1, onInterpolated.parameter2 );
				}
			}
		}
		
		if( inView && distortEffect )
		{
            const float deltaSpeed = time.delta * distortSpeed;
            if( CCToTarget( distort.x, distortTarget.x, deltaSpeed ) == false )
			{
				int random = rand();
				distortTarget.x = ( random % 10 ) * distortAmount * size.width;
			}
			
            if( CCToTarget( distort.y, distortTarget.y, deltaSpeed ) == false )
			{
				int random = rand();
				distortTarget.y = ( random % 10 ) * distortAmount * size.width;
			}
		}
		
		return true;
	}
	
	return false;
}


const bool CCWidgetBase::render(const bool foreground)
{	
	if( inView && enabled && foreground == renderForeground )
	{
		if( colour == NULL )
		{
            static CCColour colour( 1.0f, 1.0f, 1.0f, 0.75f );
			CCSetColour( colour );
		}
		else
		{
			CCSetColour( *colour );
		}
		
		if( renderButton )
		{
            if( textureIndex != 0 )
			{
				gEngine->textureManager->setTextureIndex( textureIndex );
				
				if( customUVs != NULL )
				{
					CCSetTexCoords( customUVs->uvs );
				}
				else
				{
					CCDefaultTexCoords();
				}
			}
			else
			{
				gEngine->textureManager->setTextureIndex( 0 );
			}
			
			CCRenderRectanglePoint( position, size.width, -size.height );
			
			if( renderOutline )
			{
				if( colour == NULL )
				{
                    static CCColour colour( 0.5f, 0.5f, 0.5f, 0.75f );
					CCSetColour( colour );
				}
				else
				{
                    CCColour colourObject( colour->red * 0.5f, colour->green * 0.5f, colour->blue * 0.5f, colour->alpha );
					CCSetColour( colourObject );
				}
				CCRenderRectanglePoint( position, size.width, -size.height, true );
			}
			
			if( distortEffect )
			{
                static CCColour colour( 1.0f, 1.0f, 1.0f, 0.5f );
				CCSetColour( colour );
				CCRenderRectanglePoint( position, size.width + distortTarget.x, size.height + distortTarget.y );
			}
		}
		
		return true;
	}
	
	return false;
}



void CCWidgetBase::setPosition(const CCPoint &inPosition)
{
	setPosition( inPosition.x, inPosition.y );
}


void CCWidgetBase::setPosition(const float x, const float y)
{
	position.x = x;
	position.y = y;
	positionTarget = position;
	refreshDimensions();
}


void CCWidgetBase::setTarget(const CCPoint &target)
{
	setTarget( target.x, target.y );
}


void CCWidgetBase::setTarget(const float x, const float y)
{
	positionTarget.x = x;
	positionTarget.y = y;
}


void CCWidgetBase::setSize(const CCSize &size, const bool correctAspect)
{
	setSize( size.width, size.height, correctAspect );
}


void CCWidgetBase::setSize(const float width, const float height, const bool correctAspect)
{
	size.width = width;
	size.height = height;
	
	if( correctAspect )
	{
		size.height *= gEngine->renderer->getAspectRatio();
	}
	sizeStart = sizeTarget = size;
}


void CCWidgetBase::setSizeTarget(const CCSize &size, const bool correctAspect, const float speed)
{
	setSizeTarget( size.width, size.height, correctAspect, speed );
}


void CCWidgetBase::setSizeTarget(const float width, const float height, const bool correctAspect, const float speed)
{
	sizeTarget.width = width;
	sizeTarget.height = height;
	
	if( correctAspect )
	{
		sizeTarget.height *= gEngine->renderer->getAspectRatio();
	}
	
	if( speed != -1.0f )
	{
		resizeSpeed = speed;
	}
}


void CCWidgetBase::refreshDimensions()
{
	min.x = position.x - size.width;
	min.y = position.y - size.height;
	max.x = position.x + size.width;
	max.y = position.y + size.height;
	
	inView = max.y > 0.0f && min.y < 1.0f && max.x > 0.0f && min.x < 1.0f;
}


void CCWidgetBase::setColour(const CCColour &newColour)
{
	if( colour == NULL )
	{
		colour = new CCColour();
	}
	
	*colour = newColour;

	if( colourTarget != NULL )
	{
		*colourTarget = *colour;
	}
}


void CCWidgetBase::setTargetColour(const CCColour &newColour)
{
	if( colourTarget == NULL )
	{
		colourTarget = new CCColour();
	}
	
	*colourTarget = newColour;
}


void CCWidgetBase::setColourAlpha(const float alpha)
{
	if( colour == NULL )
	{
        colour = new CCColour();
	}
	
	colour->alpha = alpha;
	
	if( colourTarget != NULL )
	{
		*colourTarget = *colour;
	}
}


void CCWidgetBase::setTargetAlpha(const float target)
{
	if( colourTarget == NULL )
	{
		if( colour == NULL )
		{
			colour = new CCColour();
		}
		
		setTargetColour( *colour );
	}
	
	colourTarget->alpha = target;
}


void CCWidgetBase::setTexture(const char *name, const CCResourceType resourceType)
{
    textureIndex = gEngine->textureManager->assignTextureIndex( name, resourceType, false, false );
}
