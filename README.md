# AndroidGPU
[![](https://jitpack.io/v/MarcoCiaramella/AndroidGPU.svg)](https://jitpack.io/#MarcoCiaramella/AndroidGPU)

Android module that brings GPGPU to Java level.

## Based on Vulkan
AndroidGPU is based on Vulkan's compute stage. It takes a GLSL kernel, compiles it to SPIR-V and runs on the GPUs available on the system.

## Description
This module aims to simplify the development of GPU code in the Android environment. Vulkan is available only in the native level and writing a compute shader only
requires knowledge of C/C++, JNI, the Vulkan API and the shading language (SPIR-V or GLSL).
AndroidGPU reduces this knowledge to the shading language and you only need to write a shader in GLSL.

### Limitations
Due to the aim above there are a lot of limitations in the shader program. Only a subset of layout interfaces are supported and using a generic compute shader
that runs fine on Vulkan's compute stage may have an undefined behavior in AndroidGPU.

The only working and tested layout interface has the following format
```glsl
#version 430

layout (local_size_x = 256, local_size_y = 256, local_size_z = 256) in;

layout(set = 0, binding = 0) readonly buffer InputBuffer {
double buff[];
} input_data;

layout(set = 0, binding = 1) readonly buffer ParamsBuffer {
double buff[];
} params_data;

layout(set = 0, binding = 2) writeonly buffer OutputBuffer {
double buff[];
} output_data;

void main() {
}
```
One set (set = 0) supported. Each layout must be of type buffer with or without readonly/writeonly qualifier and must have only one member (like double buff[]).
Instance name (like input_data) is optional depends on member names used.

## Vulkan vs OpenCL
While the counterpart OpenCL would be a better choice for GPGPU it is not an Android's standard and could not run on all devices.
Instead Vulkan is supported by Android's NDK from API level ...

## How to use
Click the jitpack badge above and follow the instruction on how to import in your project.

See [samples](https://github.com/MarcoCiaramella/AndroidGPU-Samples)