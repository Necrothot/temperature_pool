#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

#include "global_definitions/types.h"

namespace constants
{

// NOTE: set VERSION_STRING in project properties
const std::string app_version = VERSION_STRING;

namespace tsensornames
{

const std::string power_unit = "Power Unit";
const std::string modem = "Modem";
const std::string cross_board = "Cross Board";
const std::string upconverter = "Upconverter";

}

}

#endif // CONSTANTS_H
