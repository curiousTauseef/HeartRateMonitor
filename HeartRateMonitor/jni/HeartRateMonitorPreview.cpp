/*
 * HeartRateMonitorPreview.cpp
 *
 *  Created on: 23 нояб. 2013 г.
 *      Author: pavel
 */


#include <string>
#include <map>
#include <list>

#include <boost/lambda/lambda.hpp>
#include <boost/smart_ptr.hpp>

#include <android/bitmap.h>

#include <HeartBeatRateTypes.h>
#include <HeartBeatRateDefines.h>
#include <HeartRateProcessor.h>
#include <ImageFormat.h>
#include <RGBFrameSource.h>
#include <IImageDrawer.h>

#include <HeartRateCounter.h>
#include <SimpleHeartRateNumber.h>
#include <SimpleHeartRateGenerator.h>
#include <SimpleHeartRateVisualizer.h>

#include "HeartRateMonitorPreview.h"

boost::shared_ptr<hrm::NV21FrameSource> nv21;
boost::shared_ptr<hrm::HeartRateProcessor> hrp;

boost::shared_ptr<hrm::HeartRateCounter> heartRateCounter;

class HeartRateMonitorPreviewJava: public hrm::IImageDrawer {
public:
    HeartRateMonitorPreviewJava(JNIEnv * JNIEnv_, jobject self):
        _jni(0), _self(0), _hrmPreviewClass(0), _drawBitmap(0){

        LLDEBUG("HeartRateMonitorPreviewJava(JNIEnv * JNIEnv_, jobject self): _jni(0), _self(0)");
        _jni = JNIEnv_;
        _self = _jni->NewGlobalRef(self);
        if(!_self){
            LLERROR("!_self");
            return;
        }
        _hrmPreviewClass = _jni->FindClass("com/shnayder/heartratemonitor/HeartRateMonitorPreview");
        if(!_hrmPreviewClass){
            LLERROR("!_hrmPreviewClass");
            return;
        }
        _drawBitmap = _jni->GetMethodID(_hrmPreviewClass, "drawBitmap", "()V");
        if(!_drawBitmap){
            LLERROR("!_drawBitmap");
        }
        LLDEBUG("ctor HeartRateMonitorPreviewJava success");
    }
    virtual ~HeartRateMonitorPreviewJava() {
        LLDEBUG("~HeartRateMonitorPreviewJava()");
        _jni->DeleteGlobalRef(_self);
    }
    virtual void drawImage(hrm::Image image) {
    }
private:
    JNIEnv * _jni;
    jobject _self;

    jclass _hrmPreviewClass;
    jmethodID _drawBitmap;
};

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LLDEBUG("JNI_OnLoad(JavaVM* vm, void* reserved)");
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}
void JNI_OnUnload(JavaVM* vm, void* reserved){
    LLDEBUG("JNI_OnUnload");
}

jboolean Java_com_shnayder_heartratemonitor_HeartRateMonitorPreview_hrmNativeStart(
        JNIEnv* JNIEnv_, jobject self) {

    hrm::TimeCounter::instance();

    nv21 = boost::shared_ptr<hrm::NV21FrameSource>(new hrm::NV21FrameSource());
    boost::shared_ptr<hrm::RGBFrameSource> rgbfs(new hrm::RGBFrameSource(nv21));

    boost::shared_ptr<hrm::IImageDrawer> heartRateOutput(new HeartRateMonitorPreviewJava(JNIEnv_, self));

    boost::shared_ptr<hrm::IHeartRateGenerator> hrGenerator(
            new hrm::SimpleHeartRateGenerator());
    boost::shared_ptr<hrm::IHeartRateNumber> hrNumber(
            new hrm::SimpleHeartRateNumber());
    boost::shared_ptr<hrm::IHeartRateVisualizer> hrVisualizer(
            new hrm::SimpleHeartRateVisualizer());
    heartRateCounter = boost::shared_ptr<hrm::HeartRateCounter>(
            new hrm::HeartRateCounter(
                    hrGenerator,
                    hrNumber,
                    hrVisualizer
                    ));

    LLINFO("native start");
    hrp = boost::shared_ptr<hrm::HeartRateProcessor>(new hrm::HeartRateProcessor(rgbfs));
//    return hrp->start();

    return true;
}

void Java_com_shnayder_heartratemonitor_HeartRateMonitorPreview_hrmNativeStop(
        JNIEnv* JNIEnv_, jobject self) {

    LLINFO("native stop");

    if(!hrp){
        LLERROR("native stoped");
        return;
    }
//    hrp->stop();

    LLINFO("native stoped");

//    JNIEnv_->DeleteGlobalRef(HeartRateMonitorPreviewThis);
}

jboolean Java_com_shnayder_heartratemonitor_HeartRateMonitorPreview_hrmNativePassImage(
        JNIEnv* JNIEnv_, jobject self,
        jint rows, jint cols, jint type, jbyteArray data) {

    jboolean imageDataIsCopy = false;
    jbyte* imageData = JNIEnv_->GetByteArrayElements(data, &imageDataIsCopy);

    /*
     * measure time and pass image in processor
     */
    boost::tuple<hrm::TimeStamp, hrm::ElapsedTime> ts =
            hrm::TimeCounter::instance()->getTimeStampExt();
    LLINFO("current time = %.2f ms, fps = %.1f", ts.get<0>(), 1000.0 / ts.get<1>());

    LLINFO("%d x %d, type = %s, %p",
            cols,
            rows,
            hrm::AndroidImageFormat::instance()->getImageFormatString(type).c_str(),
            imageData
            );

    nv21->putFrame(uint16_t(rows), uint16_t(cols), (uint8_t *)imageData, ts.get<0>());

    JNIEnv_->ReleaseByteArrayElements(data, imageData, 0);
    return false;
}
