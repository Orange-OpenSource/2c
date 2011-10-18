/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCTextureSprites.h
 * Description : Handles the loading of sprite pages.
 *
 * Created     : 09/06/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

struct CCSpriteInfo
{
    void setUVs(CCPrimitiveSquareUVs **uvs);
    
    CCText name;
    float x1, y1, x2, y2, width, height, aspectRatio;
};


struct CCSpritesPage
{ 
    ~CCSpritesPage()
    {
        sprites.deleteObjectsAndList();
    }
    
    void loadData(const CCResourceType resourceType);
    CCSpriteInfo* getSpriteInfo(const char *spriteName);
    void setUVs(CCPrimitiveSquareUVs **uvs, const char *spriteName);
    
    CCText name;
    int textureIndex;
    CCList<CCSpriteInfo> sprites;
};


struct CCTextureSprites
{
    ~CCTextureSprites()
    {
        pages.deleteObjectsAndList();
    }
    
    CCSpriteInfo* getSpriteInfo(const char *pageName, const CCResourceType resourceType, 
                                const char *spriteName);
    
    void setUVs(CCPrimitiveSquareUVs **uvs, 
                const char *pageName, const CCResourceType resourceType, 
                const char *spriteName);
    
protected:
    CCList<CCSpritesPage> pages;
};
