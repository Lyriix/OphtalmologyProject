#include "close.hpp"


ImageType::Pointer PerformClosing(ImageType::Pointer& image, float radius)
{

    StructuringElementType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();


    BinaryMorphologicalClosingImageFilterType::Pointer closingFilter
            = BinaryMorphologicalClosingImageFilterType::New();
    closingFilter->SetInput(image);
    //closingFilter->SetForegroundValue(1); //default to maximum value of inputPixeltype
    closingFilter->SetKernel(structuringElement);
    closingFilter->Update();
    return closingFilter->GetOutput();
}
