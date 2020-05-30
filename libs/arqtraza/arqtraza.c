/*
** ****************************************************************************
** Funciones de arquitectura de trazas.
** ****************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "arqtraza.h"

static char buffer[MAXMSG_LEN];
static int nivel = 40;


/*******************************************************************/
int NivelTrazaActual()
/*******************************************************************
Funcionalidad:
>> Retorna el nivel de traza actual.
*******************************************************************/
{
   return TrzNivel();
}




/*******************************************************************/
int TrzNivel()
/*******************************************************************
Funcionalidad:
>> Devuelve el nivel de traza.
*******************************************************************/
{
   return nivel;
}


/*******************************************************************/
void ARQLog(int Nivel, char *format, ...)
/*******************************************************************
Funcionalidad:
>> Determina si la traza, en base a su indicador de nivel, ha de ser
>> volcada y si es asi, escribe en el fichero de trazas (STDOUT) los 
>> componentes de la lista variable de argumentos, cada uno de ellos
>> con el formato indicado.
*******************************************************************/
{
   va_list arglist;
   int nivel_traza;
   time_t now = time(NULL);  // get system time

   /*
   ** Obtiene el nivel de traza actual.
   */
   nivel_traza = NivelTrazaActual();

   /*
   ** Si el nivel de traza es 0 sale sin hacer nada pues 0
   ** es el nivel minimo. Por tanto, no se volcaran trazas.
   */
   if(!nivel_traza)
      return;
   /*
   ** Si el indicador de nivel de la traza a volcar es superior
   ** al nivel de traza sale sin hacer nada pues dicha traza no
   ** ha  de ser volcada por ser de nivel superior. Es decir, el
   ** nivel de traza determina el maximo nivel de las trazas que
   ** seran volcadas y por tanto, solo se volcaran aquellas trazas
   ** cuyo indicador de nivel de traza sea menor o igual que el 
   ** nivel de traza actual.
   */
   if(Nivel > nivel_traza)
      return;

   va_start(arglist, format);

   fprintf(stdout, "\n\n{\n\"log_app\": \"supervisor\",\n");
   strcpy( buffer,  ctime(&now) );
   buffer[ strlen(buffer) - 1 ] = '\0';
   fprintf(stdout, "\"log_timestamp\": \"%s\",\n", buffer );

   if ( Nivel == BAS_TRACE )
	strcpy( buffer, "BAS_TRACE" );
   else 
	if ( Nivel == INF_TRACE )
	     strcpy( buffer, "INF_TRACE" );
   	else
 	     strcpy( buffer, "ALL_TRACE" );

   fprintf(stdout, "\"log_level\": \"%s\",\n", buffer);
   vsprintf(buffer, format, arglist); 
   fprintf(stdout, "\"log_message\": \"%s\"\n}", buffer);

   fflush(stdout);	
   va_end(arglist);

}


/*******************************************************************/
void ARQError(char *format, ...)
/*******************************************************************
Funcionalidad:
>> Volcado de trazas de error, siempre y cuando el nivel de traza no 
>> sea 0. Determina si la traza, en base al nivel de traza, ha de ser
>> volcada y si es asi, escribe en el fichero de trazas los componentes
>> de la lista variable de argumentos, cada uno de ellos con el formato
>> indicado.
*******************************************************************/
{
   va_list arglist;
   int nivel_traza;
   time_t now = time(NULL);  // get system time

   /*
   ** Obtiene el nivel de traza actual.
   */
   nivel_traza = NivelTrazaActual();

   /*
   ** Si el nivel de traza es 0 sale sin hacer nada pues 0
   ** es el nivel minimo. Por tanto, no se volcaran trazas.
   */
   if(!nivel_traza)
      return;

   va_start(arglist, format);

   fprintf(stderr, "\n\n{\n\"log_app\": \"supervisor\",\n");
   strcpy( buffer,  ctime(&now) );
   buffer[ strlen(buffer) - 1 ] = '\0';
   fprintf(stderr, "\"log_timestamp\": \"%s\",\n", buffer );
   fprintf(stderr, "\"log_level\": \"ERROR\",\n");
   vsprintf(buffer, format, arglist);
   fprintf(stderr, "\"log_message\": \"%s\"\n}", buffer);

   fflush(stderr);	
   va_end(arglist);
}

