/**
 * @file array.h
 * @author Matthew Weissel (null@mattweissel.info)
 * @brief Defines a type and operations for implementing a resizable
 * array data structure.
 */
#ifndef ARRAY_H
#define ARRAY_H

#include "common.h"

/**
 * Type definition for array fields.
 */
typedef enum
{
    ARRAY_FIELD_CAPACITY
,   ARRAY_FIELD_LENGTH
,   ARRAY_FIELD_STRIDE
,   ARRAY_FIELD_COUNT
}
ARRAY_FIELD;

// Defines array default capacity.
#define ARRAY_DEFAULT_CAPACITY 10

// Defines array scale factor.
#define ARRAY_SCALE_FACTOR(cap) ( ( cap * 3 ) >> 1 )

/**
 * @brief Allocates memory for a resizable array.
 * @param icap The initial capacity.
 * @param stride The fixed element size in bytes.
 * @return A resizable array, or null on error.
 */
void*
_array_allocate
(   ARRAY_FIELD icap
,   ARRAY_FIELD stride
);

#define array_allocate(type,icap)    \
    _array_allocate ( (icap) , sizeof ( type ) )

#define array_allocate_new(type)    \
    _array_allocate ( ARRAY_DEFAULT_CAPACITY , sizeof ( type ) )

/**
 * @brief Frees the memory used by a resizable array.
 * @param arr The array to free.
 */
void
_array_free
(   void* arr
);

#define array_free(arr) \
    _array_free ( (arr) );

/**
 * @brief Reads the value of an array field.
 * @param arr The array to access.
 * @param field The field to read.
 * @return The value of the array field.
 */
u64
_array_field_get
(   void*       arr
,   ARRAY_FIELD field
);

#define array_capacity(arr) \
    _array_field_get ( (arr) , ARRAY_FIELD_CAPACITY )

#define array_length(arr) \
    _array_field_get ( (arr) , ARRAY_FIELD_LENGTH )

#define array_stride(arr) \
    _array_field_get ( (arr) , ARRAY_FIELD_STRIDE )

/**
 * @brief Sets the value of an array field.
 * @param arr The array to mutate.
 * @param field The field to set.
 * @param value The value to set.
 */
void
_array_field_set
(   void*       arr
,   ARRAY_FIELD field
,   u64         value
);

#define array_length_set(arr,value)    \
    _array_field_set ( (arr) , ARRAY_FIELD_LENGTH , (value) )

/**
 * @brief Resizes an existing array.
 * @param arr The array to mutate.
 * @return The new array.
 */
void*
_array_resize
(   void* arr
);

/**
 * @brief Adds an element to an array.
 * @param arr The array to mutate.
 * @param elem The element to add.
 * @return The array (possibly with new address).
 */
void*
_array_push
(   void*       arr
,   const void* elem
);

#define array_push(arr,value)                   \
    {                                           \
        __typeof__ ( (value) ) tmp = (value);   \
        (arr) = _array_push ( (arr) , &tmp );   \
    }

/**
 * @brief Removes an element from an array.
 * @param arr The array to mutate.
 * @param dst A destination buffer to store the data that was removed.
 */
void
_array_pop
(   void* arr
,   void* dst
);

#define array_pop(arr,dst)  \
    _array_pop ( (arr) , (dst) )

/**
 * @brief Adds an element to an array at a specified index.
 * @param arr The array to mutate.
 * @param indx The index to insert at.
 * @param elem The element to add.
 * @return The array (possibly with new address).
 */
void*
_array_insert
(   void*       arr
,   u64         indx
,   const void* elem
);

#define array_insert(arr,indx,value)                        \
    {                                                       \
        __typeof__ ( (value) ) tmp = (value);               \
        (arr) = _array_insert ( (arr) , (indx) , &tmp );    \
    }

/**
 * @brief Removes am element from an array at a specified index.
 * @param arr The array to mutate.
 * @param indx The index of the element to remove.
 * @param dst A destination buffer to store the element that was removed.
 */
void*
_array_remove
(   void*   arr
,   u64     indx
,   void*   dst
);

#define array_remove(arr,indx,dst)   \
    _array_remove ( (arr) , (indx) , (dst) )

#endif  // ARRAY_H
