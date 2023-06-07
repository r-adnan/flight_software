#ifndef _mng_data_
#define _mng_data_

#include "app_cfg.h"
#include "inputbl.h"

#define MNG_DATA_SET_MODE_CMD_EID (MNG_DATA_BASE_EID + 0)
#define MNG_DATA_EXECUTE_EID      (MNG_DATA_BASE_EID + 1)

/******************************************************************************
** Function: MNG_DATA_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void MNG_DATA_ResetStatus(void);

/******************************************************************************
** Function: write_tbl
**
*/

/******************************************************************************
** Function: send_data
**
*/