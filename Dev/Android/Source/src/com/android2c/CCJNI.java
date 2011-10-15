/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 Ð 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCJNI.java
 * Description : Interfaces with ndk library.
 *
 * Created     : 09/10/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

package com.android2c;


// Wrapper for native library
public class CCJNI
{
	static final String library = "ccjni";
	static final boolean debug = false;
	
	static CCActivity activity;
	static String dataPath;

    public static void setActivity(CCActivity inActivity)
    {
    	CCJNI.activity = inActivity;
    	dataPath = activity.getFilesDir().getAbsolutePath();
    	fileManagerSetPaths( activity.getPackageResourcePath(), dataPath );
    }
	
	// Load our library
	static 
 	{
 		System.loadLibrary( CCJNI.library );
 		System.out.println();
 	}

	
 	// main
	public static native void onSurfaceChanged(int width, int height);
	public static native void onDrawFrame();
	
	// CCGLTexture
	private int textureLoad(final String filename, final boolean mipmap, final boolean packaged)
	{
		return CCGLTexture.load( filename, mipmap, packaged );
	}
	
	private int textureGetWidth()
	{
		return CCGLTexture.getWidth();
	}
	
	private int textureGetHeight()
	{
		return CCGLTexture.getHeight();
	}
	
	private void textureReleaseRawData()
	{
		CCGLTexture.releaseRawData();
	}

	
	// CCDeviceControls
	public static native void controlsHandleTouch(float x, float y, int actionType, int finger);
	
	
	// CCDeviceFileManager
	public static native void fileManagerSetPaths(String apkPath, String dataPath);
	
	
	// CCDeviceURLManager
	static CCURLManagerJNI urlManager = null;
 
	private void urlManagerProcessRequest(final String url)
	{
		new Thread(new Runnable()
		{
			public void run() 
		    {
			 	if( urlManager == null )
			 	{
				 	urlManager = new CCURLManagerJNI();
			 	}
			 	urlManager.processRequest( url );
		 	}
	
		}).start();
	}

 	public static native void urlManagerDownloadFinished(String url, boolean success, byte[] data, int dataLength, 
 														 String[] headerNames, String[] headerValues, int headerLength);
 	
 	
 	// CCFBWebView
 	private void openWebBrowser(String url)
	{
	 	activity.openWebBrowser( url );
 	}
 	
 	private void closeWebBrowser()
	{
	 	activity.closeWebBrowser();
 	}

 	public static native void webViewURLLoaded(String url);
 	
 	
 	// CCVideoView
  	private void startVideoView(String file)
 	{
 	 	activity.startVideoView( file );
  	}
  	
  	private void stopVideoView()
 	{
 	 	activity.stopVideoView();
  	}
}
