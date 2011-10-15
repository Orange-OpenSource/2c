/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCWidgetBase.h
 * Description : Depreciated 2D widgets.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCWIDGETBASE_H__
#define __CCWIDGETBASE_H__


struct CCWidgetCallbacks
{
    CCWidgetCallbacks()
    {
        routine = NULL;
        parameter1 = parameter2 = NULL;
    }

    void run()
    {
        if( routine != NULL )
        {
            routine( parameter1, parameter2 );
        }
    }

    void (*routine)(void *parameter1, void *parameter2);
    void *parameter1, *parameter2;
};


struct CCScreenTouches;

class CCWidgetBase : public CCBaseType
{
public:
    CCWidgetBase();
    virtual void destruct();
	
    const bool inActiveSpace(const CCPoint &inPosition);
    const bool touchInActiveSpace(const CCScreenTouches &screenTouch);
    virtual const bool handleControls(const CCScreenTouches &screenTouch, const bool released=true);
    virtual const bool update(const CCGameTime &gameTime);
	virtual const bool render(const bool foreground);
	
	inline const float getWidth() const { return size.width; }
	inline const float getHeight() const { return size.height; }
    inline const CCSize getSizeStart() { return sizeStart; }
    inline const CCPoint getPosition() { return position; }
    inline const CCPoint getTargetPosition() { return positionTarget; }
	
    void setPosition(const CCPoint &inPosition);
	void setPosition(const float x, const float y);
    void setTarget(const CCPoint &target);
	void setTarget(const float x, const float y);
	
    virtual void setSize(const CCSize &size, const bool correctAspect=true);
	void setSize(const float width, const float height, const bool correctAspect=true);
    void setSizeTarget(const CCSize &size, const bool correctAspect=true, const float speed=-1.0f);
	void setSizeTarget(const float width, const float height, const bool correctAspect=true, const float speed=-1.0f);
	
	inline const bool isInView() { return inView; }
	inline const bool interpolating() const
	{
		return moving() ||
		size.width != sizeTarget.width || size.height != sizeTarget.height ||
		( colour != NULL && colourTarget != NULL && colour->equals( *colourTarget ) == false );
	}
	
	inline const bool moving() const
	{
		return positionTarget.x != position.x || positionTarget.y != position.y;
	}

    virtual const CCWidgetBase* unHover() { return NULL; }
	
	// Update min/max
	virtual void refreshDimensions();
	
	inline const float getStartX() { return min.x; }
	inline const float getStartY() { return min.y; }
	inline const float getEndX() { return max.x; }
	inline const float getEndY() { return max.y; }
	
	inline const CCColour& getColour() { return *colour; }
	inline const float getTargetAlpha() { return colourTarget != NULL ? colourTarget->alpha : 1.0f; }
	
	void setColour(const CCColour &newColour);
	void setTargetColour(const CCColour &newColour);
	
	void setColourAlpha(const float alpha);
	void setTargetAlpha(const float target);
    void setTexture(const char *name, const CCResourceType resourceType);
	
	virtual const bool isDragged() { return false; }
	
	bool enabled;
	
	bool renderButton;		// Should we render our background button
	bool renderOutline;
	bool renderForeground;	// Are we rendering in the foreground
	
	float movementSpeed, resizeSpeed;
	
	bool distortEffect;		// Apply distortion effect?
	float distortAmount, distortSpeed;
	
	float colourSpeed;
	
	CCWidgetCallbacks onInterpolated;
	struct CCPrimitiveSquareUVs *customUVs;

protected:
	bool inView;
	
    CCPoint position, positionTarget;
    CCSize size, sizeTarget, sizeStart;
	
    CCPoint min, max;
	
	CCColour *colour, *colourTarget;
	int textureIndex;
	
    CCPoint distort, distortTarget;
};

void callback_widgetDisableOnInterpolated(void *parameter1, void *parameter2);



#endif // __CCWIDGETBASE_H__
