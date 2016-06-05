#pragma once
#ifndef _SERVER_BASE_H
#define _SERVER_BASE_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <queue>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <fstream>
#include <errno.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/signal.h>
#include <pthread.h>


namespace IslandSurvival{

#define debug_pos printf("%s:%d\n" , __FILE__ , __LINE__ )
#define debug(a)  printf("%s = %d\n" , #a , a)

#define PRINT_LOG printf("[%s:%d] ",__FILE__, __LINE__); printf

enum
{
    success = 0,
    fail = 1,
    error = -1,
};

int64_t GetNowTimeUs();

int64_t GetNowTimeMs();

int32_t GetNowTimeSecond();

}
#endif   //_COMMON_BASE_H
