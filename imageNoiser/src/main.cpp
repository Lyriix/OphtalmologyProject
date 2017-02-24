#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <vector>
#include <string>
#include <chrono>
#include <time.h>
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
void CloudPoints();

int main(int argc, char *argv[])
{
    std::string fileName;
    std::string fileOutName;

    switch(argc)
    {
    case 1 :
        fileName = "blanc.jpg";
        fileOutName = "blancOut.jpg";
        break;
    case 2:
        fileName = argv[1];
        fileOutName = "whiteOut.jpg";
        break;
    case 3:
        fileName = argv[1];
        fileOutName = argv[2];
        break;
    }

    //Image Reader
    typedef unsigned short                  PixelType;
    typedef itk::Image<PixelType, 2>        ImageType;
    typedef itk::ImageFileReader<ImageType> ReaderType;

    ReaderType::Pointer readerImage = ReaderType::New();
    readerImage->SetFileName(fileName);
    readerImage->Update();

    ImageType::Pointer image = readerImage->GetOutput();
    ImageType::RegionType region = image->GetLargestPossibleRegion();
    ImageType::SizeType size = region.GetSize();

    std::cout << "Width : "<< size[0] << "; Height : " << size[1] <<std::endl;


    //CloudPoints
    int nbPoints = 100000;

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distributionX(0,size[0]);
    std::uniform_int_distribution<int> distributionY(0,size[1]);
    std::uniform_int_distribution<int> distributionValue(0,255);

    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point clk = myclock::now();
    myclock::duration d = myclock::now() - clk;

    srand(time(NULL));
    int randSeed =0;
    unsigned seed = 0;

    for(int i=0; i< nbPoints; i++)
    {
        randSeed = rand();
        seed = d.count() * randSeed;
        generator.seed(seed);

        int X = distributionX(generator);
        int Y = distributionY(generator);
        int value = distributionValue(generator);
        ImageType::IndexType idx = {{X,Y}};
        image->SetPixel(idx,value);

    }

    //imageWriter
    typedef unsigned char OutputPixeltype;
    typedef itk::Image<OutputPixeltype,2> OutputImageType;
    typedef itk::ImageFileWriter <OutputImageType> OutputWriterType;
    typedef itk::CastImageFilter <ImageType, OutputImageType> CastFilterType;
    typedef itk::RescaleIntensityImageFilter <ImageType, ImageType> RescaleFilterType;

    RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->SetInput(image);
    rescaler->Update();

    CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput(rescaler->GetOutput());
    caster->Update();

    OutputWriterType::Pointer writer = OutputWriterType::New();
    writer->SetFileName( fileOutName );
    writer->SetInput(caster->GetOutput());
    writer->Update();

}



