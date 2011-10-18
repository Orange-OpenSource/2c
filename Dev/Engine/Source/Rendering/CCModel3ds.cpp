/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCModel3ds.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCPrimitives.h"
#include "CCModel3ds.h"
#include "CCFileManager.h"
#include "CCTextureBase.h"


CCModel3ds::CCModel3ds(const char *file, 
                       const char *texture1, const CCResourceType resourceType, 
                       const bool alwaysResident, const bool mipmap, const char *texture2)
{
    primitive3ds = new CCPrimitive3ds();
    primitive3ds->load( file );
    primitive3ds->setTexture( texture1, resourceType, alwaysResident, mipmap );
    if( texture2 != NULL )
    {
        primitive3ds->textureInfo->secondaryIndex = gEngine->textureManager->assignTextureIndex( texture2, resourceType, alwaysResident, mipmap );
    }
    addPrimitive( primitive3ds );
}



// CCPrimitive3ds
CCPrimitive3ds::CCPrimitive3ds()
{
    modelUVs = NULL;
    textureUVs = NULL;
    
    width = height = depth = 0.0f;
}


void CCPrimitive3ds::destruct()
{
    if( modelUVs != NULL )
    {
        free( modelUVs );   
    }
    
    if( textureUVs != NULL )
    {
        free( textureUVs );   
    }
    
    super::destruct();
}


const bool CCPrimitive3ds::load(const char* file)
{
    CCText fullFilePath;
    CCFileManager::getFilePath( fullFilePath, file, Resource_Packaged );
    
    // Once we've copied out the data, we delete the 3dsobject
    obj3ds_type *object = new obj3ds_type();
	const int result = Load3DS( object, fullFilePath.buffer );
    ASSERT( result == 1 );
    
    // Extract the normals
    Calc3DSNormals( object );
    
    if( result == 1 )
    {
        vertexCount = object->polygons_qty * 3;
        
        modelUVs = (float*)malloc( sizeof( float ) * vertexCount * 2 );
        vertices = (float*)malloc( sizeof( float ) * vertexCount * 3 );
        normals = (float*)malloc( sizeof( float ) * vertexCount * 3 );
        
        for( int l_index = 0; l_index < object->polygons_qty; ++l_index )
        {
            const int uvIndex = l_index*3*2;
            const int vertexIndex = l_index*3*3;
            
            {
                const int index = object->polygon[l_index].a;
                ASSERT( index >= 0 );
                ASSERT( index < object->vertices_qty );
                
                // UVs
                {
                    const float u = object->mapcoord[index].u;
                    const float v = object->mapcoord[index].v;
                    modelUVs[uvIndex+0] = u;
                    modelUVs[uvIndex+1] = 1.0f - v;
                }
            
                // Vertices
                {
                    float x = object->vertex[index].x;
                    float y = object->vertex[index].y;
                    float z = object->vertex[index].z;
                    vertices[vertexIndex+0] = x;
                    vertices[vertexIndex+1] = y;
                    vertices[vertexIndex+2] = z;
                    
                    mmX.consider( x );
                    mmY.consider( y );
                    mmZ.consider( z );
                }
                
                // Normals
                {
                    float x = object->normal[index].x;
                    float y = object->normal[index].y;
                    float z = object->normal[index].z;
                    normals[vertexIndex+0] = x;
                    normals[vertexIndex+1] = y;
                    normals[vertexIndex+2] = z;
                }
            }
            
            {
                const int index = object->polygon[l_index].b;
                ASSERT( index >= 0 );
                ASSERT( index < object->vertices_qty );
                
                // UVs
                {
                    const float u = object->mapcoord[index].u;
                    const float v = object->mapcoord[index].v;
                    modelUVs[uvIndex+2] = u;
                    modelUVs[uvIndex+3] = 1.0f - v;
                }
                
                // Vertices
                {
                    float x = object->vertex[index].x;
                    float y = object->vertex[index].y;
                    float z = object->vertex[index].z;
                    vertices[vertexIndex+3] = x;
                    vertices[vertexIndex+4] = y;
                    vertices[vertexIndex+5] = z;

                    mmX.consider( x );
                    mmY.consider( y );
                    mmZ.consider( z );
                }
                
                // Normals
                {
                    float x = object->normal[index].x;
                    float y = object->normal[index].y;
                    float z = object->normal[index].z;
                    normals[vertexIndex+3] = x;
                    normals[vertexIndex+4] = y;
                    normals[vertexIndex+5] = z;
                }
            }
            
            {
                const int index = object->polygon[l_index].c;
                ASSERT( index >= 0 );
                ASSERT( index < object->vertices_qty );
                
                // UVs
                {
                    const float u = object->mapcoord[index].u;
                    const float v = object->mapcoord[index].v;
                    modelUVs[uvIndex+4] = u;
                    modelUVs[uvIndex+5] = 1.0f - v;
                }
                
                // Vertices
                {
                    float x = object->vertex[index].x;
                    float y = object->vertex[index].y;
                    float z = object->vertex[index].z;
                    vertices[vertexIndex+6] = x;
                    vertices[vertexIndex+7] = y;
                    vertices[vertexIndex+8] = z;

                    mmX.consider( x );
                    mmY.consider( y );
                    mmZ.consider( z );
                }
                
                // Normals
                {
                    float x = object->normal[index].x;
                    float y = object->normal[index].y;
                    float z = object->normal[index].z;
                    normals[vertexIndex+6] = x;
                    normals[vertexIndex+7] = y;
                    normals[vertexIndex+8] = z;
                }
            }
        }
        
        width = mmX.size();
        height = mmY.size();
        depth = mmZ.size();
        
        delete object;
        return true;
    }
    
    delete object;
    return false;
}


void CCPrimitive3ds::adjustTextureUVs()
{
    const int textureHandleIndex = textureInfo->primaryIndex;
    CCTextureHandle *textureHandle = gEngine->textureManager->getTextureHandle( textureHandleIndex );
    const CCTextureBase *texture = textureHandle->texture;
    ASSERT( texture != NULL );
    if( texture != NULL )
    {
        const float width = texture->getImageWidth();
        const float height = texture->getImageHeight();
        const float allocatedWidth = texture->getAllocatedWidth();
        const float allocatedHeight = texture->getAllocatedHeight();
        
        const float xScale = width / allocatedWidth;
        const float yScale = height / allocatedHeight;
        
        if( textureUVs == NULL )
        {
            textureUVs = (float*)malloc( sizeof( float ) * vertexCount * 2 );
        }
        
        for( uint i=0; i<vertexCount; ++i )
        {
            const int uvIndex = i*2;
            const int x = uvIndex+0;
            const int y = uvIndex+1;
            
            textureUVs[x] = modelUVs[x] * xScale;
            textureUVs[y] = modelUVs[y] * yScale;
        }
    }
}


void CCPrimitive3ds::renderVertices(const bool textured)
{
    CCSetViewMatrix();
	GLVertexPointer( 3, GL_FLOAT, 0, vertices );
    CCSetVertexAttribute( ATTRIB_NORMAL, 3, GL_FLOAT, 0, normals, true );
    CCSetTexCoords( textureUVs != NULL ? textureUVs : modelUVs );
	//SetNormals( normals );
	glDrawArrays( GL_TRIANGLES, 0, vertexCount );
}
