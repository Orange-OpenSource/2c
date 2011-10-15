/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCSceneBase.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCSceneBase.h"
#include "CCObjects.h"
#include "CCWidgets.h"


CCSceneBase::CCSceneBase()
{
	enabled = true;
    deleteMe = false;
	sceneID = 255;
	
    activeObjects.allocate( MAX_OBJECTS );
	
    parentScene = NULL;
	lifetime = 0.0f;
}


void CCSceneBase::destruct()
{
    childScenes.deleteObjectsAndList();

    while( activeObjects.length > 0 )
    {
        CCSceneObject *object = activeObjects.list[0];
        DELETE_OBJECT( object );
    }
    ASSERT( collideables.length == 0 );
    widgets.deleteObjectsAndList();
}


void CCSceneBase::deleteLater()
{
    deleteMe = true;
    deleteLinkedScenesLater();
}


void CCSceneBase::deleteLinkedScenesLater()
{
    for( int i=0; i<linkedScenes.length; ++i )
    {
        CCSceneBase *linkedScene = linkedScenes.list[i];
        linkedScene->unlinkScene( this );
        linkedScene->deleteLater();
    }
    linkedScenes.length = 0;
}


const bool CCSceneBase::handleControls(const CCGameTime &gameTime)
{
    for( int i=0; i<childScenes.length; ++i )
    {
        if( childScenes.list[i]->handleControls( gameTime ) )
        {
            return true;
        }
    }
	return false;
}


void CCSceneBase::update(const CCGameTime &gameTime)
{
    // Remove any redundant scenes
    for( int i=0; i<childScenes.length; ++i )
    {
        CCSceneBase *scene = childScenes.list[i];
        if( scene->shouldDelete() )
        {
            removeScene( scene );
            i--;
        }
    }

    if( enabled )
    {
        lifetime += gameTime.real;

        updateScene( gameTime );
        // Update our child scenes
        for( int i=0; i<childScenes.length; ++i )
        {
            CCSceneBase *scene = childScenes.list[i];
            scene->update( gameTime );
        }
        
        updateCamera( gameTime );
        for( int i=0; i<childScenes.length; ++i )
        {
            childScenes.list[i]->updateCamera( gameTime );
        }
    }
}


void CCSceneBase::updateScene(const CCGameTime &gameTime)
{
    for( int i=0; i<activeObjects.length; ++i )
    {
        if( activeObjects.list[i]->isActive() )
        {
            activeObjects.list[i]->update( gameTime );
        }
        else if( activeObjects.list[i]->deleteMe > 0 )
        {
            if( --activeObjects.list[i]->deleteMe == 0 )
            {
                CCSceneObject *deleteObject = activeObjects.list[i];
                DELETE_OBJECT( deleteObject );
                i--;
            }
        }
    }
    
    for( int i=0; i<widgets.length; ++i )
    {
        widgets.list[i]->update( gameTime );
    }
}


void CCSceneBase::updateCamera(const CCGameTime &gameTime)
{
    
}


const bool CCSceneBase::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    bool rendered = false;
    if( inCamera->getIndex() == 0 )
	{
        renderObjects( pass, alpha );
        rendered = true;
	}

    for( int i=0; i<childScenes.length; ++i )
    {
        rendered |= childScenes.list[i]->render( inCamera, pass, alpha );
    }

    return rendered;
}


void CCSceneBase::renderObjects(const int pass, const bool alpha)
{
#if defined PROFILEON
    CCProfiler profile( "CCSceneBase::renderObjects()" );
#endif

    for( int i=0; i<activeObjects.length; ++i )
    {
        CCSceneObject *object = activeObjects.list[i];
        if( object->renderPass == pass )
        {
            if( object->isActive() && ( object->octreeRender == false ) )
            {
                object->render( alpha );
            }
        }
    }
}


void CCSceneBase::renderOctreeObject(CCSceneObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
	if( inCamera->getIndex() == 0 )
	{
        object->render( alpha );
	}
}


void CCSceneBase::render2DBackground(const uint inCameraIndex)
{
    for( int i=0; i<childScenes.length; ++i )
    {
        childScenes.list[i]->render2DBackground( inCameraIndex );
    }
}


void CCSceneBase::render2DForeground(const uint inCameraIndex)
{
    for( int i=0; i<childScenes.length; ++i )
    {
        childScenes.list[i]->render2DForeground( inCameraIndex );
    }
}


void CCSceneBase::addObject(CCSceneObject *object)
{
    ASSERT( object->inScene == NULL );
    object->inScene = this;
    activeObjects.add( object );
    ASSERT( activeObjects.length < MAX_OBJECTS );
}


void CCSceneBase::removeObject(CCSceneObject* object)
{
    //DEBUGLOG( "removeObject - %@", [object description] );
	
    ASSERT( object->inScene == this );

    object->inScene = NULL;
    const bool removed = activeObjects.remove( object );
    if( removed == false )
    {
        ASSERT( removed );
    }
}


void CCSceneBase::addCollideable(CCSceneCollideable *collideable)
{
    collideables.add( collideable );
	gEngine->addCollideable( collideable );
}


void CCSceneBase::removeCollideable(CCSceneCollideable *collideable)
{
    collideables.remove( collideable );
	gEngine->removeCollideable( collideable );
}


void CCSceneBase::setParent(CCSceneBase *inParent)
{
    ASSERT( parentScene == NULL );
    parentScene = inParent;
}


void CCSceneBase::addScene(CCSceneBase *inScene)
{
    inScene->setParent( this );
    childScenes.add( inScene );
    inScene->setup();
}


void CCSceneBase::removeScene(CCSceneBase *inScene)
{
    if( childScenes.remove( inScene ) )
    {
        DELETE_OBJECT( inScene );
    }
}


void CCSceneBase::linkScene(CCSceneBase *inScene)
{
    linkedScenes.add( inScene );
}


void CCSceneBase::unlinkScene(CCSceneBase *inScene)
{
    linkedScenes.remove( inScene );
}


void CCSceneBase::addWidget(CCWidgetBase *widget)
{
    if( widget != NULL )
    {
        widgets.add( widget );
    }
}


void CCSceneBase::removeWidget(CCWidgetBase *widget)
{
    if( widget != NULL )
    {
        widgets.remove( widget );
        DELETE_OBJECT( widget );
    }
}


void CCSceneBase::insertWidget(CCWidgetBase *widget, const int index)
{
    widgets.add( widget );
    for( int i=index+1; i<widgets.length; ++i )
    {
        widgets.list[i] = widgets.list[i-1];
    }
    widgets.list[index] = widget;
}


CCWidgetButton* CCSceneBase::addButton(const char* text, const float textScale, const float buttonXScale, const float buttonYScale)
{
	CCWidgetButton *newButton = new CCWidgetButton();
	addWidget( newButton );
	if( text != NULL )
	{
		newButton->setTextAndResize( text, textScale, buttonXScale, buttonYScale );
	}
	
	return newButton;
}


CCWidgetBase* CCSceneBase::handleWidgetControls(const CCScreenTouches &touch, const bool released)
{
    CCWidgetBase *controlledWidget = NULL;

    // Run through all the contained widgets
    for( int i=0; i<widgets.length; ++i )
    {
        if( widgets.list[i]->isDragged() && widgets.list[i]->handleControls( touch, released ) )
        {
            if( controlledWidget == NULL )
            {
                controlledWidget = widgets.list[i];
            }
        }
    }

    for( int i=0; i<widgets.length; ++i )
    {
        if( widgets.list[i]->handleControls( touch, released ) )
        {
            if( controlledWidget == NULL )
            {
                controlledWidget = widgets.list[i];
            }
        }
    }

    return controlledWidget;
}


void CCSceneBase::setWidgetUnHover()
{
    for( int i=0; i<widgets.length; ++i )
    {
        widgets.list[i]->unHover();
    }
}


void CCSceneBase::requestDownload(const char *url, const char *token)
{
    class Callback : public CCURLCallback
    {
    public:
        Callback(CCSceneBase *scene, const char *token)
        {
            this->scene = scene;
            this->token = token;
        }

        void run()
        {
            scene->dataDownloaded( reply, token );
        }

    private:
        CCSceneBase *scene;
        const char *token;
    };

    gEngine->urlManager->requestURL( url, new Callback( this, token ) );
}


void CCSceneBase::dataDownloaded(CCURLRequest *reply, const char *type)
{
    // e.g.
//    if( reply->state >= URLRequest::succeeded )
//    {
//        if( strcmp( type, "refresh" ) == 0 )
//        {
//            const char *data = reply->data.buffer;
//            if( data != NULL )
//            {
//            }
//        }
//    }
}
