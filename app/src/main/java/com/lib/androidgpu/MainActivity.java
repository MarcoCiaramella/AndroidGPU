package com.lib.androidgpu;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import com.lib.androidgpulib.AndroidGPU;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        try {
            AndroidGPU androidGPU = new AndroidGPU(getAssets(), "compute.glsl");
            /*androidGPU = new AndroidGPU("#version 430\n" +
                    "\n" +
                    "layout (local_size_x = 256) in;\n" +
                    "\n" +
                    "layout(set = 0, binding = 0) readonly buffer InputBuffer {\n" +
                    "    double buff[];\n" +
                    "} input_data;\n" +
                    "\n" +
                    "layout(set = 0, binding = 1) buffer OutputBuffer {\n" +
                    "    double buff[];\n" +
                    "} output_data;\n" +
                    "\n" +
                    "void main() {\n" +
                    "\n" +
                    "    uint gID = gl_GlobalInvocationID.x;\n" +
                    "    if (gID < 256) {\n" +
                    "        output_data.buff[gID] = input_data.buff[gID];\n" +
                    "    }\n" +
                    "}");*/

            Double[] input = new Double[256];
            Double[] output = new Double[256];
            Integer[] params = new Integer[]{1, 2, 3};

            for (int i = 0; i < input.length; i++) {
                input[i] = (double) i;
            }
            androidGPU.run(256, 1, 1, 256, 1, 1, output, input, params);
            for (double d : output) {
                Log.i("####", Double.toString(d));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}