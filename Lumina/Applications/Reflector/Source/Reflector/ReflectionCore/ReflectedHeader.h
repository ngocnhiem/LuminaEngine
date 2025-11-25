#pragma once
#include <string>

#include "EASTL/string.h"
#include "EASTL/vector.h"


namespace Lumina::Reflection
{
    class FReflectedHeader
    {
    public:

        FReflectedHeader()
            :bSkipCodeGen(false)
        {}
        
        FReflectedHeader(const eastl::string& Path);

        bool Parse();
        
        
        eastl::string             FileName;
        eastl::string             HeaderPath;
        uint8_t                   bSkipCodeGen:1;
    };
}
