package com.eddy.eimagepro.core;

import android.graphics.Bitmap;

public class ImageProcessor {
	
	/**
	 * 灰度化
	 * @param sourceBitap
	 * @param destBitmap
	 */
	public native void grayImage(Bitmap sourceBitap, Bitmap destBitmap);
	
	/**
	 * 二值化
	 * @param sourceBitap
	 */
	public native void binarizationImage(Bitmap bitmap, Bitmap destBitmap);
	
	static {
		System.loadLibrary("EImagePro");
	}
	
	public static int[][] createBinaryImage( Bitmap bm )
	{
	    int[] pixels = new int[bm.getWidth()*bm.getHeight()];
	    bm.getPixels( pixels, 0, bm.getWidth(), 0, 0, bm.getWidth(), bm.getHeight() );
	    int w = bm.getWidth();

	    // Calculate overall lightness of image
	    long gLightness = 0;
	    int lLightness;
	    int c;
	    for ( int x = 0; x < bm.getWidth(); x++ )
	    {
	        for ( int y = 0; y < bm.getHeight(); y++ )
	        {
	            c = pixels[x+y*w];
	            lLightness = ((c&0x00FF0000 )>>16) + ((c & 0x0000FF00 )>>8) + (c&0x000000FF);
	            pixels[x+y*w] = lLightness;
	            gLightness += lLightness;
	        }
	    }
	    gLightness /= bm.getWidth() * bm.getHeight();
	    gLightness = gLightness * 5 / 6;

	    // Extract features
	    int[][] binaryImage = new int[bm.getWidth()][bm.getHeight()];

	    for ( int x = 0; x < bm.getWidth(); x++ )
	        for ( int y = 0; y < bm.getHeight(); y++ )
	            binaryImage[x][y] = pixels[x+y*w] <= gLightness ? 0 : 1;

	    return binaryImage;
	}

}
