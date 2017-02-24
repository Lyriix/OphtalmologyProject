#include "Thresholding.hpp"
#include <string>

//#include "itkMinimumMaximumImageCalculator.h"
//typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;

using std::cout;
using std::endl;
using std::cerr;

int main(int argc, char *argv[])
{

    if( strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0)
    {
        cout << "Soft thresholding " << endl;
        cout << "-h : help - Display help " << endl;
        cout << "inputFilename <std::string>"<<endl;
        cout << "outputFilename <std::string> "<<endl;
        cout << "threshold level (int (pixel Value))" << endl;
        return EXIT_SUCCESS;

    }
    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];



    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);
    try{
        reader->Update();
    }
    catch(itk::ExceptionObject &ex){
        cerr<<"Error while reading"<<endl;
        cerr<<ex<<endl;
        return EXIT_FAILURE;
    }

    ImageType::Pointer image = ImageType::New();
    image = reader->GetOutput();
    ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

    /*
    //Compute maximum and maximum of the input image
    ImageCalculatorFilterType::Pointer imageCalculatorFilter
            = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(image);
    imageCalculatorFilter->Compute();

    ImageType::IndexType indexOfMaximum = imageCalculatorFilter->GetIndexOfMaximum();
    ImageType::IndexType indexOfMinimum = imageCalculatorFilter->GetIndexOfMinimum();

    // ratio = (max-min)/100
    float ratio = (image->GetPixel(indexOfMaximum) - image->GetPixel(indexOfMinimum))/100.0;
    */

    // Threshold level at which perform binarization (vellow --> 0 , above -->255)
    float threshold = 50.0;
    if(argc>=4)
    {
        threshold = ::atof(argv[3]);
    }

    //std::cout<<"ratio : "<<ratio<<std::endl;
    std::cout<< " Threshold level : " << threshold <<std::endl;
    //Perform Thresholding
    for(int i=0; i<size[0]; i++)
        for(int j=0 ; j<size[1]; j++)
            for(int k=0; k<size[2]; k++)
            {
                ImageType::IndexType idx;
                idx[0] = i ; idx[1]=j; idx[2]=k;
                float valuePixel = image->GetPixel(idx);

                if(valuePixel > threshold)
                {
                    image->SetPixel(idx,255);
                }
                else
                    image->SetPixel(idx,0);
            }
    //Write output
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput(image);
    writer->SetFileName(outputFilename);
    try{
        writer->Update();
    }
    catch(itk::ExceptionObject &e)
    {
        cerr<< "error while writing result" << endl;
        cerr << e << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
