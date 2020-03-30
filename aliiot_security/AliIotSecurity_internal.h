
#ifndef  ALIIOTSECURITY_INTERNAL_H_INCLUDED
#define  ALIIOTSECURITY_INTERNAL_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

#include "zcl.h"
#include "AliIotSecurity.h"

PUBLIC teZCL_Status eCLD_AliIotSecurityCommandHandler(
                    ZPS_tsAfEvent               *pZPSevent,
                    tsZCL_EndPointDefinition    *psEndPointDefinition,
                    tsZCL_ClusterInstance       *psClusterInstance);


#if defined __cplusplus
}
#endif

#endif  /* ALIIOTSECURITY_INTERNAL_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/