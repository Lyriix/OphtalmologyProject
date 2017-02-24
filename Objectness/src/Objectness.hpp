/**
  * Code inspired from Insight Toolkit implementation
  * "Generalizing vesselness with respect to dimensionnality shape
  * Luca Antiga"
  *
  * Modified bu Charly Girot, NYU, Computer Science
  */


#include "itkHessianToObjectnessMeasureImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImage.h"

const unsigned char Dim = 3;
typedef float PixelType;

//Declare the types of the images
typedef itk::Image<PixelType,Dim> ImageType;

typedef itk::ImageFileReader<ImageType> ImageReaderType;
typedef itk::ImageFileWriter<ImageType> ImageWriterType;

typedef itk::RescaleIntensityImageFilter<ImageType> RescaleFilterType;

//Declare tye type of enahcement filter
typedef itk::SymmetricSecondRankTensor< double, Dim > HessianPixelType;
typedef itk::Image< HessianPixelType, Dim >           HessianImageType;
typedef itk::HessianToObjectnessMeasureImageFilter<HessianImageType,ImageType> ObjectnessFilterType;

//Declare the type of multiscale enhancement filter
typedef itk::MultiScaleHessianBasedMeasureImageFilter<ImageType, HessianImageType, ImageType> MultiScaleEnhancementFilterType;

