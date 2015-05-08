#include <stdlib.h>
#include <stdio.h>


#ifndef DB_HANDLER
#define DB_HANDLER
typedef struct {
    int msg;
    int *offer;
    int size_offer;
}Query;

void initialize();
Query db_handler (Query db_handler);

#endif
