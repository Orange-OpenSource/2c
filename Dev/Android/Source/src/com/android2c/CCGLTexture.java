/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 Ð 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCGLTexture.java
 * Description : Interfaces with Bitmap class to load textures.
 *
 * Created     : 09/10/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

package com.android2c;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;

public class CCGLTexture
{
	static Bitmap bitmap = null;
	
	static public int load(final String filename, final boolean mipmap, final boolean packaged)
	{
		if( packaged )
		{
			int dotIndex = filename.lastIndexOf( '.' );
			
			final String pureFilename = ( dotIndex > 0 ) ? filename.substring( 0, dotIndex ) : filename;  
			int id = CCJNI.activity.getResources().getIdentifier( pureFilename, "drawable", "com.android2c" );
	
	        try 
	        {
	        	InputStream is = CCJNI.activity.getResources().openRawResource( id );
	            bitmap = BitmapFactory.decodeStream( is );
	    		is.close();
	        } 
	        catch(IOException e) 
	        {
	            // Ignore.
	        	e.printStackTrace();
	        }
		}
		
		// Cached
		else
		{
			final String filePath = CCJNI.dataPath + "/" + filename;
			
			try 
	        {
				FileInputStream is = new FileInputStream( filePath );
	            bitmap = BitmapFactory.decodeStream( is );
	    		is.close();
	        } 
	        catch(IOException e) 
	        {
	            // Ignore.
	        	e.printStackTrace();
	        }
		}
        
        if( bitmap != null )
        {
			int[] glName = new int[1];
			
			GL11 gl = CCGLViewJNI.glContext;
	        gl.glGenTextures( 1, glName, 0 );
	
	        gl.glBindTexture( GL10.GL_TEXTURE_2D, glName[0] );

	        gl.glTexParameteri( GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MAG_FILTER, GL11.GL_LINEAR );
	        if( false && mipmap )
	        {
	        	gl.glTexParameteri( GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MIN_FILTER, GL11.GL_LINEAR_MIPMAP_LINEAR );
		        gl.glTexParameteri( GL11.GL_TEXTURE_2D, GL11.GL_GENERATE_MIPMAP, GL11.GL_TRUE );
	        }
	        else
	        {
	        	gl.glTexParameteri( GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MIN_FILTER, GL11.GL_LINEAR );
	        }
	        
	        GLUtils.texImage2D( GL11.GL_TEXTURE_2D, 0, bitmap, 0 );
	        
	        gl.glBindTexture( GL10.GL_TEXTURE_2D, 0 );
	        
	        return glName[0];
        }
        
		return 0;
	}
	
	static public int getWidth()
	{
		return bitmap.getWidth();
	}
	
	static public int getHeight()
	{
		return bitmap.getHeight();
	}

    static public void releaseRawData()
	{
    	bitmap.recycle();
    	bitmap = null;
	}
}
