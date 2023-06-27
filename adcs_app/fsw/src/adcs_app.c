/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * \file
 *   This file contains the source code for the Adcs App.
 */

/*
** Include Files:
*/
#include "adcs_app_events.h"
#include "adcs_app_version.h"
#include "adcs_app.h"
#include "adcs_app_table.h"

/* The sample_lib module provides the SAMPLE_LIB_Function() prototype */
#include <string.h>
#include "sample_lib.h"

#include "adcs_mng_data.c"
#include "adcs_mng_data.h"

/*
** global data
*/
ADCS_APP_Data_t ADCS_APP_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void ADCS_APP_Main(void)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(ADCS_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = ADCS_APP_Init();
    if (status != CFE_SUCCESS)
    {
        ADCS_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        // printf("1. failed\n");
    }

    /*
    ** ADCS Runloop
    */


    while (CFE_ES_RunLoop(&ADCS_APP_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(ADCS_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, ADCS_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);
        // if (status != CFE_SUCCESS) printf("2. failed\n");
        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(ADCS_APP_PERF_ID);

        ADCS_APP_Table_t *TblPtr;
        CFE_TBL_GetAddress((void *)&TblPtr, ADCS_APP_Data.TblHandles[0]);

        if (status == CFE_SUCCESS)
        {
            ADCS_APP_ProcessCommandPacket(SBBufPtr);
            MNG_DATA_Test();
            
           //if (ADCS_APP_Data.need_charging) 
           send_data(TblPtr);
        }
        else
        {
            CFE_EVS_SendEvent(ADCS_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "ADCS APP: SB Pipe Read Error, App Will Exit");

            ADCS_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(ADCS_APP_PERF_ID);

    CFE_ES_ExitApp(ADCS_APP_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 ADCS_APP_Init(void)
{
    int32 status;

    ADCS_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    ADCS_APP_Data.CmdCounter = 0;
    ADCS_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    ADCS_APP_Data.PipeDepth = ADCS_APP_PIPE_DEPTH;

    strncpy(ADCS_APP_Data.PipeName, "ADCS_APP_CMD_PIPE", sizeof(ADCS_APP_Data.PipeName));
    ADCS_APP_Data.PipeName[sizeof(ADCS_APP_Data.PipeName) - 1] = 0;

    strncpy(ADCS_APP_Data.EPSPipeName, "ADCS_EPS_PIPE", sizeof(ADCS_APP_Data.EPSPipeName));
    ADCS_APP_Data.EPSPipeName[sizeof(ADCS_APP_Data.EPSPipeName) - 1] = 0;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(CFE_MSG_PTR(ADCS_APP_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(ADCS_APP_HK_TLM_MID),
                 sizeof(ADCS_APP_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&ADCS_APP_Data.CommandPipe, ADCS_APP_Data.PipeDepth, ADCS_APP_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    status = CFE_SB_CreatePipe(&ADCS_APP_Data.EPSPipe, ADCS_APP_Data.PipeDepth, ADCS_APP_Data.EPSPipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ADCS_APP_SEND_HK_MID), ADCS_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ADCS_APP_CMD_MID), ADCS_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    /*
    ** Subscribe to EPS command packets 
    */

    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(ADCS_APP_EPS_MID), ADCS_APP_Data.EPSPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Error Subscribing to EPS, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    /*
    ** Register Table(s)
    */
    status = CFE_TBL_Register(&ADCS_APP_Data.TblHandles[0], "SampleAppTable", sizeof(ADCS_APP_Table_t),
                              CFE_TBL_OPT_DEFAULT, ADCS_APP_TblValidationFunc);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Error Registering Table, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }
    else
    {
        status = CFE_TBL_Load(ADCS_APP_Data.TblHandles[0], CFE_TBL_SRC_FILE, ADCS_APP_TABLE_FILE);
    }

    CFE_EVS_SendEvent(ADCS_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "ADCS App Initialized.%s",
                      ADCS_APP_VERSION_STRING);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the ADCS    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void ADCS_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case ADCS_APP_CMD_MID:
            ADCS_APP_ProcessGroundCommand(SBBufPtr);
            break;

        case ADCS_APP_SEND_HK_MID:
            ADCS_APP_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        case ADCS_APP_EPS_MID:
            printf("Case reached\n"); 
            ADCS_APP_ProcessEPSCommand(SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(ADCS_APP_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "ADCS: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}

/* EPS Commands */

void ADCS_APP_ProcessEPSCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    ADCS_APP_Data.need_charging = true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* ADCS ground commands                                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void ADCS_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" ADCS app ground commands
    */
    switch (CommandCode)
    {
        case ADCS_APP_NOOP_CC:
            if (ADCS_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(ADCS_APP_NoopCmd_t)))
            {
                ADCS_APP_Noop((ADCS_APP_NoopCmd_t *)SBBufPtr);
            }

            break;

        case ADCS_APP_RESET_COUNTERS_CC:
            if (ADCS_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(ADCS_APP_ResetCountersCmd_t)))
            {
                ADCS_APP_ResetCounters((ADCS_APP_ResetCountersCmd_t *)SBBufPtr);
            }

            break;

        case ADCS_APP_PROCESS_CC:
            if (ADCS_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(ADCS_APP_ProcessCmd_t)))
            {
                ADCS_APP_Process((ADCS_APP_ProcessCmd_t *)SBBufPtr);
            }

            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(ADCS_APP_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 ADCS_APP_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    int i;

    /*
    ** Get command execution counters...
    */
    ADCS_APP_Data.HkTlm.Payload.CommandErrorCounter = ADCS_APP_Data.ErrCounter;
    ADCS_APP_Data.HkTlm.Payload.CommandCounter      = ADCS_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(ADCS_APP_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(ADCS_APP_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < ADCS_APP_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(ADCS_APP_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* ADCS NOOP commands                                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 ADCS_APP_Noop(const ADCS_APP_NoopCmd_t *Msg)
{
    ADCS_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(ADCS_APP_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "ADCS: NOOP command %s",
                      ADCS_APP_VERSION);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 ADCS_APP_ResetCounters(const ADCS_APP_ResetCountersCmd_t *Msg)
{
    ADCS_APP_Data.CmdCounter = 0;
    ADCS_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(ADCS_APP_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "ADCS: RESET command");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 ADCS_APP_Process(const ADCS_APP_ProcessCmd_t *Msg)
{
    int32               status;
    ADCS_APP_Table_t *TblPtr;
    const char *        TableName = "ADCS_APP.AdcsAppTable";

    /* Adcs Use of Table */

    status = CFE_TBL_GetAddress((void *)&TblPtr, ADCS_APP_Data.TblHandles[0]);

    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Fail to get table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    CFE_ES_WriteToSysLog("Adcs App: Table x: %f  y: %f  z: %f  theta: %f", TblPtr->x, TblPtr->y, TblPtr->z, TblPtr->theta);

    ADCS_APP_GetCrc(TableName);

    status = CFE_TBL_ReleaseAddress(ADCS_APP_Data.TblHandles[0]);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Fail to release table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    /* Invoke a function provided by ADCS_APP_LIB */
    SAMPLE_LIB_Function();

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Verify command packet length                                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool ADCS_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(ADCS_APP_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        ADCS_APP_Data.ErrCounter++;
    }

    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify contents of First Table buffer contents                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 ADCS_APP_TblValidationFunc(void *TblData)
{
    int32               ReturnCode = CFE_SUCCESS;
    ADCS_APP_Table_t *TblDataPtr = (ADCS_APP_Table_t *)TblData;

    /*
    ** Adcs Table Validation
    */
    if (TblDataPtr->theta > ADCS_APP_TBL_ELEMENT_4_MAX)
    {
        /* First element is out of range, return an appropriate error code */
        ReturnCode = ADCS_APP_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Output CRC                                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ADCS_APP_GetCrc(const char *TableName)
{
    int32          status;
    uint32         Crc;
    CFE_TBL_Info_t TblInfoPtr;

    status = CFE_TBL_GetInfo(&TblInfoPtr, TableName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Adcs App: Error Getting Table Info");
    }
    else
    {
        Crc = TblInfoPtr.Crc;
        CFE_ES_WriteToSysLog("Adcs App: CRC: 0x%08lX\n\n", (unsigned long)Crc);
    }
}
