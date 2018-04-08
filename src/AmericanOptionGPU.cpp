#include "AmericanOptionGPU.hpp"

#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

AmericanOptionGPU::AmericanOptionGPU(float X0, float K, float r, float sigma, float T, int N) {
    this->X0 = X0;
    this->K = K;
    this->r = r;
    this->sigma = sigma;
    this->T = T;
    this->N = N;

    this->h = T / N;
    this->u = std::exp(sigma * std::sqrt(this->h));
    this->d = 1 / this->u;
    this->p = (d - std::exp(r * this->h)) / (d - u);

    // Get OpenCL platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
        std::cout << "No platforms found. Check OpenCL installation!" << std::endl;
        exit(1);
    }
    this->platform = all_platforms[0];
    std::cout << "Using platform: "
              << this->platform.getInfo<CL_PLATFORM_NAME>()
              << std::endl;

    // Get default device of the platform
    std::vector<cl::Device> all_devices;
    this->platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.size() == 0) {
        std::cout << "No devices found. Check OpenCL installation!" << std::endl;
        exit(1);
    }
    this->device = all_devices[0];
    std::cout << "Using device: "
              << this->device.getInfo<CL_DEVICE_NAME>()
              << std::endl;

    // Create context
    this->context = cl::Context(this->device);

    // Define kernel code
    std::ifstream ifs("src/kernel.cl");
    std::string kernel_code(std::istreambuf_iterator<char>(ifs),
                            (std::istreambuf_iterator<char>()));
    this->sources.push_back({kernel_code.c_str(), kernel_code.length()});

    // Build program
    this->program = cl::Program(this->context, this->sources);
    if (this->program.build({this->device}) != CL_SUCCESS) {
        std::cout << "Error building: "
                  << this->program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->device)
                  << std::endl;
        exit(1);
    }
}

AmericanOptionGPU::~AmericanOptionGPU() {}

std::string AmericanOptionGPU::toString() {
    std::string result = "AmericanOptionGPU { X0 = " + std::to_string(this->X0)
            + ", K = " + std::to_string(this->K)
            + ", r = " + std::to_string(this->r)
            + ", sigma = " + std::to_string(this->sigma)
            + ", T = " + std::to_string(this->T)
            + ", N = " + std::to_string(this->N)
            +" }";
    return result;
}

float AmericanOptionGPU::pingPongPricing(int groupSize) {
    // Create buffers on the device
    cl::Buffer buffer_A(this->context, CL_MEM_READ_WRITE, sizeof(float) * this->N);
    cl::Buffer buffer_B(this->context, CL_MEM_READ_WRITE, sizeof(float) * this->N);

    // Create queue to which we will push commands for the device.
    cl::CommandQueue queue(this->context, this->device);

    // Initialize prices
    cl::Kernel initialKernel = cl::Kernel(this->program, "initial");
    initialKernel.setArg(0, sizeof(cl_int), &this->N);
    initialKernel.setArg(1, sizeof(cl_float), &this->X0);
    initialKernel.setArg(2, sizeof(cl_float), &this->K);
    initialKernel.setArg(3, sizeof(cl_float), &this->d);
    initialKernel.setArg(4, buffer_A);
    queue.enqueueNDRangeKernel(initialKernel, cl::NullRange, cl::NDRange(this->N), cl::NullRange);

    // Block until init kernel finishes execution
    queue.enqueueBarrierWithWaitList();

    // Compute the discount factor
    float discountFactor = 1 / std::exp(this->r * this->h);

    // Compute binomial prices
    cl::Kernel pingPongKernel = cl::Kernel(this->program, "pingPong");
    pingPongKernel.setArg(0, sizeof(cl_float), &this->X0);
    pingPongKernel.setArg(1, sizeof(cl_float), &this->K);
    pingPongKernel.setArg(2, sizeof(cl_float), &this->d);
    pingPongKernel.setArg(3, sizeof(cl_float), &this->p);
    pingPongKernel.setArg(4, sizeof(cl_float), &discountFactor);
    pingPongKernel.setArg(5, sizeof(cl_int), &groupSize);
    for (int i = 0; i < N - 1; ++i) {
        pingPongKernel.setArg(6, i % 2 ? buffer_B : buffer_A);
        pingPongKernel.setArg(7, i % 2 ? buffer_A : buffer_B);

        int nodesNb = this->N - 1 - i;
        pingPongKernel.setArg(8, sizeof(cl_int), &nodesNb);

        int workItemsNb = std::ceil((float) nodesNb / groupSize);
        queue.enqueueNDRangeKernel(pingPongKernel, cl::NullRange,
                                   cl::NDRange(workItemsNb), cl::NullRange);

        // Block until init kernel finishes execution
        queue.enqueueBarrierWithWaitList();
    }

    // Finish queue
    queue.finish();

    // Return price at 0
    float price = -1;
    queue.enqueueReadBuffer(this->N % 2 ? buffer_A : buffer_B, CL_TRUE, 0, sizeof(float), &price);
    return price;
}

float AmericanOptionGPU::branchClimbingPricing() {
    int maxWorkGroupSize = this->device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    if (this->N > maxWorkGroupSize) {
        std::cerr << "Max Group Size is limited to " << maxWorkGroupSize
                  << " by the OpenCL device" << std::endl
                  << "This functionnality may not work correctly"
                  << std::endl;
    }

    // Create queue to which we will push commands for the device.
    cl::CommandQueue queue(this->context, this->device);

    // Compute the discount factor
    float discountFactor = 1 / std::exp(this->r * this->h);

    // Buffer containing the price at 0
    cl::Buffer buffer_finalPrice(this->context, CL_MEM_READ_WRITE, sizeof(float));

    // Compute binomial prices
    cl::Kernel branchClimbKernel = cl::Kernel(this->program, "branchClimbing");
    branchClimbKernel.setArg(0, sizeof(cl_int), &this->N);
    branchClimbKernel.setArg(1, sizeof(cl_float), &this->X0);
    branchClimbKernel.setArg(2, sizeof(cl_float), &this->K);
    branchClimbKernel.setArg(3, sizeof(cl_float), &this->d);
    branchClimbKernel.setArg(4, sizeof(cl_float), &this->p);
    branchClimbKernel.setArg(5, sizeof(cl_float), &discountFactor);
    branchClimbKernel.setArg(6, cl::Local(sizeof(float) * this->N));
    branchClimbKernel.setArg(7, buffer_finalPrice);
    queue.enqueueNDRangeKernel(branchClimbKernel, cl::NullRange,
                               cl::NDRange(this->N), cl::NDRange(this->N));

    // Finish queue
    queue.finish();

    // Return price at 0
    float price = -1;
    queue.enqueueReadBuffer(buffer_finalPrice, CL_TRUE, 0, sizeof(float), &price);
    return price;
}
