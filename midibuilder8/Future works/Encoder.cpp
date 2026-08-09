#include "Encoder.h"

Encoder* Encoder::_instance = nullptr;
portMUX_TYPE Encoder::_mux = portMUX_INITIALIZER_UNLOCKED;
