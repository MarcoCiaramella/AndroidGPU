//
// Created by Marco on 09/12/2022.
//

#ifndef VULKANANDROID_COMPILER_H
#define VULKANANDROID_COMPILER_H

#include <jni.h>

const char* compile_glsl_to_spirv(const char* source_text, size_t source_text_size, const char* entry_point_name, size_t* spirv_len);

#endif //VULKANANDROID_COMPILER_H
