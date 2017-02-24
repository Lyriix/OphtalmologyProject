//#include "selectArea.hpp"
//#include "anisotropicFilter.hpp"
#include <string>
#include <sys/stat.h>
#include <sys/types.h>


typedef  float PixelType;
const int dim = 3;
#include "itkImage.h"
typedef itk::Image<PixelType,dim> ImageType;

#include "itkImageFileReader.hxx"
typedef itk::ImageFileReader<ImageType> ReaderType;
#include "itkImageFileWriter.hxx"
typedef itk::ImageFileWriter<ImageType> WriterType;
