/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCWidgetButton.h
 * Description : Depreciated 2D widget buttons.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCWIDGETBUTTON_H__
#define __CCWIDGETBUTTON_H__


class CCWidgetButton : public CCWidgetBase
{
public:
	typedef CCWidgetBase super;

	CCWidgetButton();
	virtual ~CCWidgetButton();
	
	// CCWidgetBase
    virtual const bool handleControls(const CCScreenTouches &screenTouch, const bool released=true);
    virtual const bool update(const CCGameTime &gameTime);
	virtual const bool render(const bool foreground);
	virtual void refreshDimensions();
	
	void setText(const char *inText=NULL, const float scale=1.0f);
	void setTextAndResize(const char *inText=NULL, const float scale=1.0f, const float x=1.0f, const float y=1.0f);
	
	void setTextColour(const CCColour newColour);
	void setTextTargetAlpha(const float start, const float end);
	void setTextTargetAlpha(const float target);

	void setTextScale(const float scale=1.0f);
	void setTextScaleTarget(const float target=-1.0f);

	virtual const bool isDragged() { return dragging; }
	void touchLost() { dragging = false; }
	inline const bool getTextTargetAlpha() { return textAlphaTarget; }

	inline const CCWidgetButton* unHover()
	{
		if( hovering )
		{
			hovering = false;
			if( onHoverOut.routine )
			{
				onHoverOut.routine( onHoverOut.parameter1, onHoverOut.parameter2 );
			}
			return this;
		}
		
		return NULL;
	}
	
	CCWidgetCallbacks onRelease, onDrag, onHoverOver, onHoverOut;
	
	const char *text;
	char textBuffer[100];
	uint textLength;
	
	bool dropShadow;

	bool textCentered;
    CCSize textOffset;

	bool clickable, draggable;

	float textScaleTargets[2];
	bool textScaleTargetInterpolateBack;
	bool zoomed;

protected:
    CCPoint textStart;
    CCSize textSize;
	float textScale;
	uint textScaleTargetIndex;

	bool controlsReleased;

	bool dragging, hovering;
    CCPoint dragOffset;

public:
	CCColour *textColour;

protected:
	float textAlphaTarget;
};


void callback_buttonHoverOver(void *parameter1, void *parameter2);
void callback_buttonHoverOut(void *parameter1, void *parameter2);

void callback_buttonHoverOverText(void *parameter1, void *parameter2);
void callback_buttonHoverOutText(void *parameter1, void *parameter2);


#endif // __CCWIDGETBUTTON_H__
