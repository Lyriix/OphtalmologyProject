#include "blackTop.hpp"

ImageType::Pointer PerformBlackTopHat(itk::Image<PixelType, dimension>::Pointer& image, float radius)
{
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();

    BlackTopHatFilterType::Pointer blackTopHatFilter
            = BlackTopHatFilterType::New();
    blackTopHatFilter->SetInput(image);
    blackTopHatFilter->SetKernel(structuringElement);
    blackTopHatFilter->Update();

    return blackTopHatFilter->GetOutput();
}
