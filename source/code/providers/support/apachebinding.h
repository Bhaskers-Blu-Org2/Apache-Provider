/*--------------------------------------------------------------------------------
 *        Copyright (c) Microsoft Corporation. All rights reserved. See license.txt for license information.
 *     
 *        */
 /**
      \file        apachebinding.h

      \brief       Provider helper functions for Apache module/provider bindings

      \date        03-04-14
*/
/*----------------------------------------------------------------------------*/

#ifndef APACHEBINDING_H
#define APACHEBINDING_H

// Apache Portable Runtime definitions
#include <apr.h>
#include <apr_atomic.h>
#include <apr_errno.h>
#include <apr_global_mutex.h>
#include <apr_shm.h>
#include <apr_strings.h>

#include "mmap_region.h"
#include "datasampler.h"
#include "temppool.h"


/*------------------------------------------------------------------------------*/
/**
 *   ApacheBinding 
 *   Helper class to help with Apache module <-> Provider bindings
 */
class ApacheBinding
{
public:
    ApacheBinding() {};
    ~ApacheBinding() {};

    static apr_status_t OMI_Error(int err) { return APR_OS_START_USERERR + err; }
    static void DisplayError(apr_status_t status, const char *text);
    static apr_status_t Load(const char *text);
    static apr_status_t Unload(const char *text);

    static const char* GetDataString(apr_size_t offset);

    static const char *GetServerConfigFile() { return GetDataString(ms_server_data->configFileOffset); }
    static const char *GetServerProcessName() { return GetDataString(ms_server_data->processNameOffset); }
    static int GetOperatingStatus() { return ms_server_data->operatingStatus; }
    static apr_size_t GetModuleCount() { return ms_server_data->moduleCount; }
    static mmap_server_modules *GetServerModules() { return ms_server_data->modules; }
    static apr_uint32_t GetWorkerCountIdle() { return apr_atomic_read32(&ms_server_data->idleWorkers); }
    static apr_uint32_t GetWorkerCountBusy() { return apr_atomic_read32(&ms_server_data->busyWorkers); }
    static apr_uint32_t GetCPUUtilization() { return apr_atomic_read32(&ms_server_data->percentCPU); }

    static apr_size_t GetVHostCount() { return ms_vhost_data->count; }
    static mmap_vhost_elements *GetVHostElements() { return ms_vhost_data->vhosts; }

    static apr_size_t GetCertificateCount() { return ms_certificate_data->count; }
    static mmap_certificate_elements *GetCertificateElements() { return ms_certificate_data->certificates; }

    static apr_status_t LockMutex() { return apr_global_mutex_lock(ms_mutexMapRW); }
    static apr_status_t UnlockMutex() { return apr_global_mutex_unlock(ms_mutexMapRW); }

    static apr_pool_t *GetPool() { return ms_apr_pool; }

private:
    static apr_pool_t *ms_apr_pool;
    static apr_shm_t *ms_mmap_region;
    static mmap_server_data *ms_server_data;
    static mmap_vhost_data *ms_vhost_data;
    static mmap_certificate_data *ms_certificate_data;
    static mmap_string_table *ms_string_data;

    static apr_global_mutex_t *ms_mutexMapRW;

    static DataSampler ms_sampler;
    static int ms_loadCount;

    friend class DataSampler;
};

extern ApacheBinding g_apache;


//
// The providers should have an exception handler wrapping all activity.  This
// helps guarantee that the agent won't crash if there's an unhandled exception.
// In the Pegasus model, this was done in the base class.  Since we don't have
// that luxury here, we'll have macros to make it easier.
//
// PEX = Provider Exception
//
// There's an assumption here that, since this is used in the OMI-generated code,
// "context" always exists for posting the result to.
//

#define PEX_ERROR_CODE APR_EGENERAL

#define CIM_PEX_BEGIN \
    try

#define CIM_PEX_END(module) \
    catch (std::exception &e) { \
        TemporaryPool ptemp( g_apache.GetPool() ); \
        char *etext = apr_psprintf(ptemp.Get(), "%s - Exception occurred! Exception %s", module, e.what()); \
        g_apache.DisplayError( PEX_ERROR_CODE, etext ); \
        context.Post(MI_RESULT_FAILED); \
    } \
    catch (...) \
    { \
        TemporaryPool ptemp( g_apache.GetPool() ); \
        char *etext = apr_psprintf(ptemp.Get(), "%s - Unknown exception occurred!", module); \
        g_apache.DisplayError( PEX_ERROR_CODE, etext ); \
        context.Post(MI_RESULT_FAILED); \
    }

//
// Have a little function to make it easy to break into a provider (as a debugging aid)
//
// The idea here is that we sleep indefinitely; if you break in with a debugger, then
// you can set f_break to true and then step through the code.
//

#define CIM_PROVIDER_WAIT_FOR_ATTACH         \
    {                                        \
        volatile bool f_break = false;       \
        while ( !f_break )                   \
            sleep(1);                        \
    }

#endif /* APACHEBINDING_H */

/*----------------------------E-N-D---O-F---F-I-L-E---------------------------*/
