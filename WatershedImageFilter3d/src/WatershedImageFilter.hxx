#include <iostream>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkVectorCastImageFilter.h"
#include "itkVectorGradientAnisotropicDiffusionImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"

#include "itkImage.h"


#include "itkImageSeriesReader.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkExtractImageFilter.h"
#include "itkJoinSeriesImageFilter.h"

// Image IO

#include "itkNrrdImageIO.h"

#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
// Find on : itk.org
//Run as : ./WatershedImageFilter threshold level
// eg ./WatershedImagefilter 0.005 0.5
// ( a rule of thumb is to set the threshold to be about 1/100 of the level)

//Convert the final watershed to a binary image (also exists LabelMapMaskInageFilter)
#include <itkLabelMapToBinaryImageFilter.h>
#include <itkImageToImageFilter.h>
#include <itkCastImageFilter.h>

//We first realize this in 2D then in 3D

typedef unsigned long PixelType;
typedef unsigned char RGBPixelSType;
typedef itk::Image<PixelType ,2>     UnsignedCharImageType;
typedef itk::Image<float,2>             FloatImageType;

typedef itk::RGBPixel<PixelType >    RGBPixelType;
typedef itk::Image<RGBPixelType,2>      RGBImageType;
typedef itk::Image<long unsigned int,3> LabeledImageType;
typedef itk::Image<unsigned short, 3 > SLabelImageType;

typedef itk::Image<PixelType,3> UnsignedCharImageStackType;
typedef itk::Image< float, 3 > GradImageType;

typedef itk::Image<RGBPixelType,3> RGBImageStackType;

typedef itk::ImageSeriesReader<UnsignedCharImageStackType> ReaderStackType;
//typedef itk::ImageFileWriter<LabeledImageType> WriterStackType;
//typedef itk::TIFFImageIO TIFFIOType;
//typedef itk::LabelMapToBinaryImageFilter<LabeledImageType,UnsignedCharImageStackType> TLabelToBinary;

typedef itk::CastImageFilter<LabeledImageType,SLabelImageType> TImtoImFilter;
typedef itk::ImageFileWriter<UnsignedCharImageStackType> WriterStackType;
typedef itk::ImageFileWriter<SLabelImageType> LabelWriterStackType;
typedef itk::ImageFileWriter<LabeledImageType> LabelWriterT;


typedef itk::NumericSeriesFileNames NameGeneratorType;

//iterators
typedef itk::ImageSliceConstIteratorWithIndex<UnsignedCharImageStackType> SliceConstIteratorType;
//Extractor
typedef itk::ExtractImageFilter<UnsignedCharImageStackType, UnsignedCharImageType> ExtractFilterType;
//Insertor
typedef itk::JoinSeriesImageFilter<RGBImageType,RGBImageStackType> JoinSeriesFilterType;

typedef itk::ResampleImageFilter<RGBImageStackType, RGBImageStackType> ResampleFilterType;
typedef itk::AffineTransform<double, 3> AffineTransformType;

