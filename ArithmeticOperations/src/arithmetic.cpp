#include <itkAddImageFilter.h>
#include"arithmetic.hpp"

int main(int argc, char* argv[])
{
    if(strcmp(argv[1] , "-h") == 0)
        {
            std::cout << " Usage of arithmetic is : "<<std::endl;
            std::cout<< " ./arithmetic input1 input2 output operation(+,-,conv,div,*)"<<std::endl;
            return EXIT_SUCCESS;
        }

    ReaderType::Pointer reader1 = ReaderType::New();
    reader1->SetFileName(argv[1]);
    ReaderType::Pointer reader2 = ReaderType::New();
    reader2->SetFileName(argv[2]);
    try
    {
        reader1->Update();
        reader2->Update();
    }
    catch(itk::ExceptionObject &err)
    {
        std::cerr<<"Exception object caught !" << std::endl;
        std::cerr<<err<<std::endl;
        return EXIT_FAILURE;
    }

    ImageType::Pointer ResultImage = ImageType::New();
    switch(argv[4])
    {
    case '+' :
        typedef itk::AddImageFilter <ImageType, ImageType, ImageType> AddImageFilterType;
        AddImageFilterType::Pointer addImageFilter = AddImageFilterType::New();
        addImageFilter->SetInput1(image);
    }





}


