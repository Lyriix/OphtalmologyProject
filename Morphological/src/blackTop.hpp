#include "types.hpp"

#include "itkBlackTopHatImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

ImageType::Pointer PerformBlackTopHat(ImageType::Pointer&, float radius);

typedef itk::BinaryBallStructuringElement <PixelType, 3 > StructuringElementType;

typedef itk::BlackTopHatImageFilter<ImageType, ImageType, StructuringElementType > BlackTopHatFilterType;
