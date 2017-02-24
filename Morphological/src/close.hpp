#include "types.hpp"

#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryBallStructuringElement.h"


ImageType::Pointer PerformClosing(ImageType::Pointer&, float radius);

typedef itk::BinaryBallStructuringElement
    < PixelType, 3 >
    StructuringElementType;

typedef itk::BinaryMorphologicalClosingImageFilter
<ImageType, ImageType, StructuringElementType>
BinaryMorphologicalClosingImageFilterType;

