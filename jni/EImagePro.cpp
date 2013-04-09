#include <jni.h>
#include <string.h>
#include <android/bitmap.h>
#include <android/log.h>
#include "com_eddy_eimagepro_core_ImageProcessor.h"
#include <math.h>

int cvThresholdOtsu(int ** grayArr, int width, int height);
int ** initArray(int height, int width);

void JNICALL Java_com_eddy_eimagepro_core_ImageProcessor_grayImage
  (JNIEnv * env, jobject thiz, jobject sBitmap, jobject destBitmap) {

	AndroidBitmapInfo infocolor;
	AndroidBitmapInfo infogray;
	void* pixelscolor;
	void* pixelsgray;

	int ret = 0;
	if((ret = AndroidBitmap_getInfo(env, sBitmap, &infocolor)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_getInfo fail!");
		return;
	}
	if((ret = AndroidBitmap_getInfo(env, destBitmap, &infogray)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_getInfo fail!");
		return;
	}
	if (infocolor.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Bitmap format is not RGBA_8888");
		return;
	}
	if (infogray.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Bitmap format is not RGBA_8888");
		return;
	}
	if((ret = AndroidBitmap_lockPixels(env, sBitmap, &pixelscolor) ) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_lockPixels fail!");
		return;
	}
	if((ret = AndroidBitmap_lockPixels(env, destBitmap, &pixelsgray) ) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_lockPixels fail!");
		return;
	}

	int w = infocolor.width;
	int h = infocolor.height;
	int *rgbData = (int *) pixelscolor;
	int *destData = (int *) pixelsgray;

	int alpha = (int) (0xFF << 24);
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			int color = (int) rgbData[w * y + x];
			int red = (int) ((color & 0x00FF0000) >> 16);
			int green = (int) ((color & 0x0000FF00) >> 8);
			int blue = (int) (color & 0x000000FF);
			color = (red * 38 + green * 75 + blue * 15) >> 7;
			color = alpha | (color << 16) | (color << 8) | color;
			destData[y * w + x] = 0xff000000 | (color) | (color << 8) | (color << 16);
		}
	}

	AndroidBitmap_unlockPixels(env, sBitmap);
	AndroidBitmap_unlockPixels(env, destBitmap);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "convert over!");
}

void Java_com_eddy_eimagepro_core_ImageProcessor_binarizationImageNew(JNIEnv * env, jobject thiz, jobject sBitmap, jobject destBitmap) {
	AndroidBitmapInfo infocolor;
	void* pixelscolor;
	AndroidBitmapInfo infodest;
	void* pixelsdest;
	int ret = 0;
	if((ret = AndroidBitmap_getInfo(env, sBitmap, &infocolor)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_getInfo fail!");
		return;
	}
	if (infocolor.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Bitmap format is not RGBA_8888");
		return;
	}
	if((ret = AndroidBitmap_lockPixels(env, sBitmap, &pixelscolor) ) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_lockPixels fail!");
		return;
	}

	if((ret = AndroidBitmap_getInfo(env, destBitmap, &infodest)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_getInfo fail!");
		return;
	}
	if (infodest.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Bitmap format is not RGBA_8888");
		return;
	}
	if((ret = AndroidBitmap_lockPixels(env, destBitmap, &pixelsdest) ) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_lockPixels fail!");
		return;
	}

	int white = 0xFFFFFFFF; // 不透明白色
	int black = 0xFF000000; // 不透明黑色
	uint32_t *rgbData = (uint32_t *) pixelscolor;
	uint32_t *destData = (uint32_t *) pixelsdest;

	//1. 整个图的灰度平均值
	int width = infocolor.width;
	int height = infocolor.height;

	int area = width * height;
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "WW:%d, HH:%d", width, height);
	int ** gray = initArray(width, height);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "declare array over!", width, height);

	int average = 0;// 灰度平均值
	int graysum = 0;
	int graymean = 0;
	int grayfrontmean = 0;
	int graybackmean = 0;
	int pixelGray;
	int front = 0;
	int back = 0;

	for (int i = 1; i < width; i++) { // 不算边界行和列，为避免越界
		for (int j = 1; j < height; j++) {
			int x = j * width + i;
			int r = (rgbData[x] >> 16) & 0xff;
			int g = (rgbData[x] >> 8) & 0xff;
			int b = rgbData[x] & 0xff;
			pixelGray = (int) (0.3 * r + 0.59 * g + 0.11 * b);// 计算每个坐标点的灰度
			gray[i][j] = (pixelGray << 16) + (pixelGray << 8) + (pixelGray);
			destData[x] = (pixelGray << 16) + (pixelGray << 8) + (pixelGray);
			graysum += pixelGray;
		}
	}
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "计算灰度完成");

	graymean = (int) (graysum / area);// 整个图的灰度平均值
	average = graymean;
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Average: %d", average);

	// 计算整个图的二值化阈值
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (((gray[i][j]) & (0x0000ff)) < graymean) {
				graybackmean += ((gray[i][j]) & (0x0000ff));
				back++;
			} else {
				grayfrontmean += ((gray[i][j]) & (0x0000ff));
				front++;
			}
		}
	}

	int frontvalue = (int) (grayfrontmean / front);// 前景中心
	int backvalue = (int) (graybackmean / back);// 背景中心
	float G[frontvalue - backvalue + 1];
	int s = 0;
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Front: %d, Frontvalue:%d, Backvalue: %d", front, frontvalue, backvalue);

	// 以前景中心和背景中心为区间采用大津法算法（OTSU算法）
	for (int i1 = backvalue; i1 < frontvalue + 1; i1++) {
		back = 0;
		front = 0;
		grayfrontmean = 0;
		graybackmean = 0;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (((gray[i][j]) & (0x0000ff)) < (i1 + 1)) {
					graybackmean += ((gray[i][j]) & (0x0000ff));
					back++;
				} else {
					grayfrontmean += ((gray[i][j]) & (0x0000ff));
					front++;
				}
			}
		}
		grayfrontmean = (int) (grayfrontmean / front);
		graybackmean = (int) (graybackmean / back);
		G[s] = (((float) back / area) * (graybackmean - average) * (graybackmean - average) + ((float) front / area) * (grayfrontmean - average) * (grayfrontmean - average));
		s++;
	}
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "DDDDDDDDDDDDDDDD");

	float max = G[0];
	int index = 0;
	for (int i = 1; i < frontvalue - backvalue + 1; i++) {
		if (max < G[i]) {
			max = G[i];
			index = i;
		}
	}

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int in = j * width + i;
			if (((gray[i][j]) & (0x0000ff)) < (index + backvalue)) {
				destData[in] = black;
			} else {
				destData[in] = white;
			}
		}
	}

	AndroidBitmap_unlockPixels(env, sBitmap);
	AndroidBitmap_unlockPixels(env, destBitmap);
	for(int index = 0; index < width; index++) {
	    delete [] gray[index];
	}
	delete [] gray;

	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Binarization image over!");
}

/**
 * 二值化图像
 */
void Java_com_eddy_eimagepro_core_ImageProcessor_binarizationImage(JNIEnv * env, jobject thiz, jobject sBitmap, jobject destBitmap) {
	AndroidBitmapInfo infocolor;
	void* pixelscolor;
	AndroidBitmapInfo infodest;
	void* pixelsdest;
	int ret = 0;
	if((ret = AndroidBitmap_getInfo(env, sBitmap, &infocolor)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_getInfo fail!");
		return;
	}
	if (infocolor.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Bitmap format is not RGBA_8888");
		return;
	}
	if((ret = AndroidBitmap_lockPixels(env, sBitmap, &pixelscolor) ) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_lockPixels fail!");
		return;
	}

	if((ret = AndroidBitmap_getInfo(env, destBitmap, &infodest)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_getInfo fail!");
		return;
	}
	if (infodest.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Bitmap format is not RGBA_8888");
		return;
	}
	if((ret = AndroidBitmap_lockPixels(env, destBitmap, &pixelsdest) ) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "AndroidBitmap_lockPixels fail!");
		return;
	}

	int w = infocolor.width;
	int h = infocolor.height;

	int white = 0xFFFFFFFF; // 不透明白色
	int black = 0xFF000000; // 不透明黑色
	uint32_t *rgbData = (uint32_t *) pixelscolor;
	uint32_t *destData = (uint32_t *) pixelsdest;

	//灰度化
	int ** grayArr = initArray(w, h);
	int alpha = (int) (0xFF << 24);
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			int color = (int) rgbData[w * y + x];
			int red = (int) ((color & 0x00FF0000) >> 16);
			int green = (int) ((color & 0x0000FF00) >> 8);
			int blue = (int) (color & 0x000000FF);
			color = (red * 38 + green * 75 + blue * 15) >> 7;
			grayArr[x][y] = color;
		}
	}

	// OTSU获取分割阀值
	int thresh = cvThresholdOtsu(grayArr, w, h);

	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "thresh: %d", thresh);

	//二值化
	int x, y, gray;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			gray = grayArr[x][y];
			if(gray < thresh) {
				destData[y * w + x] = black;
			} else {
				destData[y * w + x] = white;
			}
		}
	}

	AndroidBitmap_unlockPixels(env, sBitmap);
	AndroidBitmap_unlockPixels(env, destBitmap);
	for(int index = 0; index < w; index++) {
		delete [] grayArr[index];
	}
	delete [] grayArr;
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Binarization image over!");
}

int ** initArray(int width, int height){
   int ** gray = new int *[width];
   for (unsigned int i = 0; i < width; ++i)
   {
	  gray[i] = new int[height];
   }

   return gray;
}

int cvThresholdOtsu(int ** grayArr, int width, int height) {
    //histogram
    float histogram[256]= {0};
    for(int i=0; i<height; i++) {
    	for(int j=0; j<width; j++) {
			histogram[grayArr[j][i]]++;
		}
	}

    //normalize histogram
    int size=height  *width;
    for(int i=0; i<256; i++)
    {
        histogram[i]=histogram[i]/size;
    }

    //average pixel value
    float avgValue=0;
    for(int i=0; i<256; i++)
    {
        avgValue+=i*histogram[i];
    }

    int threshold;
    float maxVariance=0;
    float w=0,u=0;
    for(int i=0; i<256; i++)
    {
        w+=histogram[i];
        u+=i*histogram[i];

        float t=avgValue*w-u;
        float variance=t*t/(w*(1-w));
        if(variance>maxVariance)
        {
            maxVariance=variance;
            threshold=i;
        }
    }

    return threshold;
}

