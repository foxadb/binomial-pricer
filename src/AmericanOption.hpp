#include <string>

class AmericanOption {
public:

    AmericanOption(double X0, double K, double r, double sigma, double T, int N);
    ~AmericanOption();

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
    int N;

    double h;
    double u;
    double d;
    double p;

    double *prices;
};
