#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include "itkImage.h"

typedef unsigned char PixelType;
const int dimension = 3;
typedef itk::Image<PixelType,dimension> ImageType;

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
typedef itk::ImageFileReader< ImageType > ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;

#include "itkSubtractImageFilter.h"
typedef itk::SubtractImageFilter<ImageType> SubtractType;

#endif
