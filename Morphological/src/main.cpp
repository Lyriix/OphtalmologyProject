#include "types.hpp"
#include "close.hpp"
#include "open.hpp"
#include "blackTop.hpp"
#include "whiteTop.hpp"



int main( int argc, char* argv[] )
{
    if(strcmp(argv[1] , "-h") == 0 || strcmp(argv[1] , "--help") == 0)
    {
        std::cout<<argv[0]<<std::endl;
        std::cout<<"./Morphological inputFilename outputFilename [-c,-o][radius]"<<std::endl;
        std::cout<<"-c : Closing --close"<<std::endl;
        std::cout<<"-o : Opening --open"<<std::endl;
        std::cout<<"-b : Black Top Hat --blacktop "<<std::endl;
        std::cout<<"-w : White Top Hat --whitetop "<<std::endl;
        return EXIT_SUCCESS;
    }

    std::cout << argc << std::endl;
    float radius;
    //Parse arguments
    bool closing = false;
    if(strcmp(argv[3],"-c") ==0 || strcmp(argv[3],"--close") == 0 )
    {

        //Check parameter
        if(argc==5)
        {
            radius = static_cast<float>(::atof(argv[4]));
            std::cout << radius << std::endl;
        }
        else
        {
            std::cerr << " Check Parameter '-c radius'  " << std::endl;
            return EXIT_FAILURE;
        }
        std::cout<<"Closing with Structuring element of radius : "<<radius<<std::endl;
        closing = true;
    }

    bool opening = false;
    if(strcmp(argv[3],"-o") ==0 || strcmp(argv[3],"--open") == 0 )
    {

        //Check parameter
        if(argc==5)
        {
            radius = static_cast<float>(::atof(argv[4]));
        }
        else
        {
            std::cerr << " Check Parameter '-c radius'  " << std::endl;
            return EXIT_FAILURE;
        }
        std::cout<<"Closing with Structuring element of radius : "<<radius<<std::endl;
        opening = true;
    }

    bool blacktop = false;
    if(strcmp(argv[3] , "-b") == 0 || strcmp( argv[3], "--blacktop") == 0)
    {
        //Check parameter
        if (argc == 5)
        {
            radius = static_cast<float>(::atof(argv[4]));
        }
        else
        {
            std::cerr << "Check parameter '-b radius or --blacktop raidus" << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Black Top Hat Filter with structuring element of radius : "<< radius <<std::endl;
        blacktop = true;
    }
    bool whitetop = false;
    if(strcmp(argv[3] , "-w") == 0 || strcmp( argv[3], "--whitetop") == 0)
    {
        //Check parameter
        if (argc == 5)
        {
            radius = static_cast<float>(::atof(argv[4]));
        }
        else
        {
            std::cerr << "Check parameter '-w radius or --whitetop raidus" << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "White Top Hat Filter with structuring element of radius : "<< radius <<std::endl;
        whitetop = true;
    }


    ReaderType::Pointer reader = ReaderType::New();
    std::cout<<"Reading "<< argv[1]<<std::endl;
    reader->SetFileName(argv[1]);
    try{
        reader->Update();
    }
    catch(itk::ExceptionObject &err)
    {
        std::cerr << "Error while reading" << std::endl << err << std::endl;
    }

    ImageType::Pointer image = ImageType::New();
    image = reader->GetOutput();



    ImageType::Pointer filteredImage = ImageType::New();
    if(closing == true)
    {
        std::cout<<"Closing : "<< argv[1]<<std::endl;
        filteredImage = PerformClosing(image, radius);
    }

    if(opening == true)
    {
        std::cout<<"Opening : "<< argv[1]<<std::endl;
        filteredImage = PerformOpening(image, radius);
    }

    if (blacktop == true)
    {
        std::cout << "Black Top hat Filter : "<< argv[1] << std::endl;
        filteredImage = PerformBlackTopHat(image, radius);
    }
    if (whitetop == true)
    {
        std::cout << "White Top hat Filter : "<< argv[1] << std::endl;
        filteredImage = PerformWhiteTopHat(image, radius);
    }






//    SubtractType::Pointer subtracter = SubtractType::New();
//    subtracter->SetInput1( filteredImage);
//    subtracter->SetInput2(image);
//    subtracter->Update();

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(argv[2]);
    writer->SetInput(filteredImage);
    try{
        writer->Update();
    }
    catch(itk::ExceptionObject &err)
    {
        std::cerr << "Error while writing " << std::endl << err << std::endl;
    }

//    writer->SetFileName("../diff.nrrd");
//    writer->SetInput(subtracter->GetOutput());
//    try{
//        writer->Update();
//    }
//    catch(itk::ExceptionObject &err)
//    {
//        std::cerr << "Error while writing " << std::endl << err << std::endl;
//    }




    return EXIT_SUCCESS;
}


