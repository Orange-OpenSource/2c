/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCTexturePNG.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCTexturePNG.h"
#include "CCTextureManager.h"
#include "CCDeviceFileManager.h"

#include "CCGLView.h"


CCTexturePNG::CCTexturePNG()
{
}


CCTexturePNG::~CCTexturePNG()
{
}


static int jniLoad(const char *name, const bool generateMipMap, const bool packaged)
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;
	jobject jObj = gView->jniObj;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetMethodID( jniClass, "textureLoad", "(Ljava/lang/String;ZZ)I" );
	ASSERT( mid != 0 );

	// Call the function
	jstring jFilename = jEnv->NewStringUTF( name );
	return jEnv->CallIntMethod( jObj, mid, jFilename, generateMipMap, packaged );
}


static int jniGetWidth()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;
	jobject jObj = gView->jniObj;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetMethodID( jniClass, "textureGetWidth", "()I" );
	ASSERT( mid != 0 );

	// Call the function
	return jEnv->CallIntMethod( jObj, mid );
}


static int jniGetHeight()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;
	jobject jObj = gView->jniObj;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetMethodID( jniClass, "textureGetHeight", "()I" );
	ASSERT( mid != 0 );

	// Call the function
	return jEnv->CallIntMethod( jObj, mid );
}


static void jniReleaseRawData()
{
	// JNI Java call
	JNIEnv *jEnv = gView->jniEnv;
	jobject jObj = gView->jniObj;

	jclass jniClass = jEnv->FindClass( "com/android2c/CCJNI" );
	ASSERT_MESSAGE( jniClass != 0, "Could not find Java class." );

	static jmethodID mid = jEnv->GetMethodID( jniClass, "textureReleaseRawData", "()V" );
	ASSERT( mid != 0 );

	// Call the function
	jEnv->CallVoidMethod( jObj, mid );
}


const bool CCTexturePNG::load(const char *name, const CCResourceType resourceType, const bool generateMipMap)
{
#if defined PROFILEON
    CCProfiler profile( "CCTexturePNG::load()" );
#endif

	CCText filename = name;
	filename.stripDirectory();
	filename.toLowercase();

	glName = jniLoad( filename.buffer, generateMipMap, resourceType == Resource_Packaged );
	if( glName > 0 )
	{
		allocatedWidth = imageWidth = jniGetWidth();
		allocatedHeight = imageHeight = jniGetHeight();
		jniReleaseRawData();
		return true;
	}
	return false;
}


const bool CCTexturePNG::createGLTexture()
{
	return true;
}


// Prefer to load textures via Java, since it handles non-square textures and more file types
//#include "../libpng/png.h"
//static zip_file *zipFile = NULL;
//void png_zip_read(png_structp png, png_bytep data, png_size_t size)
//{
//	zip_fread( zipFile, data, size );
//}
//
//
//const bool CCTexturePNG::load(const char *name, const bool generateMipMap, const CCResourceType resourceType)
//{
//	zipFile = NULL;
//
//	// Grab the APK (zip) file for resources
//	zip *apkArchive = zip_open( CCDeviceFileManager::apkPath.buffer, 0, NULL );
//	if( apkArchive == NULL )
//	{
//		DEBUGLOG( "Error loading APK" );
//		return false;
//	}
//
//	// Get file path
//	CCText filename( name );
//	filename.stripDirectory();
//	filename.toLowercase();
//
//	fullFilePath += "/";
//	fullFilePath += filename.buffer;
//
//	char filepath[100];
//	sprintf( filepath, "%s%s", "res/drawable/", filename.buffer );
//
//	zipFile = zip_fopen( apkArchive, filepath, 0 );
//	if( !zipFile )
//	{
//		DEBUGLOG( "Error opening file: %s.png", name );
//		return false;
//	}
//
//	// Test that the file loaded is a png
//	png_byte header[8];
//	zip_fread( zipFile, header, 8 );
//
//	int isPNG = !png_sig_cmp( header, 0, 8 );
//	if( !isPNG )
//	{
//		zip_fclose( zipFile );
//		DEBUGLOG( "File %s is not a PNG.", name );
//		return false;
//	}
//
//	png_structp png_pointer = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
//	if( !png_pointer )
//	{
//		zip_fclose( zipFile );
//		DEBUGLOG( "Failed to create PNG struct for file: %s", name );
//		return false;
//	}
//
//	png_infop info_pointer = png_create_info_struct( png_pointer );
//	if( !info_pointer )
//	{
//		png_destroy_read_struct( &png_pointer, NULL, NULL );
//		DEBUGLOG( "Failed to create PNG info for file: %s", name );
//		zip_fclose( zipFile );
//		return false;
//	}
//
//	png_infop end_pointer = png_create_info_struct( png_pointer );
//	if( !end_pointer )
//	{
//		png_destroy_read_struct( &png_pointer, &info_pointer, NULL );
//		DEBUGLOG( "Failed to create PNG end info for file: %s", name );
//		zip_fclose( zipFile );
//		return false;
//	}
//
//	if( setjmp( png_jmpbuf( png_pointer) ) )
//	{
//		png_destroy_read_struct( &png_pointer, &info_pointer, &end_pointer );
//		DEBUGLOG( "Failed during jmpbuf for file: %s", name );
//		zip_fclose( zipFile );
//		return false;
//	}
//
//	// All file initialisation is done - start read initialisation
//	png_set_read_fn( png_pointer, NULL, png_zip_read );
//
//	// We've already read in the header
//	png_set_sig_bytes( png_pointer, 8 );
//	png_read_info( png_pointer, info_pointer );
//
//	// Get the image data
//	int colour_type;
//	allocatedWidth = imageWidth = png_get_image_width( png_pointer, info_pointer );
//	allocatedHeight = imageHeight = png_get_image_height( png_pointer, info_pointer );
//	colour_type = png_get_color_type( png_pointer, info_pointer );
//
//	// Determine the format of the image and adjust accordingly
//	GLint format;
//	if( colour_type == PNG_COLOR_TYPE_RGB_ALPHA )
//	{
//		format = GL_RGBA;
//	}
//	else if( colour_type == PNG_COLOR_TYPE_PALETTE )
//	{
//		// Convert a paletted image to an RGBA one
//		png_set_palette_to_rgb( png_pointer );
//		if ( png_get_valid( png_pointer, info_pointer, PNG_INFO_tRNS ) )
//		{
//			png_set_tRNS_to_alpha( png_pointer );
//		}
//		format = GL_RGBA;
//	}
//	else if( colour_type == PNG_COLOR_TYPE_GRAY )
//	{
//		png_set_gray_to_rgb( png_pointer );
//		format = GL_RGB;
//	}
//	else if( colour_type == PNG_COLOR_TYPE_GRAY_ALPHA )
//	{
//        png_set_gray_to_rgb( png_pointer );
//        format = GL_RGBA;
//	}
//	else
//	{
//		format = GL_RGB;
//	}
//
//	png_read_update_info( png_pointer, info_pointer );
//
//	// Get the row size in bytes so we can allocate enough space
//	int rowbytes = png_get_rowbytes( png_pointer, info_pointer );
//
//	png_byte *image_data = new png_byte[rowbytes * allocatedHeight];
//	if( !image_data )
//	{
//		png_destroy_read_struct( &png_pointer, &info_pointer, &end_pointer );
//		DEBUGLOG( "Failed to create image data array for file: %s", name );
//		zip_fclose( zipFile );
//		return false;
//	}
//
//	// Give libpng a pointer to each row for it to read the image data
//	png_bytep *row_pointers = new png_bytep[allocatedHeight];
//	if( !row_pointers )
//	{
//		png_destroy_read_struct( &png_pointer, &info_pointer, &end_pointer );
//		delete[] image_data;
//		DEBUGLOG( "Failed to create row pointers for file: %s", name );
//		zip_fclose( zipFile );
//		return false;
//	}
//
//	// And set those pointers correctly
//	for( int i = 0; i < allocatedHeight; ++i )
//	{
//		 row_pointers[i] = image_data + ( i * rowbytes );
//	}
//
//	// Read in the actual image data
//	png_read_image( png_pointer, row_pointers );
//
//	// We're done with the file - close it
//	zip_fclose( zipFile );
//
//    DEBUG_OPENGL();
//
//	// OpenGL texture stuff
//	glGenTextures( 1, &glName );
//	gEngine->textureManager->bindTexture( glName );
//    DEBUG_OPENGL();
//
//	glTexImage2D( GL_TEXTURE_2D, 0, format, allocatedWidth, allocatedHeight, 0, format, GL_UNSIGNED_BYTE, image_data );
//    DEBUG_OPENGL();
//
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//
//	gEngine->textureManager->bindTexture( 0 );
//    DEBUG_OPENGL();
//
//	// Tidy up
//	png_destroy_read_struct( &png_pointer, &info_pointer, &end_pointer );
//	delete[] image_data;
//
//	return true;
//}
