#ifndef _MEM_PRI_H_
#define _MEM_PRI_H_

#include "MEM.h"

typedef union Header_tag Header;

struct MEM_Controller_tag {
    FILE        *error_fp;
    MEM_ErrorHandler    error_handler;
    MEM_FailMode        fail_mode;
    Header      *block_header;
};
#endif
