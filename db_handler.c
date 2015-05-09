#include "db_handler.h"



int db[10][4];
int available;

/* Funcion que inicializa los puestos de los vagones
 * initialize () -> void;
 */ 
void initialize() {
    available = 40;
    for (int i=0;i<10;i++) 
        for (int j=0;j<40;j++)
            db[i][j] = 0;
}


/* Funcion que administra la reserva de Vagones 
 * db_hanlder  (1 [xx yy] ) -> accepto: 1 NULL 
 * | Ocupado: 0 {xx yy yy yy | xx yy | xx yy ..} 
 * | Full: -1 NULL
 */

Query db_handler(Query q){
    Query respond;

    if (available == 0) {                                           // El vagon esta full

        respond.msg         = -1;
        respond.offer       = NULL;
        respond.size_offer  = 0;

    }
    else if(db[q.offer[0]][q.offer[1]] == 0) {                      // La reserva se realizo exitosamente

      db[q.offer[0]][q.offer[1]]    = 1;
      respond.msg                   = 1;
      respond.offer                 = NULL;
      respond.size_offer            = 0;
      available                    -= 1;

    }
    else {                                                          // El puesto esta ocupado, se devuelve el mapa del vagon
        int *aux = (int *)malloc(sizeof(int)*available);
        int k    = 0;

        for (int i = 0; i<10; i++)
            for (int j=0; j<4; j++)
                if (db[i][j] == 0) {
                    aux[k]  = (i*10) + j;
                    k      += 1;
                }

        respond.msg         = 0;
        respond.offer       = aux;
        respond.size_offer  = k;

    }
    
    return respond;
    
}
