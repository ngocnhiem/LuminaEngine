#include "TaskTypes.h"

#include "TaskSystem.h"
#include "Memory/Memory.h"

namespace Lumina
{
    void CompletionActionDelete::OnDependenciesComplete(enki::TaskScheduler* pTaskScheduler_, uint32_t threadNum_)
    {
        enki::ICompletable::OnDependenciesComplete( pTaskScheduler_, threadNum_ );
        
        Memory::Delete(Dependency.GetDependencyTask());
    }
}
