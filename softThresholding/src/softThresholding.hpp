#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

const int dim=3;
typedef  float PixelType;
typedef itk::Image<PixelType, dim> ImageType;

typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
