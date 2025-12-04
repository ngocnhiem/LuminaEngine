#pragma once
#include "Module/API.h"

namespace Lumina
{
    class FRHIImage;
    class CObject;
}

namespace Lumina::ThumbnailGenerator
{
    LUMINA_API FRHIImage* GenerateImageForObject(CObject* Object);
 
}
