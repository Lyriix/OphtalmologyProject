#include "open.hpp"

ImageType::Pointer PerformOpening(ImageType::Pointer& image , float radius)
{

      StructuringElementType structuringElement;
      structuringElement.SetRadius(radius);
      structuringElement.CreateStructuringElement();

      BinaryMorphologicalOpeningImageFilterType::Pointer openingFilter
              = BinaryMorphologicalOpeningImageFilterType::New();
      openingFilter->SetInput(image);
      openingFilter->SetKernel(structuringElement);
      openingFilter->Update();

      return openingFilter->GetOutput();
}
