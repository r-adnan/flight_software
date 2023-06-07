#include <string.h>
#include "inputbl.h"

/************************************/
/** Local File Function Prototypes **/
/************************************/

static bool LoadJsonData(size_t JsonFileLen);

/**********************/
/** Global File Data **/
/**********************/

static INPUTBL_Class_t* EsTbl = NULL;

static INPUTBL_Data_t TblData; /* Working buffer for loads */

static CJSON_Obj_t JsonTblObjs[] = {

    /*Table Data Address    Table Data Length    Updated, Data Type,  float, JSON string, core-json query length exludes '\0' */
    { &TblData.x,       sizeof(TblData.x),       false,   JSONNumber, true, {"des_att_x",     (sizeof("des_att_x")-1)}},
    { &TblData.y,       sizeof(TblData.y),       false,   JSONNumber, true, {"des_att_y",     (sizeof("des_att_y")-1)}},
    { &TblData.z,       sizeof(TblData.z),       false,   JSONNumber, true, {"des_att_z",     (sizeof("des_att_z")-1)}},
    { &TblData.theta,   sizeof(TblData.theta),   false,   JSONNumber, true, {"des_att_theta", (sizeof("des_att_theta")-1)}}
}

void INPUTBL_Constructor(INPUTBL_Class_t *InpuTblPtr, const INITBL_Class_t *IniTbl)
{
    InpuTbl = InpuTblPtr;

    CFE_PSP_MemSet(InpuTbl, 0, sizeof(INPUTBL_Class_t));

    InpuTbl->AppName = INITBL_GetStrConfig(IniTbl, CFG_APP_CFE_NAME);
    InpuTbl->JsonObjCnt = (sizeof(JsonTblObjs)/sizeof(CJSON_Obj_t));

} /* End INPUTBL_Constructor() */

/******************************************************************************
** Function: INPUTBL_ResetStatus
**
*/
void INPUTBL_ResetStatus(void)
{

   INPUTBL->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   INPUTBL->LastLoadCnt = 0;

} /* End INPUTBL_ResetStatus() */

/******************************************************************************
** Function: INPUTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr_t.
**  2. This could migrate into table manager but I think I'll keep it here so
**     user's can add table processing code if needed.
*/

bool INPUTBL_LoadCmd(TBLMGR_Tbl_t* Tbl, uint8 LoadType, const char* Filename)
{

   bool  RetStatus = false;

   if (CJSON_ProcessFile(Filename, InpuTbl->JsonBuf, INPUTBL_JSON_FILE_MAX_CHAR, LoadJsonData))
   {

      INPUTBL->Loaded = true;
      INPUTBL->LastLoadStatus = TBLMGR_STATUS_VALID;
      RetStatus = true;

   }
   else
   {

      INPUTBL->LastLoadStatus = TBLMGR_STATUS_INVALID;

   }

   return RetStatus;

} /* End INPUTBL_LoadCmd() */ 


/******************************************************************************
** Function: INPUTBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr_t.
**  2. Can assume valid table filename because this is a callback from
**     the app framework table manager that has verified the file.
**  3. DumpType is unused.
**  4. File is formatted so it can be used as a load file.
**  5. Creates a new dump file, overwriting anything that may have existed
**     previously
*/
bool INPUTBL_DumpCmd(TBLMGR_Tbl_t *Tbl, uint8 DumpType, const char *Filename)
{

   bool       RetStatus = false;
   int32      SysStatus;
   osal_id_t  FileHandle;
   os_err_name_t OsErrStr;
   char DumpRecord[256];
   char SysTimeStr[128];


   SysStatus = OS_OpenCreate(&FileHandle, Filename, OS_FILE_FLAG_CREATE, OS_READ_WRITE);

 if (SysStatus == OS_SUCCESS)
   {

      sprintf(DumpRecord,"{\n   \"app-name\": \"%s\",\n   \"tbl-name\": \"Desired Attitude\",\n",InpuTbl->AppName);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      sprintf(DumpRecord,"   \"description\": \"Table dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"     \"des_att_x\": %d\n   },\n", InpuTbl->Data.x);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"     \"des_att_y\": %d\n   }\n}\n", InpuTbl->Data.y); //this ssaid low limit also
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"     \"des_att_z\": %d\n   }\n}\n", InpuTbl->Data.z); 
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"     \"des_att_theta\": %d\n   }\n}\n", InpuTbl->Data.theta); 
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      OS_close(FileHandle);

      RetStatus = true;

   } /* End if file create */

 else
   {
      OS_GetErrorName(SysStatus, &OsErrStr);
      CFE_EVS_SendEvent(INPUTBL_DUMP_ERR_EID, CFE_EVS_EventType_ERROR,
                        "Error creating dump file '%s', status=%s",
                        Filename, OsErrStr);

   } /* End if file create error */

   return RetStatus;

} /* End of INPUTBL_DumpCmd() */

/******************************************************************************
** Function: LoadJsonData
**
** Notes:
**  1. See file prologue for full/partial table load scenarios
*/
static bool LoadJsonData(size_t JsonFileLen)
{

   bool      RetStatus = false;
   size_t    ObjLoadCnt;


   InpuTbl->JsonFileLen = JsonFileLen;

   /*
   ** 1. Copy table owner data into local table buffer
   ** 2. Process JSON file which updates local table buffer with JSON supplied values
   ** 3. If valid, copy local buffer over owner's data
   */

   memcpy(&TblData, &InpuTbl->Data, sizeof(INPUTBL_Data_t));

   ObjLoadCnt = CJSON_LoadObjArray(JsonTblObjs, InpuTbl->JsonObjCnt, InpuTbl->JsonBuf, InpuTbl->JsonFileLen);

    if (!InpuTbl->Loaded && (ObjLoadCnt != InpuTbl->JsonObjCnt))
   {

      CFE_EVS_SendEvent(INPUTBL_LOAD_ERR_EID, CFE_EVS_EventType_ERROR,
                        "Table has never been loaded and new table only contains %ld of %ld data object",
                        ObjLoadCnt, InpuTbl->JsonObjCnt);

   }
   else
   {
      memcpy(&InpuTbl->Data,&TblData, sizeof(INPUTBL_Data_t));
      InpuTbl->LastLoadCnt = ObjLoadCnt;
      RetStatus = true;
   }

   return RetStatus;

} /* End LoadJsonData() */
