#include "whiteTop.hpp"

ImageType::Pointer PerformWhiteTopHat(itk::Image<PixelType, dimension>::Pointer& image, float radius)
{
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();

    WhiteTopHatFilterType::Pointer whiteTopHatFilter
            = WhiteTopHatFilterType::New();
    whiteTopHatFilter->SetInput(image);
    whiteTopHatFilter->SetKernel(structuringElement);
    whiteTopHatFilter->Update();

    return whiteTopHatFilter->GetOutput();
}
