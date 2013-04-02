#include <jni.h>
#include <string.h>
#include <android/bitmap.h>
#include <android/log.h>
#include "com_eddy_eimagepro_core_ImageProcessor.h"

int otsu(uint32_t* colors, int w, int h);

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

	// OTSU获取分割阀值
	int thresh = otsu(destData, w, h);

	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "thresh: %d", thresh);

	//二值化
	int x, y, gray;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			gray = (int)(destData[w * y + x] & 0xFF); // 获得灰度值（red=green=blue
			if(gray < thresh) {
				destData[y * w + x] = black;
			} else {
				destData[y * w + x] = white;
			}
		}
	}

	AndroidBitmap_unlockPixels(env, sBitmap);
	AndroidBitmap_unlockPixels(env, destBitmap);
	__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Binarization image over!");
}

/**
 * OTSU算法求最适分割阈值
 */
int otsu(uint32_t* rgbData, int w, int h) {
    unsigned int pixelNum[256]; // 图象灰度直方图[0, 255]
    int n, n0, n1; //  图像总点数，前景点数， 后景点数（n0 + n1 = n）
    int w0, w1; // 前景所占比例， 后景所占比例（w0 = n0 / n, w0 + w1 = 1）
    double u, u0, u1; // 总平均灰度，前景平均灰度，后景平均灰度（u = w0 * u0 + w1 * u1）
    double g, gMax; // 图像类间方差，最大类间方差（g = w0*(u0-u)^2+w1*(u1-u)^2 = w0*w1*(u0-u1)^2）
    double sum_u, sum_u0, sum_u1; // 图像灰度总和，前景灰度总和， 后景平均总和（sum_u = n * u）
    int thresh; // 阈值

    memset(pixelNum, 0, 256 * sizeof(unsigned int)); // 数组置0

    int x, y, gray;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			gray = (int) ((rgbData[w * y + x]) & 0xFF); // 获得灰度值
			pixelNum[gray]++;
		}
	}

    // 图像总点数
    n = w * h;

    // 计算总灰度
    int k;
    for (k = 0; k <= 255; k++) {
        sum_u += k * pixelNum[k];
    }

    // 遍历判断最大类间方差，得到最佳阈值
    for (k = 0; k <= 255; k++) {
        n0 += pixelNum[k]; // 图像前景点数
        if (0 == n0) { // 未获取前景，直接继续增加前景点数
            continue;
        }
        if (n == n0) { // 前景点数包括了全部时，不可能再增加，退出循环
            break;
        }
        n1 = n - n0; // 图像后景点数

        sum_u0 += k * pixelNum[k]; // 前景灰度总和
        u0 = sum_u0 / n0; // 前景平均灰度
        u1 = (sum_u - sum_u0) / n1; // 后景平均灰度

        g = n0 * n1 * (u0 - u1) * (u0 - u1); // 类间方差（少除了n^2）

        if (g > gMax) { // 大于最大类间方差时
            gMax = g; // 设置最大类间方差
            thresh = k; // 取最大类间方差时对应的灰度的k就是最佳阈值
        }
    }

    return thresh;
}

