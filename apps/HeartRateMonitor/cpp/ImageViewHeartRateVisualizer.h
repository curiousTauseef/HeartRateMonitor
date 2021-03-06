/*
 * ImageViewHeartRateVisualizer.h
 *
 *  Created on: Feb 19, 2014
 *      Author: pavel
 */

#ifndef IMAGEVIEWHEARTRATEVISUALIZER_H_
#define IMAGEVIEWHEARTRATEVISUALIZER_H_

#include <jni.h>

#include <heartrate/HeartRate.h>

namespace hrm {

class ImageViewHeartRateVisualizer: public IHeartRateVisualizer {
public:
    ImageViewHeartRateVisualizer(JNIEnv * jniEnv, jobject object_self,
            const std::string& methodName);
    virtual ~ImageViewHeartRateVisualizer();

    virtual void visualizeHeartRate(HrmMeasurementGraph heartRateMeasuredGraph)
            throw (HRVisualizeException);

private:
    bool _isError;

    JavaVM * _javaVM;
    jobject _object_global_self;
    jmethodID _method_self_drawBitmap;
};

}  // namespace hrm

#endif /* IMAGEVIEWHEARTRATEVISUALIZER_H_ */
