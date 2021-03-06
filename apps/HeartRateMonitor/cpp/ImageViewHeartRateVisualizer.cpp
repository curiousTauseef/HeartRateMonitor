/*
 * ImageViewHeartRateVisualizer.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: pavel
 */

#include <algorithm>
#include <android/bitmap.h>

#include "ImageViewHeartRateVisualizer.h"

namespace hrm {

using std::min;
using std::max;

#define CHECK(condition, error_message){ if(_isError)return; if (condition) { \
    HeartRateTools::instance()->getLog()->ERR(error_message); \
    _isError = true; return;} }

ImageViewHeartRateVisualizer::ImageViewHeartRateVisualizer(JNIEnv * jniEnv,
        jobject object_self, const std::string& methodName) :
        _isError(false),
        _javaVM(nullptr),
        _object_global_self(nullptr),
        _method_self_drawBitmap(nullptr) {

    jint res = jniEnv->GetJavaVM(&_javaVM);
    CHECK(res != JNI_OK, "can't get java vm");

    jclass class_self = jniEnv->GetObjectClass(object_self);
    CHECK(!class_self, "can't get self class");

    _object_global_self= jniEnv->NewGlobalRef(object_self);
    CHECK(!_object_global_self, "can't create global ref self class");

    _method_self_drawBitmap = jniEnv->GetMethodID(
            class_self, methodName.c_str(), "(Landroid/graphics/Bitmap;)V");
    CHECK(!_method_self_drawBitmap, "can't get drawBigmap method");

    jniEnv->DeleteLocalRef(class_self);
}

ImageViewHeartRateVisualizer::~ImageViewHeartRateVisualizer() {
    /* todo attach current thread and delete global ref */
    HeartRateTools::instance()->getLog()->DEBUG(
            "ImageViewHeartRateVisualizer::~ImageViewHeartRateVisualizer()");
}

void ImageViewHeartRateVisualizer::visualizeHeartRate(
        HrmMeasurementGraph heartRateMeasuredGraph) throw (HRVisualizeException)
{
    if(_isError){
        HeartRateTools::instance()->getLog()->ERR("is error");
        return;
    }

    JNIEnv * jniEnv = 0;
    jint res = 0;
    res = _javaVM->AttachCurrentThread(reinterpret_cast<JNIEnv **>(&jniEnv),
            NULL);
    CHECK(res != JNI_OK, "can't attach current thread");

    int32_t rows = 480, cols = 640;
    RGBImage testImage = RGBImage(ImageFormatRGB(ImageRect(rows, cols)));

    RGB * dst = testImage.getData();
    for (int i = 0, imax = testImage.getFormat().rect._rows; i < imax; ++i) {
        for (int j = 0, jmax = testImage.getFormat().rect._cols; j < jmax; ++j, ++dst) {
            dst->r = 84;
            dst->g = 170;
            dst->b = 232;
        }
    }

    auto
    f = heartRateMeasuredGraph.front(),
    l = heartRateMeasuredGraph.back();

    auto allTime = l.timeStamp - f.timeStamp;

    auto
    maxV = std::numeric_limits<hrm::HrmMeasurement::Value>::min(),
    minV = std::numeric_limits<hrm::HrmMeasurement::Value>::max();

    for(const auto& m: heartRateMeasuredGraph){
        maxV = std::max(m.value, maxV);
        minV = std::min(m.value, minV);
    }

    auto valueDiff = maxV - minV;

    maxV += valueDiff * .1;
    minV -= valueDiff * .1;
    valueDiff = maxV - minV;

    for (int i = 0, imax = heartRateMeasuredGraph.size() - 1; i < imax; ++i) {
        auto
        m1 = heartRateMeasuredGraph[i],
        m2 = heartRateMeasuredGraph[i + 1];

        line(testImage,
                Point(
                        max<int>(0, min<int>(cols * (m1.timeStamp - f.timeStamp) / allTime, cols)),
                        max<int>(0, min<int>(rows * (m1.value - minV) / valueDiff, rows))
                ),
                Point(
                        max<int>(0, min<int>(cols * (m2.timeStamp - f.timeStamp) / allTime, cols)),
                        max<int>(0, min<int>(rows * (m2.value - minV) / valueDiff, rows))
                ),
                {255, 0, 0});
    }

    jclass class_Bitmap = (jclass)jniEnv->FindClass("android/graphics/Bitmap");
    jmethodID method_Bitmap_createBitmap = jniEnv->GetStaticMethodID(
            class_Bitmap,
            "createBitmap",
            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    jstring string_Config_Name = jniEnv->NewStringUTF("ARGB_8888");
    CHECK(!string_Config_Name, "can't create config string");

    jclass class_Bitmap_Config = jniEnv->FindClass(
            "android/graphics/Bitmap$Config");
    CHECK(!class_Bitmap, "can't find bitmap config class");

    jobject object_Bitmap_Config = jniEnv->CallStaticObjectMethod(
            class_Bitmap_Config,
            jniEnv->GetStaticMethodID(
                    class_Bitmap_Config,
                    "valueOf",
                    "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;"),
                    string_Config_Name);
    CHECK(!object_Bitmap_Config, "can't create bitmap config object");

    jobject object_Bitmap = jniEnv->CallStaticObjectMethod(
            class_Bitmap,
            method_Bitmap_createBitmap,
            cols, rows,
            object_Bitmap_Config);

    uint8_t *data = 0;
    res = AndroidBitmap_lockPixels(jniEnv, object_Bitmap, reinterpret_cast<void **>(&data));
    CHECK(res != ANDROID_BITMAP_RESULT_SUCCESS, "can't lock pixels");

    if(data){

        RGB * src = testImage.getData();
        uint8_t *dst = data;
        for (int i = 0, imax = rows; i < imax; ++i) {
            for (int j = 0, jmax = cols; j < jmax; ++j, ++src) {
                *dst++ = src->r;
                *dst++ = src->g;
                *dst++ = src->b;
                *dst++ = 255;
            }
        }

        res = AndroidBitmap_unlockPixels(jniEnv, object_Bitmap);
        CHECK(res != ANDROID_BITMAP_RESULT_SUCCESS, "can't unlock pixels");
    }

    jniEnv->CallVoidMethod(_object_global_self, _method_self_drawBitmap, object_Bitmap);

    /* clean up */
    jniEnv->DeleteLocalRef(class_Bitmap);
    jniEnv->DeleteLocalRef(class_Bitmap_Config);
    jniEnv->DeleteLocalRef(string_Config_Name);
    jniEnv->DeleteLocalRef(object_Bitmap_Config);
    jniEnv->DeleteLocalRef(object_Bitmap);

    CHECK((res = _javaVM->DetachCurrentThread()) != JNI_OK,
            "can't detach current thread");
}

}  // namespace hrm

