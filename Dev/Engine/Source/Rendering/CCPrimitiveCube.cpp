/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCPrimitiveCube.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCPrimitives.h"


CCPrimitiveCube::CCPrimitiveCube()
{
	vertices = (float*)malloc( sizeof( float ) * 8 * 3 );
}


// CCPrimitiveBase
void CCPrimitiveCube::renderVertices(const bool textured)
{	
	// Indices
	static const GLubyte faces[] = 
	{
		0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1,
	};
	static const uint numberOfFaces = sizeof( faces ) / sizeof( GLubyte );
	
	// draw the cube
    CCSetViewMatrix();
	GLVertexPointer( 3, GL_FLOAT, 0, vertices );
	glDrawElements( GL_TRIANGLE_STRIP, numberOfFaces, GL_UNSIGNED_BYTE, faces );
}


void CCPrimitiveCube::renderOutline()
{
	// Indices
	static const GLubyte faces[] = 
	{
		0, 1, 3, 2, 0, 4, 5, 1, 5, 7, 3, 7, 6, 2, 6, 4, 
	};
	static const uint numberOfFaces = sizeof( faces ) / sizeof( GLubyte );
	
    CCSetViewMatrix();
	GLVertexPointer( 3, GL_FLOAT, 0, vertices );
	glDrawElements( GL_LINE_STRIP, numberOfFaces, GL_UNSIGNED_BYTE, faces );
}


void CCPrimitiveCube::setupSquare(const float size)
{
	const float hSize = size * 0.5f;
	setupRectangle( size, -hSize, size, size );	
}


void CCPrimitiveCube::setupRectangle(const float width, const float height)
{
	setupRectangle( width, -height, height * 2.0f, width );
}


void CCPrimitiveCube::setupRectangle(const float depth, const float y, const float height, const float width)
{
	const float hWidth = width * 0.5f;
	setupTrapezoid( depth, y, height, -hWidth, hWidth, -hWidth, hWidth );
}


void CCPrimitiveCube::setupTrapezoid(const float depth, const float y, const float height, const float bL, const float bR, const float tL, const float tR)
{
	setupTrapezoidZ( depth, 0.0f, 0.0f, y, height, bL, bR, tL, tR );
}


void CCPrimitiveCube::setupTrapezoidZ(const float depth, const float bZ, const float tZ, const float y, const float height, const float bL, const float bR, const float tL, const float tR)
{	
	setup( depth, depth, bZ, tZ, y, height, bL, bR, tL, tR );
}


void CCPrimitiveCube::setupTrapezoidDepths(const float bottomDepth, const float topDepth, const float y, const float height, const float bL, const float bR, const float tL, const float tR)
{
	setup( bottomDepth, topDepth, 0.0f, 0.0f, y, height, bL, bR, tL, tR );
}


void CCPrimitiveCube::setup(const float bottomDepth, const float topDepth, const float bZ, const float tZ, const float y, const float height, const float bL, const float bR, const float tL, const float tR)
{	
	const float hBottomDepth = bottomDepth * 0.5f;
	const float hTopDepth = topDepth * 0.5f;
	float topFront = tZ - hTopDepth;
	float topBack = tZ + hTopDepth;
	float bottomFront = bZ - hBottomDepth;
	float bottomBack = bZ + hBottomDepth;
	
	float bottom = y;
	float top = y+height;
	
    vertices[0] = bL;			// 0
	vertices[1] = bottom;
	vertices[2] = bottomFront;
	
	vertices[3] = bR;			// 1
	vertices[4] = bottom;
	vertices[5] = bottomFront;
	
	vertices[6] = tL;			// 2
	vertices[7] = top;
	vertices[8] = topFront;
	
	vertices[9] = tR;			// 3
	vertices[10] = top;
	vertices[11] = topFront;
	
	// Back
    vertices[12] = bL;			// 4
	vertices[13] = bottom;
	vertices[14] = bottomBack;
	
	vertices[15] = bR;			// 5
	vertices[16] = bottom;
	vertices[17] = bottomBack;
	
	vertices[18] = tL;			// 6
	vertices[19] = top;
	vertices[20] = topBack;
	
	vertices[21] = tR;			// 7
	vertices[22] = top;
	vertices[23] = topBack;
}
