# AndroidGPU
[![](https://jitpack.io/v/MarcoCiaramella/AndroidGPU.svg)](https://jitpack.io/#MarcoCiaramella/AndroidGPU)

Android module that brings GPGPU to Java level.

## Based on Vulkan
AndroidGPU is based on Vulkan's compute stage. It takes a GLSL kernel, compiles it to SPIR-V (using [shaderc](https://github.com/google/shaderc)) and runs it on the GPU available on the system.

## Description
This module aims to simplify the development of GPU code in the Android environment. Vulkan is available only in the native level and writing a compute shader only
requires knowledge of C/C++, JNI, the Vulkan API and the shading language (SPIR-V or GLSL).
AndroidGPU reduces this knowledge to the shading language and you only need to write a shader in GLSL.

### Limitations
Due to the aim above there are a lot of limitations in the shader program. Only a subset of layout interfaces are supported and using a generic compute shader
that runs fine on Vulkan's compute stage may have an undefined behavior in AndroidGPU.

GLSL supported version is 430 and the only working and tested layout interface has the following format
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
Instead Vulkan is supported by Android's NDK from API level 24.

## How to use
Click the jitpack badge above and follow the instructions on how to import in your project.
Write the GLSL code in a file placed in asset folder and pass it to AndroidGPU constructor or pass the code directly.

For example file compute.glsl in asset folder
```glsl
#version 430

layout (local_size_x = 256) in;

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

    uint gID = gl_GlobalInvocationID.x;
    if (gID < 1024) {
        output_data.buff[gID] = input_data.buff[gID] + params_data.buff[0] + params_data.buff[1] + params_data.buff[2];
    }
}
```
and the MainActivity
```java
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        try {
            AndroidGPU androidGPU = new AndroidGPU(this, "compute.glsl");
            
            int dim = 1024;
            Double[] input = new Double[dim];
            Double[] output = new Double[dim];
            Double[] params = new Double[]{1.0, 2.0, 3.0};

            for (int i = 0; i < input.length; i++) {
                input[i] = (double) i;
            }
            androidGPU.run(this, () -> {
                Log.i("####", "Done");
            }, dim, 1, 1, 256, 1, 1, output, input, params);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
```
See [samples](https://github.com/MarcoCiaramella/AndroidGPU-Samples).

## Note
This was the first time that I approach the Vulkan API and I don't know if already exists something like this out of there for Android. I saw that Android doesn't provide a practical way to use GPU for GPGPU tasks
so I started implementing a little module to help.

Any suggestions are welcome.