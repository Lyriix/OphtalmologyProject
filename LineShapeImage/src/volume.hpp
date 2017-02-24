#ifndef VOLUME_HPP
#define VOLUME_HPP

#include "itkImage.h"

const unsigned int Dim = 3;
typedef float RealType;
typedef unsigned char BinType;

typedef unsigned short 					PixelType;
typedef itk::Image< PixelType, Dim > 	ImageType;
typedef itk::Image< RealType, Dim > 	RealImageType;
typedef itk::ImageRegion< Dim >			ImageRegionType;
typedef itk::Image< BinType, Dim > 		LabelType;

typedef itk::Image< unsigned short , Dim > 	SegLabelType;
#endif
