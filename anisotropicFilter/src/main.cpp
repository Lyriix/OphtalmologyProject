#include "anisotropicFilter.hpp"
#include "itkImageFileReader.hxx"
#include "itkImageFileWriter.hxx"
#include <math.h>
#include<string>

typedef itk::ImageFileReader<StackType> ReaderType;
typedef itk::ImageFileWriter<StackType> WriterType;


// USAGE :: arguments :: ./main inputFilename nbIterations Conductance
int main(int argc, char* argv[])
{

    if(strcmp(argv[1] , "-h") == 0)
    {
        std::cout << " Usage of anisotropicFilter is : "<<std::endl;
        std::cout<< " ./anisotropicFilter inputFilename outputFolder numIteration conductance"<<std::endl;
        return EXIT_SUCCESS;
    }

    //Parse input
    int nbIter = 0;
    std::string str_nbIter = argv[3];
    std::stringstream ss_nbIter;
    ss_nbIter << str_nbIter;
    ss_nbIter >> nbIter;

    float conductance = 0.0;
    std::string str_conductance = argv[4];
    std::stringstream ss_conductance;
    ss_conductance << str_conductance;
    ss_conductance >> conductance;

    std::string outputFilename = argv[2];

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

    float denom = (float) Dimension;
    float max_stable_timeStep = 0.5 / (float)pow(2,denom);


    std::cout<<"Conductance "<< conductance <<" nb Iteration : " << nbIter <<std::endl;


    StackType::Pointer new_image =
            PerformDenoisement(reader->GetOutput(),nbIter,max_stable_timeStep,conductance);


    //get position of ".nrrd" in the filename
    std::string filename = argv[1];
    std::size_t periodPos = filename.find_last_of(".");
    std::size_t lastSlash = filename.find_last_of("/\\" );
    std::string extension = filename.substr(periodPos);
    std::string nameNoExt = filename.substr(lastSlash+1,periodPos-lastSlash);

    //supress the .extension


    WriterType::Pointer writer = WriterType::New();

    writer->SetFileName(outputFilename);
    writer->SetInput(new_image);
    writer->Update();


    std::cout<<"DONE"<<std::endl;
}
