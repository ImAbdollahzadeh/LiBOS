#ifndef _IMAGE_LOADER__H__
#define _IMAGE_LOADER__H__

#include "../include/LiBOS_CORE.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//void ect_execution(const INT_8* file_name);

extern void _LiBOSASM_load_ext_image (char* path, void* buffer);
void        load_ext_loader          (char* path);
void        load_ext_image           (char* path);
void        dump_global_ext_table    (void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _IMAGE_LOADER__H__
