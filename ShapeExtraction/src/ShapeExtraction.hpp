#include <itkImage.h>
const unsigned int dimension = 3;
typedef unsigned short PixelType;
typedef itk::Image<PixelType,dimension> ImageType;

#include <itkImageFileReader.h>
typedef itk::ImageFileReader<ImageType> ReaderType;

typedef float fPixelType;
typedef itk::Image<fPixelType,dimension> fImageType;


#include "itkLineShapeImageFilter.h"
typedef itk::LineShapeImageFilter<ImageType> LineShapeFilterType;

#include "itkImageSliceIteratorWithIndex.h"
typedef itk::ImageSliceIteratorWithIndex< fImageType
> SliceIteratorType;
typedef itk::ImageSliceConstIteratorWithIndex< fImageType
> SliceConstIteratorType;


#include <itkImageFileWriter.h>
typedef itk::ImageFileWriter<fImageType> WriterType;
/*
#include "itkImageSliceConstIteratorWithIndex.h"





#include "itkCastImageFilter.h"
typedef itk::CastImageFilter<intImageType,ImageType> CasterType;

*/
