#ifndef _SELECTAREA_HPP_
#define _SELECTAREA_HPP_

#include "itkImageRegionIterator.h"

#include "itkImage.h"
const int Dim=3;
typedef float PixelType;
typedef itk::Image<PixelType,Dim> ImageType;

ImageType::Pointer selectArea(ImageType::Pointer image,
                              int startX, int startY,
                              int startZ, int sizeX,
                              int sizeY, int sizeZ);

typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< ImageType>       IteratorType;

#endif
