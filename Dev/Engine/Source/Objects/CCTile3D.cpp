/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCTile3D.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCTextureFontPage.h"
#include "CCControllerMovement.h"


CCTile3D::CCTile3D()
{
    movementController = NULL;
}

// CCRenderable
void CCTile3D::dirtyModelMatrix()
{
    super::dirtyModelMatrix();
    for( int i=0; i<attachments.length; ++i )
    {
        CCSceneObject *attachment = attachments.list[i];
        attachment->dirtyModelMatrix();
    }
}


void CCTile3D::setPositionXYZ(const float x, const float y, const float z)
{
    if( position.x != x || position.y != y || position.z != z )
    {
        CCVector3 distance = position;
        super::setPositionXYZ( x, y, z );
        distance.sub( position );

        for( int i=0; i<attachments.length; ++i )
        {
            CCSceneObject *attachment = attachments.list[i];
            attachment->translate( -distance.x, -distance.y, -distance.z );
        }

        if( movementController != NULL )
        {
            movementController->setTarget( position );
        }

        CCOctreeRefreshObject( this );
    }
}


void CCTile3D::translate(const float x, const float y, const float z)
{
    super::translate( x, y, z );

    for( int i=0; i<attachments.length; ++i )
    {
        CCSceneObject *attachment = attachments.list[i];
        attachment->translate( x, y, z );
    }

    CCOctreeRefreshObject( this );
}