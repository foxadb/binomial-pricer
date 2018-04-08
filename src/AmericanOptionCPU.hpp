#include <string>

class AmericanOptionCPU {
public:

    AmericanOptionCPU(float X0, float K, float r, float sigma, float T, int N);
    ~AmericanOptionCPU();

    std::string toString();
    float payoff(float stock);
    float pricing();
    float getPrice(int i, int j);

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

    float** prices;
};
