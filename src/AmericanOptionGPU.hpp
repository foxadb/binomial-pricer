#include <CL/cl.hpp>
#include <string>

class AmericanOptionGPU {
public:

    AmericanOptionGPU(float X0, float K, float r, float sigma, float T, int N);
    ~AmericanOptionGPU();

    std::string toString();
    float pingPongPricing(int groupSize);
    float branchClimbPricing();

private:
    float X0;
    float K;
    float r;
    float sigma;
    float T;
    int N;

    float h;
    float u;
    float d;
    float p;

    cl::Platform platform;
    cl::Device device;
    cl::Context context;

    cl::Program program;
    cl::Program::Sources sources;
};
