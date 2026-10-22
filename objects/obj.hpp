
#include <fstream>
#include <string>
#include "object.hpp"

#pragma once

class OBJ : public Object
{
public:

    void MakeOBJ(std::string filepath)  {
        // Try to open the file
        std::ifstream file(filepath);
    }
};
