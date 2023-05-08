
#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <string.h>

#include <string>
#include <fstream>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include <winsock2.h>
#include <windows.h>

#ifdef _WIN32
    #define USE_WIN32
#endif

#ifdef USE_WIN32
#else
    #define closesocket close
#endif

#include "log.h"

using namespace std;

#include "utils.h"
#include "net.h"
#endif
