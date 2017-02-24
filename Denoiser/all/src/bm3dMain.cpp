#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string.h>
#include "main.hpp"
#include "bm3d.h"
//#include "utilities.h"

#define YUV       0
#define YCBCR     1
#define OPP       2
#define RGB       3
#define DCT       4
#define BIOR      5
#define HADAMARD  6
#define NONE      7



using namespace std;
int bm3d(ImageType::Pointer image_noisy, float fSigma, const bool useSD_1, const bool useSD_2, string tau_2D_hard, string tau_2D_wien,
         string color_space);

/**
 * @file bm3dMain.cpp
 * @brief Main file for use of BM3D in a research way of comparing to other filter. Do not use lib_fftw to process DCT.
 *
 * @author MARC LEBRUN <marc.lebrun@cmla.ens-cachan.fr>
 * */

int bm3d(ImageType::Pointer image_noisy, float fSigma, const bool useSD_1, const bool useSD_2, string tau_2D_hard, string tau_2D_wien,
         string color_space)
{
    //Check if there is the right call for the algorithm

    cout << "usage : BM3D image sigma noisy basic denoised difference bias \
            difference_bias computeBias tau_2d useSD_hard \
            tau_2d_wien useSD_wien color_space" << endl;

            //! Declarations
    ImageType::Pointer img = ImageType::New();
    ImageType::Pointer img_noisy = ImageType::New();
    ImageType::Pointer img_basic = ImageType::New();
    ImageType::Pointer img_denoised = ImageType::New();
    ImageType::Pointer img_bias = ImageType::New();
    ImageType::Pointer img_diff = ImageType::New();
    ImageType::Pointer img_basic_bias = ImageType::New();
    ImageType::Pointer img_diff_bias = ImageType::New();
    unsigned width, height, chnls;
    ImageType::SizeType size = image_noisy->GetLargestPossibleRegion().GetSize();
    width = size[0];
    height = size[1];
    chnls = size[2];


    //! Variables initialization
    const unsigned tau_2D_Hard = (strcmp(tau_2D_hard,"dct") == 0 ? DCT : (strcmp(tau_2D_hard,"bior") == 0 ? BIOR : NONE ));

    if(tau_2D_Hard == NONE)
    {
        cout<<"tau_2d_hard is not known, Choice is "<< endl;
        cout << " -dct" << endl;
        cout << " -bior" << endl;
        return EXIT_FAILURE;
    }

    const unsigned tau_2D_Wien  = (strcmp(tau_2D_wien, "dct" ) == 0 ? DCT :
                                                                      (strcmp(tau_2D_wien, "bior") == 0 ? BIOR : NONE));

    if (tau_2D_Wien == NONE)
    {
        cout << "tau_2d_wien is not known. Choice is :" << endl;
        cout << " -dct" << endl;
        cout << " -bior" << endl;
        return EXIT_FAILURE;
    };

    const unsigned colorSpace = (strcmp(color_space, "rgb"  ) == 0 ? RGB   :
                                                                     (strcmp(color_space, "yuv"  ) == 0 ? YUV   :
                                                                                                          (strcmp(color_space, "ycbcr") == 0 ? YCBCR :
                                                                                                                                               (strcmp(color_space, "opp"  ) == 0 ? OPP   : NONE))));

    if (colorSpace == NONE)
    {
        cout << "color_space is not known. Choice is :" << endl;
        cout << " -rgb" << endl;
        cout << " -yuv" << endl;
        cout << " -opp" << endl;
        cout << " -ycbcr" << endl;
        return EXIT_FAILURE;
    };

    unsigned       wh           = (unsigned) width * height;
    unsigned       whc          = (unsigned) wh * chnls;
    //bool           compute_bias = (bool) atof(argv[9]);

    //! Denoising

    if(run_bm3d(fSigma, image_noisy, img_basic, img_denoised, width, height, chnls,
                useSD_1, useSD_2, tau_2D_Hard, tau_2D_Wien, colorSpace) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    //Return imahe

    return EXIT_SUCCESS;


}
