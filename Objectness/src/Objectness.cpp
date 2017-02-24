/**
  * Code inspired from Insight Toolkit implementation
  * Generalizing vesselness with respect to dimensionnality shape
  * Luca Antiga
  *
  * Modified bu Charly Girot, NYU, Computer Science
  */

#include "Objectness.hpp"

class InputParser{
public:
    InputParser (int &argc, char **argv){
        for (int i=1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    /// @author iain
    const std::string& getCmdOption(const std::string &option) const{
        std::vector<std::string>::const_iterator itr;
        itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()){
            return *itr;
        }
        return NULL;
    }
    /// @author iain
    bool cmdOptionExists(const std::string &option) const{
        return std::find(this->tokens.begin(), this->tokens.end(), option)
                != this->tokens.end();
    }
private:
    std::vector <std::string> tokens;
};

int main(int argc, char* argv[])
{

    if(strcmp(argv[1] , "-h") == 0 || strcmp(argv[1] , "--help") == 0)
    {
        std::cout<<"Objectness filter : "<<std::endl;
        std::cout<<"input Filename"<<std::endl
                <<"outputFilename"<<std::endl
               <<"-t(default) OR -p OR -b (tube, plate, blop Extraction) (exclusive : only the last one is taken into account)"<<std::endl
              <<"-sigmaMin value (default 1.0)"<<std::endl
             <<"-sigmaMax value (default SigmaMin )"<<std::endl
            <<"-nSigma value (default 10) (i.e : number of sigma steps)"<<std::endl
           <<"-ex (i.e extractBright) bool(default true)"<<std::endl;
        return EXIT_SUCCESS;
    }

    //Default Arguments

    double sigmaMin = 1.0;
    double sigmaMax = sigmaMin;
    int sigmaStep = 10;
    bool extractBright = true;
    int M; //0 blobs ; 1 vessels ; 2 plates, 3 hyperplates

    //Parse Arguments
    InputParser input(argc,argv);
    std::string inputFilename;
    std::string outputFilename;
    if(argc>=3)
    {
        inputFilename=argv[1];
        outputFilename=argv[2];
        std::cout<<"Running Extraction of "<<inputFilename<<std::endl;

    }
    else
    {
        std::cerr<<"At least inputFilename and outputFilename expected as parameters"<<std::endl;
        return EXIT_FAILURE;
    }
    if(input.cmdOptionExists("-ex") )
    {
        if( strcmp(input.getCmdOption("-ex").c_str(), "true") == 0 )
            extractBright = true;
        else
            extractBright=false;
    }
    if(input.cmdOptionExists("-sigmaMin"))
    {
        sigmaMin = static_cast<double>(::atof(input.getCmdOption("-sigmaMin").c_str()));

    }
    if(input.cmdOptionExists("-sigmaMax"))
    {
        sigmaMax = static_cast<double>(::atof(input.getCmdOption("-sigmaMax").c_str()));
    }
    else
        sigmaMax = sigmaMin;


    // 0 blobs ; 1 vessels ; 2 plates, 3 hyperplates
    if(input.cmdOptionExists("-t")) //Tube extraction
    {
        M=1;
        std::cout << "Vesselness extraction" << std::endl;
    }
    if(input.cmdOptionExists("-p")) //Sheet Extraction
    {
        M=2;
        std::cout << "Plate extraction" << std::endl;
    }
    if(input.cmdOptionExists("-b")) //Blob extraction
    {
        M=0;
        std::cout << "blob extraction " << std::endl;
    }
    if( input.cmdOptionExists("-nSigma"))
    {
        sigmaStep = static_cast<int>(::atoi(input.getCmdOption("-nSigma").c_str()));
    }


    //read the input image
    ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(inputFilename);
    try{
        reader->Update();
    }
    catch(itk::ExceptionObject &exc){
        std::cerr << exc << std::endl;
        return EXIT_FAILURE;
    }



    //get the objectness filter and set the parameters
    ObjectnessFilterType::Pointer objectnessFilter = ObjectnessFilterType::New();
    objectnessFilter->SetScaleObjectnessMeasure(true); //Why ? // The metric is scaled with the magnitude of the largest eigen value
    objectnessFilter->SetBrightObject(extractBright);
    objectnessFilter->SetAlpha(0.5);
    objectnessFilter->SetBeta(0.5);
    objectnessFilter->SetGamma(5.0);
    objectnessFilter->SetObjectDimension(M);

    //Instantiate the multiscale filter and set the parameters
    // we'll extract the hessian at differents sigma scale
    MultiScaleEnhancementFilterType::Pointer multiScaleEnhancementFilter =
            MultiScaleEnhancementFilterType::New();
    multiScaleEnhancementFilter->SetInput(reader->GetOutput());
    multiScaleEnhancementFilter->SetHessianToMeasureFilter(objectnessFilter);
    multiScaleEnhancementFilter->SetSigmaStepMethodToLogarithmic();
    multiScaleEnhancementFilter->SetSigmaMinimum(sigmaMin);
    multiScaleEnhancementFilter->SetSigmaMaximum(sigmaMax);
    multiScaleEnhancementFilter->SetNumberOfSigmaSteps(sigmaMin == sigmaMax ? 1 : sigmaStep);

    //Now run the multiscale filter
    try
    {
        multiScaleEnhancementFilter->Update();
    }
    catch(itk::ExceptionObject &ex){
        std::cerr << &ex << std::endl;
        return EXIT_FAILURE;
    }

    //Write the enhanced image
    ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(multiScaleEnhancementFilter->GetOutput());
    try{
        writer->Update();
    }
    catch(itk::ExceptionObject &exc)
    {
        std::cout << "writer 1 "<<std::endl;
        std::cerr << exc << std::endl;
        return EXIT_FAILURE;
    }

    //Write the image containig the best response scales
    /*
    ImageWriterType::Pointer writer2 = ImageWriterType::New();
    writer2->SetFileName("bestResponseScale_"+outputFilename);
    writer2->SetInput(multiScaleEnhancementFilter->GetScalesOutput());
    try{
        writer2->Update();
    }
    catch(itk::ExceptionObject &exce)
    {
        std::cout << "writer 2 "<<std::endl;
        std::cerr<< exce << std::endl;
        return EXIT_FAILURE;
    }*/

    /*
    std::cout<<inputFilename<<std::endl<<
               outputFilename<<std::endl<<
               lineExtraction<<std::endl<<
               sheetExtraction<<std::endl<<
               blobExtraction<<std::endl<<
               sigmaMin<<std::endl<<
               extractBright<<std::endl;
    */

    return EXIT_SUCCESS;

}
