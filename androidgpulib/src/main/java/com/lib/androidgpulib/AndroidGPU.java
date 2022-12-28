package com.lib.androidgpulib;

import android.content.res.AssetManager;

import java.io.InputStream;

public class AndroidGPU {

    static {
        System.loadLibrary("androidgpu");
    }

    private byte[] spirv;
    private static final String GLSL_VERSION = "#version 430";

    public AndroidGPU(AssetManager assetManager, String assetGlsl) throws Exception {
        InputStream inputStream = assetManager.open(assetGlsl);
        byte[] buffer = new byte[inputStream.available()];
        inputStream.read(buffer);
        checkAndBuild(new String(buffer));
    }

    public AndroidGPU(String glsl) throws Exception {
        checkAndBuild(glsl);
    }

    private void checkAndBuild(String glsl) throws Exception {
        checkVersion(glsl);
        spirv = build(glsl);
    }

    public void run(int dim_x,
                    int dim_y,
                    int dim_z,
                    int local_size_x,
                    int local_size_y,
                    int local_size_z,
                    Object[] output,
                    Object[]... input) throws Exception {
        checkDataTypes(output, input);
        run(spirv, dim_x, dim_y, dim_z, local_size_x, local_size_y, local_size_z, output, input);
    }

    private void checkVersion(String glsl) throws Exception {
        if (!glsl.startsWith(GLSL_VERSION)) {
            throw new Exception("GLSL version not specified or invalid. Must be " + GLSL_VERSION);
        }
    }

    private void checkDataType(Object[] arr) throws Exception {
        if (!(arr instanceof Integer[]) && !(arr instanceof Double[]) && !(arr instanceof Float[])) {
            throw new Exception("Supported data types are Integer[], Float[], Double[]");
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
