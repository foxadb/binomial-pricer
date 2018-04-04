#include <CL/cl.hpp>
#include <string>

class AmericanOptionGPU {
public:

    AmericanOptionGPU(double X0, double K, double r, double sigma, double T, int N);
    ~AmericanOptionGPU();

    std::string toString();
    double pricing(int groupSize);

private:
    double X0;
    double K;
    double r;
    double sigma;
    double T;
    size_t N;

    double h;
    double u;
    double d;
    double p;

    cl::Platform platform;
    cl::Device device;
    cl::Context context;

    cl::Program program;
    cl::Program::Sources sources;
};
