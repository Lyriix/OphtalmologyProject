


#include "connectedComponent.hpp"


int main(int argc, char* argv[])
{

    if ( strcmp(argv[1], "-h") == 0 )
    {
        std::cout << "./ConnectedComponent inputfilename outputFilename"<< std::endl;
        return EXIT_FAILURE;
    }
    char* output = argv[2];
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[1]);
    reader->Update();

    ConnectedComponentImageFilterType::Pointer connected =
            ConnectedComponentImageFilterType::New();

    connected->SetInput(reader->GetOutput());
    connected->SetBackgroundValue(0);
    connected->SetFullyConnected(1);
    connected->Update();

    std::cout<<"Number Of Objects: "<<connected->GetObjectCount() <<std::endl;

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput(connected->GetOutput());
    writer->SetFileName(argv[2]);
    writer->Update();

    return EXIT_SUCCESS;

}
