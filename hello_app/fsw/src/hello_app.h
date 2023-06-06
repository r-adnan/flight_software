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
**    Define the Hello application
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

#ifndef _hello_app_
#define _hello_app_

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

#define HELLO_INIT_APP_EID    (HELLO_BASE_EID + 0)
#define HELLO_NOOP_EID        (HELLO_BASE_EID + 1)
#define HELLO_EXIT_EID        (HELLO_BASE_EID + 2)
#define HELLO_INVALID_MID_EID (HELLO_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
** - See EDS command definitions in hello.xml
*/


/******************************************************************************
** Telmetery Packets
** - See EDS command definitions in hello.xml
*/
** HELLO_Class
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

   HELLO_HkTlm_t  HkTlm;

/*
   ** HELLO State & Contained Objects
   */

   uint32           PerfId;
   CFE_SB_PipeId_t  CmdPipe;
   CFE_SB_MsgId_t   CmdMid;
   CFE_SB_MsgId_t   ExecuteMid;
   CFE_SB_MsgId_t   SendHkMid;

   EXOBJ_Class_t  ExObj;

} HELLO_Class_t;


/*******************/
/** Exported Data **/
/*******************/

extern HELLO_Class_t  Hello;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: HELLO_AppMain
**
*/
void HELLO_AppMain(void);


/******************************************************************************
** Function: HELLO_NoOpCmd
**
*/
bool HELLO_NoOpCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: HELLO_ResetAppCmd
**
*/
bool HELLO_ResetAppCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr);


#endif /* _hello_ */
