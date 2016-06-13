/**********************************************************************************************
 * Filename:       Immediate_Alert.c
 *
 * Description:    This file contains the implementation of the service.
 *
 *                 Generated by:
 *                 BDS version: 1.0.2116.0
 *                 Plugin:      Texas Instruments CC26xx BLE SDK v2.1 GATT Server plugin 1.0.5 beta
 *                 Time:        Mon Jun 13 2016 10:49:18 GMT+03:00
 *

 * Copyright (c) 2015, Texas Instruments Incorporated
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
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "Immediate_Alert.h"

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

// Immediate_Alert Service UUID
CONST uint8_t ImmediateAlertUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(IMMEDIATE_ALERT_SERV_UUID), HI_UINT16(IMMEDIATE_ALERT_SERV_UUID)
};

// Alert_Level UUID
CONST uint8_t ia_Alert_LevelUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(IA_ALERT_LEVEL_UUID), HI_UINT16(IA_ALERT_LEVEL_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static ImmediateAlertCBs_t *pAppCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t ImmediateAlertDecl = { ATT_BT_UUID_SIZE, ImmediateAlertUUID };

// Characteristic "Alert Level" Properties (for declaration)
static uint8_t ia_Alert_LevelProps = GATT_PROP_WRITE_NO_RSP;

// Characteristic "Alert Level" Value variable
static uint8_t ia_Alert_LevelVal[IA_ALERT_LEVEL_LEN] = {0};

// Length of data in characteristic "Alert Level" Value variable, initialized to minimal size.
static uint16_t ia_Alert_LevelValLen = IA_ALERT_LEVEL_LEN_MIN;

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t Immediate_AlertAttrTbl[] = 
{
  // Immediate_Alert Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&ImmediateAlertDecl
  },
    // Alert Level Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ia_Alert_LevelProps
    },
      // Alert Level Characteristic Value
      {
        { ATT_BT_UUID_SIZE, ia_Alert_LevelUUID },
        GATT_PERMIT_WRITE,
        0,
        ia_Alert_LevelVal
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t Immediate_Alert_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr, 
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t Immediate_Alert_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t Immediate_AlertCBs =
{
  Immediate_Alert_ReadAttrCB,  // Read callback function pointer
  Immediate_Alert_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * ImmediateAlert_AddService- Initializes the ImmediateAlert service by registering
 *          GATT attributes with the GATT server.
 *
 */
bStatus_t ImmediateAlert_AddService( void )
{
  uint8_t status;

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( Immediate_AlertAttrTbl, 
                                        GATT_NUM_ATTRS( Immediate_AlertAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &Immediate_AlertCBs );

  return ( status );
}

/*
 * ImmediateAlert_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t ImmediateAlert_RegisterAppCBs( ImmediateAlertCBs_t *appCallbacks )
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
 * ImmediateAlert_SetParameter - Set a ImmediateAlert parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate 
 *            data type (example: data type of uint16_t will be cast to 
 *            uint16_t pointer).
 */
bStatus_t ImmediateAlert_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*
 * ImmediateAlert_GetParameter - Get a ImmediateAlert parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate 
 *            data type (example: data type of uint16_t will be cast to 
 *            uint16_t pointer).
 */
bStatus_t ImmediateAlert_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case IA_ALERT_LEVEL_ID:
      *len = MIN(*len, ia_Alert_LevelValLen);
      memcpy(value, ia_Alert_LevelVal, *len);
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*********************************************************************
 * @fn          Immediate_Alert_ReadAttrCB
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
static bStatus_t Immediate_Alert_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr, 
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;

  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has READ permissions.
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return status;
}


/*********************************************************************
 * @fn      Immediate_Alert_WriteAttrCB
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
static bStatus_t Immediate_Alert_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;

  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow notification and indication, but do not check if really allowed per CCCD.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY |
                                                     GATT_CLIENT_CFG_INDICATE );
  }
  // See if request is regarding the Alert_Level Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, ia_Alert_LevelUUID, pAttr->type.len) )
  {
    if ( offset >= IA_ALERT_LEVEL_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else if ( offset + len > IA_ALERT_LEVEL_LEN )
    {
      status = ATT_ERR_INVALID_VALUE_SIZE;
    }
    else if ( offset + len < IA_ALERT_LEVEL_LEN_MIN && 
              ( method == ATT_EXECUTE_WRITE_REQ || method == ATT_WRITE_REQ ) )
    {
      // Refuse writes that are lower than minimum.
      // Note: Cannot determine if a Reliable Write (to several chars) is finished, so those will
      //       only be refused if this attribute is the last in the queue (method is execute).
      //       Otherwise, reliable writes are accepted and parsed piecemeal.
      status = ATT_ERR_INVALID_VALUE_SIZE;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify application and update length if enough data is written.
      //
      // Note: If reliable writes are used (meaning several attributes are written to using ATT PrepareWrite),
      //       the application will get a callback for every write with an offset + len larger than _LEN_MIN.
      // Note: For Long Writes (ATT Prepare + Execute towards only one attribute) only one callback will be issued,
      //       because the write fragments are concatenated before being sent here.
      if ( offset + len >= IA_ALERT_LEVEL_LEN_MIN)
      {
        paramID = IA_ALERT_LEVEL_ID;
        ia_Alert_LevelValLen = offset + len; // Update data length.
      }
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
