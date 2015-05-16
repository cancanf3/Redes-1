/*		reseva.x			*/

struct puesto {
	int fila;
	int columna;
};

program RESERVA_PROG {
	version RESERVA_VERS {
		int RESERVAR(puesto)=1;
		string MOSTRAR_DISPONIBLES(void)=2;
	}=1;
}= 0x31111111;
