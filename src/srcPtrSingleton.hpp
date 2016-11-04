#ifndef src_ptr_singleton_hpp
#define src_ptr_singleton_hpp

#include <srcPtrDeclPolicy.hpp>

//The singleton is currently used as a temporary workaround to tell srcPtrDeclData to srcPtrPolicy
class srcPtrSingleton
{
    srcPtrDeclPolicy::srcPtrDeclData value;
    static srcPtrSingleton *currentInstance;

  public:
    srcPtrDeclPolicy::srcPtrDeclData GetValue()
    {
        return value;
    }
    void SetValue(srcPtrDeclPolicy::srcPtrDeclData v)
    {
        value = v;
    }
    static srcPtrSingleton *Instance()
    {
        if (!currentInstance)
          currentInstance = new srcPtrSingleton;
        return currentInstance;
    }
};

srcPtrSingleton *srcPtrSingleton::currentInstance = 0;

#endif
