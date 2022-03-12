#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int status;
    size_t global;

    cl_command_queue commands = clCreateCommandQueue(*context, *device, 0, &status);
    CHECK(status, "clCreateCommandQueue");

    cl_kernel kernel = clCreateKernel(*program, "convolution", &status);
    CHECK(status, "clCreateKernel");

    cl_mem d_inputImage = clCreateBuffer(*context,  CL_MEM_READ_ONLY, sizeof(float) * imageHeight * imageWidth, NULL, NULL);
    cl_mem d_outputImage = clCreateBuffer(*context, CL_MEM_WRITE_ONLY, sizeof(float) * imageHeight * imageWidth, NULL, NULL);
    cl_mem d_filter = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(float) * filterWidth * filterWidth, NULL, NULL);
    status = !d_inputImage || !d_outputImage || !d_filter;
    CHECK(status, "clCreateBuffer");

    status = 0;
    status |= clEnqueueWriteBuffer(commands, d_inputImage, CL_TRUE, 0, sizeof(float) * imageHeight * imageWidth, inputImage, 0, NULL, NULL);
    status |= clEnqueueWriteBuffer(commands, d_filter, CL_TRUE, 0, sizeof(float) * filterWidth * filterWidth, filter, 0, NULL, NULL);
    CHECK(status, "clEnqueueWriteBuffer");

    status = 0;
    status |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_inputImage);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_filter);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_outputImage);
    status |= clSetKernelArg(kernel, 3, sizeof(cl_int), &imageHeight);
    status |= clSetKernelArg(kernel, 4, sizeof(cl_int), &imageWidth);
    status |= clSetKernelArg(kernel, 5, sizeof(cl_int), &filterWidth);
    CHECK(status, "clSetKernelArg");

    // size_t local;
    // status = clGetKernelWorkGroupInfo(kernel, *device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    // printf("Local Size = %d\n", CL_KERNEL_WORK_GROUP_SIZE);

    global = imageHeight * imageWidth;
    status = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);
    CHECK(status, "clEnqueueNDRangeKernel");

    status = clEnqueueReadBuffer(commands, d_outputImage, CL_TRUE, 0, sizeof(float) * imageHeight * imageWidth, outputImage, 0, NULL, NULL);
    CHECK(status, "clEnqueueReadBuffer");
}