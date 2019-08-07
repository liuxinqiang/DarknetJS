#include <iostream>
#include <node_api.h>
#include "detect/detect.h"


napi_value init(napi_env env, napi_value exports) {

	napi_value localFuncList[2];

	napi_create_function(env, nullptr, 0, DetectInit, nullptr, localFuncList);

	napi_create_function(env, nullptr, 0, DetectImage, nullptr, localFuncList + 1);

	napi_set_named_property(env, exports, "detectInit", localFuncList[0]);

	napi_set_named_property(env, exports, "detectImage", localFuncList[1]);

	return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);