/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCTile3DFrameBuffer.h
 * Description : A tile which displays the contents of a frame buffer
 *
 * Created     : 07/02/12
 *-----------------------------------------------------------
 */

#ifndef __CCTILE3DFRAMEBUFFER_H__
#define __CCTILE3DFRAMEBUFFER_H__


class CCTile3DFrameBuffer : public CCTile3DButton
{
public:
    typedef CCTile3DButton super;
    
    CCTile3DFrameBuffer(CCSceneBase *scene);
    virtual void destruct();

	// CCSceneObject
	virtual const bool update(const CCTime &time);
    virtual void render(const bool alpha);
    
    // Touchable
    virtual const bool handleProjectedTouch(const CCCameraProjectionResults &cameraProjectionResults,
                                            const CCSceneCollideable *hitObject, 
                                            const CCVector3 &hitPosition,
                                            const CCScreenTouches &touch, 
                                            const CCTouchAction touchAction);
    
    virtual const bool handleSceneTouch(const CCCameraProjectionResults &cameraProjectionResults,
                                        const CCSceneCollideable *hitObject, 
                                        const CCVector3 &hitPosition,
                                        const CCScreenTouches &screenTouch, 
                                        const CCTouchAction touchAction);
    
    void setFrameBufferId(const int frameBufferId);
    void linkScene(CCSceneBase *scene)
    {
        linkedScenes.add( scene );
        scene->enabled = false;
    }
    
    static void ResetRenderFlag()
    {
        renderedThisFrame = 0;
    }

protected:
    int frameBufferId;
    CCList<CCSceneBase> linkedScenes;
    
    bool sceneControlsActive;
    CCScreenTouches touches[CCControls::numberOfTouches];
    
    bool firstRender;
    static CCList<CCTile3DFrameBuffer> frameBuffers;
    static uint renderedThisFrame;
};


#endif // __CCTILE3DFRAMEBUFFER_H__
