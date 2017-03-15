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

#include "mico_config.h"
#include "gatewayclient.h"
#include "mico.h"
#include "SocketUtils.h"
#include "json_c/json.h"
#include "micokit_ext.h"
#include "sensor/DHT11/DHT11.h"

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

int ibm_fun(void); 

char* get_json_str()  ;

int application_start( void )
{
  DHT11_Init();
  
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

//void myCallback (char* commandName, char* format, void* payload)
void myCallback (char* type, char* id, char* commandName, char *format, void* payload, size_t payloadlen)
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
  
  //int rc001 = 60; 
  
  GatewayClient client;
  //init gateway.cfg
  // for Quickstart, no need to pass the authmethod and token(last 2 params)
  rc = initializeGateway(&client,"w9tm98", "internetofthings.ibmcloud.com", "MiCO_Watson_Connection_Gateway", "EMW3165Gateway", "token", "HjWeBs&RjmJSslWgEV");
  
  // For registered mode
  // rc = initialize(&client,"ordid","deviceType","deviceid","token","r+i2I720I+EkZe1c@8");
  
  if(rc != SUCCESS1){
    printf("Initialize returned rc = %d.\n Quitting..\n", rc);
    return 0;
  }
  
  
  
  setKeepAliveInterval(59);
  
  rc = connectGateway(&client);
  
  if(rc != SUCCESS1){
    printf("Connection failed and returned rc = %d.\n Quitting..", rc);
    return 0;
  }
  
  //Registering the function "myCallback" as the command handler.
  setGatewayCommandHandler(&client, myCallback);
  // providing "+" will subscribe to all the command of all formats.
  subscribeToDeviceCommands(&client, "MiCO_Watson_Connection_Gateway", "EMW3165Gateway", "+", "+", 0);
  
  while(!interrupt) 
  {

    //publishing gateway events
    //rc= publishGatewayEvent(&client, "status","json", "{\"d\" : {\"temp\" : 34 }}", QOS0);
    char *p=get_json_str();
    ibmwatson_log("p=%s",p);
    rc= publishGatewayEvent(&client, "status","json", p, QOS0);
    ibmwatson_log("Publishing the event stat with rc=%d",rc);
    free(p);
    sleep(2);
  }
  
  printf("Quitting!!\n");
  
  //Be sure to disconnect the gateway at exit
  disconnectGateway(&client);
}

char* get_json_str()  
{  
  /*control info*/
   uint8_t dht11_temp_data = 0;
   uint8_t dht11_hum_data = 0;
   char *str= (char*)malloc(100);
  
   //DHT11_Init();
   DHT11_Read_Data(&dht11_temp_data, &dht11_hum_data);
  
  /*1:construct json object*/
  struct json_object *device_object=json_object_new_object();
  json_object_object_add(device_object, "temp", json_object_new_int(dht11_temp_data));  
  json_object_object_add(device_object, "hum", json_object_new_int(dht11_hum_data)); 
  
  
  strcpy(str,json_object_to_json_string(device_object));
  
  /*2:parse finished,free memory*/
  json_object_put(device_object);/*free memory*/   
  device_object=NULL;
  return str;
}  
