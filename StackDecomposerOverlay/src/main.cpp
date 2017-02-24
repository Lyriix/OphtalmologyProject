#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkOrientImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMapOverlayImageFilter.h"
#include "itkLabelOverlayImageFilter.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "itkPNGImageIO.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageSeriesWriter.h"
//#include <itkCustomColormapFunction.h>
#include <itkLabelOverlayFunctor.h>
#include "itkMinimumMaximumImageCalculator.h"
#include "itkImageFileWriter.h"
using  std::cout;
using  std::cerr;
using  std::endl;


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

int main( int argc, char* argv[])
{
    /**
     * Argument for Helping the user
     */
    if(strcmp(argv[1], "-h") == 0 )
    {
        std::cout << " Usage of stackDecomposer :" << std::endl;
        std::cout << "./stackDecompsoser inputImage inputLabel outputFolder " <<std::endl;
        return EXIT_SUCCESS;
    }

    /**
     * Parse input
     */
    const char * inputFileName = argv[1];
    const char * labelFileName = argv[2];
    const char * outputFolder = argv[3];


    /**
     * Reading input Image
     */

    const int Dimension = 3;
    typedef unsigned short  inputPixelType;
    typedef itk::Image<inputPixelType, Dimension > inputImageType;
    typedef itk::ImageFileReader<inputImageType> inputReaderType;






    inputReaderType::Pointer inputReader = inputReaderType::New();
    inputReader->SetFileName(inputFileName);
    try{
        inputReader->Update();
    }
    catch(itk::ExceptionObject &err){
        cerr<<"Exception object caught while reading input image"<<endl;
        cerr << err << endl;
        return EXIT_FAILURE;
    }

    /**
     * Orienting the image in the right direction
     */

    inputImageType::DirectionType direction;
    direction(0,0) = 1;    direction(0,1) = 0;    direction(0,2) = 0;
    direction(1,0) = 0;    direction(1,1) = 1;    direction(1,2) = 0;
    direction(2,0) = 0;    direction(2,1) = 0;    direction(2,2) = 1;

    itk::OrientImageFilter< inputImageType, inputImageType>::Pointer inputOrienter =
            itk::OrientImageFilter< inputImageType, inputImageType>::New();

    inputOrienter->SetGivenCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPS);
    inputOrienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
    inputOrienter->SetInput(inputReader->GetOutput());
    inputOrienter->Update();

    inputImageType::Pointer inputImage = inputImageType::New();
    inputImage = inputOrienter->GetOutput();

    /**
     *  Reading the label Image
     */

    typedef int LabelType;
    typedef itk::LabelObject<LabelType, Dimension > LabelObjectType;
    typedef itk::LabelMap< LabelObjectType > LabelMapType;

    typedef itk::Image< LabelType, 3 > LabelImageType;
    typedef itk::ImageFileReader< LabelImageType > LabelReaderType;

    LabelReaderType::Pointer labelReader = LabelReaderType::New();
    labelReader->SetFileName(labelFileName);
    try{
        labelReader->Update();
    }
    catch( itk::ExceptionObject &err){
        cerr<<"Excpetion object caught while reading label image"<<endl;
        cerr<< err << endl;
        return EXIT_FAILURE;
    }

    /**
     * Orienting the label in the right direction
     */

    LabelImageType::DirectionType directionLabel;
    directionLabel(0,0) = 1;    directionLabel(0,1) = 0;    directionLabel(0,2) = 0;
    directionLabel(1,0) = 0;    directionLabel(1,1) = 1;    directionLabel(1,2) = 0;
    directionLabel(2,0) = 0;    directionLabel(2,1) = 0;    directionLabel(2,2) = 1;

    itk::OrientImageFilter<LabelImageType, LabelImageType >::Pointer labelOrienter =
            itk::OrientImageFilter<LabelImageType, LabelImageType>::New();
    labelOrienter->SetGivenCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_LPS);
    labelOrienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
    labelOrienter->SetInput(labelReader->GetOutput());
    labelOrienter->Update();

    /**
     * Apply a colormap to the LabelImage
     */
    typedef itk::RGBPixel< long unsigned int> RGBPixelType;
    typedef itk::Image< RGBPixelType, Dimension> RGBImageType;
    typedef itk::ScalarToRGBColormapImageFilter<LabelImageType, RGBImageType> RGBFilterType;
    RGBFilterType::Pointer rgbfilter = RGBFilterType::New();
    rgbfilter->SetInput(labelOrienter->GetOutput());

    /**
      *Create our Own Colormap BWR --> Doesn't work - Can't overlay an RGB image on a Feature Image
      * --> Need to create a functor the same way we created an overlay and applied it to the OverlayFilter
      */
    /*
    typedef itk::Function::CustomColormapFunction<LabelImageType::PixelType, RGBImageType::PixelType > ColormapType;
    ColormapType::Pointer colormap = ColormapType::New();
    ColormapType::ChannelType redChannel;
    ColormapType::ChannelType blueChannel;
    ColormapType::ChannelType greenChannel;

    int b = 4;
    int c= 95;
    for (int i=0 ; i<4 ; i++)
    {
        redChannel.push_back(0);
        greenChannel.push_back(i*255/b);
        blueChannel.push_back(i*255/b);
    }
    for (int i=5 ; i<100 ; i++)
    {
        redChannel.push_back(0);//255-i*c/250);
        greenChannel.push_back(255-i*255/c);
        blueChannel.push_back(255);
    }
    colormap->SetRedChannel(redChannel);
    colormap->SetGreenChannel(greenChannel);
    colormap->SetBlueChannel(blueChannel);
    rgbfilter->SetColormap(colormap);
    rgbfilter->Update();
    */
    /**
     * Convert the labelImage to a LabelMap
     */
    typedef itk::LabelImageToLabelMapFilter<LabelImageType , LabelMapType > ConverterType ;
    ConverterType::Pointer converter =  ConverterType::New();
    converter->SetInput(labelOrienter->GetOutput());
    converter->Update();

    //NB :: itkLabelOverlayImageFilter VS itkLabelMAPOverlayImageFilter

    /**
     * Create the overlay
     */
    typedef itk::LabelMapOverlayImageFilter< LabelMapType, inputImageType, RGBImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(converter->GetOutput());
    filter->SetFeatureImage(inputImage);
    filter->SetOpacity(0.6);
    typedef itk::Functor::LabelOverlayFunctor<inputPixelType, LabelType, RGBPixelType> FunctorType;
    FunctorType functor;
    functor.ResetColors();
    functor.SetBackgroundValue(0);


    typedef itk::MinimumMaximumImageCalculator< LabelImageType > MinMaxCalculatorType;
    MinMaxCalculatorType::Pointer minmaxcalc = MinMaxCalculatorType::New();
    minmaxcalc->SetImage(labelReader->GetOutput());
    minmaxcalc->Compute();
    inputImageType::IndexType indexOfMin = minmaxcalc->GetIndexOfMinimum();
    inputImageType::IndexType indexOfMax = minmaxcalc->GetIndexOfMaximum();
    inputImageType::PixelType minInput = inputImage->GetPixel(indexOfMin);
    inputImageType::PixelType maxInput = inputImage->GetPixel(indexOfMax);
    cout<<minInput<<endl;
    cout<<maxInput<<endl;
    int b = 1;
    /*for (int i=0 ; i<b ; i++)
    {
        functor.AddColor(maxInput,i*maxInput/b, i*maxInput/b);
        //functor.AddColor(maxInput,0,0);
        //functor.AddColor(0,0,1500);
    }
    for(int i=b-1 ; i<b+1 ; i++)
    {
        //functor.AddColor(maxInput, maxInput, maxInput);
    }
    for (int i=b ; i<2 ; i++)
    {
        functor.AddColor(0,maxInput-i*maxInput/(100-b), maxInput);
        //functor.AddColor(0,0,maxInput);
    }*/

    /** Create 3 Colors */
    std::vector<int> blue; blue.push_back(0); blue.push_back(0); blue.push_back(maxInput);
    std::vector<int> white; white.push_back(maxInput); white.push_back(maxInput); white.push_back(maxInput);
    std::vector<int> red; red.push_back(maxInput); red.push_back(0); red.push_back(0);

    int n_colors = 20;
    /** Interpolate btw each point */
    for( int i=1 ; i < n_colors/2 ; i++)
    {
        functor.AddColor(blue[0]/i + white[0]/(n_colors-i), blue[1]/i + white[0]/(n_colors-i), blue[2]/i + white[0]/(n_colors-i));
    }
    for(int i = n_colors/2 ; i< n_colors ; i++)
    {
        functor.AddColor(white[0]/i + red[0]/(n_colors -i), white[1]/i + red[0]/(n_colors - i), white[2]/i + red[0]/(n_colors-i));
    }

    cout << functor.GetNumberOfColors() <<endl ;
    filter->SetFunctor(functor);
    filter->Update();


    /**
     * Create Output Folder
     */

    //createoutputfolder
    std::vector<std::string> metaOutput = SplitFilename(outputFolder);
    std::string outputDirectory = metaOutput.at(0);
    itksys::SystemTools::MakeDirectory(outputDirectory);


    /**
     * Defintition of Output Types
     */
    typedef itk::PNGImageIO ImageIOType;
    ImageIOType::Pointer pngIO = ImageIOType::New();

    typedef itk::NumericSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();

    typedef itk::RGBPixel<unsigned short> PixelType2D;
    typedef itk::Image< PixelType2D, Dimension -1 > ImageType2D;

    typedef itk::ImageSeriesWriter< FilterType::OutputImageType , ImageType2D > SeriesWriterType;

    /**
     * Conversion of the metadata
     */


    /**
     * Writing the output serie
     */

    SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
    seriesWriter->SetInput(filter->GetOutput());
    seriesWriter->SetImageIO(pngIO);


    inputImageType::RegionType region = inputImage->GetRequestedRegion();
    inputImageType::IndexType start = region.GetIndex();
    inputImageType::SizeType size = region.GetSize();

    std::string format = outputDirectory;
    format += "/image%03d.png";

    int i=2;
    namesGenerator->SetSeriesFormat( format.c_str() );
    namesGenerator->SetStartIndex( start[i] );
    namesGenerator->SetEndIndex( start[i] + size[i] - 1 );
    namesGenerator->SetIncrementIndex( 1 );
    seriesWriter->SetFileNames( namesGenerator->GetFileNames() );

    try
    {
        seriesWriter->Update();
        //seriesWriterWithoutOverlay->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
        std::cerr << "Exception thrown while writing the series " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
    }

    /**
      *Writing the 3D outputDirector
      */
    /*cout<< b << endl;
    typedef itk::ImageFileWriter<RGBImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName("testImg/coucou/try.nii.gz");
    writer->SetInput(filter->GetOutput());
    writer->Update();*/

    return EXIT_SUCCESS;




}
