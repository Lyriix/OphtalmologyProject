    #include "WatershedImageFilter.hxx"
#include <sys/stat.h>
#include <sys/types.h>




int main  ( int argc, char*argv[])
{
    //verify arguments
    if(argc <4)
    {
        std::cerr<<"Input"<<std::endl;
        std::cerr <<"Parameters " <<std::endl;
        std::cerr << "threshold level "<<std::endl;
        return 1;
    }

    //Parse arguments

    std::string strInput = argv[1];              //input file path

    std::string strThreshold = argv[2];          //input threshold
    float threshold = .0;
    std::stringstream ssThreshold;
    ssThreshold << strThreshold;
    ssThreshold >> threshold;

    std::string strLevel = argv[3];              //input level
    float level = 0.0;
    std::stringstream ssLevel;
    ssLevel <<strLevel;
    ssLevel >> level;

    //Create output folder
    int chRes = mkdir( "Results_3d", 0777 );
    std::stringstream outputFilename ;
    outputFilename <<  "../watershed_" <<threshold<<"_"<<level<<".nrrd";

    //Output arguments
    std::cout << "Running with : " << std ::endl
              <<"Threshold : " << threshold <<std::endl
             << "Level : " << level << std::endl;

    std::cout<<" Reader " << std::endl;
    ReaderStackType::Pointer reader = ReaderStackType::New();
    reader->SetFileName(strInput);
    reader->Update();


    //Get original origine of the stack : Use at end
    const RGBImageStackType::PointType origineOriginale = reader->GetOutput()->GetOrigin();
    std::cout<<"Gradient Magnitude " << std::endl;

    //Computes the gradient magnitude of an image region at each pixel
    typedef itk::GradientMagnitudeImageFilter<UnsignedCharImageStackType, GradImageType> GradientMagnitudeImageFilterType;
    GradientMagnitudeImageFilterType::Pointer gradientMagnitudeImageFilter = GradientMagnitudeImageFilterType::New();
    gradientMagnitudeImageFilter->SetInput(reader->GetOutput());
    gradientMagnitudeImageFilter->Update();
    std::cout<<" Watershed " << std::endl;



    //custom parameters
    typedef itk::WatershedImageFilter<GradImageType> WatershedFilterType;
    WatershedFilterType::Pointer watershed = WatershedFilterType::New();
    watershed->SetThreshold(threshold);
    watershed->SetLevel(level);
    watershed->SetInput(gradientMagnitudeImageFilter->GetOutput());
    watershed->SetNumberOfThreads(12);
    watershed->Update();

    LabeledImageType::Pointer labelImg = watershed->GetOutput();

    TImtoImFilter::Pointer imageToImageFilter = TImtoImFilter::New();
    imageToImageFilter->SetInput(labelImg);
    imageToImageFilter->Update();

    LabelWriterStackType::Pointer writerTiff = LabelWriterStackType::New();
    writerTiff->SetFileName(outputFilename.str());
    writerTiff->SetInput(imageToImageFilter->GetOutput());
    writerTiff->SetImageIO(itk::NrrdImageIO::New());

    try
    {
        writerTiff->Update();
    }
    catch(itk::ExceptionObject & error)
    {
        std::cerr<<"Error : "<<error << std::endl;
        return EXIT_FAILURE;
    }

    LabelWriterT::Pointer  labelWriter = LabelWriterT::New();
    labelWriter->SetFileName("../label.nrrd");
    labelWriter->SetInput(labelImg);
    labelWriter->SetImageIO(itk::NrrdImageIO::New());

    try
    {
        labelWriter->Update();
    }
    catch(itk::ExceptionObject & error)
    {
        std::cerr<<"Error : "<<error << std::endl;
        return EXIT_FAILURE;
    }



//    try
//    {
//        writerTiff->Update();
//    }
//    catch(itk::ExceptionObject & error)
//    {
//        std::cerr<<"Error : "<<error << std::endl;
//        return EXIT_FAILURE;
//    }

    return EXIT_SUCCESS;
}



