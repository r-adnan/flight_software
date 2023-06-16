/*
**  Copyright 2022 bitValence, Inc.
**  All Rights Reserved.
**
**  This program is free software; you can modify and/or redistribute it
**  under the terms of the GNU Affero General Public License
**  as published by the Free Software Foundation; version 3 with
**  attribution addendums as found in the LICENSE.txt
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Affero General Public License for more details.
**
**  Purpose:
**    Define the Adcs application
**
**  Notes:
**   1. This file was generated generated from the cFSAT 'Hello World'
**      app template for the Application C Framework (app_c_fw).
**
**  References:
**    1. OpenSatKit Object-based Application Developer's Guide
**    2. cFS Application Developer's Guide
**
*/

#ifndef _adcs_app_
#define _adcs_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "exobj.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define ADCS_INIT_APP_EID    (ADCS_BASE_EID + 0)
#define ADCS_NOOP_EID        (ADCS_BASE_EID + 1)
#define ADCS_EXIT_EID        (ADCS_BASE_EID + 2)
#define ADCS_INVALID_MID_EID (ADCS_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
** - See EDS command definitions in adcs.xml
*/


/******************************************************************************
** Telmetery Packets
** - See EDS command definitions in adcs.xml
*/


/******************************************************************************
** ADCS_Class
*/
typedef struct
{

   /* 
   ** App Framework
   */ 
    
   INITBL_Class_t  IniTbl; 
   CMDMGR_Class_t  CmdMgr;
   TBLMGR_Class_t  TblMgr;
   
   /*
   ** Command Packets
   */

 
   /*
   ** Telemetry Packets
   */
   
   ADCS_HkTlm_t  HkTlm;
   
   /*
   ** ADCS State & Contained Objects
   */ 
           
   uint32           PerfId;
   CFE_SB_PipeId_t  CmdPipe;
   CFE_SB_MsgId_t   CmdMid;
   CFE_SB_MsgId_t   ExecuteMid;
   CFE_SB_MsgId_t   SendHkMid;
   
} ADCS_Class_t;


/*******************/
/** Exported Data **/
/*******************/

extern ADCS_Class_t  Adcs;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: ADCS_AppMain
**
*/
void ADCS_AppMain(void);


/******************************************************************************
** Function: ADCS_NoOpCmd
**
*/
bool ADCS_NoOpCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: ADCS_ResetAppCmd
**
*/
bool ADCS_ResetAppCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


#endif /* _adcs_ */
