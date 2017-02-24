#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkLabelToRGBImageFilter.h"
#include "itksys/SystemTools.hxx"
#include <sstream>


const unsigned int Dimension = 3;
typedef unsigned char                       PixelType;
typedef itk::Image<PixelType, Dimension>    ImageType;
typedef itk::Image<unsigned short, Dimension> OutputImageType;

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<OutputImageType> WriterType;


typedef itk::ConnectedComponentImageFilter<ImageType, OutputImageType> ConnectedComponentImageFilterType;

