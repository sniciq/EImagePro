package com.eddy.eimagepro;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.ImageView;

import com.eddy.eimagepro.core.ImageProcessor;
import com.eddy.eimagepro.core.ImageProcessor_java;

public class MainActivity extends Activity {
	private ImageView imageView;
	private Bitmap tempBitmap;
	private Bitmap destBitmap;
	private ImageProcessor imageProcessor;
	private ImageProcessor_java imageProcessor_java;
	
	private String mCurrentPhotoPath;
	private Bitmap currentBitmap = null;
	private static final String JPEG_FILE_PREFIX = "IMG_";
	private static final String JPEG_FILE_SUFFIX = ".jpg";
	private static final int actionCode_takePicture = 1;
	private static final int actionCode_openGallery = 2;

	private AlbumStorageDirFactory mAlbumStorageDirFactory = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		imageView = (ImageView) findViewById(R.id.imageView);
		imageProcessor = new ImageProcessor();
		imageProcessor_java = new ImageProcessor_java();
		
		currentBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.mt2);
		
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.FROYO) {
			mAlbumStorageDirFactory = new FroyoAlbumDirFactory();
		} else {
			mAlbumStorageDirFactory = new BaseAlbumDirFactory();
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public void openGallery(View v) {
		Intent intent = new Intent();  
        /* 开启Pictures画面Type设定为image */  
        intent.setType("image/*");  
        /* 使用Intent.ACTION_GET_CONTENT这个Action */  
        intent.setAction(Intent.ACTION_GET_CONTENT);   
        /* 取得相片后返回本画面 */  
        startActivityForResult(intent, actionCode_openGallery);
	}
	
	public void takePicture(View v) {
		Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
		File f = null;
		try {
			f = setUpPhotoFile();
			mCurrentPhotoPath = f.getAbsolutePath();
			takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(f));
			startActivityForResult(takePictureIntent, actionCode_takePicture);
		} catch (Exception e) {
			e.printStackTrace();
			f = null;
		}
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if(requestCode == actionCode_takePicture) {
			if (mCurrentPhotoPath != null) {
				setPic(mCurrentPhotoPath);
				galleryAddPic();
				System.out.println(mCurrentPhotoPath);
				mCurrentPhotoPath = null;
			}
		}
		else if(requestCode == actionCode_openGallery) {
			if (resultCode == RESULT_OK) {  
				Uri uri = data.getData();
				System.out.println(getRealPathFromURI(uri));
				setPic(getRealPathFromURI(uri));
			}
		}
	}
	
	public void toGray(View v) {
		tempBitmap = currentBitmap.copy(Bitmap.Config.ARGB_8888, true);
		destBitmap = tempBitmap.copy(Bitmap.Config.ARGB_8888, true);
		imageProcessor.grayImage(tempBitmap, destBitmap);
		imageView.setImageBitmap(destBitmap);
	}
	
	public void binarizationImage(View v) {
		tempBitmap = currentBitmap.copy(Bitmap.Config.ARGB_8888, true);
		destBitmap = currentBitmap.copy(Bitmap.Config.ARGB_8888, true);
		imageProcessor.binarizationImage(tempBitmap, destBitmap);
		imageView.setImageBitmap(destBitmap);
	}
	
	public void test(View v) {
		ColorMatrix bwMatrix = new ColorMatrix(new float[]{
				0.5f,0.5f,0.5f,0,0, 
                0.5f,0.5f,0.5f,0,0, 
                0.5f,0.5f,0.5f,0,0, 
                0,0,0,1,0,0, 
                0,0,0,0,1,0 
                });
		bwMatrix.setSaturation(0);
		final ColorMatrixColorFilter colorFilter= new ColorMatrixColorFilter(bwMatrix);
		Bitmap rBitmap = currentBitmap.copy(Bitmap.Config.ARGB_8888, true);
		Paint paint=new Paint();
		paint.setColorFilter(colorFilter);
		Canvas myCanvas =new Canvas(rBitmap);
		myCanvas.drawBitmap(rBitmap, 0, 0, paint);
		imageView.setImageBitmap(rBitmap);
	}
	
	/**
	 * JAVA实现二值化
	 * @param v
	 */
	public void javaBinary(View v) {
		Bitmap rBitmap = currentBitmap.copy(Bitmap.Config.ARGB_8888, true);
		Bitmap rBitMao = imageProcessor_java.binarization(rBitmap);
		imageView.setImageBitmap(rBitMao);
	}
	
	/**
	 * 新二值化
	 * @param v
	 */
	public void binarizationImageNew(View v) {
		tempBitmap = currentBitmap.copy(Bitmap.Config.ARGB_8888, true);
		destBitmap = currentBitmap.copy(Bitmap.Config.ARGB_8888, true);
		imageProcessor.binarizationImageNew(tempBitmap, destBitmap);
		imageView.setImageBitmap(destBitmap);
	}
	
	public String getRealPathFromURI(Uri contentUri) {
		String[] proj = { MediaStore.Images.Media.DATA };
		Cursor mImageCursor = getContentResolver().query(contentUri, proj, null, null, null );
		int column_index = mImageCursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
		mImageCursor.moveToFirst();
		return mImageCursor.getString(column_index);
	}
	
	private void galleryAddPic() {
		Intent mediaScanIntent = new Intent("android.intent.action.MEDIA_SCANNER_SCAN_FILE");
		File f = new File(mCurrentPhotoPath);
		Uri contentUri = Uri.fromFile(f);
		mediaScanIntent.setData(contentUri);
		this.sendBroadcast(mediaScanIntent);
	}
	
	private void setPic(String filepath) {
		int targetW = imageView.getWidth();
		int targetH = imageView.getHeight();
		BitmapFactory.Options bmOptions = new BitmapFactory.Options();
		bmOptions.inJustDecodeBounds = false;
		BitmapFactory.decodeFile(filepath, bmOptions);
		int photoW = bmOptions.outWidth;
		int photoH = bmOptions.outHeight;

		int scaleFactor = 1;
		if ((targetW != 0) && (targetH != 0)) {
			scaleFactor = Math.min(photoW / targetW, photoH / targetH);
		}

		bmOptions.inJustDecodeBounds = false;
		bmOptions.inSampleSize = scaleFactor;
		bmOptions.inPurgeable = true;

		Bitmap bitmap = BitmapFactory.decodeFile(filepath, bmOptions);
		currentBitmap = BitmapFactory.decodeFile(filepath);
		imageView.setImageBitmap(bitmap);
	}
	
	private File setUpPhotoFile() throws IOException {
		String timeStamp = new SimpleDateFormat("yyyyMMdd-HHmmss", Locale.US).format(new Date());
		String imageFileName = JPEG_FILE_PREFIX + timeStamp + "_";
		File albumF = getAlbumDir();
		File imageF = File.createTempFile(imageFileName, JPEG_FILE_SUFFIX, albumF);
		mCurrentPhotoPath = imageF.getAbsolutePath();
		return imageF;
	}
	
	private File getAlbumDir() {
		File storageDir = null;
		if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
			storageDir = mAlbumStorageDirFactory.getAlbumStorageDir(getString(R.string.album_name));
			if (storageDir != null) {
				if (!storageDir.mkdirs()) {
					if (!storageDir.exists()) {
						Log.d("CameraSample", "failed to create directory");
						return null;
					}
				}
			}
		} else {
			Log.v(getString(R.string.app_name), "External storage is not mounted READ/WRITE.");
		}
		return storageDir;
	}
}
