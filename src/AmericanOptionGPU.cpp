#include "AmericanOptionGPU.hpp"

#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

AmericanOptionGPU::AmericanOptionGPU(double X0, double K, double r, double sigma, double T, int N) {
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

double AmericanOptionGPU::pricing(int groupSize) {
    // Create buffers on the device
    cl::Buffer buffer_A(this->context, CL_MEM_READ_WRITE, sizeof(double) * this->N);
    cl::Buffer buffer_B(this->context, CL_MEM_READ_WRITE, sizeof(double) * this->N);

    // Create queue to which we will push commands for the device.
    cl::CommandQueue queue(this->context, this->device);

    // Initialize prices
    cl::Kernel initial_prices = cl::Kernel(this->program, "initial_prices");
    initial_prices.setArg(0, buffer_A);
    initial_prices.setArg(1, sizeof(cl_int), &this->N);
    initial_prices.setArg(2, sizeof(cl_double), &this->X0);
    initial_prices.setArg(3, sizeof(cl_double), &this->K);
    initial_prices.setArg(4, sizeof(cl_double), &this->u);
    initial_prices.setArg(5, sizeof(cl_double), &this->d);
    queue.enqueueNDRangeKernel(initial_prices, cl::NullRange, cl::NDRange(this->N), cl::NullRange);

    // Block until init kernel finishes execution
    queue.enqueueBarrierWithWaitList();

    // Compute binomial prices
    cl::Kernel binomial_pricer = cl::Kernel(this->program, "binomial_pricer");
    binomial_pricer.setArg(0, sizeof(cl_int), &this->N);
    binomial_pricer.setArg(1, sizeof(cl_double), &this->X0);
    binomial_pricer.setArg(2, sizeof(cl_double), &this->K);
    binomial_pricer.setArg(3, sizeof(cl_double), &this->r);
    binomial_pricer.setArg(4, sizeof(cl_double), &this->h);
    binomial_pricer.setArg(5, sizeof(cl_double), &this->u);
    binomial_pricer.setArg(6, sizeof(cl_double), &this->d);
    binomial_pricer.setArg(7, sizeof(cl_double), &this->p);
    binomial_pricer.setArg(8, sizeof(cl_int), &groupSize);
    for (int i = 0; i < N - 1; ++i) {
        binomial_pricer.setArg(9, i % 2 ? buffer_B : buffer_A);
        binomial_pricer.setArg(10, i % 2 ? buffer_A : buffer_B);

        int nodesNb = this->N - 1 - i;
        binomial_pricer.setArg(11, sizeof(cl_int), &nodesNb);

        int workItemsNb = std::ceil((double) nodesNb / groupSize);
        queue.enqueueNDRangeKernel(binomial_pricer, cl::NullRange, cl::NDRange(workItemsNb), cl::NullRange);

        // Block until init kernel finishes execution
        queue.enqueueBarrierWithWaitList();
    }

    // Finish queue
    queue.finish();

    // Return price at 0
    double price = -1;
    queue.enqueueReadBuffer(this->N % 2 ? buffer_A : buffer_B, CL_TRUE, 0, sizeof(double), &price);
    return price;
}
