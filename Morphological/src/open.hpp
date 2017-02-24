#include "types.hpp"

#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

ImageType::Pointer PerformOpening(ImageType::Pointer&, float radius);

typedef itk::BinaryBallStructuringElement
    < ImageType::PixelType, ImageType::ImageDimension >
    StructuringElementType;

typedef itk::BinaryMorphologicalOpeningImageFilter
<ImageType, ImageType, StructuringElementType>
BinaryMorphologicalOpeningImageFilterType;
