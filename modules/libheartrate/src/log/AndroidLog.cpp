/*
 * AndroidLog.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: pavel
 */

#include "AndroidLog.h"

namespace hrm {

AndroidLog::AndroidLog() {
}

AndroidLog::~AndroidLog() {
}

void AndroidLog::DEBUG(const std::string& message) {
    __android_log_print(ANDROID_LOG_DEBUG, HRM_TAG, "%s", message.c_str());
}

void AndroidLog::INFO(const std::string& message) {
    __android_log_print(ANDROID_LOG_INFO, HRM_TAG, "%s", message.c_str());
}

void AndroidLog::ERR(const std::string& message) {
    __android_log_print(ANDROID_LOG_ERROR, HRM_TAG, "%s", message.c_str());
}

void AndroidLog::WARN(const std::string& message) {
    __android_log_print(ANDROID_LOG_WARN, HRM_TAG, "%s", message.c_str());
}

}  // namespace hrm

