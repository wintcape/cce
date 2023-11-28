/**
 * @author Matthew Weissel (null@mattweissel.info)
 * @file freelist.c
 * @brief Implementation of the freelist header.
 * (see freelist.h for additional details)
 */
#include "container/freelist.h"

#include "core/logger.h"
#include "core/string.h"

#include "math/math.h"

// Type definition for a freelist node.
typedef struct node_t
{
    u64             offs;
    u64             size;
    struct node_t*  next;

}
node_t;

// Type definition for internal state.
typedef struct
{
    u64     cap;
    u64     max_entries;
    node_t* head;
    node_t* ls;
}
state_t;

static
node_t*
freelist_get_node
(   freelist_t* ls
);

static
void
freelist_return_node
(   node_t* node
);

void
freelist_create
(   u64         cap
,   u64*        memory_requirement
,   void*       memory
,   freelist_t* ls
)
{
    const u64 max_entries = max ( 20U
                                , cap / ( sizeof ( void* ) * sizeof ( node_t ) )
                                );

    *memory_requirement = sizeof ( state_t ) + sizeof ( node_t ) * max_entries;

    if ( !memory )
    {
        return;
    }

    u64 min_ = ( sizeof ( state_t ) + sizeof ( node_t ) ) * 8;
    if ( cap < min_ )
    {
        f32 arg_amt;
        f32 min_amt;
        const char* arg_unit = string_bytesize ( cap , &arg_amt );
        const char* min_unit = string_bytesize ( min_ , &min_amt );
        LOGWARN ( "Requested freelist with capacity of %.2f %s."
                  "\tNOTE:  Freelist is inefficient when handling less than %.2f %s."
                , arg_amt , arg_unit
                , min_amt , min_unit
                );
    }

    ( *ls ).memory = memory;

    memory_clear ( ( *ls ).memory , *memory_requirement );
    
    state_t* state = ( *ls ).memory;
    ( *state ).ls = ( *ls ).memory + sizeof ( state_t );
    ( *state ).max_entries = max_entries;
    ( *state ).cap = cap;

    memory_clear ( ( *state ).ls , sizeof ( node_t ) * ( *state ).max_entries );
    ( *state ).head = &( *state ).ls[ 0 ];
    ( *( ( *state ).head ) ).offs = 0;
    ( *( ( *state ).head ) ).size = cap;
    ( *( ( *state ).head ) ).next = 0;
}

void
freelist_destroy
(   freelist_t* ls
)
{
    if ( !ls || !( *ls ).memory )
    {
        return;
    }

    state_t* state = ( *ls ).memory;
    memory_clear ( ( *ls ).memory
                 ,   sizeof ( state_t )
                   + sizeof ( node_t ) * ( *state ).max_entries
                 );
    ( *ls ).memory = 0;
}

bool
freelist_allocate
(   freelist_t* ls
,   u64         size
,   u64*        offs
)
{
    if ( !ls || !offs || !( *ls ).memory )
    {
        return false;
    }

    state_t* state = ( *ls ).memory;
    node_t* node = ( *state ).head;
    node_t* prev = 0;

    while ( node )
    {
        if ( ( *node ).size == size )
        {
            *offs = ( *node ).offs;
            
            node_t* out = 0;
            if ( prev )
            {
                ( *prev ).next = ( *node ).next;
                out = node;
            }
            else
            {
                out = ( *state ).head;
                ( *state ).head = ( *node ).next;
            }

            freelist_return_node ( out );
            return true;
        }
        else if ( ( *node ).size > size )
        {
            *offs = ( *node ).offs;
            ( *node ).size -= size;
            ( *node ).offs += size;
            return true;
        }

        prev = node;
        node = ( *node ).next;
    }

    f32 req_amt;    
    f32 rem_amt;
    const char* req_unit = string_bytesize ( size
                                           , &req_amt
                                           );
    const char* rem_unit = string_bytesize ( freelist_query_free ( ls )
                                           , &rem_amt
                                           );
    LOGWARN ( "freelist_allocate: No block with enough free space found (requested: %.2f %s, available: %.2f %s)."
            , req_amt , req_unit
            , rem_amt , rem_unit
            );

    return false;
}

bool
freelist_free
(   freelist_t* ls
,   u64         size
,   u64         offs
)
{
    if ( !ls || !size || !( *ls ).memory )
    {
        return false;
    }

    state_t* state = ( *ls ).memory;
    node_t* node = ( *state ).head;
    node_t* prev = 0;

    if ( !node )
    {
        node_t* new = freelist_get_node ( ls );
        ( *new ).offs = offs;
        ( *new ).size = size;
        ( *new ).next = 0;
        ( *state ).head = new;
        return true;
    }

    while ( node )
    {
        if ( ( *node ).offs + ( *node ).size == offs )
        {
            ( *node ).size += size;
            if (   ( *node ).next
                && ( *node ).offs + ( *node ).size == ( *( ( *node ).next ) ).offs
               )
            {
                ( *node ).size += ( *( ( *node ).next ) ).size;
                node_t* next = ( *node ).next;
                ( *node ).next = ( *( ( *node ).next ) ).next;
                freelist_return_node ( next );
            }
            return true;
        }
        else if ( ( *node ).offs == offs )
        {
            LOGERROR ( "freelist_free: Double free occurred at memory offset %llu."
                     , ( *node ).offs
                     );
            return false;
        }
        else if ( ( *node ).offs > offs )
        {
            node_t* new = freelist_get_node ( ls );
            ( *new ).offs = offs;
            ( *new ).size = size;

            if ( prev )
            {
                ( *prev ).next = new;
                ( *new ).next = node;
            }
            else
            {
                ( *new ).next = node;
                ( *state ).head = new;
            }

            if (    ( *new ).next
                 && ( *new ).offs + ( *new ).size == ( *( ( *new ).next ) ).offs
               )
            {
                ( *new ).size += ( *( ( *new ).next ) ).size;
                node_t* node_ = ( *new ).next;
                ( *new ).next = ( *node_ ).next;
                freelist_return_node ( node_ );
            }
            if (    prev
                 && ( *prev ).offs + ( *prev ).size == ( *new ).offs
               )
            {
                ( *prev ).size += ( *new ).size;
                node_t* node_ = new;
                ( *prev ).next = ( *node_ ).next;
                freelist_return_node ( node_ );
            }

            return true;
        }

        if (    !( *node ).next
             && ( *node ).offs + ( *node ).size < offs
           )
        {
            node_t* new = freelist_get_node ( ls );
            ( *new ).offs = offs;
            ( *new ).size = size;
            ( *new ).next = 0;
            ( *node ).next = new;
            return true;
        }

        prev = node;
        node = ( *node ).next;

    }// End while.

    LOGWARN ( "freelist_free: Did not find a block to free. Memory corruption possible." );
    return false;
}

bool
freelist_resize
(   freelist_t* ls
,   u64*        memory_requirement
,   void*       memory_new
,   u64         cap_new
,   void**      memory_old
)
{
    if (    !ls
         || !memory_requirement
         || ( *( ( state_t* )( ( *ls ).memory ) ) ).cap > cap_new
       )
    {
        return false;
    }

    const u64 max_entries = max ( 20U
                                , cap_new / ( sizeof ( void* ) )
                                );

    *memory_requirement = sizeof ( state_t ) + sizeof ( node_t ) * max_entries;

    if ( !memory_new )
    {
        return true;
    }

    *memory_old = ( *ls ).memory;

    state_t* state_old = ( *ls ).memory;
    const u64 cap_diff = cap_new - ( *state_old ).cap;

    ( *ls ).memory = memory_new;

    memory_clear ( ( *ls ).memory , *memory_requirement );

    state_t* state = ( *ls ).memory;
    ( *state ).ls = ( *ls ).memory + sizeof ( state_t );
    ( *state ).max_entries = max_entries;
    ( *state ).cap = cap_new;

    memory_clear ( ( *state ).ls
                 , sizeof ( node_t ) * ( *state ).max_entries
                 );

    ( *state ).head = &( *state ).ls[ 0 ];

    node_t* node_new = ( *state ).head;
    node_t* node_old = ( *state_old ).head;

    if ( !node_old )
    {
        ( *( ( *state ).head ) ).offs = ( *state_old ).cap;
        ( *( ( *state ).head ) ).size = cap_diff;
        ( *( ( *state ).head ) ).next = 0;
        return true;
    }
    
    while ( node_old )
    {
        node_t* new = freelist_get_node ( ls );

        ( *new ).offs = ( *node_old ).offs;
        ( *new ).size = ( *node_old ).size;
        ( *new ).next = 0;
        ( *node_new ).next = new;

        node_new = ( *node_new ).next;
        
        if ( ( *node_old ).next )
        {
            node_old = ( *node_old ).next;
        }
        else
        {
            if ( ( *node_old ).offs + ( *node_old ).size == ( *state_old ).cap )
            {
                ( *new ).size += cap_diff;
            }
            else
            {
                node_t* new_end = freelist_get_node ( ls );
                ( *new_end ).offs = ( *state_old ).cap;
                ( *new_end ).size = cap_diff;
                ( *new_end ).next = 0;
                ( *new ).next = new_end;
            }

            break;
        }
    }

    return true;
}

void
freelist_clear
(   freelist_t* ls
)
{
    if ( !ls || !( *ls ).memory )
    {
        return;
    }

    state_t* state = ( *ls ).memory;

    memory_clear ( ( *state ).ls
                 , sizeof ( node_t ) * ( *state ).max_entries
                 );

    ( *( ( *state ).head ) ).offs = 0;
    ( *( ( *state ).head ) ).size = ( *state ).cap;
    ( *( ( *state ).head ) ).next = 0;
}

// Expensive!
u64
freelist_query_free
(   freelist_t* ls
)
{
    if ( !ls || !( *ls ).memory )
    {
        return 0;
    }

    u64 sum = 0;
    state_t* state = ( *ls ).memory;
    node_t* node = ( *state ).head;

    while ( node )
    {
        sum += ( *node ).size;
        node = ( *node ).next;
    }

    return sum;
}

static
node_t*
freelist_get_node
(   freelist_t* ls
)
{
    state_t* state = ( *ls ).memory;
    
    for ( u64 i = 1; i < ( *state ).max_entries; ++i )
    {
        if ( ( *state ).ls[ i ].size == 0 )
        {
            ( *state ).ls[ i ].next = 0;
            ( *state ).ls[ i ].offs = 0;
            return &( *state ).ls[ i ];
        }
    }

    return 0;
}

static
void
freelist_return_node
(   node_t* node
)
{
    ( *node ).offs = 0;
    ( *node ).size = 0;
    ( *node ).next = 0;
}
