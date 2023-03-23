#pragma once

#define TCP_PORT 49990
#define UDP_PORT 49999
#define BUF_SIZE 1024
#define QUEUE_SIZE 10
#define IP_ADDRESS "172.30.1.49"

#include "Message.h"
#include "MapData.h"

extern int connectedClientCount;
extern MapData* mapData;
extern Message* messageData;