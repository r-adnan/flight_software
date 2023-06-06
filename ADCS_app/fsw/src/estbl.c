#include <string.h>
#include "estbl.h"

/************************************/
/** Local File Function Prototypes **/
/************************************/

static bool LoadJsonData(size_t JsonFileLen);

/**********************/
/** Global File Data **/
/**********************/

static ESTBL_Class_t* EsTbl = NULL;

static ESTBL_Data_t TblData; /* Working buffer for loads */

static CJSON_Obj_t JsonTblObjs[] = {

    /*Table Data Address    Table Data Length           Updated, Data Type,  float, JSON string, core-json query length exludes '\0' */
    { &TblData.position,    sizeof(TblData.position),   false,   JSONNumber, false, {"position", (sizeof("position")-1)}},
    { &TblData.velocity,    sizeof(TblData.velocity),   false,   JSONNumber, false, {"velocity", (sizeof("velocity")-1)}},
    { &TblData.estError,    sizeof(TblData.estError),   false,   JSONNumber, false, {"est_error", (sizeof("est_error")-1)}},
    { &TblData.proError,    sizeof(TblData.proError),   false,   JSONNumber, false, {"process_error", (sizeof("process_error")-1)}},
    { &TblData.prevEst,     sizeof(TblData.prevEst),    false,   JSONNumber, false, {"prev_ests", (sizeof("prev_est")-1)}},
    { &TblData.ppsData,     sizeof(TblData.ppsData),    false,   JSONNumber, false, {"pps_data", (sizeof("pps_data")-1)}},
    { &TblData.desAtt,      sizeof(TblData.desAtt),     false,   JSONNumber, false, {"desired_attitude", (sizeof("desired_attitude")-1)}},
}

void ESTBL_Constructor(ESTBL_Class_t *EsTblPtr, const INITBL_Class_t *IniTbl)
{
    EsTbl = EsTblPtr;

    CFE_PSP_MemSet(EsTbl, 0, sizeof(ESTBL_Class_t));

    EsTbl->AppName = INITBL_GetStrConfig(IniTbl, CFG_APP_CFE_NAME);
    EsTbl->JsonObjCnt = (sizeof(JsonTblObjs)/sizeof(CJSON_Obj_t));

} /* End ESTBL_Constructor() */

/******************************************************************************
** Function: EXOBJTBL_ResetStatus
**
*/
void ESTBL_ResetStatus(void)
{

   ESTBL->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   ESTBL->LastLoadCnt = 0;

} /* End ESTBL_ResetStatus() */

/******************************************************************************
** Function: EXOBJTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr_t.
**  2. This could migrate into table manager but I think I'll keep it here so
**     user's can add table processing code if needed.
*/

bool EESTBL_LoadCmd(TBLMGR_Tbl_t* Tbl, uint8 LoadType, const char* Filename)
{

   bool  RetStatus = false;

   if (CJSON_ProcessFile(Filename, EsTbl->JsonBuf, ESTBL_JSON_FILE_MAX_CHAR, LoadJsonData))
   {

      ESTBL->Loaded = true;
      ESTBL->LastLoadStatus = TBLMGR_STATUS_VALID;
      RetStatus = true;

   }
   else
   {

      ESTBL->LastLoadStatus = TBLMGR_STATUS_INVALID;

   }

   return RetStatus;

} /* End EXOBJTBL_LoadCmd() */ 