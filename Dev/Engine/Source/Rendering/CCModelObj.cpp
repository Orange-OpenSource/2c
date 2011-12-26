/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCModelObj.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCPrimitives.h"
#include "CCModelObj.h"
#include "CCFileManager.h"
#include "CCTextureBase.h"

#include "ObjLoader.h"


CCModelObj::CCModelObj(const char *file, 
                       const char *texture1, const CCResourceType resourceType, 
                       const bool alwaysResident, const bool mipmap, const char *texture2)
{
    primitive = new CCPrimitiveObj();
    primitive->load( file );
    primitive->setTexture( texture1, resourceType, alwaysResident, mipmap );
    if( texture2 != NULL )
    {
        primitive->textureInfo->secondaryIndex = gEngine->textureManager->assignTextureIndex( texture2, resourceType, alwaysResident, mipmap );
    }
    addPrimitive( primitive );
}



// CCPrimitiveObj
CCPrimitiveObj::CCPrimitiveObj()
{
    vertexCount = 0;
    
    modelUVs = NULL;
    textureUVs = NULL;
    
    width = height = depth = 0.0f;
}


void CCPrimitiveObj::destruct()
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


const bool CCPrimitiveObj::load(const char* file)
{
    CCText fullFilePath;
    CCFileManager::getFilePath( fullFilePath, file, Resource_Packaged );
    
    /* attempt to load the file */
    ObjMesh *objMesh = LoadOBJ( fullFilePath.buffer );
    
    {
        for( uint i=0; i<objMesh->m_iNumberOfFaces; ++i )
        {   
            ObjFace *pf = &objMesh->m_aFaces[i];
            if( pf->m_iVertexCount >= 3 )
            {
                uint faceVertexCount = pf->m_iVertexCount;
                do
                {
                    vertexCount += 3;
                    faceVertexCount--;
                } while( faceVertexCount >= 3 );
            }
        }
        
        modelUVs = (float*)malloc( sizeof( float ) * vertexCount * 2 );
        vertices = (float*)malloc( sizeof( float ) * vertexCount * 3 );
        normals = (float*)malloc( sizeof( float ) * vertexCount * 3 );
        
        int uvIndex = 0;
        int vertexIndex = 0;
        for( uint i=0; i<objMesh->m_iNumberOfFaces; ++i )
        {   
            ObjFace *pf = &objMesh->m_aFaces[i];
            if( pf->m_iVertexCount < 3 )
            {
                continue;
            }
            
            uint vertexStartIterator = 1;
            uint vertexEndIterator = 3;
            do
            {
                // Convert GL_POLYGON to GL_TRIANGLES by reusing the first vert, with the others
                {
                    uint j=0;
                    // UVs
                    {
                        ObjTexCoord &texCoord = objMesh->m_aTexCoordArray[ pf->m_aTexCoordIndicies[j] ];
                        modelUVs[uvIndex+0] = texCoord.u;
                        modelUVs[uvIndex+1] = 1.0f - texCoord.v;
                    }
                    
                    // Vertices
                    {
                        ObjVertex &vertex = objMesh->m_aVertexArray[ pf->m_aVertexIndices[j] ];
                        vertices[vertexIndex+0] = vertex.x;
                        vertices[vertexIndex+1] = vertex.y;
                        vertices[vertexIndex+2] = vertex.z;
                        
                        mmX.consider( vertex.x );
                        mmY.consider( vertex.y );
                        mmZ.consider( vertex.z );
                    }
                    
                    // Normals
                    {
                        ObjNormal &normal = objMesh->m_aNormalArray[ pf->m_aNormalIndices[j] ];
                        normals[vertexIndex+0] = normal.x;
                        normals[vertexIndex+1] = normal.y;
                        normals[vertexIndex+2] = normal.z;
                    }
                    
                    uvIndex += 2;
                    vertexIndex += 3;
                }
                
                for( uint j=vertexStartIterator; j<vertexEndIterator; ++j )
                {
                    // UVs
                    {
                        ObjTexCoord &texCoord = objMesh->m_aTexCoordArray[ pf->m_aTexCoordIndicies[j] ];
                        modelUVs[uvIndex+0] = texCoord.u;
                        modelUVs[uvIndex+1] = 1.0f - texCoord.v;
                    }
                    
                    // Vertices
                    {
                        ObjVertex &vertex = objMesh->m_aVertexArray[ pf->m_aVertexIndices[j] ];
                        vertices[vertexIndex+0] = vertex.x;
                        vertices[vertexIndex+1] = vertex.y;
                        vertices[vertexIndex+2] = vertex.z;
                        
                        mmX.consider( vertex.x );
                        mmY.consider( vertex.y );
                        mmZ.consider( vertex.z );
                    }
                    
                    // Normals
                    {
                        ObjNormal &normal = objMesh->m_aNormalArray[ pf->m_aNormalIndices[j] ];
                        normals[vertexIndex+0] = normal.x;
                        normals[vertexIndex+1] = normal.y;
                        normals[vertexIndex+2] = normal.z;
                    }
                    
                    uvIndex += 2;
                    vertexIndex += 3;
                }
                
                vertexStartIterator++;
                vertexEndIterator++;
                
            } while( vertexEndIterator <= pf->m_iVertexCount );
        }
        
        width = mmX.size();
        height = mmY.size();
        depth = mmZ.size();
    }
    
    DeleteOBJ( objMesh->m_iMeshID );
    return vertexCount > 0;
}


void CCPrimitiveObj::adjustTextureUVs()
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


void CCPrimitiveObj::renderVertices(const bool textured)
{
    CCSetViewMatrix();
	GLVertexPointer( 3, GL_FLOAT, 0, vertices );
    CCSetVertexAttribute( ATTRIB_NORMAL, 3, GL_FLOAT, 0, normals, true );
    CCSetTexCoords( textureUVs != NULL ? textureUVs : modelUVs );

	glDrawArrays( GL_TRIANGLES, 0, vertexCount );
}
