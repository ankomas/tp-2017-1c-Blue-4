AnSISOP_funciones functions = {
		.AnSISOP_definirVariable		= dummy_definirVariable,
		.AnSISOP_obtenerPosicionVariable= dummy_obtenerPosicionVariable,
		.AnSISOP_finalizar 				= dummy_finalizar,
		.AnSISOP_dereferenciar			= dummy_dereferenciar,
		.AnSISOP_asignar				= dummy_asignar,

};
AnSISOP_kernel kernel_functions = { };

t_pcb pcb;

t_puntero dummy_definirVariable(t_nombre_variable variable){
	printf("definir la variable %c\n", variable);
	
	return nuevaVar(pcb,variable);
}

t_puntero dummy_obtenerPosicionVariable(t_nombre_variable variable) {
	printf("Obtener posicion de %c\n", variable);
	t_puntero direccion=**memoria dame posicion**
	return POSICION_MEMORIA;
}

void dummy_finalizar(void){
	termino = true;
	printf("Finalizar\n");
}

bool terminoElPrograma(void){
	return termino;
}

t_valor_variable dummy_dereferenciar(t_puntero puntero) {
	printf("Dereferenciar %d y su valor es: %d\n", puntero, CONTENIDO_VARIABLE);
	return CONTENIDO_VARIABLE;
}

void dummy_asignar(t_puntero puntero, t_valor_variable variable) {
	printf("Asignando en %d el valor %d\n", puntero, variable);
}
