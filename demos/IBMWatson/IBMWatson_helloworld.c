/**
******************************************************************************
* @file    IBMWatson_entry.c
* @author  William Xu
* @version V1.0.0
* @date    16-Feb-2017
* @brief   MiCO&IBM Watson wifi Demo
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/

#include "../IBMWatson_TH_Data/mico_config.h"
#include "../IBMWatson_TH_Data/src/iotfclient.h"
#include "mico.h"
#include "SocketUtils.h"

volatile int interrupt = 0;
#define ibmwatson_log(M, ...) custom_log("mico", M, ##__VA_ARGS__)


static mico_semaphore_t wait_sem = NULL;

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
  switch ( status )
  {
  case NOTIFY_STATION_UP:
    mico_rtos_set_semaphore( &wait_sem );
    break;
  case NOTIFY_STATION_DOWN:
  case NOTIFY_AP_UP:
  case NOTIFY_AP_DOWN:
    break;
  }
}

//int ibm_fun(void);

int application_start( void )
{
  OSStatus err = kNoErr;
  
  mico_rtos_init_semaphore( &wait_sem, 1 );
  
  /*Register user function for MiCO nitification: WiFi status changed */
  err = mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED,
                                    (void *) micoNotify_WifiStatusHandler, NULL );
  require_noerr( err, exit );
  
  /* Start MiCO system functions according to mico_config.h */
  err = mico_system_init( mico_system_context_init( 0 ) );
  require_noerr( err, exit );
  
  /* Wait for wlan connection*/
  mico_rtos_get_semaphore( &wait_sem, MICO_WAIT_FOREVER );
  ibmwatson_log( "wifi connected successful" );
  ibm_fun();
exit:
  mico_rtos_delete_thread( NULL );
  return err;
}

void myCallback (char* commandName, char* format, void* payload)
{
	printf("------------------------------------\n" );
	printf("The command received :: %s\n", commandName);
	printf("format : %s\n", format);
	printf("Payload is : %s\n", (char *)payload);

	printf("------------------------------------\n" );
}

int ibm_fun(void)
{
  //12位10进制数
  
  int rc = -1;
  
  
  Iotfclient client;

  // for Quickstart, no need to pass the authmethod and token(last 2 params)
  rc = initialize(&client,"w9tm98", "internetofthings.ibmcloud.com", "MiCO_Watson_Connection", "EMW31652017", "token", "*uvtSgr97?g0WS9eV4");
  
  // For registered mode
  // rc = initialize(&client,"ordid","deviceType","deviceid","token","r+i2I720I+EkZe1c@8");
  
  if(rc != SUCCESS1){
    printf("Initialize returned rc = %d.\n Quitting..\n", rc);
    return 0;
  }
  
  rc = connectiotf(&client);
  
  if(rc != SUCCESS1){
    printf("Connection returned rc = %d.\n Quitting..\n", rc);
    return 0;
  }
  
  setCommandHandler(&client, myCallback);
  subscribeCommands(&client);
  subscribeToDeviceCommands(&client, "MiCO_Watson_Connection", "EMW31652017", "+", "+", 0);
  printf("Connection Successful. Press Ctrl+C to quit\n");
 // printf("View the visualization at https://quickstart.internetofthings.ibmcloud.com/#/device/%s\n", deviceId);
  
  char *data = "{\"d\" : {\"x\" : 26 }}";
  
  while(!interrupt) 
  {
    printf("Publishing the event stat with rc ");
    rc= publishEvent(&client,"status","json", data , QOS0);
    printf(" %d\n", rc);
    sleep(2);
  }
  
  printf("Quitting!!\n");
  
  disconnect(&client);
  
}

