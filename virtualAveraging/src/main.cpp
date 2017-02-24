#include <iostream>
#include <vector>
#include<iterator>

#include <itkImage.h>
#include <itkIntTypes.h>

#include <time.h>

#include <math.h>
// Image IO
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "vtkView.h"
#include "vtkImageViewer.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"


#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRGBPixel.h"
#include "itkVectorResampleImageFilter.h"

#include "itkGaussianDistribution.h"
#include "itkNormalVariateGenerator.h"
//#include <map>
#include "gaussianGenerator.hpp"
#include "access.hpp"


using namespace std;


int main(int argc, char* argv[])
{

    Selection selection = GAUSSIAN_PIX;
    Choice choice = GAUSSIAN_DEV ;

    std::string fileName;
    std::string fileOutName;

    switch(argc)
    {
    case 1 :
        fileName = "blancOut.jpg";
        fileOutName = "blancOutOut.jpg";
        break;
    case 2:
        fileName = argv[1];
        fileOutName = "lenaNoisyOut.jpg";
        break;
    case 3:
        fileName = argv[1];
        fileOutName = argv[2];
        break;
    default:
        break;

    }

    typedef unsigned short                     PixelType;
    typedef itk::Image < PixelType, 2 >        ImageType;
    typedef itk::ImageFileReader < ImageType > ReaderType;


    //Read an image in itk::image, the file type is determined by the extension of the specified filename
    ReaderType::Pointer readerImage = ReaderType::New();

    readerImage->SetFileName(fileName);
    readerImage->Update();

    ImageType::Pointer image = readerImage->GetOutput();

    //extract image information
    ImageType::RegionType region = image->GetLargestPossibleRegion();
    ImageType::SizeType size = region.GetSize();

    int width = size[0];
    int height = size[1];

    std::cout << "Width : " << width << "; Height : " << height << std::endl;

    //FOR EACH PIXEL IN THE IMAGE
    for(int i=1 ; i<width ;i++)
    {
        for(int j = 1 ; j < height ; j++)
        {

            std::vector<PixelType> neighboor; //vector within pixel intensities
            /*SELECTION OF THE NEIGHBOORHOOD (x9)*/
            for(int a = i-1; a <= i+1 ; a++)
            {
                for(int b = j-1 ; b <= j+1 ; b++)
                {
                    ImageType::IndexType pixelIndex = {{a,b}};
                    PixelType pixel = image->GetPixel(pixelIndex); //get the value of the pixel
                    neighboor.push_back(pixel);
                }
            }
            //HOW PIXEL ARE CHOSEN
            std::vector<PixelType> vectorToAverage; //Modified vector intensity
            switch(selection)
            {
            case ALL : //Each neighboor and himself is accessed and modify
            {
                vectorToAverage = accessEachOnce(neighboor, choice);
                break;
            }
            case UNIFORM : //We access Randomly Uniform to one pixel of the set of the neighboorhood
            {
                vectorToAverage =accessUniform(neighboor, choice);
                break;
            }
            case GAUSSIAN_PIX :// Gaussian Random access to the set
            {
                vectorToAverage = accessGaussian(neighboor, choice);
            }

            }


            // Computation of the average intensity of the modified vector
            PixelType sum=0;
            int average=0;
            int count=0;
            for(std::vector<PixelType>::iterator it = vectorToAverage.begin();
                it != vectorToAverage.end();
                it++)
            {
                sum += *it;
                count++;

            }
          //  std::cout<< count << std::endl ;
            average = sum/count;
            if(average>255) average = 255;
            if(average<0) average = 0;
            // if( average > 255)
            //std::cout << average << std::endl;

            //Replacement of the original value of the centered pixel
            ImageType::IndexType pixelIndex = {{i,j}};
            image->SetPixel(pixelIndex,average);
        }
    }

    //Save the modified image
    typedef unsigned char                                                OutputPixelType;
    typedef itk::Image < OutputPixelType, 2 >                            OutputImageType;
    typedef itk::ImageFileWriter < OutputImageType >                     OutputWriterType;
    typedef itk::CastImageFilter < ImageType, OutputImageType >          CastFilterType;
    typedef itk::RescaleIntensityImageFilter < ImageType, ImageType >    RescaleFilterType;

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




    return 0;
}






