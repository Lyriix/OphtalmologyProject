
#include "selectArea.hpp"



#include "itkImageFileReader.h"
typedef itk::ImageFileReader< ImageType > ReaderType;
#include "itkImageFileWriter.h"
typedef itk::ImageFileWriter<ImageType> WriterType;

int main( int argc, char* argv[] )
{
    if(strcmp(argv[1],"-h")==0)
    {
        std::cout<< argv[1] <<std::endl;
        std::cout << "inputFilename, outputFilename, x1 x2 y1 y2 z1 z2" <<std::endl;
        return EXIT_SUCCESS;
    }
    ReaderType::Pointer reader = ReaderType::New();
    try{
    reader->SetFileName(argv[1]);
    }
    catch(itk::ExceptionObject e)
    {
        std::cerr<<e<<std::endl;
    }
    reader->Update();
    ImageType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    std::cout<<"Size of original : "<< size[0]<<","<<size[1]<<","<<size[2]<<std::endl;
    int x1=140, x2=274,y1=435,y2=691,z1=154,z2=320;

    std::stringstream ss1,ss2,ss3,ss4,ss5,ss6;
    std::string s1,s2,s3,s4,s5,s6;
    if(argc>=3){
        s1=argv[3];
        ss1 << s1 ;
        ss1 >> x1 ;
    }
    if(argc>=4){
        s2=argv[4];
        ss2 << s2 ;
        ss2 >> x2 ;
    }
    if(argc>=5){
        s3=argv[5];
        ss3 << s3;
        ss3 >> y1;
    }
    if(argc>=6){
        s4=argv[6];
        ss4 << s4;
        ss4 >> y2;
    }
    if(argc>=7){
        s5=argv[7];
        ss5 << s5;
        ss5 >> z1;
    }
    if(argc>=8){
        s6=argv[8];
        ss6 << s6;
        ss6 >> z2;
    }



    std::cout << "Extraction of the region of interest ..." << std::endl;
    ImageType::Pointer new_image =
            selectArea(reader->GetOutput(),x1,y1,z1,x2-x1,y2-y1,z2-z1);

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput(new_image);
    writer->SetFileName(argv[2]);
    writer->Update();
}
