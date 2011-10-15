/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCPrimitiveSphere.h
 * Description : Sphere drawable component.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCPRIMITIVESPHERE_H__
#define __CCPRIMITIVESPHERE_H__


class CCPrimitiveSphere : public CCPrimitiveBase
{
public:
	typedef CCPrimitiveBase super;

	// CCPrimitiveBase
	virtual void renderVertices(const bool textured);

	void setup(const float radius);
protected:
	uint vertexCount;
};


#endif // __CCPRIMITIVESPHERE_H__
