/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCSceneBase.h
 * Description : Handles the drawing and updating of objects.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCSCENEBASE_H__
#define __CCSCENEBASE_H__


class CCSceneObject;
class CCWidgetBase;
class CCWidgetButton;

class CCSceneBase : public CCBaseType
{
public:
	CCSceneBase();
	virtual void destruct();
	
	virtual void setup() {};
	virtual void restart() {}
	
    void deleteLater();
    void deleteLinkedScenesLater();
    inline const bool shouldDelete() { return deleteMe; }
	
    // Called by the Engine to let the scene fetch and handle the controls
    virtual const bool updateControls(const CCTime &time);
    
    // Called by updateControls or a user object perhaps rendering the scene in a frame buffer with specific touches to handle the controls with
    virtual const bool handleTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2, const CCTime &time) { return false; }
    
    virtual const bool handleBackButton() { return false; }
    
	const bool update(const CCTime &time);
    const bool updateTask(const CCTime &time);
    
protected:
    virtual const bool updateScene(const CCTime &time);
    virtual const bool updateCamera(const CCTime &time);
	
public:
    virtual const bool render(const CCCameraBase *inCamera, const int pass, const bool alpha);
    
protected:
    virtual void renderObjects(const CCCameraBase *inCamera, const int pass, const bool alpha);
    
public:
	// For sorted objects, we get passed the object to draw here
	virtual void renderOctreeObject(CCSceneObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);

    virtual void render2DBackground(const uint inCameraIndex);
    virtual void render2DForeground(const uint inCameraIndex);
	
	// Add object to the scene and place in the created list
	void addObject(CCSceneObject *object);
	void removeObject(CCSceneObject* object);
    
    void addCollideable(CCSceneCollideable *collideable);
    void removeCollideable(CCSceneCollideable *collideable);
    
    void setParent(CCSceneBase *inParent);
    void addChildScene(CCSceneBase *inScene);
    void removeChildScene(CCSceneBase *inScene);
    void linkScene(CCSceneBase *inScene);
    void unlinkScene(CCSceneBase *inScene);

    virtual void beginOrientationUpdate() {}
    virtual void finishOrientationUpdate() {}
	
protected:
	// Add a widget button
	void addWidget(CCWidgetBase *widget);
	void removeWidget(CCWidgetBase *widget);
	void insertWidget(CCWidgetBase *widget, const int index=0);
	CCWidgetButton* addButton(const char* text, const float textScale=1.0f, const float buttonXScale=1.0f, const float buttonYScale=1.0f);
    CCWidgetBase* handleWidgetControls(const struct CCScreenTouches &touch, const bool released);
    void setWidgetUnHover();

    // HTTP requests
    void requestDownload(const char *url, const char *token);
    virtual void dataDownloaded(CCURLRequest *reply, const char *token);

public:
    bool enabled;
    
protected:
    bool deleteMe;

protected:
    CCList<CCSceneObject> objects;
    CCList<CCSceneCollideable> collideables;
    CCDestructList<CCWidgetBase> widgets;

    CCSceneBase *parentScene;
    CCList<CCSceneBase> childScenes;
    CCList<CCSceneBase> linkedScenes;

    float lifetime;
};


#endif // __CCSCENEBASE_H__
