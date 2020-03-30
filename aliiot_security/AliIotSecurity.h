
#ifndef ALIIOTSECURITY_H
#define ALIIOTSECURITY_H

#include <jendefs.h>
#include "zcl.h"
#include "zcl_options.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define IOTX_PRODUCT_KEY_LEN            (20)
#define IOTX_DEVICE_NAME_LEN            (32)
#define IOTX_DEVICE_SECRET_LEN          (64)
#define IOTX_PRODUCT_SECRET_LEN         (64)

/* Cluster ID's */
#define GENERAL_CLUSTER_ID_ALIIOTSECURITY                        0xfc01


#ifndef CLD_ALIIOTSECURITY_CLUSTER_REVISION
    #define CLD_ALIIOTSECURITY_CLUSTER_REVISION                         1
#endif 


typedef enum 
{
    /* ALIIOTSECURITY attribute set attribute ID's  */
    E_CLD_ALIIOTSECURITY_ATTR_ID_ISEFFECTIVE           = 0x0000,    /* Mandatory */

    E_CLD_ALIIOTSECURITY_ATTR_ID_PRODUCTKEY            = 0x4000,    /* Mandatory */
    E_CLD_ALIIOTSECURITY_ATTR_ID_PRODUCTSECRET,                     /* Optional */
    E_CLD_ALIIOTSECURITY_ATTR_ID_DEVICENAME,                        /* Mandatory */
    E_CLD_ALIIOTSECURITY_ATTR_ID_DEVICESECRET,                        /* Mandatory */
} teCLD_AliIotSecurity_ClusterID;


typedef struct
{
    uint8                   u8Dummy;
} tsCLD_AliIotSecurityCustomDataStructure;


#ifdef ALIIOTSECURITY_SERVER
/*AliIotSecurity Cluster */
typedef struct
{
    zbool                       bIsEffective;

    tsZCL_CharacterString       sProductKey;
    uint8                       au8ProductKey[IOTX_PRODUCT_KEY_LEN];

#ifdef CLD_ALIIOTSECURITY_ATTR_PRODUCTSECRET
    tsZCL_CharacterString   sProductSecret;
    uint8                       au8ProductSecret[IOTX_PRODUCT_SECRET_LEN];
#endif

    tsZCL_CharacterString       sDeviceName;
    uint8                       au8DeviceName[IOTX_DEVICE_NAME_LEN];

    tsZCL_CharacterString   sDeviceSecret;
    uint8                       au8DeviceSecret[IOTX_DEVICE_SECRET_LEN];

    zuint16                     u16ClusterRevision;

} tsCLD_AliIotSecurity;
#endif


#ifdef ALIIOTSECURITY_CLIENT
/*AliIotSecurity Cluster */
typedef struct
{
    zuint16                     u16ClusterRevision;
} tsCLD_AliIotSecurity_Client;
#endif


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

PUBLIC  teZCL_Status eCLD_AliIotSecurityCreateAliIotSecurity(
                tsZCL_ClusterInstance              *psClusterInstance,
                bool_t                              bIsServer,
                tsZCL_ClusterDefinition            *psClusterDefinition,
                void                               *pvEndPointSharedStructPtr,
                uint8                              *pu8AttributeControlBits,
                tsCLD_AliIotSecurityCustomDataStructure     *psCustomDataStructure);



/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

#ifdef ALIIOTSECURITY_SERVER
extern tsZCL_ClusterDefinition sCLD_AliIotSecurity;
extern const tsZCL_AttributeDefinition asCLD_AliIotSecurityClusterAttributeDefinitions[];
extern uint8 au8AliIotSecurityAttributeControlBits[];
#endif

#ifdef ALIIOTSECURITY_CLIENT
extern tsZCL_ClusterDefinition sCLD_AliIotSecurityClient;
extern const tsZCL_AttributeDefinition asCLD_AliIotSecurityClientClusterAttributeDefinitions[];
extern uint8 au8AliIotSecurityClientAttributeControlBits[];
#endif


#endif /* ALIIOTSECURITY_H */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/