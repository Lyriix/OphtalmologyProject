#include "gaussianGenerator.hpp"


//Generation a gaussian integer for deviation value
int GaussianGenerator()
{
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0,3.0);

    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration d = myclock::now() - beginning;

    double randSeed = rand();
    unsigned seed2 = d.count() * randSeed;
    generator.seed( seed2 );
    double value = distribution(generator);
    return round(value);

    //     const int nrolls=10000;  // number of experiments
    //       const int nstars=100;
    //        int p[20]={};

    //         for (int i=0; i<nrolls; ++i) {
    //           double number = distribution(generator) ;
    //           if ((number>=-10.0)&&(number<10.0)) ++p[int(number) + 10];
    //         }

    //         std::cout << "normal_distribution (5.0,2.0):" << std::endl;

    //         for (int i=0; i<20; ++i) {
    //           std::cout << i << "-" << (i+1) << ": ";
    //           std::cout << std::string(p[i]*nstars/nrolls,'*') << std::endl;
    //         }


}

int GaussianGenerator(double mean, double stdDev)
{
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean,stdDev);

    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning2 = myclock::now();
    myclock::duration d = myclock::now() - beginning2;

    double randSeed = rand();
    unsigned seed2 = d.count() * randSeed;
    generator.seed( seed2 );
    double value = distribution(generator);
    //std::cout<< round(value) << std::endl ;
    return round(value);

}
