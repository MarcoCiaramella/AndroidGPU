package com.lib.androidgpulib;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;

import java.io.InputStream;

public class AndroidGPU {

    static {
        System.loadLibrary("androidgpu");
    }

    private byte[] spirv;
    private static final String GLSL_VERSION = "#version 430";

    /**
     *
     * @param context
     * @param assetGlsl file from assets folder with GLSL code
     * @throws Exception
     */
    public AndroidGPU(Context context, String assetGlsl) throws Exception {
        InputStream inputStream = context.getAssets().open(assetGlsl);
        byte[] buffer = new byte[inputStream.available()];
        inputStream.read(buffer);
        checkAndBuild(new String(buffer));
    }

    /**
     *
     * @param glsl GLSL code as string
     * @throws Exception
     */
    public AndroidGPU(String glsl) throws Exception {
        checkAndBuild(glsl);
    }

    /**
     * Runs the GLSL code.
     * @param activity
     * @param callback function called on UI thread when GPU task ends
     * @param dim_x data dimension along X
     * @param dim_y data dimension along Y
     * @param dim_z data dimension along Z
     * @param local_size_x must match local_size_x in layout
     * @param local_size_y must match local_size_y in layout
     * @param local_size_z must match local_size_z in layout
     * @param output a buffer for the output. Object must be Integer, Float or Double.
     * @param input one or more buffers for input data. Object must be Integer, Float or Double.
     * @throws Exception
     */
    public void run(Activity activity,
                    Runnable callback,
                    int dim_x,
                    int dim_y,
                    int dim_z,
                    int local_size_x,
                    int local_size_y,
                    int local_size_z,
                    Object[] output,
                    Object[]... input) throws Exception {
        checkDataTypes(output, input);
        new Thread(() -> {
            AndroidGPU.this.run(spirv, dim_x, dim_y, dim_z, local_size_x, local_size_y, local_size_z, output, input);
            activity.runOnUiThread(callback);
        }).start();
    }

    private void checkAndBuild(String glsl) throws Exception {
        checkVersion(glsl);
        spirv = build(glsl);
    }

    private void checkVersion(String glsl) throws Exception {
        if (!glsl.startsWith(GLSL_VERSION)) {
            throw new Exception("GLSL version not specified or invalid. Must be " + GLSL_VERSION);
        }
    }

    private void checkDataType(Object[] arr) throws Exception {
        if (!(arr instanceof Integer[]) && !(arr instanceof Double[]) && !(arr instanceof Float[])) {
            throw new Exception("Supported data types are Integer[], Float[] and Double[]");
        }
    }

    private void checkDataTypes(Object[] output, Object[]... input) throws Exception {
        checkDataType(output);
        for (Object[] arr : input) {
            checkDataType(arr);
        }
    }

    private native void run(byte[] spirv,
                            int dim_x,
                            int dim_y,
                            int dim_z,
                            int local_size_x,
                            int local_size_y,
                            int local_size_z,
                            Object[] output,
                            Object[]... input);

    private native byte[] build(String glsl);
}
