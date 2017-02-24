#include "softThresholding.hpp"
#include <string>

#include "itkMinimumMaximumImageCalculator.h"
typedef itk::MinimumMaximumImageCalculator <ImageType>
ImageCalculatorFilterType;

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
        cout << "lower threshold level (int in %)" << endl;
        cout << "higher threshold level (int in %)" << endl;
        return EXIT_SUCCESS;

    }
    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    cout <<outputFilename<<endl;
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

    //Compute maximum and maximum of the input image
    ImageCalculatorFilterType::Pointer imageCalculatorFilter
            = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(image);
    imageCalculatorFilter->Compute();

    ImageType::IndexType indexOfMaximum = imageCalculatorFilter->GetIndexOfMaximum();
    ImageType::IndexType indexOfMinimum = imageCalculatorFilter->GetIndexOfMinimum();

    // ratio = (max-min)/100
    float ratio = (image->GetPixel(indexOfMaximum) - image->GetPixel(indexOfMinimum))/100.0;


    float lowerThreshold = 25.0 * ratio;
    float higherThreshold = 125.0 *ratio;

    if(argc >= 4 )
    {
        lowerThreshold = ::atof(argv[3]) * ratio;
    }
    if(argc >= 5 )
    {
        higherThreshold = ::atof(argv[4]) * ratio;
    }

    std::cout<<"ratio : "<<ratio<<std::endl;
    std::cout<<"lower Threshold : "<<lowerThreshold << ", Higher Threshold : " << higherThreshold <<std::endl;
    float sigma = 1.0 *ratio;

    for(int i=0; i<size[0]; i++)
        for(int j=0 ; j<size[1]; j++)
            for(int k=0; k<size[2]; k++)
            {
                ImageType::IndexType idx;
                idx[0] = i ; idx[1]=j; idx[2]=k;
                float valuePixel = image->GetPixel(idx);

                if(valuePixel > lowerThreshold && valuePixel < higherThreshold)
                {
                    image->SetPixel(idx,valuePixel);
                }
                else if(valuePixel < lowerThreshold)
                    image->SetPixel(idx,0);
                else if(valuePixel>higherThreshold)
                    image->SetPixel(idx,valuePixel+sigma);

            }



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
