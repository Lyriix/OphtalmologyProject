#include "itkImage.h"
#include "itkImageFileReader.hxx"
#include "itkImageFileWriter.hxx"

typedef float PixelType;
typedef itk::Image<PixelType,3> ImageType;

typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;

