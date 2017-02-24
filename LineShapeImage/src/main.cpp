
#include "volume.hpp"
#include "math.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <algorithm>
#include "itkCastImageFilter.h"
typedef itk::CastImageFilter<itk::Image<float,3>, ImageType> CastType;
typedef itk::ImageFileReader< ImageType > ReaderType;
typedef itk::ImageFileReader< RealImageType > RealReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::ImageFileWriter<RealImageType> RealWriterType;
typedef itk::ImageFileWriter<LabelType> BinWriterType;
typedef itk::ImageFileWriter<SegLabelType> SegWriterType;
#include "itkLineShapeImageFilter.h"
typedef itk::LineShapeImageFilter< ImageType > 		LineShapeFilterType;
#include <itkConnectedComponentImageFilter.h>
typedef itk::ConnectedComponentImageFilter<LabelType, LabelType> LabelerType;


#include "selectArea.hpp"
#include "anisotropicFilter.hpp"

#include "itkNrrdImageIO.h"

#include "itkRescaleIntensityImageFilter.h"
typedef itk::RescaleIntensityImageFilter<RealImageType,RealImageType> rescaleType;

#include "itkMinimumMaximumImageCalculator.h"
typedef itk::MinimumMaximumImageCalculator<RealImageType> MinMaxCalculatorType;

#include "itkCastImageFilter.h"
typedef itk::CastImageFilter<RealImageType,ImageType> CasterType;
using namespace std;





int main( int argc, char* argv[] )
{
    std::cout<<argv[1]<<std::endl;
    if(strcmp(argv[1] , "-h") == 0 || strcmp(argv[1] , "--help") == 0)
    {
        std::cout<<"Structure Extraction : "<<std::endl;
        std::cout<<"- input Filename"<<std::endl<<"-nb Iterations (default 1)"<<std::endl<<
                   "-sigma (default 1.0)"<<std::endl<<"-outputFolder (default current)"<<
                   std::endl<<"-extractBright (default true)" <<std::endl;
        return EXIT_SUCCESS;
    }

    //Default value
    int nb_iter = 1;
    double sigma_init = 1.0;
    bool stop = false;
    std::string outputFolderName = "./";
    bool extractBright = true;
    //Parse arguments
    if(argc>2)
    {
        std::stringstream ss_nb_iter, ss_conductance, ss_threshold, ss_level;
        std::string str_nb_iter = argv[2];
        ss_nb_iter << str_nb_iter;
        ss_nb_iter >> nb_iter;
        if(argc>3)
        {
            std::stringstream ss_sigma;
            std::string str_sigma = argv[3];
            ss_sigma << str_sigma;
            ss_sigma >> sigma_init;


            if(argc>4)
            {
                outputFolderName = argv[4];
                if(argc>5)
                    extractBright = argv[5];
            }
        }
    }

    /** Read the input file */
    ReaderType::Pointer realReader = ReaderType::New();
    realReader->SetFileName(argv[1]);
    try{
        realReader->Update();
    }
    catch(itk::ExceptionObject &exc){
        std::cerr<<"Error while reading : Exception object caught !" << std::endl;
        std::cerr<<exc<<std::endl;
    }



    //So the type will be accepted in LineShapeFilter Hesian and functor
    std::cout<<"LineShape begin ..." ;
    LineShapeFilterType::Pointer lineShapeFilter = LineShapeFilterType::New();
    lineShapeFilter->SetInput( realReader->GetOutput());
    lineShapeFilter->SetExtractBrightLine( extractBright );
    lineShapeFilter->EigenValuesExtractionOn();
    lineShapeFilter->LabelImageOn();
    lineShapeFilter->SetNumberOfThreads(12);
    //lineShapeFilter->SetDimensionsProcessed(3); //Useless for our process


    RealImageType::Pointer eigImg0 ;
    RealImageType::Pointer eigImg1 ;
    RealImageType::Pointer eigImg2 ;

    LabelType::Pointer lineBin = LabelType::New();
    LabelType::Pointer planeBin = LabelType::New();
    RealImageType::Pointer sphereImage = RealImageType::New();
    RealImageType::Pointer planeImage = RealImageType::New();
    RealImageType::Pointer lineImage = RealImageType::New();

    double sigma;

    for(int i = 1 ; i<=nb_iter ;i++)
    {
        if(i>1)
            sigma =  i* + sigma_init;
        else
            sigma = sigma_init;
        /** Line Shape Filter */


        lineShapeFilter->SetSigma( sigma );
        lineShapeFilter->Update();
        std::cout<<"Line Shape SIGMA "<< sigma<<std::endl;

        eigImg0 = lineShapeFilter->GetEigenValuesOutput( 0 );
        eigImg1 = lineShapeFilter->GetEigenValuesOutput( 1 );
        eigImg2 = lineShapeFilter->GetEigenValuesOutput( 2 );



        lineImage->SetRegions( eigImg0->GetLargestPossibleRegion() );
        lineImage->SetSpacing( eigImg0->GetSpacing() );
        lineImage->SetOrigin( eigImg0->GetOrigin() );
        lineImage->Allocate();


        planeImage->SetRegions( realReader->GetOutput()->GetLargestPossibleRegion() );
        planeImage->SetSpacing( realReader->GetOutput()->GetSpacing() );
        planeImage->SetOrigin( realReader->GetOutput()->GetOrigin() );
        planeImage->Allocate();

        sphereImage->SetRegions( eigImg0->GetLargestPossibleRegion() );
        sphereImage->SetSpacing( eigImg0->GetSpacing() );
        sphereImage->SetOrigin( eigImg0->GetOrigin() );
        sphereImage->Allocate();

        lineBin->SetRegions( eigImg0->GetLargestPossibleRegion() );
        lineBin->SetSpacing( eigImg0->GetSpacing() );
        lineBin->SetOrigin( eigImg0->GetOrigin() );
        lineBin->Allocate();

        planeBin->SetRegions( eigImg0->GetLargestPossibleRegion() );
        planeBin->SetSpacing( eigImg0->GetSpacing() );
        planeBin->SetOrigin( eigImg0->GetOrigin() );
        planeBin->Allocate();



        float Cp=0, Cs=0, Cl=0;
        int numPixels = 0;
        int numChanged = 0;
        int numUnchanged = 0;
        int numNull = 0;
        RealImageType::SizeType dim3 = lineShapeFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
        for( int x = 0; x < dim3[0]; x++ )
            for( int y = 0; y < dim3[1]; y++ )
                for( int z = 0; z < dim3[2]; z++ )
                {

                    RealImageType::IndexType idx;
                    idx[0] = x;
                    idx[1] = y;
                    idx[2] = z;

                    if(i==1)
                    {
                        planeImage->SetPixel(idx,0.0);
                        //std::cout<<idx<<std::endl;
                    }


                    //Need to have the eigen values of the same sign for bright or dark lines
                    float lambda1 , lambda2, lambda3;
                    if(eigImg0->GetPixel( idx ) <= 0)
                        lambda1 = eigImg0->GetPixel( idx ) * sigma * sigma;
                    else
                        lambda1 = 0.0;
                    if(eigImg1->GetPixel( idx ) <= 0)
                        lambda2 = eigImg1->GetPixel( idx ) * sigma * sigma;
                    else
                        lambda2 = 0.0;
                    if(eigImg2->GetPixel( idx ) <= 0)
                        lambda3 = eigImg2->GetPixel( idx ) * sigma * sigma;
                    else
                        lambda3 = 0.0;

                    eigImg0->SetPixel( idx, lambda1 );
                    eigImg1->SetPixel( idx , lambda2);
                    eigImg2->SetPixel( idx, lambda3 );
                    if(lambda3 != 0.0 ){
                        //if(lambda1 ==0)
                        Cp = (lambda3 - lambda2) / lambda3;
                        Cs = lambda1/lambda3;
                        Cl = (lambda2 - lambda1) / lambda3;
                    }
                    else
                    {
                        Cl = .0;
                        Cp = 0.0;
                        Cs = 0.0;
                    }


                    /*if(Cp > planeImage->GetPixel(idx))
                    {
                        planeImage->SetPixel(idx,Cp);
                    }
                    if(Cl > lineImage->GetPixel(idx))
                    {
                        lineImage->SetPixel(idx,Cl);
                    }
                    if(Cp == 1)
                        stop = false;
*/
                    if(abs(lambda3) > 10e-4 )
                    {
                        planeImage->SetPixel(idx,Cp*10000);
                        if(Cp != 0)
                            planeBin->SetPixel(idx,1);
                        if(Cl != 0)
                            lineBin->SetPixel(idx,1);
                        lineImage->SetPixel(idx,Cl);
                        sphereImage->SetPixel(idx,Cs);

                    }
                    else
                    {
                        planeBin->SetPixel(idx,0);
                        planeImage->SetPixel(idx,0);
                        lineImage->SetPixel(idx,0);
                        sphereImage->SetPixel(idx,0);
                        lineBin->SetPixel(idx,0);
                    }


                }

        if(stop)
            i = nb_iter;
        std::cout<<"Num changed: "<<numChanged<<"  Num pixels: "<<numPixels<<"  Num Unchanged: "<<numUnchanged<<"  numNull: "<<numNull<< "   sum : "<<numNull+numUnchanged+numChanged<<std::endl;
    }




    eigImg0->Update();
    eigImg1->Update();
    eigImg2->Update();
    lineImage->Update();
    planeImage->Update();
    sphereImage->Update();
    lineBin->Update();
    planeBin->Update();

    //    LabelerType::Pointer labeler = LabelerType::New();

    //    labeler->SetInput(planeBin);
    //    labeler->Update();


    RealWriterType::Pointer realWriter = RealWriterType::New();
    BinWriterType::Pointer binWriter = BinWriterType::New();
    WriterType::Pointer writer = WriterType::New();


    //    writer->SetInput(caster->GetOutput());
    //    writer->SetFileName("../area_selected.nrrd");
    //    writer->Update();

    //        realWriter->SetInput(anisotropicFilter->GetOutput());
    //        realWriter->SetFileName("../area_denoised.nrrd");
    //        realWriter->Update();
    std::stringstream ssfilenameoutput;
    ssfilenameoutput << outputFolderName <<"/";
    std::string pathFolder = ssfilenameoutput.str();

    realWriter->SetInput(lineImage);
    realWriter->SetFileName(pathFolder + "line.nrrd");
    realWriter->Update();

    realWriter->SetInput(planeImage);
    realWriter->SetFileName(pathFolder + "plane.nrrd");
    realWriter->Update();

    //    realWriter->SetInput(sphereImage);
    //    realWriter->SetFileName(pathFolder + "sphere.nrrd");
    //    realWriter->Update();

    realWriter->SetInput(eigImg0);
    realWriter->SetFileName(pathFolder + "eigImg1.nrrd");
    realWriter->Update();


    realWriter->SetInput(eigImg1);
    realWriter->SetFileName(pathFolder + "eigImg2.nrrd");
    realWriter->Update();



    realWriter->SetInput(eigImg2);
    realWriter->SetFileName(pathFolder +"eigImg3.nrrd");
    realWriter->Update();


    //    binWriter->SetInput(lineBin);
    //    binWriter->SetFileName(pathFolder +"lineBin.nrrd");
    //    binWriter->Update();

    //    binWriter->SetInput(planeBin);
    //    binWriter->SetInput(planeBin);
    //    binWriter->SetFileName(pathFolder +"planeBin.nrrd");
    //    binWriter->Update();


    return 0;
}


