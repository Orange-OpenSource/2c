/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCXML.h
 * Description : A single branch of XML including begin and end tags.
 *               Buggy. Use JSON instead if possible.
 *
 * Created     : 09/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCXML_H__
#define __CCXML_H__


class XMLNode
{
public:
	XMLNode();
	~XMLNode();
	
	void parse();
	XMLNode* getRoot();
	void next(XMLNode* node);
	XMLNode* next();
	const bool tagIs(const char *s);
	const bool tagContains(const char *s);
	const char* tag();
	const char* body();
    const char* attribute(const char *name, const char *defaultValue, const bool warning=false);
    const int attributeInt(const char *name, const int defaultValue, const bool warning=false);
    const float attributeFloat(const char *name, const float defaultValue=0.0f, const bool warning=false);
    const bool attributeBool(const char *name, const bool defaultValue, const bool warning=false);
	const bool attributeBool(const char *name, bool *value);
	const bool attributeIs(const char *name, const char *value);
    
    void replaceFloat(const char *name, float &value);
	
protected:
	void formatTag();
	
    XMLNode*	m_Root;         // Children of this node
    XMLNode*	m_Next;         // Another node along my level
    char*	m_Start;        // Start of this node
    uint		m_TagSize;      // How big is our start tag (including attributes)
    uint		m_BodySize;     // How big is our body
};


// XMLDocument handles the file and getting the data read to parse
class XMLDocument
{
public:
	XMLDocument();
	~XMLDocument();
	
    const bool load(const char *filename);
    const bool loadData(const char *inData, const uint size);
	
	// Start the loading of a sprite descriptor file
    XMLNode* loadRootNode(const char *filename);

    XMLNode* getRoot();
	
protected:
	XMLNode* m_Root;
	char *m_data;
};


#endif // __CCXML_H__
