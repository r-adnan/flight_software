#ifndef _mng_data_
#define _mng_data_

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

/******************************************************************************
** Function: write_tbl
**
*/

void write_tbl(const ADCS_APP_Table_t *TblObj, const char *filename);

/******************************************************************************
** Function: send_data
**
*/

void send_data(ADCS_APP_Table_t *TblObj);

void MNG_DATA_Test(void);

#endif