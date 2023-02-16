//
// Created by Marco on 29/11/2022.
//

#include "vulkan_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "compiler.h"
#include "def.h"
#include <string.h>
#include <math.h>


#define CHECK(result) if (VK_SUCCESS != (result)) { LOGE("Failure at %u %s\n", __LINE__, __FILE__); exit(-1); }





VkResult vkGetBestComputeQueueNPH(VkPhysicalDevice physicalDevice, uint32_t* queueFamilyIndex) {
    uint32_t queueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, 0);

    VkQueueFamilyProperties* const queueFamilyProperties = (VkQueueFamilyProperties*) alloca(
            sizeof(VkQueueFamilyProperties) * queueFamilyPropertiesCount);

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties);

    // first try and find a queue that has just the compute bit set
    for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
        // mask out the sparse binding bit that we aren't caring about (yet!) and the transfer bit
        const VkQueueFlags maskedFlags = (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT) &
                                          queueFamilyProperties[i].queueFlags);

        if (!(VK_QUEUE_GRAPHICS_BIT & maskedFlags) && (VK_QUEUE_COMPUTE_BIT & maskedFlags)) {
            *queueFamilyIndex = i;
            return VK_SUCCESS;
        }
    }

    // lastly get any queue that'll work for us
    for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
        // mask out the sparse binding bit that we aren't caring about (yet!) and the transfer bit
        const VkQueueFlags maskedFlags = (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT) &
                                          queueFamilyProperties[i].queueFlags);

        if (VK_QUEUE_COMPUTE_BIT & maskedFlags) {
            *queueFamilyIndex = i;
            return VK_SUCCESS;
        }
    }

    return VK_ERROR_INITIALIZATION_FAILED;
}

const char* vkDeviceTypeAsString(VkPhysicalDeviceProperties physicalDeviceProperties) {
    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER) return "VK_PHYSICAL_DEVICE_TYPE_OTHER";
    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) return "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) return "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) return "VK_PHYSICAL_DEVICE_TYPE_CPU";
    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM) return "VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM";
}

const VkPhysicalDevice* vkGetPhysicalDevices(VkInstance instance, uint32_t* num_devices) {
    uint32_t physicalDeviceCount = 0;
    CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, 0));
    VkPhysicalDevice* physicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
    CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices));
    *num_devices = physicalDeviceCount;
    return physicalDevices;
}

int isGPU(VkPhysicalDeviceProperties* pPhysicalDeviceProperties) {
    return pPhysicalDeviceProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || pPhysicalDeviceProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || pPhysicalDeviceProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
}

const VkPhysicalDevice* vkGetGPUs(VkInstance instance) {
    uint32_t num_devices;
    const VkPhysicalDevice* physicalDevices = vkGetPhysicalDevices(instance, &num_devices);

    uint32_t num_gpus = 0;
    for (uint32_t i = 0; i < num_devices; i++) {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
        LOGI("Found device %s of type %s", physicalDeviceProperties.deviceName,
             vkDeviceTypeAsString(physicalDeviceProperties));

        if (isGPU(&physicalDeviceProperties)) {
            num_gpus++;
        }
    }

    VkPhysicalDevice* bestPhysicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * num_gpus);
    uint32_t j = 0;
    for (uint32_t i = 0; i < num_devices; i++) {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
        if (isGPU(&physicalDeviceProperties)) {
            bestPhysicalDevices[j++] = physicalDevices[i];
        }
    }

    return bestPhysicalDevices;
}

const char* cstring(JNIEnv* env, jstring js) {
    const char* cs = (*env)->GetStringUTFChars(env, js, 0);
    return cs;
}

void free_cstring(JNIEnv* env, jstring js, const char* cs) {
    (*env)->ReleaseStringUTFChars(env, js, cs);
}

jsize getArrayLength(JNIEnv* env, jarray arr) {
    return (*env)->GetArrayLength(env, arr);
}

jsize getStringLength(JNIEnv* env, jstring string) {
    return (*env)->GetStringUTFLength(env, string);
}

jbyte* cByteArray(JNIEnv* env, jbyteArray arr) {
    return arr ? (*env)->GetByteArrayElements(env, arr, 0) : NULL;
}

jbyteArray new_jbyteArray(JNIEnv* env, const char* bytes, size_t len) {
    jbyteArray arr = (*env)->NewByteArray(env, len);
    (*env)->SetByteArrayRegion(env, arr, 0, len, bytes);
    return arr;
}

jobject getObject(JNIEnv* env, jobjectArray arr, jsize index) {
    return (*env)->GetObjectArrayElement(env, arr, index);
}

void setObject(JNIEnv* env, jobjectArray arr, jobject obj, jsize index) {
    (*env)->SetObjectArrayElement(env, arr, index, obj);
}

jboolean isIntegerArr(JNIEnv* env, jobjectArray arr) {
    return (*env)->IsInstanceOf(env, arr, (*env)->FindClass(env, "[Ljava/lang/Integer;"));
}

jboolean isDoubleArr(JNIEnv* env, jobjectArray arr) {
    return (*env)->IsInstanceOf(env, arr, (*env)->FindClass(env, "[Ljava/lang/Double;"));
}

jboolean isFloatArr(JNIEnv* env, jobjectArray arr) {
    return (*env)->IsInstanceOf(env, arr, (*env)->FindClass(env, "[Ljava/lang/Float;"));
}

jsize sizeOf(JNIEnv* env, jobjectArray arr) {
    jsize size = 0;
    if (isIntegerArr(env, arr)) size = sizeof(jint);
    else if (isFloatArr(env, arr)) size = sizeof(jfloat);
    else if (isDoubleArr(env, arr)) size = sizeof(jdouble);
    return getArrayLength(env, arr) * size;
}

jsize calcSize(JNIEnv* env, jobjectArray arr) {
    jsize size = 0;
    for (int i = 0; i < getArrayLength(env, arr); i++) {
        size += sizeOf(env, (jobjectArray)getObject(env, arr, i));
    }
    return size;
}

VkDeviceSize calcMemorySize(JNIEnv* env, jobjectArray output, jobjectArray input) {
    return sizeOf(env, output) + calcSize(env, input);
}

jint* getIntArrayElements(JNIEnv* env, jobjectArray arr) {
    jmethodID intValue = (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Integer"), "intValue", "()I");
    jint* ints = (jint*) malloc(sizeof(jint) * getArrayLength(env, arr));
    for (int i = 0; i < getArrayLength(env, arr); i++) {
        ints[i] = (*env)->CallIntMethod(env, getObject(env, arr, i), intValue);
    }
    return ints;
}

jfloat* getFloatArrayElements(JNIEnv* env, jobjectArray arr) {
    jmethodID floatValue = (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Float"), "floatValue", "()F");
    jfloat* floats = (jfloat*) malloc(sizeof(jfloat) * getArrayLength(env, arr));
    for (int i = 0; i < getArrayLength(env, arr); i++) {
        floats[i] = (*env)->CallFloatMethod(env, getObject(env, arr, i), floatValue);
    }
    return floats;
}

jdouble* getDoubleArrayElements(JNIEnv* env, jobjectArray arr) {
    jmethodID doubleValue = (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Double"), "doubleValue", "()D");
    jdouble* doubles = (jdouble*) malloc(sizeof(jdouble) * getArrayLength(env, arr));
    for (int i = 0; i < getArrayLength(env, arr); i++) {
        doubles[i] = (*env)->CallDoubleMethod(env, getObject(env, arr, i), doubleValue);
    }
    return doubles;
}

void setIntArrayElements(JNIEnv* env, jobjectArray jarr, jint* carr) {
    jmethodID newInteger = (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Integer"), "<init>", "(I)V");
    for (int i = 0; i < getArrayLength(env, jarr); i++) {
        setObject(env, jarr, (*env)->NewObject(env, (*env)->FindClass(env, "java/lang/Integer"), newInteger, carr[i]), i);
    }
}

void setFloatArrayElements(JNIEnv* env, jobjectArray jarr, jfloat* carr) {
    jmethodID newFloat = (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Float"), "<init>", "(F)V");
    for (int i = 0; i < getArrayLength(env, jarr); i++) {
        setObject(env, jarr, (*env)->NewObject(env, (*env)->FindClass(env, "java/lang/Float"), newFloat, carr[i]), i);
    }
}

void setDoubleArrayElements(JNIEnv* env, jobjectArray jarr, jdouble* carr) {
    jmethodID newDouble = (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Double"), "<init>", "(D)V");
    for (int i = 0; i < getArrayLength(env, jarr); i++) {
        setObject(env, jarr, (*env)->NewObject(env, (*env)->FindClass(env, "java/lang/Double"), newDouble, carr[i]), i);
    }
}

void* getCArray(JNIEnv* env, jobjectArray arr) {
    if (isIntegerArr(env, arr)) return getIntArrayElements(env, arr);
    if (isFloatArr(env, arr)) return getFloatArrayElements(env, arr);
    if (isDoubleArr(env, arr)) return getDoubleArrayElements(env, arr);
    return NULL;
}

void setJArray(JNIEnv* env, jobjectArray jarr, void* carr) {
    if (isIntegerArr(env, jarr)) setIntArrayElements(env, jarr, carr);
    if (isFloatArr(env, jarr)) setFloatArrayElements(env, jarr, carr);
    if (isDoubleArr(env, jarr)) setDoubleArrayElements(env, jarr, carr);
}

void loadMem(JNIEnv* env, jobjectArray input, void* payload) {
    // payload punta alla memoria allocata per il dispositivo
    jsize input_len = getArrayLength(env, input);
    for (int i = 0; i < input_len; i++) {
        jobjectArray arr = (jobjectArray)getObject(env, input, i);
        jsize size = sizeOf(env, arr);
        memcpy(payload, getCArray(env, arr), size);
        payload += size;
    }
}

VkBuffer vkAllocBuffer(VkDevice device, VkDeviceMemory memory, uint32_t queueFamilyIndex, VkDeviceSize size, VkDeviceSize memoryOffset) {
    const VkBufferCreateInfo bufferCreateInfo = {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            0,
            0,
            size,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            1,
            &queueFamilyIndex
    };
    VkBuffer buffer;
    CHECK(vkCreateBuffer(device, &bufferCreateInfo, 0, &buffer));
    CHECK(vkBindBufferMemory(device, buffer, memory, memoryOffset));
    return buffer;
}

VkDescriptorSet vkGetDescriptorSet(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkBuffer* buffers, size_t buffers_len) {
    VkDescriptorPoolSize descriptorPoolSize = {
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            buffers_len
    };
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            0,
            0,
            1,
            1,
            &descriptorPoolSize
    };
    VkDescriptorPool descriptorPool;
    CHECK(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, 0, &descriptorPool));
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            0,
            descriptorPool,
            1,
            &descriptorSetLayout
    };
    VkDescriptorSet descriptorSet;
    CHECK(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet));
    VkWriteDescriptorSet* writeDescriptorSets = (VkWriteDescriptorSet*) malloc(sizeof(VkWriteDescriptorSet) * buffers_len);
    for (int i = 0; i < buffers_len; i++) {
        VkDescriptorBufferInfo* descriptorBufferInfo = (VkDescriptorBufferInfo*) malloc(sizeof(VkDescriptorBufferInfo));
        descriptorBufferInfo->buffer = buffers[i];
        descriptorBufferInfo->offset = 0;
        descriptorBufferInfo->range = VK_WHOLE_SIZE;
        VkWriteDescriptorSet writeDescriptorSet = {
                VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                0,
                descriptorSet,
                i,
                0,
                1,
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                0,
                descriptorBufferInfo,
                0
        };
        writeDescriptorSets[i] = writeDescriptorSet;
    }
    vkUpdateDescriptorSets(device, buffers_len, writeDescriptorSets, 0, 0);
    return descriptorSet;
}

JNIEXPORT jbyteArray JNICALL
Java_com_lib_androidgpulib_AndroidGPU_build(JNIEnv* env, jobject this, jstring jglsl) {
    const char* cglsl = cstring(env, jglsl);
    size_t spirv_len;
    const char* spirv = compile_glsl_to_spirv(cglsl, getStringLength(env, jglsl), "main", &spirv_len);
    free_cstring(env, jglsl, cglsl);
    return new_jbyteArray(env, spirv, spirv_len);
}

JNIEXPORT void JNICALL
Java_com_lib_androidgpulib_AndroidGPU_run(
        JNIEnv* env,
        jobject this,
        jbyteArray spirv,
        jint dim_x,
        jint dim_y,
        jint dim_z,
        jint local_size_x,
        jint local_size_y,
        jint local_size_z,
        jobjectArray output,
        jobjectArray input) {

    if (!initVulkan()) {
        exit(-1);
    }

    CHECK(VK_ERROR_DEVICE_LOST);

    const VkApplicationInfo applicationInfo = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            0,
            "",
            0,
            "",
            0,
            VK_MAKE_VERSION(1, 0, 9)
    };

    const VkInstanceCreateInfo instanceCreateInfo = {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            0,
            0,
            &applicationInfo,
            0,
            0,
            0,
            0
    };

    VkInstance instance;
    CHECK(vkCreateInstance(&instanceCreateInfo, 0, &instance));

    VkPhysicalDevice gpu = vkGetGPUs(instance)[0];

    uint32_t queueFamilyIndex = 0;
    CHECK(vkGetBestComputeQueueNPH(gpu, &queueFamilyIndex));

    const float queuePrioritory = 1.0f;
    const VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            0,
            0,
            queueFamilyIndex,
            1,
            &queuePrioritory
    };

    const VkDeviceCreateInfo deviceCreateInfo = {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            0,
            0,
            1,
            &deviceQueueCreateInfo,
            0,
            0,
            0,
            0,
            0
    };

    VkDevice device;
    CHECK(vkCreateDevice(gpu, &deviceCreateInfo, 0, &device));

    VkPhysicalDeviceMemoryProperties properties;

    vkGetPhysicalDeviceMemoryProperties(gpu, &properties);

    const VkDeviceSize memorySize = calcMemorySize(env, output, input);

    // set memoryTypeIndex to an invalid entry in the properties.memoryTypes array
    uint32_t memoryTypeIndex = VK_MAX_MEMORY_TYPES;

    for (uint32_t k = 0; k < properties.memoryTypeCount; k++) {
        if ((VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & properties.memoryTypes[k].propertyFlags) &&
            (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & properties.memoryTypes[k].propertyFlags) &&
            (memorySize < properties.memoryHeaps[properties.memoryTypes[k].heapIndex].size)) {
            memoryTypeIndex = k;
            break;
        }
    }

    CHECK(memoryTypeIndex == VK_MAX_MEMORY_TYPES ? VK_ERROR_OUT_OF_HOST_MEMORY : VK_SUCCESS);

    const VkMemoryAllocateInfo memoryAllocateInfo = {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            0,
            memorySize,
            memoryTypeIndex
    };

    VkDeviceMemory memory;
    CHECK(vkAllocateMemory(device, &memoryAllocateInfo, 0, &memory));

    void* payload;
    CHECK(vkMapMemory(device, memory, 0, memorySize, 0, (void *)&payload));

    loadMem(env, input, payload);

    vkUnmapMemory(device, memory);

    size_t buffers_len = getArrayLength(env, input) + 1;
    VkBuffer* buffers = (VkBuffer*) malloc(sizeof(VkBuffer) * buffers_len);
    VkDeviceSize memoryOffset = 0;
    for (int i = 0; i < buffers_len - 1; i++) {
        VkDeviceSize size = sizeOf(env, getObject(env, input, i));
        buffers[i] = vkAllocBuffer(device, memory, queueFamilyIndex, size, memoryOffset);
        memoryOffset += size;
    }
    buffers[buffers_len - 1] = vkAllocBuffer(device, memory, queueFamilyIndex, sizeOf(env, output),
                                             memoryOffset);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            0,
            0,
            getArrayLength(env, spirv),
            (const uint32_t *) cByteArray(env, spirv)
    };
    VkShaderModule shader_module;
    CHECK(vkCreateShaderModule(device, &shaderModuleCreateInfo, 0, &shader_module));

    VkDescriptorSetLayoutBinding* descriptorSetLayoutBindings = (VkDescriptorSetLayoutBinding*) malloc(sizeof(VkDescriptorSetLayoutBinding) * buffers_len);
    for (int i = 0; i < buffers_len; i++) {
        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
                i,
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                1,
                VK_SHADER_STAGE_COMPUTE_BIT,
                0
        };
        descriptorSetLayoutBindings[i] = descriptorSetLayoutBinding;
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            0,
            0,
            buffers_len,
            descriptorSetLayoutBindings
    };

    VkDescriptorSetLayout descriptorSetLayout;
    CHECK(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, 0, &descriptorSetLayout));

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            0,
            0,
            1,
            &descriptorSetLayout,
            0,
            0
    };

    VkPipelineLayout pipelineLayout;
    CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, 0, &pipelineLayout));

    VkComputePipelineCreateInfo computePipelineCreateInfo = {
            VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            0,
            0,
            {
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    0,
                    0,
                    VK_SHADER_STAGE_COMPUTE_BIT,
                    shader_module,
                    "main",
                    0
            },
            pipelineLayout,
            0,
            0
    };

    VkPipeline pipeline;
    CHECK(vkCreateComputePipelines(device, 0, 1, &computePipelineCreateInfo, 0, &pipeline));

    VkDescriptorSet descriptorSet = vkGetDescriptorSet(device, descriptorSetLayout, buffers,
                                                       buffers_len);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            0,
            0,
            queueFamilyIndex
    };
    VkCommandPool commandPool;
    CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, 0, &commandPool));

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            0,
            commandPool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            1
    };

    VkCommandBuffer commandBuffer;
    CHECK(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer));

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            0,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            0
    };

    CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, 0);
    vkCmdDispatch(commandBuffer, ceil(dim_x / (double) local_size_x), ceil(dim_y / (double) local_size_y), ceil(dim_z / (double) local_size_z));
    CHECK(vkEndCommandBuffer(commandBuffer));

    VkQueue queue;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

    VkSubmitInfo submitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            0,
            0,
            0,
            0,
            1,
            &commandBuffer,
            0,
            0
    };

    CHECK(vkQueueSubmit(queue, 1, &submitInfo, 0));

    CHECK(vkQueueWaitIdle(queue));

    CHECK(vkMapMemory(device, memory, 0, memorySize, 0, (void *)&payload));

    setJArray(env, output, payload + calcSize(env, input));
}