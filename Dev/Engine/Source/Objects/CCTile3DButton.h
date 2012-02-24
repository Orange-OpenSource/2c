/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCTile3DButton.h
 * Description : A dynamic button widget.
 *
 * Created     : 14/09/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCTILE3DBUTTON_H__
#define __CCTILE3DBUTTON_H__


class CCModelText;

class CCTile3DButton : public CCTile3D
{
public:
    typedef CCTile3D super;
    
    CCTile3DButton(CCSceneBase *scene);
    
    void setupTile(const float width, const float height, const char *text=NULL);
    
    // Create tile with width and textures aspect ratio
    void setupTextured(const float width, const char *textureName);
    void setupTexturedHeight(const float heigth, const char *textureName);
    
    // Create tile with the text height
    void setupText(const char *text, const float height, const bool centered);
    
    virtual void construct(CCSceneBase *scene);
    virtual void destruct();
    
	// CCRenderable
	virtual void refreshModelMatrix();

	// CCSceneObject
	virtual const bool update(const CCTime &time);
    virtual void render(const bool alpha);

    virtual void setTileSize(const float width, const float height);
    void setTileTexture(const char *name, const CCResourceType resourceType);
    void setTileColour(const CCColour &inColour, const bool immediatley);
    void setTileColourAlpha(const float inAlpha, const bool immediatley);
    const CCColour& getTileColour() { return *tileModel->getColour(); }
    void setTileScale(const CCVector3 inScale, const bool immediatley);
    inline CCPrimitiveSquare* getTileSquare() { return tileSquare; }
    
    virtual void setText(const char *text, const float height=-1.0f)
    {
        textModel->setText( text, height );
    }
    
    void setTextColour(const CCColour &colour, const bool immediatly)
    {
        textModel->setColour( colour, immediatly );
    }
    
    void setTextAlpha(const float alpha, const bool immediatly)
    {
        textModel->setColourAlpha( alpha, immediatly );
    }
    
    void setTextFont(const char *font)
    {
        textModel->setFont( font );
    }
    
    virtual void setTextHeight(const float height)
    {
        textModel->setHeight( height );
    }
    
    void setRenderDepth(const bool toggle) { renderDepth = toggle; }
    
    // Touchable
    virtual const bool handleProjectedTouch(const CCCameraProjectionResults &cameraProjectionResults,
                                            const CCSceneCollideable *hitObject, 
                                            const CCVector3 &hitPosition,
                                            const CCScreenTouches &touch, 
                                            const CCTouchAction touchAction);

    // Called when the tile is touched
    virtual void touchActionPressed(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction);
    
    // Called when a touch is moved over this tile
    virtual void touchActionMoved(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction);

    // Called when the tile is released
    virtual void touchActionRelease(const CCTouchAction touchAction);

protected:
    // Callbacks
    virtual void handleTouchRelease();

public:
    inline void allowTouchRotation(const bool allow) { touchRotationAllowed = allow; }
    const CCVector3 getRotationTarget() 
    {
        if( touchRotationInterpolator.interpolators.length > 0 )
        {
            return touchRotationInterpolator.interpolators.list[touchRotationInterpolator.interpolators.length-1]->getTarget();
        }
        return rotation;
    }

    void allowTouchMovement(const bool allow=true)
    {
        touchMovementAllowed = allow;
    }
    
    void setTouchRotationSpeed(const float speed)
    {
        touchRotationSpeed = speed;
    }
    
    void setTouchDepressRange(const float range)
    {
        touchDepressRange = range;
    }

protected:
    bool renderDepth;

    CCModelBase *tileModel;
    CCPrimitiveSquare *tileSquare;
    
    CCSceneObject *textObject;
    CCModelText *textModel;
    
    CCInterpolatorV3<CCInterpolatorSin2Curve> scaleInterpolator;

    bool touchMovementAllowed;
    
    bool touchRotationAllowed;
    CCInterpolatorListV3<CCInterpolatorLinear> touchRotationInterpolator;
    float touchRotationMagnitude;
    float touchRotationSpeed;
    
    float touchDepressRange;
    float touchDepressDepth;

    // Touch Position interpolation
    CCVector3 touchDepressPosition;
    CCInterpolatorListV3<CCInterpolatorSin2Curve> touchDepressInterpolator;
    
public:
    CCInterpolatorLinearColour tileColourInterpolator;
};


#endif // __CCTILE3DBUTTON_H__
