#ifndef BELUGA_TRACK_H_
#define BELUGA_TRACK_H_

#include <stdint.h>
#include "CbCommonInclude.h"
#include <string.h>

bool BelugaTrack(const std::map<std::string, std::string>& data, const char* category, const char* action, const char* label, const int32_t value);

#endif