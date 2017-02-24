
#include "main.hpp"


int main(int argc, char* argv[])
{
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[1]);
    try{ reader->Update(); }
    catch(itk::ExceptionObject exception) { std::cerr<< exception<< std::endl ; }




    /** Application of BM3D */

    /** Application of Non Local Means */

    /** Application of Anisotropic Diffusion */



    /** Calcul CNR for each one */
    /** Calcul SNR for each one */


    return EXIT_SUCCESS;

}
