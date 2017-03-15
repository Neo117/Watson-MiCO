#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME := Lib_mqtt_client_c


$(NAME)_SOURCES := 	cJSON.c \
                    MQTTClient.c \
					MQTTConnectClient.c\
					MQTTConnectServer.c\
					MQTTDeserializePublish.c\
					MQTTFormat.c\
					MQTTLinux.c \
					MQTTPacket.c\
					MQTTSerializePublish.c\
					MQTTSubscribeClient.c\
					MQTTSubscribeServer.c\
					MQTTUnsubscribeClient.c\
					MQTTUnsubscribeServer.c\
					gatewayclient.c \
					
				   
GLOBAL_INCLUDES := 	. \
					
					
					
				   	