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
**    Define the EXOBJ_Class 
**
**  Notes:
**    None
**
**  References:
**    1. OpenSatKit Object-based Application Developer's Guide
**    2. cFS Application Developer's Guide
**
*/
#ifndef _exobj_
#define _exobj_

/*
** Includes
*/

#include "app_cfg.h"
#include "exobjtbl.h"


/***********************/
/** Macro Definitions **/
/***********************/


/*
** Event Message IDs
*/

#define EXOBJ_SET_MODE_CMD_EID (EXOBJ_BASE_EID + 0)
#define EXOBJ_EXECUTE_EID      (EXOBJ_BASE_EID + 1)


/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
** - See EDS command definitions in adcs.xml
*/


/******************************************************************************
** EXOBJ_Class
*/

/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: EXOBJ_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void EXOBJ_ResetStatus(void);


/******************************************************************************
** Function: EXOBJ_Execute
**
** Perform periodic processing that is trigger by an 'execute' request message
** from the scheduler app.
*/
void EXOBJ_Execute(void);


#endif /* _exobj_ */
