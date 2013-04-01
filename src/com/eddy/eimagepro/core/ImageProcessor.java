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
	public native void binarizationImage(Bitmap bitmap);
	
	static {
		System.loadLibrary("EImagePro");
	}

}
