/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <string.h>

#include "zcl.h"
#include "zcl_customcommand.h"

#include "AliIotSecurity.h"
#include "AliIotSecurity_internal.h"


#include "zcl_options.h"


#include "dbg.h"
#ifdef DEBUG_CLD_ALIIOTSECURITY
#define TRACE_ALIIOTSECURITY TRUE
#else
#define TRACE_ALIIOTSECURITY FALSE
#endif


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#if defined(CLD_ALIIOTSECURITY) && !defined(ALIIOTSECURITY_SERVER) && !defined(ALIIOTSECURITY_CLIENT)
#error You Must Have either ALIIOTSECURITY_SERVER and/or ALIIOTSECURITY_CLIENT defined zcl_options.h
#endif

/****************************************************************************/
/***        Public Functions                                              ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_AliIotSecurityCommandHandler
 **
 ** DESCRIPTION:
 ** Handles Message Cluster custom commands
 **
 ** PARAMETERS:               Name                      Usage
 ** ZPS_tsAfEvent            *pZPSevent                 Zigbee stack event structure
 ** tsZCL_EndPointDefinition *psEndPointDefinition      EP structure
 ** tsZCL_ClusterInstance    *psClusterInstance         Cluster structure
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_AliIotSecurityCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance)
{



 return(E_ZCL_SUCCESS);

}


