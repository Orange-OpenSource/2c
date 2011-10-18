/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCWidgetButton.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCWidgets.h"
#include "CCTextureFontPage.h"


void callback_buttonHoverOver(void *parameter1, void *parameter2)
{
	CCWidgetButton *button = (CCWidgetButton*)parameter2;
	button->setSizeTarget( button->getSizeStart().width * 1.25f, button->getSizeStart().height * 1.25f, false ); 
        button->zoomed = true;
}


void callback_buttonHoverOut(void *parameter1, void *parameter2)
{
	CCWidgetButton *button = (CCWidgetButton*)parameter2;
	button->setSizeTarget( button->getSizeStart().width, button->getSizeStart().height, false ); 
	button->zoomed = false;
}


void callback_buttonHoverOverText(void *parameter1, void *parameter2)
{
	CCWidgetButton *button = (CCWidgetButton*)parameter2;
	button->setTextScaleTarget( button->textScaleTargets[1] * 1.25f ); 
}


void callback_buttonHoverOutText(void *parameter1, void *parameter2)
{
	CCWidgetButton *button = (CCWidgetButton*)parameter2;
	button->setTextScaleTarget( button->textScaleTargets[1] ); 
}


CCWidgetButton::CCWidgetButton()
{	
	textBuffer[0] = 0;
	text = NULL;
	textLength = 0;
	
	dropShadow = false;
	
	textColour = NULL;
	textAlphaTarget = 1.0f;
	
	textCentered = true;
    textOffset = CCSize( 0.0f, 0.0f );
	
        clickable = true;
	draggable = false;
	
    textStart = CCPoint( 0.0f, 0.0f );
    textSize = CCSize( 0.0f, 0.0f );
	setTextScale( 1.0f );
        textScaleTargetInterpolateBack = true;
	zoomed = false;
	
        controlsReleased = true;
	
	hovering = false;
	dragging = false;
    dragOffset = CCPoint( -25.0f * gEngine->renderer->screenSizeMultiple.width, 25.0f * gEngine->renderer->screenSizeMultiple.height );
}


CCWidgetButton::~CCWidgetButton()
{
	if( textColour != NULL )
	{
		delete textColour;
	}
}


// CCWidgetBase
const bool CCWidgetButton::handleControls(const CCScreenTouches &screenTouch, const bool released)
{
    if( enabled && clickable && renderForeground )
    {
        const bool active = touchInActiveSpace( screenTouch );

        if( active || dragging )
        {
            if( draggable )
            {
                CCPoint offsetPosition = screenTouch.position;
                offsetPosition.x += dragOffset.x;
                offsetPosition.y -= dragOffset.y;
                setPosition( offsetPosition.x, offsetPosition.y );
                dragging = true;

                if( onDrag.routine )
                {
                    onDrag.routine( onDrag.parameter1, onDrag.parameter2 );
                }
            }

            if( hovering == false && onHoverOver.routine )
            {
                onHoverOver.routine( onHoverOver.parameter1, onHoverOver.parameter2 );
                hovering = true;
            }

            if( controlsReleased == released )
            {
                dragging = false;
                if( onRelease.routine )
                {
                    onRelease.routine( onRelease.parameter1, onRelease.parameter2 );
                }

                unHover();
            }
            return true;
        }
        else
        {
            unHover();
        }
    }

    return false;
}


const bool CCWidgetButton::update(const CCGameTime &gameTime)
{
	if( super::update( gameTime ) )
	{
		if( textColour != NULL )
		{
            CCToTarget( textColour->alpha, textAlphaTarget, gameTime.delta );
		}
		
        const float speed = gameTime.delta * 3.0f;
		if( textScaleTargetIndex == 0 )
		{
            if( CCToTarget( textScale, textScaleTargets[0], speed ) == false )
			{
				textScaleTargetIndex = 1;
			}
		}
		else if( textScaleTargetInterpolateBack )
		{
            CCToTarget( textScale, textScaleTargets[1], speed );
		}

        return true;
	}
	
	return false;
}


const bool CCWidgetButton::render(const bool foreground)
{	
	if( super::render( foreground ) )
	{
		if( foreground == renderForeground )
		{
			if( text != NULL )
			{
				if( textColour != NULL )
				{
					if( textColour->alpha == 0.0f )
					{
						return false;
					}
					
					CCSetColour( *textColour );
				}
				else
				{
					CCSetColour( CCColour() );
				}
				
                gEngine->textureManager->fontPages.list[0]->renderText( text, textLength, textStart.x, textStart.y, textCentered, true, textScale );
				
				if( dropShadow )
				{
					GLColor4f( 0.0f, 0.0f, 0.0f, 0.25f );
					gEngine->textureManager->fontPages.list[0]->renderText( text, textLength,
                                                                           textStart.x + gEngine->renderer->screenSizeMultiple.width,
                                                                           textStart.y + gEngine->renderer->screenSizeMultiple.height,
                                                                           textCentered, true, textScale );
				}
			}
		}
		
        return true;
	}
	
	return false;
}


void CCWidgetButton::refreshDimensions()
{
	super::refreshDimensions();
	
	const float textHeight = gEngine->textureManager->fontPages.list[0]->getHeight( text, textLength, textScale );
	if( textCentered )
	{
		textStart.x = position.x;
		textStart.y = position.y - textSize.height + textHeight * gEngine->renderer->aspectRatio;
	}
	else 
	{
		textStart.x = min.x + ( textOffset.width * size.width * 2.0f );
		textStart.y = min.y + ( textOffset.height * size.height * 2.0f ) + textHeight * gEngine->renderer->aspectRatio;
	}
}


void CCWidgetButton::setText(const char *inText, const float scale)
{
	if( inText == NULL )
	{
		inText = textBuffer;
	}
	
	text = inText;
	textLength = strlen( text );
	setTextScale( scale );
	
	const float textWidth = gEngine->textureManager->fontPages.list[0]->getWidth( text, textLength, textScale );
	const float textHeight = gEngine->textureManager->fontPages.list[0]->getHeight( text, textLength, textScale );
	
    textSize = CCSize( textWidth * 0.5f, textHeight * 0.5f );
	
	refreshDimensions();
}


void CCWidgetButton::setTextAndResize(const char *inText, const float scale, const float x, const float y)
{
	setText( inText, scale );
	
	size = textSize;
	size.width *= x;
	size.height *= y;
	setSize( size, false );
	
	refreshDimensions();
}


void CCWidgetButton::setTextTargetAlpha(const float start, const float end)
{
	if( textColour == NULL )
	{
		setTextColour( CCColour() );
	}
	textColour->alpha = start;
	textAlphaTarget = end;
}


void CCWidgetButton::setTextTargetAlpha(const float target)
{
	setTextTargetAlpha( textColour == NULL ? 0.0f : textColour->alpha, target );
}


void CCWidgetButton::setTextColour(const CCColour newColour)
{
	if( textColour == NULL )
	{
		textColour = new CCColour();
	}
	
	*textColour = newColour;
	textAlphaTarget = textColour->alpha;
}


void CCWidgetButton::setTextScale(const float scale)
{
	textScale = scale;
	for( uint i=0; i<2; ++i )
	{
		textScaleTargets[i] = scale;
	}
	textScaleTargetIndex = 0;
}


void CCWidgetButton::setTextScaleTarget(const float target)
{
	textScaleTargetIndex = 0;
	textScaleTargets[0] = target == -1.0f ? textScale : target;
}
