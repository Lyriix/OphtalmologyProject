#include "anisotropicFilter.hpp"

StackType::Pointer PerformDenoisement(StackType::Pointer stack, int numberOfIterations,
                        float timeStep, float conductance)
{
    AnisotropicFilterType::Pointer anisotropicFilter = AnisotropicFilterType::New();
    anisotropicFilter->SetInput(stack);

    //Set filter parameters
    anisotropicFilter->SetNumberOfIterations(numberOfIterations); //typically five
    anisotropicFilter->SetTimeStep(timeStep); //typical 0.25 in 2d 0.125 in 3d
    anisotropicFilter->SetConductanceParameter(conductance);

    anisotropicFilter->Update();

    return anisotropicFilter->GetOutput();
}
