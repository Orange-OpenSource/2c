/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCSceneObjectSkybox.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"


CCSceneObjectSkybox::CCSceneObjectSkybox()
{
	interpolating = false;
}


void CCSceneObjectSkybox::setup(const float size, const char *name)
{
    char textBuffer[6][128];
    
    sprintf( textBuffer[0], "%s_top.png", name );
    sprintf( textBuffer[1], "%s_bottom.png", name );
    sprintf( textBuffer[2], "%s_east.png", name );
    sprintf( textBuffer[3], "%s_west.png", name );
    sprintf( textBuffer[4], "%s_north.png", name );
    sprintf( textBuffer[5], "%s_south.png", name );

    setup( size, textBuffer[0], textBuffer[1], textBuffer[2], textBuffer[3], textBuffer[4], textBuffer[5] );
}


void CCSceneObjectSkybox::setup(const float size, 
                                const char *top, 
                                const char *bottom, 
                                const char *east, 
                                const char *west, 
                                const char *north,
                                const char *south)
{
	const float hSize = size * 0.5f;
	
	renderPass = render_background;
	model = new CCModelBase();
	model->setColour( CCColour() );
	
	CCPrimitiveSquare *squarePrimitive;
	
	// Top
	squarePrimitive = new CCPrimitiveSquare();
	squarePrimitive->setupYFacing( hSize, hSize, hSize, true, -1.0f );
	model->addPrimitive( squarePrimitive );
	squarePrimitive->setTexture( top, Resource_Packaged );
	
	// Bottom
	squarePrimitive = new CCPrimitiveSquare();
	squarePrimitive->setupYFacing( hSize, -hSize, hSize, true, 1.0f );
	model->addPrimitive( squarePrimitive );
	squarePrimitive->setTexture( bottom, Resource_Packaged );
	
	// East
	squarePrimitive = new CCPrimitiveSquare();
	squarePrimitive->setupXFacing( hSize, hSize, hSize );
	model->addPrimitive( squarePrimitive );
	squarePrimitive->setTexture( east, Resource_Packaged );

	// West
	squarePrimitive = new CCPrimitiveSquare();
	squarePrimitive->setupXFacing( -hSize, hSize, hSize );
	model->addPrimitive( squarePrimitive );
	squarePrimitive->setTexture( west, Resource_Packaged );
	
	// North
	squarePrimitive = new CCPrimitiveSquare();
	squarePrimitive->setup( -hSize, -hSize, -hSize, hSize, -hSize, hSize, -hSize, hSize );
	model->addPrimitive( squarePrimitive );
	squarePrimitive->setTexture( north, Resource_Packaged );
	
	// South
	squarePrimitive = new CCPrimitiveSquare();
	squarePrimitive->setup( hSize, hSize, -hSize, hSize, -hSize, hSize, -hSize, hSize );
	model->addPrimitive( squarePrimitive );
	squarePrimitive->setTexture( south, Resource_Packaged );
}


const bool CCSceneObjectSkybox::update(const CCTime &time)
{
	if( interpolating )
	{
        bool updated = false;
        
		const float speed = time.delta * 0.5f;
        if( scaleInterpolator.update( speed ) )
        {
            updated = updateModelMatrix = true;
        }
		
		updated |= CCToTarget( model->getColour()->alpha, 1.0f, time.delta );
		
		if( updated == false )
		{
			interpolating = false;
			DELETE_POINTER( scale );
			setTransparent( false );
		}
	}
    
    return false;
}


void CCSceneObjectSkybox::render(const bool alpha)
{
	if( alpha == transparent )
	{
        glDisable( GL_CULL_FACE );
		glDisable( GL_DEPTH_TEST );
		super::render( alpha );
		glEnable( GL_DEPTH_TEST );
        glEnable( GL_CULL_FACE );
	}
}


void CCSceneObjectSkybox::animateScale(const float startScale)
{
    interpolating = true;
	CCVector3FillPtr( &scale, startScale );
    scaleInterpolator.setup( scale, 1.0f );
	model->getColour()->alpha = 0.0f;
	setTransparent( true );
}
