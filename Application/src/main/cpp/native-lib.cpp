#include <jni.h>
#include <string>
#include <android/log.h>
#include <time.h>

#include <cstddef>
#include "libyuv.h"

using namespace libyuv;
using namespace std;

//vector<int> val_seq;

void test()
{
    int i = 0;

}



int test2(int i){
    i = i +10;
    return i;
}

void YUV420toRGB8888 (unsigned char* yuv,unsigned char* rgb, int mWidth, int mHeight){//}, int[] rgb){
    int i, j, Y,Cr,Cb, R,G,B;
    int mSize = mHeight * mWidth;

    for(j = 0; j < mHeight; j++) {
        int pixPtr = j * mWidth; 		//YUVの配列は輝度、色差（CrCb交互）の順に並んでいる
        int cPtr =(mSize) + (j >> 1) * (mWidth); 	//色差は２行でひとつ（2x2ピクセルに1つ）
        for(i = 0; i < mWidth; i+=2) {		//一度に2ピクセル処理（さらに２行同時のほうが効率的）
            Y=yuv[pixPtr];	if(Y < 0) Y += 256;	//byte型は符号つきなので0～255に変えてやる
            Cr=yuv[cPtr ++];	if(Cr < 0) Cr += 128; else Cr -= 128;	//-128～127に
            Cb=yuv[cPtr ++];	if(Cb < 0) Cb += 128; else Cb -= 128;
            R =Y+1.402*Cr;	if(R < 0) R = 0;else if(R > 255) R = 255;	//RGBのそれぞれの値が
            G = Y-0.714*Cr-0.344*Cb;if(G < 0) G = 0;else if(G > 255) G = 255;	//0～255に収まるようにする
            B = Y+1.772*Cb;	if(B < 0) B = 0;else if(B > 255) B = 255;

//            rgb[pixPtr++] =0xff000000 + (R << 16) + (G << 8) + B;		//int型で1ピクセル分の色を表現

            Y=yuv[pixPtr];	if(Y < 0) Y += 255; 	//色差は２列でひとつなのでさっきのを使う

            R =Y+1.402*Cr;		if(R < 0) R = 0;else if(R > 255) R = 255;
            G = Y-0.714*Cr-0.344*Cb;if(G < 0) G = 0;else if(G > 255) G = 255;
            B = Y+1.772*Cb;	if(B < 0) B = 0;else if(B > 255) B = 255;
//            rgb[pixPtr++] =0xff000000 + (R << 16) + (G << 8) + B;
        }
    }

//    NV21ToARGB(const uint8* src_y, int src_stride_y,
//                   const uint8* src_uv, int src_stride_uv,
//                   uint8* dst_argb, int dst_stride_argb,
//                   int width, int height);
    return;
}

#define CLIP(x) do{if(x < 0){x = 0;} else if(x > 255){x = 255;}} while(0)
#define CONVERT_R(Y, V)    ((298 * (Y - 16) + 409 * (V - 128) + 128) >> 8)
#define CONVERT_G(Y, U, V) ((298 * (Y - 16) - 100 * (U - 128) - 208 * (V - 128) + 128) >> 8)
#define CONVERT_B(Y, U)    ((298 * (Y - 16) + 516 * (U - 128) + 128) >> 8)

void YUV420toRGB8888_2(unsigned char*  yuvBuffer,unsigned char*  rgbBuffer, int width, int height) {//}, int[] rgb){
    unsigned char* uvStart = yuvBuffer + width * height;
    unsigned char y[2] = { 0, 0 };
    unsigned char u = 0;
    unsigned char v = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    for (int rowCnt = 0; rowCnt < height; rowCnt++)
    {
        for (int colCnt = 0; colCnt < width; colCnt += 2)
        {
            u = *(uvStart + colCnt + 0);
            v = *(uvStart + colCnt + 1);

            for (int cnt = 0; cnt < 2; cnt++)
            {
                y[cnt] = yuvBuffer[rowCnt * width + colCnt + cnt];

                r = CONVERT_R(y[cnt], v);
                CLIP(r);
                g = CONVERT_G(y[cnt], u, v);
                CLIP(g);
                b = CONVERT_B(y[cnt], u);
                CLIP(b);
                rgbBuffer[(rowCnt * width + colCnt + cnt) * 3 + 0] = (unsigned char)r;
                rgbBuffer[(rowCnt * width + colCnt + cnt) * 3 + 1] = (unsigned char)g;
                rgbBuffer[(rowCnt * width + colCnt + cnt) * 3 + 2] = (unsigned char)b;
            }
        }

        uvStart += width * (rowCnt % 2);
    }
}

//void rgbToYuv(JNIEnv *env, jobject, jbyteArray rgbArray, jint width, jint height, jbyteArray yuvArray) {
void rgbToYuv(jbyte *rgb, jint width, jint height, jbyte* yuv) {


//    jbyte *rgb = env->GetByteArrayElements(rgbArray, NULL);
//    jbyte *yuv = env->GetByteArrayElements(yuvArray, NULL);

    int rgbIndex = 0;
    int yIndex = 0;
    int uvIndex = width * height;
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            int r = rgb[rgbIndex] & 0xFF;
            int g = rgb[rgbIndex + 1] & 0xFF;
            int b = rgb[rgbIndex + 2] & 0xFF;

            int y = (int) (0.257 * r + 0.504 * g + 0.098 * b + 16);
            int u = (int) (-0.148 * r - 0.291 * g + 0.439 * b + 128);
            int v = (int) (0.439 * r - 0.368 * g - 0.071 * b + 128);

            yuv[yIndex++] = (jbyte) (y < 0 ? 0 : y > 255 ? 255 : y);
            if ((i & 0x01) == 0 && (j & 0x01) == 0) {
                yuv[uvIndex++] = (jbyte) (v < 0 ? 0 : v > 255 ? 255 : v);
                yuv[uvIndex++] = (jbyte) (u < 0 ? 0 : u > 255 ? 255 : u);
            }

            rgbIndex += 4;
        }
    }

//    env->ReleaseByteArrayElements(yuvArray, yuv, 0);
//    env->ReleaseByteArrayElements(rgbArray, rgb, 0);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_android_camera2basic_Camera2BasicFragment_testJNI(
        JNIEnv *env,
        jobject /* this */,
        jbyteArray image,
        jbyteArray dst,
        jint width,
        jint height
) {


    __android_log_print(ANDROID_LOG_DEBUG, "imsize", "(%d, %d)", width, height);
    clock_t st= clock();

    //jboolean b = true;
    jbyte* yuv =  (jbyte*)env->GetByteArrayElements(image, NULL);
    jbyte* rgb =  (jbyte*)env->GetByteArrayElements(dst, NULL);
    int nSize = env->GetArrayLength(image);

//    env->ReleaseBooleanArrayElements(image, bufferPtr , 0);
    __android_log_print(ANDROID_LOG_DEBUG, "imsize", "%d", nSize);
//    YUV420toRGB8888 ((unsigned char*)yuv, (unsigned char*)rgb, width, height);
    YUV420toRGB8888_2 ((unsigned char*)yuv, (unsigned char*)rgb, width, height);
//    rgbToYuv(image, jint width, jint height, jbyte* yuv)


    // Default test
    //__android_log_print(ANDROID_LOG_DEBUG, "NativeCode", "%s(%d)", __FILE__, __LINE__);
    //std::string hello = "Hello from C++";
    //return env->NewStringUTF(hello.c_str());

    // vector test
    //val_seq.push_back(1);
    //__android_log_print(ANDROID_LOG_DEBUG, "NativeCode", "(%d)", val_seq.size());
    //char val_char[512];
    //sprintf(val_char, "%d", val_seq.size());
    //return env->NewStringUTF(val_char);

    // function test
    //int i = test3(val_seq.size());
    //val_seq.push_back(1);

    clock_t en= clock();
    __android_log_print(ANDROID_LOG_DEBUG, "imsize", "(%f)", (float)(en-st)/(float)CLOCKS_PER_SEC);



    char val_char[512];
    sprintf(val_char, "%d", 0);
    return 0;//env->NewStringUTF(val_char);

}


