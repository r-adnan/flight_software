#ifndef _exobjtbl_
#define _exobjtbl_

/*
** Includes 
*/

#include "app_cfg.h"

/*
** Event Message IDs
*/

#define ESTBL_DUMP_ERR_EID   (ESTBL_BASE_EID + 0)
#define ESTBL_LOAD_ERR_EID   (ESTBL_BASE_EID + 1)

/******************************************************************************
** Table - Local table copy used for table loads
**
*/

typedef struct
{

   uint16   position;
   uint16   velocity;
   uint16   estError;
   uint16   proError;
   uint16   prevEst;
   uint16   ppsData;
   uint16   desAtt;

} EXOBJTBL_Data_t;

/* Return pointer to owner's table data */
typedef EXOBJTBL_Data_t* (*EXOBJTBL_GetDataPtr_t)(void);

typedef struct
{

   /*
   ** Table parameter data
   */

   ESTBL_Data_t Data;

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

} ESTBL_Class_t;

/************************/
/** Exported Functions **/
/************************/

