package com.eddy.eimagepro;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.ImageView;

import com.eddy.eimagepro.core.ImageProcessor;

public class MainActivity extends Activity {
	private ImageView imageView;
	private Bitmap orgBitmap;
	private Bitmap destBitmap;
	private ImageProcessor imageProcessor;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		imageView = (ImageView) findViewById(R.id.imageView);
		imageProcessor = new ImageProcessor();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public void toGray(View v) {
		orgBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.mt);
		orgBitmap = orgBitmap.copy(Bitmap.Config.ARGB_8888, true);
		destBitmap = orgBitmap.copy(Bitmap.Config.ARGB_8888, true);
		imageProcessor.grayImage(orgBitmap, destBitmap);
		imageView.setImageBitmap(destBitmap);
	}
	
	public void binarizationImage(View v) {
		Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.mt);
		
		bitmap = bitmap.copy(Bitmap.Config.ARGB_8888, true);
		if(bitmap == null) {
			bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.mt);
			imageView.setImageBitmap(bitmap);
			return;
		}
		imageProcessor.binarizationImage(bitmap);
		imageView.setImageBitmap(bitmap);
	}
	
}
