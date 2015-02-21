#ifndef _DBG_PRI_H_
#define _DBG_PRI_H_

#include <stdio.h>
#include "DBG.h"

struct DBG_Controller_tag {
    FILE        *debug_write_fp;
    int         current_debug_level;
};

#endif
