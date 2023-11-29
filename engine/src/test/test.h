/**
 * @file test.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Test management subsystem.
 */
#ifndef TEST_H
#define TEST_H

#include "common.h"

#define BYPASS  2

typedef u8 ( *PFN_test )();

void
test_startup
( void );

void
test_register
(   PFN_test
,   char*       desc
);

bool
test_run_all
( void );

#endif  // TEST_H
