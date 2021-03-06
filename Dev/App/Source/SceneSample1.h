/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : SceneSample1.h
 * Description : Sample demonstrating a scene with one tile.
 *
 * Created     : 15/10/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class SceneSample1 : public CCSceneAppUI
{
public:
    typedef CCSceneAppUI super;

    SceneSample1();

    // CCSceneBase
	virtual void setup();
	virtual void destruct();
    
protected:
	virtual const bool updateScene(const CCTime &gameTime);
    virtual const bool updateCamera(const CCTime &gameTime);
    
public:
	virtual void renderOctreeObject(CCSceneObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);
    
protected:
    virtual const bool touchPressed(const CCScreenTouches &touch);
    virtual const bool touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta);
    virtual const bool touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction);

    virtual const bool touchCameraRotating(const float x, const float y);
};