//
// Created by Marco on 07/12/2022.
//

#ifndef VULKANANDROID_ASSET_H
#define VULKANANDROID_ASSET_H

#include <android/asset_manager_jni.h>

const char* read(JNIEnv* env, jobject assetManager, const char* filename, size_t* len);

#endif //VULKANANDROID_ASSET_H
