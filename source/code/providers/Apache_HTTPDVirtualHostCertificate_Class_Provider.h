/* @migen@ */
#ifndef _Apache_HTTPDVirtualHostCertificate_Class_Provider_h
#define _Apache_HTTPDVirtualHostCertificate_Class_Provider_h

#include "Apache_HTTPDVirtualHostCertificate.h"
#ifdef __cplusplus
# include <micxx/micxx.h>
# include "module.h"

MI_BEGIN_NAMESPACE

/*
**==============================================================================
**
** Apache_HTTPDVirtualHostCertificate provider class declaration
**
**==============================================================================
*/

class Apache_HTTPDVirtualHostCertificate_Class_Provider
{
/* @MIGEN.BEGIN@ CAUTION: PLEASE DO NOT EDIT OR DELETE THIS LINE. */
private:
    Module* m_Module;

public:
    Apache_HTTPDVirtualHostCertificate_Class_Provider(
        Module* module);

    ~Apache_HTTPDVirtualHostCertificate_Class_Provider();

    void Load(
        Context& context);

    void Unload(
        Context& context);

    void EnumerateInstances(
        Context& context,
        const String& nameSpace,
        const PropertySet& propertySet,
        bool keysOnly,
        const MI_Filter* filter);

    void GetInstance(
        Context& context,
        const String& nameSpace,
        const Apache_HTTPDVirtualHostCertificate_Class& instance,
        const PropertySet& propertySet);

    void CreateInstance(
        Context& context,
        const String& nameSpace,
        const Apache_HTTPDVirtualHostCertificate_Class& newInstance);

    void ModifyInstance(
        Context& context,
        const String& nameSpace,
        const Apache_HTTPDVirtualHostCertificate_Class& modifiedInstance,
        const PropertySet& propertySet);

    void DeleteInstance(
        Context& context,
        const String& nameSpace,
        const Apache_HTTPDVirtualHostCertificate_Class& instance);

/* @MIGEN.END@ CAUTION: PLEASE DO NOT EDIT OR DELETE THIS LINE. */
};

MI_END_NAMESPACE

#endif /* __cplusplus */

#endif /* _Apache_HTTPDVirtualHostCertificate_Class_Provider_h */

