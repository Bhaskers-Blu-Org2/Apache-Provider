/* @migen@ */
#include <MI.h>
#include "Apache_HTTPDVirtualHost_Class_Provider.h"

#include <string>
#include <vector>

// Apache Portable Runtime definitions
#include "apr.h"
#include "apr_errno.h"
#include "apr_global_mutex.h"
#include "apr_shm.h"

// Virtual host memory mapped file definitions
#include "apachebinding.h"

// Convert a string encoded in this project's base-64 encoding into a 16-bit integer
static apr_uint16_t decode64(const char* str)
{
    return (((apr_uint16_t)(unsigned char)*str       - 0x0030) << 12) |
           (((apr_uint16_t)(unsigned char)*(str + 1) - 0x0030) <<  6) |
            ((apr_uint16_t)(unsigned char)*(str + 2) - 0x0030);
}

MI_BEGIN_NAMESPACE

static void EnumerateOneInstance(Context& context,
        bool keysOnly,
        apr_size_t item)
{
    Apache_HTTPDVirtualHost_Class inst;
    mmap_vhost_elements *vhosts = g_apache.GetVHostElements();

    // Insert the key into the instance
    inst.InstanceID_value(g_apache.GetDataString(vhosts[item].instanceIDOffset));

    if (! keysOnly)
    {
        // Get the IP addresses and ports from the array of (IP address, port) items
        // corresponding to this host

        std::vector<mi::String> ipAddressesArray;
        std::vector<mi::Uint16> portsArray;

        const char* ptr = g_apache.GetDataString(vhosts[item].addressesAndPortsOffsets);
        while (*ptr != '\0')
        {
            ipAddressesArray.push_back(ptr);
            ptr += strlen(ptr) + 1;
            portsArray.push_back(decode64(ptr));
            ptr += strlen(ptr) + 1;
        }

        // Insert the values into the instance

        inst.ServerName_value(g_apache.GetDataString(vhosts[item].hostNameOffset));

        inst.DocumentRoot_value(g_apache.GetDataString(vhosts[item].documentRootOffset));
        inst.ServerAdmin_value(g_apache.GetDataString(vhosts[item].serverAdminOffset));
        inst.ErrorLog_value(g_apache.GetDataString(vhosts[item].logErrorOffset));
        inst.CustomLog_value(g_apache.GetDataString(vhosts[item].logCustomOffset));
        inst.AccessLog_value(g_apache.GetDataString(vhosts[item].logAccessOffset));
        mi::StringA ipAddressesA(&ipAddressesArray[0], (MI_Uint32)ipAddressesArray.size());
        inst.IPAddresses_value(ipAddressesA);
        mi::Uint16A portsA(&portsArray[0], (MI_Uint32)portsArray.size());
        inst.Ports_value(portsA);
    }

    context.Post(inst);
}

Apache_HTTPDVirtualHost_Class_Provider::Apache_HTTPDVirtualHost_Class_Provider(
    Module* module) :
    m_Module(module)
{
}

Apache_HTTPDVirtualHost_Class_Provider::~Apache_HTTPDVirtualHost_Class_Provider()
{
}

void Apache_HTTPDVirtualHost_Class_Provider::Load(
        Context& context)
{
    CIM_PEX_BEGIN
    {
        if (APR_SUCCESS != g_apache.Load("VirtualHost"))
        {
            context.Post(MI_RESULT_FAILED);
            return;
        }

        // Notify that we don't wish to unload
        MI_Result r = context.RefuseUnload();
        if ( MI_RESULT_OK != r )
        {
            g_apache.DisplayError(g_apache.OMI_Error(r), "Apache_HTTPDVirtualHost_Class_Provider refuses to not unload");
        }

        context.Post(MI_RESULT_OK);
    }
    CIM_PEX_END( "Apache_HTTPDVirtualHost_Class_Provider::Load" );
}

void Apache_HTTPDVirtualHost_Class_Provider::Unload(
        Context& context)
{
    CIM_PEX_BEGIN
    {
        if (APR_SUCCESS != g_apache.Unload("VirtualHost"))
        {
            context.Post(MI_RESULT_FAILED);
            return;
        }

        context.Post(MI_RESULT_OK);
    }
    CIM_PEX_END( "Apache_HTTPDVirtualHost_Class_Provider::Load" );
}

void Apache_HTTPDVirtualHost_Class_Provider::EnumerateInstances(
    Context& context,
    const String& nameSpace,
    const PropertySet& propertySet,
    bool keysOnly,
    const MI_Filter* filter)
{
    apr_status_t status;

    CIM_PEX_BEGIN
    {
        /* Lock the mutex to walk the list */
        if (APR_SUCCESS != (status = g_apache.LockMutex()))
        {
            g_apache.DisplayError(status, "VirtualHost::EnumerateInstances: failed to lock mutex");
            context.Post(MI_RESULT_FAILED);
            return;
        }

        for (apr_size_t i = 2; i <= g_apache.GetVHostCount() - 1; i++)
        {
            EnumerateOneInstance(context, keysOnly, i);
        }

        // Also display _Default
        EnumerateOneInstance(context, keysOnly, 1);

        context.Post(MI_RESULT_OK);
    }
    CIM_PEX_END( "Apache_HTTPDVirtualHost_Class_Provider::EnumerateInstances" );

    // Be sure mutex gets unlocked, regardless if an exception occurs
    g_apache.UnlockMutex();
}

void Apache_HTTPDVirtualHost_Class_Provider::GetInstance(
    Context& context,
    const String& nameSpace,
    const Apache_HTTPDVirtualHost_Class& instanceName,
    const PropertySet& propertySet)
{
    context.Post(MI_RESULT_NOT_SUPPORTED);
}

void Apache_HTTPDVirtualHost_Class_Provider::CreateInstance(
    Context& context,
    const String& nameSpace,
    const Apache_HTTPDVirtualHost_Class& newInstance)
{
    context.Post(MI_RESULT_NOT_SUPPORTED);
}

void Apache_HTTPDVirtualHost_Class_Provider::ModifyInstance(
    Context& context,
    const String& nameSpace,
    const Apache_HTTPDVirtualHost_Class& modifiedInstance,
    const PropertySet& propertySet)
{
    context.Post(MI_RESULT_NOT_SUPPORTED);
}

void Apache_HTTPDVirtualHost_Class_Provider::DeleteInstance(
    Context& context,
    const String& nameSpace,
    const Apache_HTTPDVirtualHost_Class& instanceName)
{
    context.Post(MI_RESULT_NOT_SUPPORTED);
}


MI_END_NAMESPACE
