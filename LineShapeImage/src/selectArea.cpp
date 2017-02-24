
#include "selectArea.hpp"

ImageType::Pointer selectArea(ImageType::Pointer image,
           int startX, int startY, int startZ,
           int sizeX, int sizeY, int sizeZ)
{
    ImageType::RegionType inputRegion;
    ImageType::RegionType::IndexType inputStart;
    ImageType::RegionType::SizeType size;

    inputStart[0] = startX;
    inputStart[1] = startY;
    inputStart[2] = startZ;

    size[0] = sizeX;
    size[1] = sizeY;
    size[2] = sizeZ;

    inputRegion.SetSize(size);
    inputRegion.SetIndex(inputStart);


    ImageType::RegionType outputRegion;

    ImageType::RegionType::IndexType outputStart;
    outputStart[0]=0;
    outputStart[1]=0;
    outputStart[2]=0;

    outputRegion.SetSize(size);
    outputRegion.SetIndex(outputStart);

    //check that the region is contained within the input image
    if( ! image->GetRequestedRegion().IsInside(inputRegion) )
    {
        std::cerr << "ERROR"<<std::endl;
        std::cerr << " The region "<< inputRegion << " is not contained within the input image region "
                  << image->GetRequestedRegion() << std::endl;
        //return EXIT_FAILURE;
    }

    ImageType::Pointer outputImage = ImageType::New();
    outputImage->SetRegions(outputRegion);
    const ImageType::SpacingType spacing = image->GetSpacing();
    const ImageType::PointType inputOrigin = image->GetOrigin();
    double outputOrigin[ Dim ];
    for(unsigned int i=0 ; i<Dim ; i++)
        outputOrigin[i] = inputOrigin[i] + spacing[i] * inputStart[i];

    outputImage->SetSpacing(spacing);
    outputImage->SetOrigin(outputOrigin);
    outputImage->Allocate();

    ConstIteratorType inputIt(image,inputRegion);
    IteratorType outputIt(outputImage,outputRegion);
    inputIt.GoToBegin();
    outputIt.GoToBegin();
    while(!inputIt.IsAtEnd())
    {
        outputIt.Set(inputIt.Get());
        ++inputIt; ++outputIt;
    }

    return outputImage;
}
