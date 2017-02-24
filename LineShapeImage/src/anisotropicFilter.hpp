
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"

#include "volume.hpp"


//Should be generalized to N-Dimensions

//typedef itk::CurvatureAnisotropicDiffusionImageFilter<
  //  ImageType, ImageType > AnisotropicFilterType;
typedef itk::GradientAnisotropicDiffusionImageFilter<
    ImageType, RealImageType > AnisotropicFilterType;

