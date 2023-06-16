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
**    Implement the Hello application
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


/*
** Includes
*/

#include <string.h>
#include "hello_app.h"
#include "hello_eds_cc.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ    (&(Hello.IniTbl))
#define  CMDMGR_OBJ    (&(Hello.CmdMgr))
#define  TBLMGR_OBJ    (&(Hello.TblMgr))
#define  EXOBJ_OBJ     (&(Hello.ExObj))


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(void);
static int32 ProcessCommands(void);
static void SendHousekeepingTlm(void);


/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum_t IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)  


/*****************/
/** Global Data **/
/*****************/

HELLO_Class_t  Hello;


/******************************************************************************
** Function: HELLO_AppMain
**
*/
void HELLO_AppMain(void)
{

   uint32 RunStatus = CFE_ES_RunStatus_APP_ERROR;
   
   CFE_EVS_Register(NULL, 0, CFE_EVS_NO_FILTER);

   if (InitApp() == CFE_SUCCESS)      /* Performs initial CFE_ES_PerfLogEntry() call */
   {
      RunStatus = CFE_ES_RunStatus_APP_RUN; 
   }
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {
      
      RunStatus = ProcessCommands();  /* Pends indefinitely & manages CFE_ES_PerfLogEntry() calls */
      
   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("HELLO App terminating, run status = 0x%08X\n", RunStatus);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(HELLO_EXIT_EID, CFE_EVS_EventType_CRITICAL, "HELLO App terminating, run status = 0x%08X", RunStatus);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of HELLO_AppMain() */


/******************************************************************************
** Function: HELLO_NoOpCmd
**
*/
bool HELLO_NoOpCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   CFE_EVS_SendEvent (HELLO_NOOP_EID, CFE_EVS_EventType_INFORMATION,
                      "No operation command received for HELLO App version %d.%d.%d",
                      HELLO_MAJOR_VER, HELLO_MINOR_VER, HELLO_PLATFORM_REV);

   return true;


} /* End HELLO_NoOpCmd() */


/******************************************************************************
** Function: HELLO_ResetAppCmd
**
** Notes:
**   1. Framework objects require an object reference since they are
**      reentrant. Applications use the singleton pattern and store a
**      reference pointer to the object data during construction.
*/
bool HELLO_ResetAppCmd(void* ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   
   EXOBJ_ResetStatus();
	  
   return true;

} /* End HELLO_ResetAppCmd() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = APP_C_FW_CFS_ERROR;
   

   /*
   ** Initialize objects 
   */
   
   if (INITBL_Constructor(INITBL_OBJ, HELLO_INI_FILENAME, &IniCfgEnum))
   {
   
      Hello.PerfId  = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_PERF_ID);
      CFE_ES_PerfLogEntry(Hello.PerfId);

      Hello.CmdMid     = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_HELLO_CMD_TOPICID));
      Hello.ExecuteMid = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_HELLO_EXE_TOPICID));
      Hello.SendHkMid  = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_HELLO_SEND_HK_TOPICID));
      
      Status = CFE_SUCCESS; 
  
   } /* End if INITBL Constructed */
  
   if (Status == CFE_SUCCESS)
   {

      /* Must constructor table manager prior to any app objects that contained tables */
      TBLMGR_Constructor(TBLMGR_OBJ);
   
      /*
      ** Constuct app's contained objects
      */
            
      EXOBJ_Constructor(EXOBJ_OBJ, INITBL_OBJ, TBLMGR_OBJ);
      
      /*
      ** Initialize app level interfaces
      */
      
      CFE_SB_CreatePipe(&Hello.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_APP_CMD_PIPE_NAME));  
      CFE_SB_Subscribe(Hello.CmdMid,     Hello.CmdPipe);
      CFE_SB_Subscribe(Hello.ExecuteMid, Hello.CmdPipe);
      CFE_SB_Subscribe(Hello.SendHkMid,  Hello.CmdPipe);

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, HELLO_NOOP_CC,  NULL, HELLO_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, HELLO_RESET_CC, NULL, HELLO_ResetAppCmd, 0);
      
      CMDMGR_RegisterFunc(CMDMGR_OBJ, HELLO_LOAD_TBL_CC, TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, HELLO_DUMP_TBL_CC, TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);

      CMDMGR_RegisterFunc(CMDMGR_OBJ, HELLO_SET_COUNTER_MODE_CC, EXOBJ_OBJ, EXOBJ_SetModeCmd, sizeof(HELLO_CounterMode_Payload_t));


      /*
      ** Initialize app messages 
      */

      CFE_MSG_Init(CFE_MSG_PTR(Hello.HkTlm.TelemetryHeader), 
                   CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_HELLO_HK_TLM_TOPICID)),
                   sizeof(HELLO_HkTlm_t));

      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(HELLO_INIT_APP_EID, CFE_EVS_EventType_INFORMATION,
                        "HELLO App Initialized. Version %d.%d.%d",
                        HELLO_MAJOR_VER, HELLO_MINOR_VER, HELLO_PLATFORM_REV);

   } /* End if CHILDMGR constructed */
   
   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
** 
*/
static int32 ProcessCommands(void)
{
   
   int32  RetStatus = CFE_ES_RunStatus_APP_RUN;
   int32  SysStatus;

   CFE_SB_Buffer_t  *SbBufPtr;
   CFE_SB_MsgId_t   MsgId = CFE_SB_INVALID_MSG_ID;


   CFE_ES_PerfLogExit(Hello.PerfId);
   SysStatus = CFE_SB_ReceiveBuffer(&SbBufPtr, Hello.CmdPipe, CFE_SB_PEND_FOREVER);
   CFE_ES_PerfLogEntry(Hello.PerfId);

   if (SysStatus == CFE_SUCCESS)
   {
      SysStatus = CFE_MSG_GetMsgId(&SbBufPtr->Msg, &MsgId);

      if (SysStatus == CFE_SUCCESS)
      {

         if (CFE_SB_MsgId_Equal(MsgId, Hello.CmdMid))
         {
            CMDMGR_DispatchFunc(CMDMGR_OBJ, &SbBufPtr->Msg);
         } 
         else if (CFE_SB_MsgId_Equal(MsgId, Hello.ExecuteMid))
         {
            EXOBJ_Execute();
         }
         else if (CFE_SB_MsgId_Equal(MsgId, Hello.SendHkMid))
         {   
            SendHousekeepingTlm();
         }
         else
         {   
            CFE_EVS_SendEvent(HELLO_INVALID_MID_EID, CFE_EVS_EventType_ERROR,
                              "Received invalid command packet, MID = 0x%08X", 
                              CFE_SB_MsgIdToValue(MsgId));
         }

      } /* End if got message ID */
   } /* End if received buffer */
   else
   {
      RetStatus = CFE_ES_RunStatus_APP_ERROR;
   } 

   return RetStatus;
   
} /* End ProcessCommands() */


/******************************************************************************
** Function: SendHousekeepingTlm
**
*/
static void SendHousekeepingTlm(void)
{

   HELLO_HkTlm_Payload_t *Payload = &Hello.HkTlm.Payload;

   /* Good design practice in case app expands to more than one table */
   const TBLMGR_Tbl_t* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);

   /*
   ** Framework Data
   */
   
   Payload->ValidCmdCnt   = Hello.CmdMgr.ValidCmdCnt;
   Payload->InvalidCmdCnt = Hello.CmdMgr.InvalidCmdCnt;
   
   /*
   ** Table Data 
   ** - Loaded with status from the last table action 
   */

   Payload->LastTblAction       = LastTbl->LastAction;
   Payload->LastTblActionStatus = LastTbl->LastActionStatus;

   
   /*
   ** Example Object Data
   */

   Payload->CounterMode  = Hello.ExObj.CounterMode;
   Payload->CounterValue = Hello.ExObj.CounterValue;
   

   CFE_SB_TimeStampMsg(CFE_MSG_PTR(Hello.HkTlm.TelemetryHeader));
   CFE_SB_TransmitMsg(CFE_MSG_PTR(Hello.HkTlm.TelemetryHeader), true);

} /* End SendHousekeepingTlm() */

