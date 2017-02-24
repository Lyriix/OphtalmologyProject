


#include <vector>
#include "gaussianGenerator.hpp"
enum Selection{ALL, UNIFORM, GAUSSIAN_PIX}; //how pixel of the neighboorhood are chosen
enum Choice {RANDOM, GAUSSIAN_DEV}; //How the deviation value is chosen
std::vector<unsigned short> accessEachOnce(std::vector<unsigned short> pixelVector, Choice choice);
std::vector<unsigned short> accessUniform(std::vector<unsigned short> pixelVector, Choice choice);
std::vector<unsigned short> accessGaussian(std::vector<unsigned short> pixelVector, Choice choice);

