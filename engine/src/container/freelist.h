/**
 * @file freelist.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines a freelist data structure.
 */
#ifndef FREELIST_H
#define FREELIST_H

#include "common.h"

// Type definition for a freelist.
typedef struct
{
    void* memory;
}
freelist_t;

void
freelist_create
(   u64         cap
,   u64*        memory_requirement
,   void*       memory
,   freelist_t* ls
);

void
freelist_destroy
(   freelist_t* ls
);

bool
freelist_allocate
(   freelist_t* ls
,   u64         size
,   u64*        offs
);

bool
freelist_free
(   freelist_t* ls
,   u64         size
,   u64         offs
);

bool
freelist_resize
(   freelist_t* ls
,   u64*        memory_requirement
,   void*       memory_new
,   u64         cap_new
,   void**      memory_old
);

void
freelist_reset
(   freelist_t* ls
);

u64
freelist_query_free
(   freelist_t* ls
);

#endif  // FREELIST_H
