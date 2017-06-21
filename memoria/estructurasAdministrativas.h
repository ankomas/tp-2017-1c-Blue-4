/*
 * estructurasAdministrativas.h
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */

#ifndef ESTRUCTURASADMINISTRATIVAS_H_
#define ESTRUCTURASADMINISTRATIVAS_H_

void* memoria, *cache;

void inicializarMemoria();
/*
 * 	Crea la memoria inicializada en pid = -2, y carga la tabla de paginas con pid = -1
 */

void inicializarCache();

uint32_t cargarAMemoria(uint32_t, void*, uint32_t);
/*
 * 	Carga en memoria un dato
 * 	No puedo probar si anda :/
 */

void mostrarDeMemoria(uint32_t);
/*
 * 	Muestra un marco
 * 	No anda bien :/
 */

/**
 * actualiza los marcos disponibles en mi sistema
 * @param marcos
 * @return codigo de exito o error.
 */
int actualizarMarcosDisponibles(int );


/**
 * Cant de marcos que requiere una cant de memoria
 * @Parametro: Tamaño
 * @Valor de Retorno: Entero
 */
int cuantosMarcosRepresenta(int);

/**
 * Funcion creada para inicializar la memoria
 */
void inicializarMemoria();

/**
 * mutex para las funciones que usan un array "procesosActivos";
 */
pthread_mutex_t mutex_procesosActivos;

/**
 * lista la data ( pid y paginasActuales) de un proceso
 * @param uint32_t pid
 * @return 0 (en caso de exito) | -1(en caso de error)
 */
uint32_t listar_DataDeProcesoActivo(uint32_t );


/**
 * Lista la data ( pid y paginasActuales) de  todos los procesos activos
 * @return 0 (en caso de exito) | -1(en caso de error)
 *
 */
uint32_t listar_DataDeTodosLosProcesosActivos();


/**
 *  Obtiene las paginas actuales de un proceso
 * @param pid
 * @return 0 (en caso de exito) | -1(en caso de error)
 */
uint32_t obtener_paginasActualesDeProcesoActivo(uint32_t );


/**
 * Aumenta la cantidad de paginas a un proceso activo
 * @param pid
 * @param paginasAActualizar
 * @return 0 (en caso de exito) | -1(en caso de error)
 */
uint32_t aumentar_PaginasActualesDeProcesoActivo(uint32_t ,uint32_t );


/**
 * Elimina la data (pid y paginas actuales ) de un proceso
 * @param pid
 * @return 0 (en caso de exito) | -1(en caso de error)
 */
uint32_t eliminar_DataDeProcesoActivo(uint32_t );


/**
 * Agrega la data (pid y paginas actuales ) de un proceso
 * @param pid
 * @param paginasActuales
 * @return 0 (en caso de exito) | -1(en caso de error)
 */
uint32_t agregar_DataDeProcesoActivo(uint32_t ,uint32_t );

/**
 * Disminuye las paginas actuales de un proceso activo
 * @param uint32_t pid
 * @param uint32_t pagina que se desea disminuir
 * @return 0 (en caso de exito) | -1(en caso de error)
 */
uint32_t disminuir_PaginasActualesDeProcesoActivo(uint32_t ,uint32_t );


/**
 * Obtiene la pagina de inicio de un proceso activo
 * @param uint32_t pid
 * @return 0 (en caso de exito) | -1(en caso de error)
 */
uint32_t obtener_PaginaDeInicioDeProcesoActivo(uint32_t );

/**
 * Obtiene la proxima pagina a asignar
 * @param uint32_t pid
 * @return 0 (en caso de exito) | -1(en caso de error)
 */
uint32_t obtener_ProximaPaginaAAsignar(uint32_t );

#endif /* ESTRUCTURASADMINISTRATIVAS_H_ */
