/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCXML.cpp
 *-----------------------------------------------------------
 */

#include "CCPlatform.h"
#include "CCXML.h"
#include "CCFileManager.h"


// Static variables to prevent pushing them on the stack
static char *s_Data		= NULL;
static char *s_End		= NULL;
static bool  s_Error	= false;


// XMLNode
XMLNode::XMLNode()
{
    m_Root		= NULL;
    m_Next		= NULL;
    m_Start		= NULL;
    m_BodySize	= 0;
    m_TagSize	= 0;
}


XMLNode::~XMLNode()
{
    delete m_Next;
    delete m_Root;
}


void XMLNode::parse()
{
    XMLNode *tail = NULL;
    bool insideString = false;
    bool insideComment = false;
    while( ( s_Data < s_End ) && ( !s_Error ) )
    {
        if( insideComment )
        {
            // We are inside a comment still... anything goes, we just ignore it
            if( ( s_Data[0]=='-' ) && ( s_Data[1] == '-' ) && ( s_Data[2] == '>' ) )
            {
                insideComment = false;
                s_Data += 2;
            }
        }
        else if( ( s_Data[0] == '<' ) && ( !insideString ) )
        {
            // Found start of a tag
            if( m_Start )
            {
                // We already have a start position, this could be our end, or another new node
                if( s_Data[1]=='/' )
                {
                    // An end tag of some sort... ours?
                    bool ourEnd = true;
                    int ignore = 1;
                    for( uint i=0; ( i<m_TagSize ) && ( ourEnd ); ++i )
                    {
                        if( ( ( m_Start[i] == ' ' ) || ( m_Start[i] == 0 ) ) && ( ( s_Data[i+2] == ' ' ) || ( s_Data[i+2] == '>' ) ) )
                        {
                             break;
                        }

                        if( m_Start[i] != s_Data[i+2] )
                        {
                            ourEnd = false;
                        }
                        ++ignore;
                    }
                    if( !ourEnd )
                    {
                        // This was an ending tag, but not ours!?
                        printf("XMLNode::Parse(...) : ERROR! Found end tag that didn't match our starting tag!\n\nOpen  = <%s>\nClose = %s\n\n",m_Start,s_Data);
                        s_Error = true;
                        return;
                    }
                    m_BodySize = (uint)(s_Data - m_Start);
                    s_Data += ignore;
                }
                else if( s_Data[1] == '!' )
                {
                    // Comment... By saying we are inside comment, we ignore everything until a matching --> is found
                    insideComment = true;
                }
                else
                {
                    // Not an end tag, start of a new child node then
                    XMLNode* node = new XMLNode();
                    if( tail == NULL )
                    {
                        m_Root = node;
                        tail = node;
                    }
                    else
                    {
                        tail->next( node );
                        tail = node;
                    }
                    tail->parse();
                }
            }
            else
            {
                    if( s_Data[1] == '/' )
                    {
                            // This is an ending tag, but we never even started?
                            printf("XMLNode::Parse(...) : ERROR! Found end tag without a starting tag!\n\n--> %s\n",s_Data);
                            s_Error = true;
                            return;
                    }
                    else if( s_Data[1] == '!' )
                    {
                            // Comment... By saying we are inside comment, we ignore everything until a matching --> is found
                            insideComment = true;
                    }
                    else
                    {
                            // Found start tag for this node
                            m_Start = &s_Data[1];
                    }
            }
        }
        else if( ( s_Data[0] == '>' ) && ( !insideString ) )
        {
            // Found end of a tag
            if( m_BodySize )
            {
                // We had a body, and found our end tag... so this must be the end?
                ++s_Data;
                return;
            }
            else if( m_Start )
            {
                // We had a start, so this is probably the end of our start tag - body follows... unless previous character was /
                if( m_TagSize )
                {
                    printf("XMLNode::Parse(...) : ERROR! Already finished parsing the tag!\n\n--> %s\n",s_Data);
                    s_Error = true;
                    return;
                }
                m_TagSize = (uint)( s_Data - m_Start );
                if( m_TagSize == 0 )
                {
                    printf("XMLNode::Parse(...) : ERROR! Empty tag!\n\n--> %s\n",m_Start);
                    s_Error = true;
                    return;
                }

                formatTag();

                // Check for an immediate terminator...
                if( *(s_Data-1) == '/' )
                {
                    s_Data++;
                    return;
                }
            }
            else
            {
                // We didn't even start a tag? This is an error
                printf("XMLNode::Parse(...) : ERROR! Found '>' that didn't belong!\n\n--> %s\n",s_Data);
                s_Error = true;
                return;
            }
        }
        else if( ( s_Data[0] == '\\' ) && ( s_Data[1] == '\"' ) )
        {
            // Quote inside string, potentially ignore it
            if( !insideString )
            {
                printf("XMLNode::Parse(...) : ERROR! Found invalid syntax inside tag but not in string!\n\n--> %s\n",s_Data);
                s_Error = true;
                return;
            }
        }
        else if( *s_Data == '\"' )
        {
            // Quote
            insideString = !insideString;
        }

        // Next character
        ++s_Data;
    }
}


XMLNode* XMLNode::getRoot()
{ 
    return m_Root;
}


void XMLNode::next(XMLNode* node)
{
    m_Next = node;
}


XMLNode* XMLNode::next()
{ 
    return m_Next;
}


const bool XMLNode::tagIs(const char *s)
{ 
    return strcmp( tag(), s ) == 0;
}


const bool XMLNode::tagContains(const char *s)
{
    return strstr( tag(), s ) != NULL;
}


const char* XMLNode::tag()
{
    ASSERT_MESSAGE( m_Start != NULL, "XMLNode not valid" );
    m_Start[m_TagSize] = 0;
    return m_Start;
}


const char* XMLNode::body()
{
    ASSERT_MESSAGE( m_Start != NULL , "XMLNode not valid" );
    m_Start[m_BodySize] = 0;
    return &m_Start[m_TagSize+1];
}


const char* XMLNode::attribute(const char *name, const char *defaultValue, const bool warning)
{
    char buf[128] = {0};
    strcpy( buf, name );
    strcat( buf, "=\"" );
    for( uint i=0; i<m_TagSize; ++i )
    {
        if( m_Start[i] == 0 )
        {
            char* s = strstr(&m_Start[i+1],buf);
            if( s == &m_Start[i+1] )
            {
                 return &m_Start[i+strlen(buf)+1];
            }
        }
    }
    if( warning )
    {
        printf( "XMLNode::Attribute(\"%s\") : WARNING! Attribute doesn't exist\n", name );
    }
    return defaultValue;
}


const int XMLNode::attributeInt(const char *name, const int defaultValue, const bool warning)
{
    const char *a = attribute( name, NULL, false );
    if( a == NULL )
    {
        if( warning )
        {
            printf( "XMLNode::AttributeInt(...) : WARNING! Attribute \"%s\" is missing\n", name );
        }
        return defaultValue;
    }
    return atoi( a );
}


const float XMLNode::attributeFloat(const char *name, const float defaultValue, const bool warning)
{
    const char *a = attribute( name, NULL, false );
    if( a == NULL )
    {
        if( warning )
        {
            printf( "XMLNode::AttributeFloat(...) : WARNING! Attribute \"%s\" is missing\n", name );
        }
        return defaultValue;
    }
    return (float)atof( a );
}


const bool XMLNode::attributeBool(const char *name, const bool defaultValue, const bool warning)
{
    const char *a = attribute( name, NULL, false );
    if( a == NULL )
    {
        if( warning )
        {
            printf( "XMLNode::AttributeBool(...) : WARNING! Attribute \"%s\" is missing\n", name );
        }
        return defaultValue;
    }
    return ( strcmp( a, "true" ) == 0 );
}


const bool XMLNode::attributeBool(const char *name, bool *value)
{
    const char *a = attribute( name, NULL, false );
    if( a == NULL )
    {
        return false;
    }

    *value = ( strcmp( a, "true" ) == 0 );

    return true;
}


const bool XMLNode::attributeIs(const char *name, const char *value)
{
    const char *a = attribute( name, NULL, false );
    if( a == NULL )
    {
        printf( "XMLNode::AttributeIs(...) : WARNING! Attribute \"%s\" is missing\n", name );
        return false;
    }
    return ( strcmp( a, value ) == 0 );
}


void XMLNode::replaceFloat(const char *name, float &value)
{
    const char *a = attribute( name, NULL, false );
    if( a != NULL )
    {
        value = (float)atof( a );
    }
}


void XMLNode::formatTag()
{
    // The tag is made up of a tag name and any params, separate them all with NULL characters
    bool inString = false;
    for( uint i=0; i<m_TagSize; ++i )
    {
        if ((m_Start[i]=='\\')&&(inString))         { ++i; }
        else if ((m_Start[i]=='\"')&&(inString))    { m_Start[i] = 0; inString = false; }
        else if ((m_Start[i]=='\"'))                { inString = true; }
        else if ((m_Start[i]==' ')&&(!inString))    { m_Start[i] = 0; }
    }
}


// XMLDocument
XMLDocument::XMLDocument()
{
    m_Root = NULL;
}


XMLDocument::~XMLDocument()
{
    delete m_Root;
    if( m_data != NULL )
	{
        //free( m_data );
		m_data = NULL;
	}
}


const bool XMLDocument::load(const char *filename)
{
    const uint fileSize = CCFileManager::getPackagedFile( filename, &m_data );
    
    // Search and remove all new line, tab and return characters
    for( uint i=0; i<fileSize; ++i )
    {
        if( ( m_data[i] == 26 ) || ( m_data[i] == EOF ) || ( m_data[i] == '\e' ) )
        {
            m_data[i] = 0;
        }
        else if( ( m_data[i] == '\n' ) || ( m_data[i] == '\t' ) || ( m_data[i] == '\r' ) || ( m_data[i] == '\f' ) )
        {
            m_data[i] = ' ';
        }
    }

    // At this point we have a nice easy to parse chunk of data that is NULL terminated
    s_Data  = m_data;
    s_End   = &m_data[fileSize];
    s_Error = false;

    m_Root = new XMLNode();
    m_Root->parse();

    return !s_Error;
}


const bool XMLDocument::loadData(const char *inData, const uint size)
{
	m_data = (char*)malloc( size );
	memcpy( m_data, inData, size );
	
	// Search and remove all new line, tab and return characters
	for( uint i=0; i<size; ++i )
	{
		if( ( m_data[i] == 26 ) || ( m_data[i] == EOF ) || ( m_data[i] == '\e' ) )
		{
			m_data[i] = 0;
		}
		else if( ( m_data[i] == '\n' ) || ( m_data[i] == '\t' ) || ( m_data[i] == '\r' ) || ( m_data[i] == '\f' ) )
		{
			m_data[i] = ' ';
		}
	}
	
	// At this point we have a nice easy to parse chunk of data that is NULL terminated
	s_Data	= m_data;
	s_End   = &m_data[size];
	s_Error	= false;
	
	m_Root = new XMLNode();
	m_Root->parse();
	
	return !s_Error;
}


XMLNode* XMLDocument::loadRootNode(const char *filename)
{
    // Get root node
    if( load( filename ) )
    {
        XMLNode *root = getRoot();
        ASSERT_MESSAGE( root != NULL, "XMLDocument::loadSpritePage(...) : ERROR! Missing XML root node" );

        // Parse xml data
        root = root->getRoot();
        return root;
    }

    return NULL;
}


XMLNode* XMLDocument::getRoot()
{
    return m_Root;
}
