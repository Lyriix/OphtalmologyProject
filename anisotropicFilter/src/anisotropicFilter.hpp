#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkImage.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"

typedef  float PixelType;
const int Dimension = 3;

typedef itk::Image<PixelType,Dimension>     StackType;
typedef itk::Image<PixelType,Dimension-1>   SliceType;

//Should be generalized to N-Dimensions
//typedef itk::GradientAnisotropicDiffusionImageFilter<StackType,StackType> AnisotropicFilterType;
typedef itk::CurvatureAnisotropicDiffusionImageFilter<
StackType, StackType > AnisotropicFilterType;


StackType::Pointer PerformDenoisement(StackType::Pointer stack, int numberOfIterations,
                        float timeStep, float conductance);
