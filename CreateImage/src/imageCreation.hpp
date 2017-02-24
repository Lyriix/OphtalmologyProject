#include "itkImage.h"


#include "itkImageFileWriter.h"
#include "itkNrrdImageIO.h"
#include "itkNiftiImageIO.h"

typedef signed short PixelType;
typedef itk::Image<PixelType, 3> ImageType;
typedef itk::ImageFileWriter<ImageType> WriterType;


#include "itkGroupSpatialObject.h"
#include "itkEllipseSpatialObject.h"
#include "itkCylinderSpatialObject.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkPlaneSpatialObject.h"
typedef itk::EllipseSpatialObject< 3 >   EllipseType;
  typedef itk::CylinderSpatialObject               CylinderType;
  typedef itk::GroupSpatialObject< 3 >     GroupType;
typedef itk::PlaneSpatialObject<3> PlaneType;


typedef itk::SpatialObjectToImageFilter<
    GroupType, ImageType >   SpatialObjectToImageFilterType;
