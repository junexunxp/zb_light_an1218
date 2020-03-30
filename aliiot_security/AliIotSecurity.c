
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"
#include "zcl_customcommand.h"
#include "AliIotSecurity.h"
#include "string.h"
#include "AliIotSecurity_internal.h"


#include "dbg.h"
#ifdef DEBUG_CLD_ALIIOTSECURITY
#define TRACE_ALIIOTSECURITY TRUE
#else
#define TRACE_ALIIOTSECURITY FALSE
#endif

#ifdef CLD_ALIIOTSECURITY

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
#ifdef ALIIOTSECURITY_SERVER

const tsZCL_AttributeDefinition asCLD_AliIotSecurityClusterAttributeDefinitions[] = {
    {E_CLD_ALIIOTSECURITY_ATTR_ID_ISEFFECTIVE,  (E_ZCL_AF_RD|E_ZCL_AF_MS),  E_ZCL_BOOL,     (uint32)(&((tsCLD_AliIotSecurity*)(0))->bIsEffective),0},     /* Mandatory */
    
    {E_CLD_ALIIOTSECURITY_ATTR_ID_PRODUCTKEY,  (E_ZCL_AF_RD|E_ZCL_AF_MS),   E_ZCL_CSTRING,  (uint32)(&((tsCLD_AliIotSecurity*)(0))->sProductKey),0},     /* Mandatory */

#ifdef CLD_ALIIOTSECURITY_ATTR_PRODUCTSECRET
    {E_CLD_ALIIOTSECURITY_ATTR_ID_PRODUCTSECRET, (E_ZCL_AF_RD|E_ZCL_AF_MS), E_ZCL_CSTRING,   (uint32)(&((tsCLD_AliIotSecurity*)(0))->sProductSecret),0},     /* Optinal */
#endif

    {E_CLD_ALIIOTSECURITY_ATTR_ID_DEVICENAME,   (E_ZCL_AF_RD|E_ZCL_AF_MS),  E_ZCL_CSTRING,   (uint32)(&((tsCLD_AliIotSecurity*)(0))->sDeviceName),0},     /* Mandatory */
         
    {E_CLD_ALIIOTSECURITY_ATTR_ID_DEVICESECRET,   (E_ZCL_AF_RD|E_ZCL_AF_MS), E_ZCL_CSTRING,  (uint32)(&((tsCLD_AliIotSecurity*)(0))->sDeviceSecret),0},     /* Mandatory */

    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,     (E_ZCL_AF_RD|E_ZCL_AF_GA|E_ZCL_AF_MS),     E_ZCL_UINT16,   (uint32)(&((tsCLD_AliIotSecurity*)(0))->u16ClusterRevision),0},   /* Mandatory  */
};


    tsZCL_ClusterDefinition sCLD_AliIotSecurity = {
            GENERAL_CLUSTER_ID_ALIIOTSECURITY,
            TRUE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_AliIotSecurityClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_AliIotSecurityClusterAttributeDefinitions,
            NULL
    #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED        
            ,
           0,
           NULL       
    #endif        
    };

    uint8 au8AliIotSecurityAttributeControlBits[(sizeof(asCLD_AliIotSecurityClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];

#endif


#ifdef ALIIOTSECURITY_CLIENT

const tsZCL_AttributeDefinition asCLD_AliIotSecurityClientClusterAttributeDefinitions[] = {
    {E_CLD_GLOBAL_ATTR_ID_CLUSTER_REVISION,     (E_ZCL_AF_RD|E_ZCL_AF_GA|E_ZCL_AF_MS),               E_ZCL_UINT16,   (uint32)(&((tsCLD_AliIotSecurity_Client*)(0))->u16ClusterRevision),0},  /* Mandatory  */
};
    tsZCL_ClusterDefinition sCLD_AliIotSecurityClient = {
            GENERAL_CLUSTER_ID_ALIIOTSECURITY,
            TRUE,
            E_ZCL_SECURITY_NETWORK,
            (sizeof(asCLD_AliIotSecurityClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition)),
            (tsZCL_AttributeDefinition*)asCLD_AliIotSecurityClientClusterAttributeDefinitions,
            NULL
    #ifdef ZCL_COMMAND_DISCOVERY_SUPPORTED
            ,
            0,
            NULL
    #endif
    };
    uint8 au8AliIotSecurityClientAttributeControlBits[(sizeof(asCLD_AliIotSecurityClientClusterAttributeDefinitions) / sizeof(tsZCL_AttributeDefinition))];
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 **
 ** NAME:       eCLD_AliIotSecurityCreateAliIotSecurity
 **
 ** DESCRIPTION:
 ** Creates an AliIotSecurity cluster
 **
 ** PARAMETERS:                     Name                        Usage
 ** tsZCL_ClusterInstance           *psClusterInstance          Cluster structure
 ** bool_t                          bIsServer                   Server Client flag
 ** tsZCL_ClusterDefinition         *psClusterDefinition        Cluster Definition
 ** void                            *pvEndPointSharedStructPtr  Endpoint shared structure
 ** uint8                           *pu8AttributeControlBits    Attribute control bits
 ** tsCLD_OnOffCustomDataStructure  *psCustomDataStructure      Custom data Structure
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/

PUBLIC  teZCL_Status eCLD_AliIotSecurityCreateAliIotSecurity(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits,
                tsCLD_AliIotSecurityCustomDataStructure     *psCustomDataStructure)
{
   
    #ifdef STRICT_PARAM_CHECK 
        /* Parameter check */
        if(psClusterInstance==NULL)
        {
            return E_ZCL_ERR_PARAMETER_NULL;
        }
    #endif
    
    // cluster data
    vZCL_InitializeClusterInstance(
           psClusterInstance, 
           bIsServer,
           psClusterDefinition,
           pvEndPointSharedStructPtr,
           pu8AttributeControlBits,
           NULL,
           eCLD_AliIotSecurityCommandHandler);    
    
    if(psClusterInstance->pvEndPointSharedStructPtr != NULL)
    {
        if(bIsServer)
        {     
             // set default state
            #ifdef ALIIOTSECURITY_SERVER

                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->bIsEffective = TRUE;
        
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sProductKey.u8MaxLength =  sizeof(((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8ProductKey);
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sProductKey.u8Length = strlen(((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8ProductKey);
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sProductKey.pu8Data = ((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8ProductKey;

                #ifdef CLD_ALIIOTSECURITY_ATTR_PRODUCTSECRET
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sProductSecret.u8MaxLength = sizeof(((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8ProductSecret);
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sProductSecret.u8Length = strlen(((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8ProductSecret);
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sProductSecret.pu8Data = ((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8ProductSecret;
                #endif

                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sDeviceName.u8MaxLength = sizeof(((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8DeviceName);
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sDeviceName.u8Length = strlen(((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8DeviceName);
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sDeviceName.pu8Data = ((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8DeviceName;

                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sDeviceSecret.u8MaxLength = sizeof(((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8DeviceSecret);
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sDeviceSecret.u8Length = strlen(((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8DeviceSecret);
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->sDeviceSecret.pu8Data = ((tsCLD_AliIotSecurity*)pvEndPointSharedStructPtr)->au8DeviceSecret;
            
                
                
                ((tsCLD_AliIotSecurity *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ClusterRevision =  CLD_ALIIOTSECURITY_CLUSTER_REVISION;
            #endif
        }
        else
        {
            #ifdef ALIIOTSECURITY_CLIENT
                ((tsCLD_AliIotSecurity_Client *)(psClusterInstance->pvEndPointSharedStructPtr))->u16ClusterRevision = CLD_ALIIOTSECURITY_CLUSTER_REVISION;
            #endif
        }
    }

    return E_ZCL_SUCCESS;

}

#endif /* CLD_ALIIOTSECURITY */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/