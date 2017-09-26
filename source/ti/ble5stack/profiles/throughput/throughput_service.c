/**********************************************************************************************
 * Filename:       Throughput_Service.c
 *
 * Description:    This file contains the implementation of the service.
 *
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include "bcomdef.h"
#include "osal.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "throughput_service.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* GLOBAL VARIABLES
*/

// Throughput_Service Service UUID
CONST uint8_t Throughput_ServiceUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(THROUGHPUT_SERVICE_SERV_UUID)
};

// Update_PDU UUID
CONST uint8_t Throughput_Service_Update_PDUUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(THROUGHPUT_SERVICE_UPDATE_PDU_UUID)
};
// Update_PHY UUID
CONST uint8_t Throughput_Service_Update_PHYUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(THROUGHPUT_SERVICE_UPDATE_PHY_UUID)
};
// Toggle_Throughput UUID
CONST uint8_t Throughput_Service_Toggle_ThroughputUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT_UUID)
};
// Notify_Data UUID
CONST uint8_t Throughput_Service_Notify_DataUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(THROUGHPUT_SERVICE_NOTIFY_DATA_UUID)
};
// Wr_Toggle_Throughput UUID
CONST uint8_t Throughput_Service_Wr_Toggle_ThroughputUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(THROUGHPUT_SERVICE_WR_TOGGLE_THROUGHPUT_UUID)
};
// Write_Data UUID
CONST uint8_t Throughput_Service_Write_DataUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(THROUGHPUT_SERVICE_WRITE_DATA_UUID)
};
// Indication_Mode UUID
CONST uint8_t Throughput_Service_Indication_ModeUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(THROUGHPUT_SERVICE_INDICATION_MODE_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static Throughput_ServiceCBs_t *pAppCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t Throughput_ServiceDecl = { ATT_UUID_SIZE, Throughput_ServiceUUID };

// Characteristic "Update_PDU" Properties (for declaration)
static uint8_t Throughput_Service_Update_PDUProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Update_PDU" Value variable
static uint8_t Throughput_Service_Update_PDUVal[THROUGHPUT_SERVICE_UPDATE_PDU_LEN] = {0};

// Characteristic "Update_PHY" Properties (for declaration)
static uint8_t Throughput_Service_Update_PHYProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Update_PHY" Value variable
static uint8_t Throughput_Service_Update_PHYVal[THROUGHPUT_SERVICE_UPDATE_PHY_LEN] = {0};

// Characteristic "Toggle_Throughput" Properties (for declaration)
static uint8_t Throughput_Service_Toggle_ThroughputProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Toggle_Throughput" Value variable
static uint8_t Throughput_Service_Toggle_ThroughputVal[THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT_LEN] = {0};

// Characteristic "Notify_Data" Properties (for declaration)
static uint8_t Throughput_Service_Notify_DataProps = GATT_PROP_NOTIFY | GATT_PROP_INDICATE;
// Characteristic "Notify_Data" Value variable
static uint8_t Throughput_Service_Notify_DataVal[THROUGHPUT_SERVICE_NOTIFY_DATA_LEN] = {0};
// Characteristic "Notify_Data" CCC
static gattCharCfg_t *notifyDataClientCharCfg;

// Characteristic "Wr_Toggle_Throughput" Properties (for declaration)
static uint8_t Throughput_Service_Wr_Toggle_ThroughputProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Wr_Toggle_Throughput" Value variable
static uint8_t Throughput_Service_Wr_Toggle_ThroughputVal[THROUGHPUT_SERVICE_WR_TOGGLE_THROUGHPUT_LEN] = {0};

// Characteristic "Write_Data" Properties (for declaration)
static uint8_t Throughput_Service_Write_DataProps = GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;
// Characteristic "Write_Data" Value variable
static uint8_t Throughput_Service_Write_DataVal[THROUGHPUT_SERVICE_WRITE_DATA_LEN] = {0};

// Characteristic "Indication_Mode" Properties (for declaration)
static uint8_t Throughput_Service_Indication_ModeProps = GATT_PROP_WRITE | GATT_PROP_READ;
// Characteristic "Indication_Mode" Value variable
static uint8_t Throughput_Service_Indication_ModeVal[THROUGHPUT_SERVICE_INDICATION_MODE_LEN] = {0};

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t Throughput_ServiceAttrTbl[] =
{
  // Throughput_Service Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&Throughput_ServiceDecl
  },
    //0. Update_PDU Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &Throughput_Service_Update_PDUProps
    },
      // Update_PDU Characteristic Value
      {
        { ATT_UUID_SIZE, Throughput_Service_Update_PDUUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        Throughput_Service_Update_PDUVal
      },
    //1. Update_PHY Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &Throughput_Service_Update_PHYProps
    },
      // Update_PHY Characteristic Value
      {
        { ATT_UUID_SIZE, Throughput_Service_Update_PHYUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        Throughput_Service_Update_PHYVal
      },
    //2. Toggle_Throughput Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &Throughput_Service_Toggle_ThroughputProps
    },
      // Toggle_Throughput Characteristic Value
      {
        { ATT_UUID_SIZE, Throughput_Service_Toggle_ThroughputUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        Throughput_Service_Toggle_ThroughputVal
      },
    //3. Notify_Data Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &Throughput_Service_Notify_DataProps
    },
      // Notify_Data CHaracteristic Value
      {
        { ATT_UUID_SIZE, Throughput_Service_Notify_DataUUID},
          GATT_PERMIT_READ,
          0,
          Throughput_Service_Notify_DataVal
      },
      // Notify_Data Client Characteristic Configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID},
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *) &notifyDataClientCharCfg
      },
    //4. Wr_Toggle_Throughput Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &Throughput_Service_Wr_Toggle_ThroughputProps
    },
        // Wr_Toggle_Throughput Characteristic Value
      {
        { ATT_UUID_SIZE, Throughput_Service_Wr_Toggle_ThroughputUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        Throughput_Service_Wr_Toggle_ThroughputVal
      },
    //5. Write_Data Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &Throughput_Service_Write_DataProps
    },
      // Write_Data Characteristic Value
      {
        { ATT_UUID_SIZE, Throughput_Service_Write_DataUUID },
        GATT_PERMIT_WRITE,
        0,
        Throughput_Service_Write_DataVal
      },
    //6. Indication_Mode Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &Throughput_Service_Indication_ModeProps
    },
      // Indication_Mode Characteristic Value
      {
        { ATT_UUID_SIZE, Throughput_Service_Indication_ModeUUID },
        GATT_PERMIT_WRITE | GATT_PERMIT_READ,
        0,
        Throughput_Service_Indication_ModeVal
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t Throughput_Service_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                           uint8 *pValue, uint16 *pLen, uint16 offset,
                                           uint16 maxLen, uint8 method );
static bStatus_t Throughput_Service_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint16 len, uint16 offset,
                                            uint8 method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t Throughput_ServiceCBs =
{
  Throughput_Service_ReadAttrCB,  // Read callback function pointer
  Throughput_Service_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * Throughput_Service_AddService- Initializes the Throughput_Service service by registering
 *          GATT attributes with the GATT server.
 *
 */
bStatus_t Throughput_Service_AddService( void )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  notifyDataClientCharCfg = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                           linkDBNumConns );
  if ( notifyDataClientCharCfg == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg(INVALID_CONNHANDLE, notifyDataClientCharCfg);

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( Throughput_ServiceAttrTbl,
                                        GATT_NUM_ATTRS( Throughput_ServiceAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &Throughput_ServiceCBs );

  return ( status );
}

/*
 * Throughput_Service_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t Throughput_Service_RegisterAppCBs( Throughput_ServiceCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*
 * Throughput_Service_SetParameter - Set a Throughput_Service parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t Throughput_Service_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case THROUGHPUT_SERVICE_UPDATE_PDU:
      if ( len == THROUGHPUT_SERVICE_UPDATE_PDU_LEN )
      {
        memcpy(Throughput_Service_Update_PDUVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case THROUGHPUT_SERVICE_UPDATE_PHY:
      if ( len == THROUGHPUT_SERVICE_UPDATE_PHY_LEN )
      {
        memcpy(Throughput_Service_Update_PHYVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT:
      if ( len == THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT_LEN )
      {
        memcpy(Throughput_Service_Toggle_ThroughputVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case THROUGHPUT_SERVICE_INDICATION_MODE:
      if ( len == THROUGHPUT_SERVICE_INDICATION_MODE_LEN )
      {
        memcpy(Throughput_Service_Indication_ModeVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*
 * Throughput_Service_GetParameter - Get a Throughput_Service parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t Throughput_Service_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case THROUGHPUT_SERVICE_UPDATE_PDU:
      memcpy(value, Throughput_Service_Update_PDUVal, THROUGHPUT_SERVICE_UPDATE_PDU_LEN);
      break;

    case THROUGHPUT_SERVICE_UPDATE_PHY:
      memcpy(value, Throughput_Service_Update_PHYVal, THROUGHPUT_SERVICE_UPDATE_PHY_LEN);
      break;

    case THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT:
      memcpy(value, Throughput_Service_Toggle_ThroughputVal, THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT_LEN);
      break;

    case THROUGHPUT_SERVICE_INDICATION_MODE:
      memcpy(value, Throughput_Service_Indication_ModeVal, THROUGHPUT_SERVICE_INDICATION_MODE_LEN);
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}

/*
 * Throughput_Service_GetNotiHandle - Get the handle of Notify attr.
 *
 */
uint16_t Throughput_Service_GetNotiHandle( void )
{
    return Throughput_ServiceAttrTbl[THROUGHPUT_SERVICE_NOTIFY_DATA*2+2].handle;
}

/*********************************************************************
 * @fn          Throughput_Service_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t Throughput_Service_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                       uint8 *pValue, uint16 *pLen, uint16 offset,
                                       uint16 maxLen, uint8 method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the Update_PDU Characteristic Value
  if ( ! memcmp(pAttr->type.uuid, Throughput_Service_Update_PDUUUID, pAttr->type.len) )
  {
    if ( offset > THROUGHPUT_SERVICE_UPDATE_PDU_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, THROUGHPUT_SERVICE_UPDATE_PDU_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the Update_PHY Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, Throughput_Service_Update_PHYUUID, pAttr->type.len) )
  {
    if ( offset > THROUGHPUT_SERVICE_UPDATE_PHY_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, THROUGHPUT_SERVICE_UPDATE_PHY_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the Toggle_Throughput Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, Throughput_Service_Toggle_ThroughputUUID, pAttr->type.len) )
  {
    if ( offset > THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the Indication_Mode Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, Throughput_Service_Indication_ModeUUID, pAttr->type.len) )
  {
    if ( offset > THROUGHPUT_SERVICE_INDICATION_MODE_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, THROUGHPUT_SERVICE_INDICATION_MODE_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has READ permissions.
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return status;
}


/*********************************************************************
 * @fn      Throughput_Service_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t Throughput_Service_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                        uint8 *pValue, uint16 len, uint16 offset,
                                        uint8 method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;

  // See if request is regarding a Client Characteristic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY | GATT_CLIENT_CFG_INDICATE);
  }
  // See if request is regarding the Update_PDU Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, Throughput_Service_Update_PDUUUID, pAttr->type.len) )
  {
    if ( offset + len > THROUGHPUT_SERVICE_UPDATE_PDU_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify application if entire expected value is written
      if ( offset + len == THROUGHPUT_SERVICE_UPDATE_PDU_LEN)
        paramID = THROUGHPUT_SERVICE_UPDATE_PDU;
    }
  }
  // See if request is regarding the Update_PHY Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, Throughput_Service_Update_PHYUUID, pAttr->type.len) )
  {
    if ( offset + len > THROUGHPUT_SERVICE_UPDATE_PHY_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify application if entire expected value is written
      if ( offset + len == THROUGHPUT_SERVICE_UPDATE_PHY_LEN)
        paramID = THROUGHPUT_SERVICE_UPDATE_PHY;
    }
  }
  // See if request is regarding the Toggle_Throughput Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, Throughput_Service_Toggle_ThroughputUUID, pAttr->type.len) )
  {
    if ( offset + len > THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify application if entire expected value is written
      if ( offset + len == THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT_LEN)
        paramID = THROUGHPUT_SERVICE_TOGGLE_THROUGHPUT;
    }
  }
  // See if request is regarding the Indication_Mode Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, Throughput_Service_Indication_ModeUUID, pAttr->type.len) )
  {
    if ( offset + len > THROUGHPUT_SERVICE_INDICATION_MODE_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify application if entire expected value is written
      if ( offset + len == THROUGHPUT_SERVICE_INDICATION_MODE_LEN)
        paramID = THROUGHPUT_SERVICE_INDICATION_MODE;
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has WRITE permissions.
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (paramID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
      pAppCBs->pfnChangeCb( paramID ); // Call app function from stack task context.

  return status;
}
