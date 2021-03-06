#ifndef __PRECOMPILE_H__
#define __PRECOMPILE_H__

#pragma warning ( disable : 4267 )

#include "rb_common.h"

#include <vector>
#include <string>

#include "assert.h"
#include "math.h"
#include "stdlib.h"

#include "jreflect.h"
#include "logger.h"

#include "stdlib.h"
#include "jstring.h"
#include "fstream.h"
#include "regpool.h"
#include "path.h"
#include "iscriptserver.h"

#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4020 )

extern "C"
{
#include "lua.h"
#include "lstate.h"
#include "lualib.h"
#include "lauxlib.h"
//#include "pluto.h"
}

#endif
