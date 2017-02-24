#include "medianFilter.hpp"
#include "itkImageFileReader.hxx"
#include "itkImageFileWriter.hxx"
#include <math.h>
#include<string>

typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;


// USAGE :: arguments :: ./main inputFilename nbIterations Conductance
int main(int argc, char* argv[])
{

    if(strcmp(argv[1] , "-h") == 0)
    {
        std::cout << " Usage of anisotropicFilter is : "<<std::endl;
        std::cout<< " ./medianFilter inputFilename outputFolder "<<std::endl;
        return EXIT_SUCCESS;
    }

    //Parse input


    std::string outputFoldername = argv[2];

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[1]);

    try
    {
        reader->Update();
    }
    catch(itk::ExceptionObject &err)
    {
        std::cerr<<"Exception object caught !" << std::endl;
        std::cerr<<err<<std::endl;
        return EXIT_FAILURE;
    }



    ImageType::Pointer new_image =
            PerformDenoisement(reader->GetOutput());


    //get position of ".nrrd" in the filename
    std::string filename = argv[1];
    std::size_t periodPos = filename.find_last_of(".");
    std::size_t lastSlash = filename.find_last_of("/\\" );
    std::string extension = filename.substr(periodPos);
    std::string nameNoExt = filename.substr(lastSlash+1,periodPos-lastSlash);

    //supress the .extension


    WriterType::Pointer writer = WriterType::New();
    std::stringstream ssfilenameOutput;
    ssfilenameOutput << outputFoldername <<"/"<<nameNoExt<<extension;
    std::cout<<"Writed in : " << ssfilenameOutput.str() << std::endl;
    writer->SetFileName(ssfilenameOutput.str());
    writer->SetInput(new_image);
    writer->Update();


    std::cout<<"DONE"<<std::endl;
}
