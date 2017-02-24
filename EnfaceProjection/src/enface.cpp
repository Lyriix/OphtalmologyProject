#include "enface.hpp"


typedef unsigned short              PixelType;
typedef itk::Image< PixelType, 2 >  ImageType2D;
typedef itk::Image< PixelType, 3 >  ImageType3D;

typedef itk::ImageLinearIteratorWithIndex< ImageType2D > LinearIteratorType;
typedef itk::ImageSliceConstIteratorWithIndex< ImageType3D
> SliceIteratorType;

typedef itk::ImageFileReader< ImageType3D > ReaderType;
typedef itk::ImageFileWriter< ImageType2D > WriterType;

int main(int argc, char* argv[])
{
    if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1],"--help")==0)
    {
        std::cout<<"Enface projection : " << std::endl;
        std::cout<<"-input Filename"<<std::endl
                <<"-output filename"<<std::endl
                <<"-Projection direction"<<std::endl;
        return EXIT_SUCCESS;
    }

    //   Verify the number of parameters on the command line.
    if ( argc < 4 )
    {
        std::cerr << "Missing parameters. " << std::endl;
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0]
                << " inputImageFile outputImageFile projectionDirection"
                << std::endl;
        return EXIT_FAILURE;
    }

    ImageType3D::ConstPointer inputImage;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[1]);
    try
    {
        reader->Update();
        inputImage=reader->GetOutput();
    }
    catch( itk::ExceptionObject &err)
    {
        std::cerr << "Exception object caught while reading update" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    //The projection direction is read from the command ine. The projection image will
    // be the size of the 2D plane orthogonal to the projection direction.
    //Its spanning vectors are the two remaining coordinate axes in the volume.
    // These axes are recorded in the direction array.

    unsigned int projectionDirection = static_cast<unsigned int>( ::atoi(argv[3]));
    unsigned int i, j;
    unsigned int direction[2];
    for(i=0, j=0 ; i<3 ; ++i)
    {
        if(i != projectionDirection)
        {
            direction[j] = i ;
            j++;
        }
    }


    //The direction array is now used to define the projection
    //image size based based on the input image size.
    // The output image is created so that its common
    // dimension(s) with the input image are the same size. FOr example, if
    // we project along the x axis  of the input, the size and origin of the y axes
    //of the input and output will match. this makes the code slightly more
    //complicated, but prevents a counter intuitive rotation of the output

    ImageType2D::RegionType region;
    ImageType2D::RegionType::SizeType size;
    ImageType2D::RegionType::IndexType index;

    ImageType3D::RegionType requestedRegion = inputImage->GetRequestedRegion();

    index[ direction[0] ] = requestedRegion.GetIndex()[ direction[0] ];
    index[ direction[1] ] = requestedRegion.GetIndex()[ direction[1] ];

    size[ direction[0] ] = requestedRegion.GetSize()[ direction[0] ];
    size[ 1-direction[0] ] = requestedRegion.GetSize()[ direction[1] ];

    region.SetSize(size);;
    region.SetIndex(index);;

    ImageType2D::Pointer outputImage = ImageType2D::New();

    outputImage->SetRegions( region);;
    outputImage->Allocate();

    //Next we create the necessary iterators. The const slice iterator walks
    // the 3d input image, and the non-const linear iterator walks the 2D output image.
    // The iterators are initialized to walk the same linear path trhough a slice.
    // Remember that the second direction of the slice iterator defines the direction
    // that linear iteration walks within a slice.

    SliceIteratorType inputIt(inputImage, inputImage->GetRequestedRegion() );
    LinearIteratorType outputIt(outputImage, outputImage->GetRequestedRegion() );

    inputIt.SetFirstDirection( direction[1] );
    inputIt.SetSecondDirection( direction[0] );
    outputIt.SetDirection( 1 - direction[0] );

    //Now we are ready to compute the projection. The first step is to initialize
    // all of the projection values to their non positivte minimum value. The
    //projection values are then upfates row by roz from the first slice of the
    //input. At the eend of the first slice, the input iterator steprs to the first
    // row in the next slice, while the output iterators, whose underlying image
    // consists of onlu one slice, rewinds to its first row. The  process repeats
    // until the last slice of the input is processed

    outputIt.GoToBegin();
    while( ! outputIt.IsAtEnd() )
    {
        while(! outputIt.IsAtEndOfLine() )
        {
            outputIt.Set( itk::NumericTraits<unsigned short>::NonpositiveMin() );
            ++outputIt;
        }
        outputIt.NextLine();
    }

    inputIt.GoToBegin();
    outputIt.GoToBegin();

    while(! inputIt.IsAtEnd() )
    {
        while( !inputIt.IsAtEndOfSlice() )
        {
            while( !inputIt.IsAtEndOfLine() )
            {
               outputIt.Set( std::max(outputIt.Get(),inputIt.Get()));
               // outputIt.Set( (outputIt.Get() + inputIt.Get()) / 2 );
                ++inputIt;
                ++outputIt;
            }
            outputIt.NextLine();
            inputIt.NextLine();
        }
        outputIt.GoToBegin();
        inputIt.NextSlice();
    }

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( argv[2] );
    writer->SetInput(outputImage);
    try
    {
        writer->Update();
    }
    catch ( itk::ExceptionObject &err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
