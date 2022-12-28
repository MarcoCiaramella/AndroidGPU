//
// Created by Marco on 13/12/2022.
//

#ifndef VULKANANDROID_DEF_H
#define VULKANANDROID_DEF_H

#include <android/log.h>

#define  LOG_TAG    "JNI"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif //VULKANANDROID_DEF_H
