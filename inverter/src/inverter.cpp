#include "itkImage.h"
typedef unsigned short PixelType;
typedef itk::Image<PixelType, 3> ImageType;
#include "itkImageFileReader.h"
typedef itk::ImageFileReader<ImageType> ReaderType;
#include "itkImageFileWriter.h"
typedef itk::ImageFileWriter<ImageType> WriterType;



int main(int argc, char* argv[])
{

    if(argv[1] == "-h" )
    {
        std::cout<<"Inverter : "<<std::endl;
        std::cout<<" Input Filename ; Output Filename"<<std::endl;
    }

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[1]);
    reader->Update();

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(argv[2]);

    ImageType::Pointer image = reader->GetOutput();
    ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    ImageType::IndexType idx;

    for(int i=0; i< size[0]; i++)
    {
        idx[0] = i;
        for(int j=0; j<size[1]; j++)
        {
            idx[1]= j;
            for(int k=0; k<size[2]; k++)
            {
                idx[2]=k;

                if(image->GetPixel(idx) != 0)
                    image->SetPixel(idx,0);
                else
                    image->SetPixel(idx,255);
            }
        }
    }

    writer->SetInput(image);
    writer->Update();

    return EXIT_SUCCESS;
}
