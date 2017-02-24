#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
typedef unsigned char PixelType;
typedef itk::Image<PixelType, 3> ImageType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
#include <itkGrayscaleFillholeImageFilter.h>
typedef itk::GrayscaleFillholeImageFilter<ImageType,ImageType> FilterType;

#include "itkBinaryThresholdImageFilter.h"
typedef itk::BinaryThresholdImageFilter<
ImageType, ImageType >  BinarizeType;

#include "itkVotingBinaryIterativeHoleFillingImageFilter.h"
typedef itk::VotingBinaryIterativeHoleFillingImageFilter<
ImageType >  VotingType;
int main(int argc, char*argv[])
{

    if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0 )
    {
        std::cout << "PostProcess OCT images beans/pocket extractions"<<std::endl;
        std::cout << "-input filename (a binary image)"<< std::endl << "-outputFilename" <<std::endl;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);
    reader->Update();

    BinarizeType::Pointer binarizer = BinarizeType::New();
    binarizer->SetInput(reader->GetOutput());
    binarizer->SetInsideValue(255);
    binarizer->SetOutsideValue(0);
    binarizer->SetLowerThreshold(10);
    binarizer->SetUpperThreshold(255);
    binarizer->Update();

    VotingType::Pointer voting = VotingType::New();
    ImageType::SizeType indexRadius;
    indexRadius[0] = 5.0;
    indexRadius[1] = 5.0;
    indexRadius[2] = 5.0;

    voting->SetRadius(indexRadius);
    voting->SetBackgroundValue(0);
    voting->SetForegroundValue(255);
    voting->SetMajorityThreshold( 2 );
    voting->SetMaximumNumberOfIterations( 1);
    voting->SetInput(binarizer->GetOutput());
    voting->Update();
    //    FilterType::Pointer filler = FilterType::New();
    //    filler->SetInput(binarizer->GetOutput());
    //    filler->FullyConnectedOn();
    //    filler->Update();


    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(voting->GetOutput());
    writer->Update();



}
