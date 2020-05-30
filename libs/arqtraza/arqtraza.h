#ifndef ARQTRAZA_H
#define ARQTRAZA_H

#include <stdio.h>
#include <stdarg.h>

/*
** Funciones de arquitectura de trazas.
*/

/*
** Definicion maxima longitud para el nombre
** del fichero de trazas.
*/
#define MAXFILETRZ_LEN	256
#define MAXMSG_LEN	4096

/*
** Definicion de longitudes para construccion
** de la fecha en formato aammdd (anno con 2 dígitos)
*/
#define FECH_DIA_LEN	2
#define FECH_MES_LEN	2
#define FECH_ANO_LEN	2	

/*
** Definicion de los niveles de traza.
*/
#define NO_TRACE	0
#define ERR_TRACE	10
#define BAS_TRACE	20
#define INF_TRACE	30
#define ALL_TRACE	40


/*
** ********************************************************************
** ******************* Prototipo de Funciones *************************
** ********************************************************************
*/

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************/
void ARQLog(int Nivel, char *format, ...);
/*******************************************************************
Funcionalidad:
>> Determina si la traza, en base a su indicador de nivel, ha de ser
>> volcada y si es asi, escribe en el fichero de trazas los componentes
>> de la lista variable de argumentos, cada uno de ellos con el formato
>> indicado.
*******************************************************************/

/*******************************************************************/
void ARQError(char *format, ...);
/*******************************************************************
Funcionalidad:
>> Volcado de trazas de error, siempre y cuando el nivel de traza no 
>> sea 0. Determina si la traza, en base al nivel de traza, ha de ser
>> volcada y si es asi, escribe en el fichero de trazas los componentes
>> de la lista variable de argumentos, cada uno de ellos con el formato
>> indicado.
*******************************************************************/

#ifdef __cplusplus
}
#endif

#endif


