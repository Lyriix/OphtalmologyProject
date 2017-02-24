#include "itkMedianImageFilter.h"
#include "itkImage.h"


typedef  float PixelType;
const int Dimension = 3;

typedef itk::Image<PixelType,Dimension>     ImageType;
typedef itk::MedianImageFilter<ImageType,ImageType> MedianFilterType;


ImageType::Pointer PerformDenoisement(ImageType::Pointer stack);
