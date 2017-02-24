#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"

#include "main.hpp"


//Should be generalized to N-Dimensions

typedef itk::CurvatureAnisotropicDiffusionImageFilter<
    UnsignedCharImageStackType, UnsignedCharImageStackType > AnisotropicFilterType;
//typedef itk::GradientAnisotropicDiffusionImageFilter<
//    UnsignedCharImageStackType, UnsignedCharImageStackType > AnisotropicFilterType;

UnsignedCharImageStackType::Pointer PerformDenoisement(UnsignedCharImageStackType::Pointer stack, int numberOfIterations,
                        float timeStep, float conductance);
