//
// Created by Marco on 07/12/2022.
//

#include "asset.h"
#include <stdlib.h>

const char* read(JNIEnv* env, jobject assetManager, const char* filename, size_t* len) {
    AAsset* file = AAssetManager_open(AAssetManager_fromJava(env, assetManager), filename, AASSET_MODE_BUFFER);
    size_t fileLength = AAsset_getLength(file);
    char* fileContent = (char*) malloc(fileLength);
    AAsset_read(file, fileContent, fileLength);
    AAsset_close(file);
    *len = fileLength;
    return fileContent;
}