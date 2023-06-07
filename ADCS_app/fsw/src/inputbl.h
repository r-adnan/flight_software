#ifndef _inputbl_
#define _inputbl_

/*
** Includes 
*/

#include "app_cfg.h"

/*
** Event Message IDs
*/

#define INPUTBL_DUMP_ERR_EID   (INPUTBL_BASE_EID + 0)
#define INPUTBL_LOAD_ERR_EID   (INPUTBL_BASE_EID + 1)

/******************************************************************************
** Table - Local table copy used for table loads
**
*/

typedef struct
{

   double   x;
   double   y;
   double   z; 
   double   theta;


} INPUTBL_Data_t;

/* Return pointer to owner's table data */
typedef INPUTBL_Data_t* (*INPUTBL_GetDataPtr_t)(void);

typedef struct
{

   /*
   ** Table parameter data
   */

   INPUTBL_Data_t Data;

   /*
   ** Standard CJSON table data
   */

   const char   *AppName;
   bool         Loaded;   /* Has entire table been loaded? */
   uint8        LastLoadStatus;
   uint16       LastLoadCnt;

   size_t       JsonObjCnt;
   char         JsonBuf[ESTBL_JSON_FILE_MAX_CHAR];
   size_t       JsonFileLen;

} INPUTBL_Class_t;

/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: INPUTBL_Constructor
**
** Initialize the example table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is
**      registered with the table manager.
**
*/
void INPUTBL_Constructor(INPUTBL_Class_t *TblObj, const INITBL_Class_t *IniTbl);

/******************************************************************************
** Function: INPUTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void INPUTBL_ResetStatus(void);

/******************************************************************************
** Function: INPUTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr_t.
**  2. Can assume valid table file name because this is a callback from
**     the app framework table manager.
**
*/
bool INPUTBL_LoadCmd(TBLMGR_Tbl_t *Tbl, uint8 LoadType, const char *Filename);


/******************************************************************************
** Function: INPUTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr_t.
**  2. Can assume valid table file name because this is a callback from
**     the app framework table manager.
**
*/
bool INPUTBL_DumpCmd(TBLMGR_Tbl_t *Tbl, uint8 DumpType, const char *Filename);


#endif /* _inputbl_ */



