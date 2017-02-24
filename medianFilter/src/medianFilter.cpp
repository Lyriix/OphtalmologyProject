#include "medianFilter.hpp"

ImageType::Pointer PerformDenoisement(ImageType::Pointer stack)
{
    MedianFilterType::Pointer medianFilter = MedianFilterType::New();
    ImageType::SizeType indexRadius;
    indexRadius[0]=1; //radius along x
    indexRadius[1]=1;// radius along y
    indexRadius[2]=1;// radius along z

    medianFilter->SetRadius(indexRadius);
    medianFilter->SetInput(stack);
    medianFilter->Update();

    return medianFilter->GetOutput();
}
