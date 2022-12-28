//
// Created by Marco on 09/12/2022.
//

#include "compiler.h"
#include "asset.h"
#include <shaderc/shaderc.h>
#include <stddef.h>
#include "def.h"

const char* compilation_status_as_string(shaderc_compilation_status compilation_status) {
    if (compilation_status == shaderc_compilation_status_success) return "shaderc_compilation_status_success";
    if (compilation_status == shaderc_compilation_status_invalid_stage) return "shaderc_compilation_status_invalid_stage";
    if (compilation_status == shaderc_compilation_status_compilation_error) return "shaderc_compilation_status_compilation_error";
    if (compilation_status == shaderc_compilation_status_internal_error) return "shaderc_compilation_status_internal_error";
    if (compilation_status == shaderc_compilation_status_null_result_object) return "shaderc_compilation_status_null_result_object";
    if (compilation_status == shaderc_compilation_status_invalid_assembly) return "shaderc_compilation_status_invalid_assembly";
    if (compilation_status == shaderc_compilation_status_validation_error) return "shaderc_compilation_status_validation_error";
    if (compilation_status == shaderc_compilation_status_transformation_error) return "shaderc_compilation_status_transformation_error";
    if (compilation_status == shaderc_compilation_status_configuration_error) return "shaderc_compilation_status_configuration_error";
}

const char* compile_glsl_to_spirv(const char* source_text, size_t source_text_size, const char* entry_point_name, size_t* spirv_len) {
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, source_text, source_text_size, shaderc_compute_shader, "glsl_code", entry_point_name, 0);
    shaderc_compilation_status compilation_status = shaderc_result_get_compilation_status(result);
    if (compilation_status != shaderc_compilation_status_success) {
        LOGI("%s", compilation_status_as_string(compilation_status));
        LOGI("%s", shaderc_result_get_error_message(result));
    }
    *spirv_len = shaderc_result_get_length(result);
    return shaderc_result_get_bytes(result);
}
