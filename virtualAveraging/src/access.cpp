#include "access.hpp"

/*Access each pixel once */
std::vector<unsigned short> accessEachOnce(std::vector<unsigned short> pixelVector, Choice choice) //rappel : passage argument par reference -> permet de manipuler directement la variable associee
{


    srand(time(NULL));
    int randNoise =0;
    for( std::vector<unsigned short>::iterator it = pixelVector.begin(); it != pixelVector.end() ; it++)
    {
        switch(choice)
        {
        case RANDOM :

            //add  uniform deviation
            randNoise = rand() % 13 -6 ; //from 0 to 12 minus 6 => from -6 to +6
            *it += randNoise;
            if(*it < 0){ *it = 0;}
            if( *it > 255){ *it = 255;}
            break;

        case GAUSSIAN_DEV:

            //add gaussian deviation
            randNoise = GaussianGenerator();

            *it += randNoise;
            if(*it < 0){ *it = 0;}
            if( *it > 255){ *it = 255;}
            break;

        default:
            break;

        }


    }

    return pixelVector;
}


//Access Randomly one pixel of pixelVector 'nbSelectPixels' times
std::vector<unsigned short> accessUniform(std::vector<unsigned short> pixelVector, Choice choice)
{

    typedef std::chrono::high_resolution_clock myclock;
    myclock::time_point beginning = myclock::now();
    myclock::duration d;
    srand (time(NULL));
    std::vector<unsigned short> modifiedVector = pixelVector;

    int nbSelectPixels = 20;

    for ( int i = 0; i< nbSelectPixels ; i++)
    {
        d = myclock::now() - beginning;
        int iRandom = rand()*d.count() % 9 ; //random nunber between 0 AND 8 , Ideally, 9 should be vector.size/length
        //std::cout << iRandom << std::endl;
        int pixelValue = pixelVector[iRandom];
        //add deviation
        switch(choice)
        {
        case RANDOM : {
            int randNoise = rand() % 13 -6 ; //from 0 to 12 minus 6 => from -6 to +6
            pixelValue += randNoise;
           // if(pixelValue < 0) pixelValue = 0;
           // if( pixelValue > 255) pixelValue = 255;
            modifiedVector.push_back(pixelValue);
            break;
        }

        case GAUSSIAN_DEV:{
            int randNoise = GaussianGenerator();
            //std::cout << randNoise << std::endl ;
                          pixelValue += randNoise;
                       //  if(pixelValue < 0) pixelValue = 0;
                        // if( pixelValue > 255) pixelValue = 255;
                         modifiedVector.push_back(pixelValue);
                         break;
        }

        default:
            break;

        }
    }
    return modifiedVector;
}


//Random access Gaussian Law
std::vector<unsigned short> accessGaussian(std::vector<unsigned short> pixelVector, Choice choice)
{
    std::vector<unsigned short> modifiedVector = pixelVector;
    int iGaussian=0, jGaussian=0,idx=0;
    int nbRepet = 20;

    for(int i=0; i<nbRepet; i++)
    {

        iGaussian = GaussianGenerator(1.0,1.0) ;
        if(iGaussian < 0 ) iGaussian = 0;
        if(iGaussian > 2 ) iGaussian = 2;
        //std::cout<< "iGqussiqn : " << iGaussian << " i :" << i<<  std::endl ;
        jGaussian = GaussianGenerator(1.0,1.0) ;
        if(jGaussian < 0 ) jGaussian = 0;
        if(jGaussian > 2 ) jGaussian = 2;
        //std::cout<< "jGqussiqn : " << jGaussian << std::endl ;
        idx=iGaussian+3*jGaussian;
        //std::cout << "index : " << idx << std::endl;
        //std::cout << "size pixel vector : " << pixelVector.size() << std::endl ;
       // std::cout <<GaussianGenerator(0.0,1.0)<< ' << '<< jGaussian << std::endl;

        int pixelValue = pixelVector[idx];

        switch(choice)
        {
        case UNIFORM:
        {
            int randNoise = rand() % 13 -6 ; //from 0 to 12 minus 6 => from -6 to +6
            pixelValue += randNoise;
           // if(pixelValue < 0) pixelValue = 0;
           // if( pixelValue > 255) pixelValue = 255;
            modifiedVector.push_back(pixelValue);
            break;
        }
        case GAUSSIAN_PIX:
        {
            int randNoise = GaussianGenerator(0.0,3.0);
            pixelValue += randNoise;
          //  if(pixelValue < 0) pixelValue = 0;
           // if( pixelValue > 255) pixelValue = 255;
            modifiedVector.push_back(pixelValue);
            break;
        }
        }
    }
    return modifiedVector;
}

