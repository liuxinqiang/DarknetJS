#include <iostream>
#include <node_api.h>
#include "improcess.h"
#define HAVE_STRUCT_TIMESPEC
#include <darknet.h>

napi_value DetectInit(napi_env, napi_callback_info);

napi_value DetectImage(napi_env, napi_callback_info);