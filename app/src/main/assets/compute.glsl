#version 430

layout (local_size_x = 256) in;

layout(set = 0, binding = 0) readonly buffer InputBuffer {
    double buff[];
} input_data;

layout(set = 0, binding = 1) buffer OutputBuffer {
    double buff[];
} output_data;

void main() {

    uint gID = gl_GlobalInvocationID.x;
    if (gID < 256) {
        output_data.buff[gID] = input_data.buff[gID];
    }
}