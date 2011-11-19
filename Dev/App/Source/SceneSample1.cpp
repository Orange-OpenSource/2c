/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : SceneSample1.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Samples.h"


SceneSample1::SceneSample1()
{
    // Create a new camera
    camera = new CCCameraAppUI();
    gEngine->addCamera( camera );
    
    // Set it up to take up the entire screen
    camera->setupViewport( 0.0f, 0.0f, 1.0f, 1.0f );
}


// Called after our constructor is called
void SceneSample1::setup()
{
    // Set our virtual camera width to be 320
    // This means that the width of the view will be 320
    setCameraWidth( 320.0f );
    
    // Create a tile
    {
        CCTile3DButton *tile = new CCTile3DButton( this,            // Pass in this scene
                                                   "Hello World",   // Tell it to say 'Hello World'
                                                   64.0f,           // Specify the height of the text
                                                   true );          // Request the text to be centered

        // Set the colour of the text model to be white
        tile->textModel->setColour( CCColour() );

        // Add the tile to our list of touchable tiles, to allow for user interaction
        addTile( tile );
    }

//    {
//        CCTile3DButton *tile = new CCTile3DButton( this,                            // Pass in this scene
//                                                   320.0f,                          // Specify the width of the tile
//                                                   "Resources/background.png" );    // Texture to load
//        
//        // Add the tile to our list of touchable tiles, to allow for user interaction
//        addTile( tile );
//    }

    // refresh the scene range
    refreshCameraView();
}


void SceneSample1::destruct()
{
    super::destruct();
}


// CCSceneBase
void SceneSample1::updateScene(const CCGameTime &gameTime)
{
    // Called once a frame and internally updates all objects managed by this scene
    super::updateScene( gameTime );
}


void SceneSample1::updateCamera(const CCGameTime &gameTime)
{
    // Called once a frame and internally updates the camera view
    super::updateCamera( gameTime );
}


void SceneSample1::renderOctreeObject(CCSceneObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    // Called on render by our octree which handles drawing objects only in view
    if( camera == inCamera )
	{
        object->render( alpha );
	}
}


void SceneSample1::touchPressed(const CCScreenTouches &touch)
{
    // Callback for when a touch is first detected
}


const bool SceneSample1::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    // Callback for when a touch is moving
    return super::touchMoving( touch, touchDelta );
}


const bool SceneSample1::touchReleased(const CCScreenTouches &touch)
{
    // Callback for when a touch is released
    return super::touchReleased( touch );
}


const bool SceneSample1::touchCameraRotating(const float x, const float y)
{
    // Callback for when two touches are panning the camera to rotate
    return super::touchCameraRotating( x, y );
}
