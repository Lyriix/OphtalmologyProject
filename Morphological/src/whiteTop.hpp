#include "types.hpp"

#include "itkWhiteTopHatImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

ImageType::Pointer PerformWhiteTopHat(ImageType::Pointer&, float radius);

typedef itk::BinaryBallStructuringElement <PixelType, 3 > StructuringElementType;

typedef itk::WhiteTopHatImageFilter<ImageType, ImageType, StructuringElementType > WhiteTopHatFilterType;
