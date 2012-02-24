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
    camera->setCameraWidth( 320.0f );
    
    // Background
    {
        CCTile3DButton *tile = new CCTile3DButton( this );  // Pass in this scene
        tile->setupTextured( 640.0f,                        // Specify the width of the tile
                             "Resources/background.png" );  // Texture to load
        
        // Add the tile to our list of touchable tiles, to allow for user interaction
        addTile( tile );
    }
    
    // Create a tile
    CCTile3DButton *tileHelloWorld = NULL;
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setupText( "Hello World",                     // Tell it to say 'Hello World'
                         64.0f,                             // Specify the height of the text
                         true );                            // Request the text to be centered

        // Set the colour of the text model to be black
        tile->setTextColour( CCColour( 1.0f ), true );

        addTile( tile );
        
        tileHelloWorld = tile;
    }
    
    // Create a tile with an image
    {
        CCTile3DButton *tile = new CCTile3DButton( this );
        tile->setupTextured( 128.0f, "Resources/f7u12_derp.png" );
        
        addTile( tile );
        
        // Position it underneath our hello world tile
        tile->positionTileBelow( tileHelloWorld );
    }

    // refresh the scene range
    refreshCameraView();
}


void SceneSample1::destruct()
{
    super::destruct();
}


// CCSceneBase
const bool SceneSample1::updateScene(const CCTime &gameTime)
{
    // Called once a frame and internally updates all objects managed by this scene
    return super::updateScene( gameTime );
}


const bool SceneSample1::updateCamera(const CCTime &gameTime)
{
    // Called once a frame and internally updates the camera view
    return super::updateCamera( gameTime );
}


void SceneSample1::renderOctreeObject(CCSceneObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    // Called on render by our octree which handles drawing objects only in view
    if( camera == inCamera )
	{
        object->renderObject( camera, alpha );
	}
}


const bool SceneSample1::touchPressed(const CCScreenTouches &touch)
{
    // Callback for when a touch is first detected
    return super::touchPressed( touch );
}


const bool SceneSample1::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    // Callback for when a touch is moving
    return super::touchMoving( touch, touchDelta );
}


const bool SceneSample1::touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    // Callback for when a touch is released
    return super::touchReleased( touch, touchAction );
}


const bool SceneSample1::touchCameraRotating(const float x, const float y)
{
    // Callback for when two touches are panning the camera to rotate
    return super::touchCameraRotating( x, y );
}
