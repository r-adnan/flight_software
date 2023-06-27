#include <string.h>
#include <stdio.h>
#include "adcs_mng_data.h"

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
** Function:  write_tbl
** DescritionL: write table data to a .txt
*/

void write_tbl(const ADCS_APP_Table_t *TblObj, const char *filename)
{
   FILE *file = fopen(filename, "w");

   // if (file == NULL){
   //    printf("Error opening file. \n");
   //    return;
   // }

   if (fprintf(file, "%f\n", TblObj->x) < 0 || 
       fprintf(file, "%f\n", TblObj->y) < 0 ||
       fprintf(file, "%f\n", TblObj->z) < 0 ||
       fprintf(file, "%f\n", TblObj->theta) < 0) {
       printf("Error writing to file. \n");
       fclose(file);
       return;
   }

   // printf("x: %f  y: %f  z: %f  theta: %f  \n", TblObj->x, TblObj->y, TblObj->z, TblObj->theta);       

   fprintf(file, "%f\n", TblObj->x);
   fprintf(file, "%f\n", TblObj->y);
   fprintf(file, "%f\n", TblObj->z);
   fprintf(file, "%f\n", TblObj->theta);

   fclose(file);

   printf("Table successfully written into des_att.txt\n");

}

/******************************************************************************
** Function:  send_data
** Description: send .txt to rpi
*/

void send_data(ADCS_APP_Table_t *TblObj)
{
   write_tbl(TblObj, "../../../apps/adcs_app/fsw/src/des_att.txt");

   
}

// Test Function

void MNG_DATA_Test(void)
{
   
  printf("App successfully run.\n");

   
}

