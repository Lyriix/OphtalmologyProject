#include "itkImageRegionIterator.h"

#include "volume.hpp"

ImageType::Pointer selectArea(ImageType::Pointer image,
                              int startX, int startY,
                              int startZ, int sizeX,
                              int sizeY, int sizeZ);

typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< ImageType>       IteratorType;

