#include "imageCreation.hpp"

int main(int argc, char* argv[])
{

    char* type = argv[1];
    if(std::strcmp(argv[1],"-h") == 0)
    {
        std::cout << " Usage of Create image " << std::endl;
        std::cout << "./CreateImage [square][sphere][tube]{exclusive}"<<std::endl;
        return EXIT_SUCCESS;
    }

    ImageType::RegionType region;
    ImageType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;

    float taille = 250;
    ImageType::SizeType size;
    size[0]=taille;
    size[1]=taille;
    size[2]=taille;

    region.SetSize(size);
    region.SetIndex(start);

    ImageType::Pointer image = ImageType::New();
    image->SetRegions(region);
    image->Allocate();

    /** Square 2 : 2*150*150 */
    if(std::strcmp(type,"square") == 0)
    {
        for(unsigned int i = 150 ; i < 153 ; i++)
            for(unsigned int j=100; j<200 ; j++ )
                for(unsigned int k=100; k<200; k++)
                {
                    ImageType::IndexType pixelIndex2;
                    pixelIndex2[0]=i;
                    pixelIndex2[1]=j;
                    pixelIndex2[2]=k;

                    image->SetPixel(pixelIndex2, 255);
                }
    }

    //    /** Line */

    //            ImageType::IndexType centre;
    //            centre[0] = taille/4; centre[1] = taille/4; centre[2] = taille/4;
    //            float r = 2.0;
    //    for(unsigned int k = 150 ; k<300; k++)
    //        for(unsigned int i = 0 ; i < 100 ; i++)
    //            for(unsigned int j=0; j<100 ; j++)
    //            {
    //                if(centre[0] - r < centre[0] + i && centre[0] + r > centre[0] +i)
    //                    if(centre[1] - r < centre[1] + j && centre[1] + r > centre[1] +j)
    //                    {
    //                        ImageType::IndexType pixelIndex2;
    //                        pixelIndex2[0]=centre[0] +i;
    //                        pixelIndex2[1]=centre[1] +j;
    //                        pixelIndex2[2]=k;

    //                        image->SetPixel(pixelIndex2, 255);
    //                    }
    //            }

    float r= 4.0;
    ImageType::IndexType centre;
    centre[0] = taille/4; centre[1] = taille/4; centre[2] = taille/4;
    /** TUbe */
    if(std::strcmp(type,"tube") == 0)
    {
        for(int a =taille/4 ; a <taille/2 ; a++)
            for(int kO = 0 ; kO < 40 ; kO++)
                for(float ra=0.0 ; ra< r ; ra += 0.001f)
                {

                    float th = kO*3.14f / 20.0f;

                    float x = centre[0] + ra*cos(th);
                    float y = centre[1] + ra*sin(th);
                    float z = centre[2] + a;
                    {
                        ImageType::IndexType pixelIndex3;
                        pixelIndex3[0]=floor(x);
                        pixelIndex3[1]=floor(y);
                        pixelIndex3[2]=floor(z);

                        image->SetPixel(pixelIndex3, 255);
                    }

                }
    }
    /** Blop / SPhere */
    if(std::strcmp(type,"sphere")==0)
    {
        for(int kO = 0 ; kO < 40 ; kO++)
        {
            for(int kp = 0 ; kp <40 ; kp++)
            {
                for(float ra = 0.0f ; ra < r ; ra +=0.001f)
                {
                    float th = (float)kO*3.14f / 20.0f;
                    float  phi = (float)kp*3.14f/20.0f;
                    float x = centre[0]+10 + ra*cos(th)*sin(phi);
                    float y = centre[1]+10 + ra*sin(th)*sin(phi);
                    float z = centre[2] +ra*cos(phi);
                    {
                        ImageType::IndexType pixelIndex3;
                        pixelIndex3[0]=floor(x);
                        pixelIndex3[1]=floor(y);
                        pixelIndex3[2]=floor(z);

                        image->SetPixel(pixelIndex3, 255);
                    }
                }
            }
        }
    }

/*

    SpatialObjectToImageFilterType::Pointer imageFilter =
            SpatialObjectToImageFilterType::New();


    ImageType::SpacingType spacing;
    spacing[0] =  2*size[0] / size[0];
    spacing[1] =  2*size[1] / size[1];
    spacing[2] =  2*size[2] / size[2];

    imageFilter->SetSpacing( spacing );


    EllipseType::Pointer ellipse    = EllipseType::New();
    CylinderType::Pointer cylinder = CylinderType::New();
    PlaneType::Pointer plane = PlaneType::New();

    ellipse->SetRadius(  size[0] * 0.08 * spacing[0] );
    cylinder->SetRadius(  size[0] * 0.05 * spacing[0] );
    cylinder->SetHeight( size[2] * 0.30 * spacing[2]);
    ImageType::PointType planeLowerPoint;
    planeLowerPoint[0] = 2;
    planeLowerPoint[1] = 2;
    planeLowerPoint[2] = 2;
    plane->SetLowerPoint(planeLowerPoint);
    ImageType::PointType planeUpperPoint;
    planeUpperPoint[0] = 20;
    planeUpperPoint[1] = 20;
    planeUpperPoint[2] = 2;
    plane->SetUpperPoint(planeUpperPoint);




    typedef GroupType::TransformType                 TransformType;
    TransformType::Pointer transform1 = TransformType::New();
    TransformType::Pointer transform2 = TransformType::New();
    TransformType::Pointer transform3 = TransformType::New();
    transform1->SetIdentity();
    transform2->SetIdentity();
    transform3->SetIdentity();

    TransformType::OutputVectorType  translation;
    TransformType::CenterType        center;
    translation[ 0 ] =  size[0] * spacing[0] *0.75;
    translation[ 1 ] =  size[1] * spacing[1] *0.75;
    translation[ 2 ] =  size[2] * spacing[2] *0.75;
    transform1->Translate( translation, false );

    translation[ 0 ] =  size[0] * spacing[0] *0.5;
    translation[ 1 ] =  size[1] * spacing[1] *0.5;
    translation[ 2 ] =  size[2] * spacing[2] *0.5;
    //transform2->Rotate( 1, 2, itk::Math::pi / 2.0 );
    transform2->Translate( translation, false );

    translation[ 0 ] =  size[0] * spacing[0] *1.25;
    translation[ 1 ] =  size[1] * spacing[1] *1.25;
    translation[ 2 ] =  size[2] * spacing[2] *1.25;
    transform3->Translate( translation, false );

    ellipse->SetObjectToParentTransform( transform1 );
    cylinder->SetObjectToParentTransform( transform2 );
    plane->SetObjectToParentTransform( transform3 );

    GroupType::Pointer group = GroupType::New();
    group->AddSpatialObject( ellipse );
    group->AddSpatialObject( cylinder );
    group->AddSpatialObject( plane );

    imageFilter->SetInput(  group  );




    const PixelType airHounsfieldUnits  = 0;
    const PixelType boneHounsfieldUnits =   255;
    ellipse->SetDefaultInsideValue(   boneHounsfieldUnits );
    cylinder->SetDefaultInsideValue( boneHounsfieldUnits );
    plane->SetDefaultInsideValue( boneHounsfieldUnits );
    ellipse->SetDefaultOutsideValue(   airHounsfieldUnits );
    cylinder->SetDefaultOutsideValue( airHounsfieldUnits );
    plane->SetDefaultOutsideValue( airHounsfieldUnits );
    imageFilter->SetUseObjectValue( true );
    imageFilter->SetOutsideValue( airHounsfieldUnits );
    imageFilter->Update();
*/

    WriterType::Pointer writer = WriterType::New();
    std::string filename = "image_test_";
    filename += argv[1];
    filename += ".nii.gz";
    writer->SetFileName(filename);
    writer->SetInput(image);
    writer->SetImageIO(itk::NiftiImageIO::New());
    writer->Update();



    return EXIT_SUCCESS;
}
