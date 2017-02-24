#include "ShapeExtraction.hpp"

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
        std::cout<<"Structure Extraction : "<<std::endl;
        std::cout<<"input Filename"<<std::endl
                 <<"outputFilename"<<std::endl
                 <<"-l(default) OR -s OR -b (line, sheet, blop Extraction)"<<std::endl
                 <<"-sigma value (default 1.0)"<<std::endl
                 <<"-ex (i.e extractBright) bool(default true)"<<std::endl;
        return EXIT_SUCCESS;
    }

    //Default Arguments

    double sigma = 1.0;
    bool extractBright = true;
    bool lineExtraction = true; // Default
    bool sheetExtraction = false;
    bool blobExtraction = false;
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
    if(input.cmdOptionExists("-sigma"))
    {
        sigma = static_cast<double>(::atof(input.getCmdOption("-sigma").c_str()));

    }



    if(input.cmdOptionExists("-l")) //Line extraction
    {
        lineExtraction = true;
    }
    if(input.cmdOptionExists("-s")) //Sheet Extraction
    {
        sheetExtraction = true;
        lineExtraction = false;
    }
    if(input.cmdOptionExists("-b")) //Blob extraction
    {
        blobExtraction = true;
        lineExtraction = false;
    }

    //Read input
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);
    try{
     reader->Update();
    }
    catch(itk::ExceptionObject &err){
        std::cerr<<"Error while reading input"<<std::endl;
        std::cerr << err << std::endl;
    }


    std::cout<<"Shape Extraction  with sigma " << sigma << " ..."<<std::endl;
    LineShapeFilterType::Pointer shapeFilter = LineShapeFilterType::New();
    shapeFilter->SetInput(reader->GetOutput());
    shapeFilter->SetExtractBrightLine(extractBright);
    shapeFilter->EigenValuesExtractionOn();
    //shapeFilter->SetNumberOfThreads(12);
    shapeFilter->SetSigma(sigma);
    shapeFilter->Update();


    fImageType::Pointer eigImg1 = shapeFilter->GetEigenValuesOutput(0);
    fImageType::Pointer eigImg2 = shapeFilter->GetEigenValuesOutput(1);
    fImageType::Pointer eigImg3 = shapeFilter->GetEigenValuesOutput(2);

    fImageType::Pointer resultImage = fImageType::New();
    resultImage->SetRegions(eigImg1->GetLargestPossibleRegion());
    resultImage->SetSpacing(eigImg1->GetSpacing());
    resultImage->SetOrigin(eigImg1->GetOrigin());
    resultImage->Allocate();


    float C = 0.0f;
    //Methode on parcours x,y,z
    // ImageType::SizeType dim = shapeFilter->GetOutput()->GetLargestPossibleRegion().GetSize();

    //Essayons la methode iterator pour aller plus vite

    SliceIteratorType It(resultImage, resultImage->GetRequestedRegion());
    SliceConstIteratorType eig1It(eigImg1, eigImg1->GetRequestedRegion());
    SliceConstIteratorType eig2It(eigImg2, eigImg2->GetRequestedRegion());
    SliceConstIteratorType eig3It(eigImg3, eigImg3->GetRequestedRegion());

    It.SetFirstDirection(0);
    It.SetSecondDirection(1);
    eig1It.SetFirstDirection(0);
    eig1It.SetSecondDirection(1);
    eig2It.SetFirstDirection(0);
    eig2It.SetSecondDirection(1);
    eig3It.SetFirstDirection(0);
    eig3It.SetSecondDirection(1);


    while( !It.IsAtEnd())
    {
        while( !It.IsAtEndOfSlice())
        {
            while( !It.IsAtEndOfLine())
            {
                float lambda1 = 0.0f , lambda2 = 0.0f , lambda3 = 0.0f;
                //All the lambdas need to be the same sign to extract bright or dark structures
                if(eig1It.Get() <= 0)
                {
                    lambda1 = eig1It.Get() * sigma * sigma;
                }
                else
                    lambda1 = 0.0f;
                if(eig2It.Get() <= 0)
                {
                    lambda2 = eig2It.Get() * sigma * sigma;
                }
                else
                    lambda2 = 0.0f;
                if(eig3It.Get() <= 0)
                {
                    lambda3 = eig3It.Get() * sigma * sigma;
                }
                else
                    lambda3 = 0.0f;

                if(lambda3 != 0.0f)
                {
                    if(sheetExtraction)
                        C = (lambda3 - lambda2) / lambda3;
                    else if(lineExtraction)
                        C = (lambda2 - lambda1) / lambda3;
                    else if(blobExtraction)
                        C = lambda1 / lambda3;
                }
                else
                    C=0.0f;

                if(abs(lambda3) > 10e-4 )
                {
                    It.Set(C*10000);
                }
                else
                    It.Set(0);

                ++It;
                ++eig1It;
                ++eig2It;
                ++eig3It;

            }
            It.NextLine();
            eig1It.NextLine();
            eig2It.NextLine();
            eig3It.NextLine();
        }
        It.NextSlice();
        eig1It.NextSlice();
        eig2It.NextSlice();
        eig3It.NextSlice();
    }



    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(resultImage);

    std::cout<<"Writing  ";
    if(lineExtraction)
        std::cout << "line extraction result ";
    else if(sheetExtraction)
        std::cout << "sheet extraction result ";
    else if(blobExtraction)
        std::cout << "blop extraction result ";
    else
        std::cout << " unknown result";
    std::cout<<"in : "<<outputFilename<<std::endl;

    try{
        writer->Update();
    }
    catch(itk::ExceptionObject &err)
    {
            std::cerr << " Error While writing result " << std::endl;
            std::cerr << err << std::endl;
    }

    std::cout<<inputFilename<<std::endl<<
               outputFilename<<std::endl<<
               lineExtraction<<std::endl<<
               sheetExtraction<<std::endl<<
               blobExtraction<<std::endl<<
               sigma<<std::endl<<
               extractBright<<std::endl;

}
