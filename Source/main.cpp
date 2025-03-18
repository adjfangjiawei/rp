#include "Frontend/Parser/Parser.h"
#include "Frontend/Sema/Sema.h"
#include "Backend/CodeGen/CodeGen.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }

    try
    {
        // TODO: 实现编译流程
        std::cout << "RP Language Compiler" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
