/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCModel3ds.h
 * Description : Loads and handles a 3ds model.
 *
 * Created     : 05/08/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCMODEL3DS_H__
#define __CCMODEL3DS_H__

#include "3dsloader.h"

class CCPrimitive3ds : public CCPrimitiveBase
{
public:
    typedef CCPrimitiveBase super;
    
    CCPrimitive3ds();
    virtual void destruct();
    
	const bool load(const char *file);
    
    // PrimitiveBase
protected:
    // Adjust the model's UVs to match the loaded texture, 
    // as non-square textures load into a square texture which means the mapping requires adjustment
    void adjustTextureUVs();
    
public:
	virtual void renderVertices(const bool textured);
    
    const float getWidth() { return width; }
    const float getHeight() { return height; }
    const float getDepth() { return depth; }
    
protected:
	uint vertexCount;
    float *modelUVs;
    float *textureUVs;
    
    float width, height, depth;
    CCMinMax mmX, mmY, mmZ;
};

class CCModel3ds : public CCModelBase
{
public:
    typedef CCModelBase super;

	CCModel3ds(const char *file, 
               const char *texture1, const CCResourceType resourceType=Resource_Packaged, 
               const bool alwaysResident=false, const bool mipmap=false, const char *texture2=NULL);
    
    const float getWidth() { return primitive3ds->getWidth(); }
    const float getHeight() { return primitive3ds->getHeight(); }
    const float getDepth() { return primitive3ds->getDepth(); }

public:
	CCPrimitive3ds *primitive3ds;
};

#endif // __CCMODEL3DS_H__
