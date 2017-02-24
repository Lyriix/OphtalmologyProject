#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesWriter.h"

#include "itkNumericSeriesFileNames.h"
#include "itkPNGImageIO.h"
#include "itkMetaDataObject.h"


#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>       // std::cout
#include <string>         // std::string
#include <vector>


 #include "itkOrientImageFilter.h"


std::vector<std::string> SplitFilename (const std::string& str);


int main(int argc, char* argv[])
{
    //Argument for help
    if(strcmp(argv[1], "-h") == 0 )
    {
        std::cout << " Usage of stackDecomposer :" << std::endl;
        std::cout << "./stackDecompsoser inputImage inputLabel outputFolder " <<std::endl;
        return EXIT_SUCCESS;
    }

    const char * inputFileName = argv[1];
    const char * labelFileName = argv[2];
    const char * outputFolder = argv[3];

    //Definition of Images Type
    const int Dimension = 3;
    typedef unsigned short         PixelType;
    typedef itk::Image<PixelType,Dimension> ImageType;
    typedef itk::Image<PixelType,Dimension - 1> ImageType2D;

    //Definition of reader Type
    typedef itk::ImageFileReader<ImageType> ReaderType;

    //Reader Input Image
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFileName);
    try{
        reader->Update();
    }
    catch(itk::ExceptionObject &err){
        std::cerr<<"Excepion Object caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }


    //Diection matrix of image
    ImageType::DirectionType direction;
        direction(0,0) = 1;
        direction(0,1) = 0;
        direction(0,2) = 0;
        direction(1,0) = 0;
        direction(1,1) = 1;
        direction(1,2) = 0;
        direction(2,0) = 0;
        direction(2,1) = 0;
        direction(2,2) = 1;


     //itk::SpatialOrientation::ValidCoordinateOrientationFlags fileOrientation;

     //Orienting the image in the write direction
     itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter =
             itk::OrientImageFilter<ImageType,ImageType>::New();
     orienter->SetGivenCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPS);
     // deprecated

     orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
     orienter->SetInput(reader->GetOutput());
     orienter->Update();

     //Image Oriented as input image
    ImageType::Pointer inputImage = ImageType::New();
    inputImage = orienter->GetOutput();

    //createoutputfolder
    std::vector<std::string> metaOutput = SplitFilename(outputFolder);
    std::string outputDirectory = metaOutput.at(0);
    itksys::SystemTools::MakeDirectory(outputDirectory);

    //Definition of Image Output Type & Output names Type & Series Writer
    typedef itk::PNGImageIO ImageIOType;
    typedef itk::NumericSeriesFileNames NamesGeneratorType;

    ImageIOType::Pointer pngIO = ImageIOType::New();
    typedef itk::ImageSeriesWriter<ImageType,ImageType2D> SeriesWriterTypeWithoutOverlay;
    NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();


    //Conversion of the metadata
    itk::MetaDataDictionary & dict = pngIO->GetMetaDataDictionary();
    std::string tagkey, value;
    tagkey = "0008|0060"; // Modality
    value = "MR";
    itk::EncapsulateMetaData<std::string>(dict, tagkey, value );
    tagkey = "0008|0008"; // Image Type
    value = "DERIVED\\SECONDARY";
    itk::EncapsulateMetaData<std::string>(dict, tagkey, value);
    tagkey = "0008|0064"; // Conversion Type
    value = "DV";
    itk::EncapsulateMetaData<std::string>(dict, tagkey, value);


    //Writer
    SeriesWriterTypeWithoutOverlay::Pointer seriesWriterWithoutOverlay = SeriesWriterTypeWithoutOverlay::New();
    seriesWriterWithoutOverlay->SetInput( inputImage );
    seriesWriterWithoutOverlay->SetImageIO( pngIO );


    ImageType::RegionType region = inputImage->GetRequestedRegion();
    std::cout<< region << std::endl;
    ImageType::IndexType start = region.GetIndex();
    ImageType::SizeType  size  = region.GetSize();

    std::string format = outputDirectory;
    format += "/image%03d.png";
    int i=2;
    namesGenerator->SetSeriesFormat( format.c_str() );
    namesGenerator->SetStartIndex( start[i] );
    namesGenerator->SetEndIndex( start[i] + size[i] - 1 );
    namesGenerator->SetIncrementIndex( 1 );
    seriesWriterWithoutOverlay->SetFileNames( namesGenerator->GetFileNames() );
    try
      {
        seriesWriterWithoutOverlay->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Exception thrown while writing the series " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;


}


std::vector<std::string> SplitFilename (const std::string& str) {
  //std::cout << "Splitting: " << str << '\n';
  unsigned found = str.find_last_of("/\\");
  std::string path = str.substr(0,found);
  std::string file = str.substr(found+1);

  unsigned found2 = file.find_first_of(".");
  std::string filename = file.substr(0,found2);
  std::string extension = file.substr(found2+1);

  std::vector<std::string> metaOutputName;
  metaOutputName.push_back(path);
  metaOutputName.push_back(file);
  metaOutputName.push_back(filename);
  metaOutputName.push_back(extension);
  return metaOutputName;


}
