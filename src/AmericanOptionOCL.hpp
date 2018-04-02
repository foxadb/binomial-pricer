#include <CL/cl.hpp>
#include <string>

class AmericanOptionOCL {
public:

    AmericanOptionOCL(double X0, double K, double r, double sigma, double T, int N);
    ~AmericanOptionOCL();

    std::string toString();
    double payoff(double stock);
    void pricing();
    double getPrice(int i, int j);

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

    double* prices;

    cl::Platform platform;
    cl::Device device;
    cl::Context context;

    cl::Program program;
    cl::Program::Sources sources;
};
