#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <errno.h>
#include "mysql.h"

#include "tcputil.h"
#include "arqtraza.h"
#include "supervisor.h"


/*
** Array de sockets encargados de atender
** las peticiones desde clientes tcp. 
*/
sockinfo_t sockets[MAX_OPENED_SOCKETS];

/*
** Para terminar el bucle de SocketsEventLoop.
*/
int stop_loop = 0;

static configuration config;

char buffer[MAX_BUFFER];
char actions[MAX_ACTIONS][MAX_BUFFER];

MYSQL *MySQLConnection;
MYSQL *MySQLConRet;


void main(int argc, char *argv[])
{
   int  highest_sd = 0;
   int  i, p, ret, sd_aux, contcfg;   
   struct sigaction act,oact;

   long interval;
   char strport[MAXPORT_LEN + 1];
   char *strinterval;


   // Capturar la sennal de interrupcion
   ARQLog(INF_TRACE, "%s::Capturando senial de interrupcion.", argv[0]);
   signal(SIGTERM, CleanUp);
   signal(SIGPIPE, SIG_IGN);
   
   // Leer configuracion (ahora mediante variables de entorno)
   if (readConfig())
   {
      ARQError("readConfig::error captura parametros por variables de entorno.");
      return;
   }
   
   /* leer Interval */
   interval = config.SupervisorInterval;   	
   ARQLog(INF_TRACE, "Intervalo del supervisor definido con valor %d.", interval);
   ARQLog(INF_TRACE, "Version supervisor: %s", SUPERVISOR_VERSION);
   ARQLog(INF_TRACE, "Puerto de escucha: %d.", config.ListenPort);

   /* cnx database */
   MySQLConnection = mysql_init( NULL );
   MySQLConRet = mysql_real_connect( MySQLConnection,
                                     config.dbHost,
                                     config.dbUser,
                                     config.dbPassword,
                                     config.dbDatabase,
                                     0,
                                     NULL,
                                     0 );

   if ( MySQLConRet == NULL )
   {
        ARQError("Error in MySQL connection [%s]", (char *) mysql_error(MySQLConnection) );
        return;
   }
   else
   {
        ARQLog(INF_TRACE, "MySQL connection is OK. Client: %s", mysql_get_client_info() );
   }


   /* llenar tabla bbdd */
   if (readDatabase())
   {
      ARQError("readDatabase::error BBDD actions.");
      return;
   }

   sprintf(strport, "%.*d", MAXPORT_LEN, config.ListenPort);

   // Limpia el array de sockets inicializandolos a 0.
   bzero(sockets, MAX_OPENED_SOCKETS*sizeof(sockinfo_t));
   
   // Nos preparamos para escuchar peticiones de clientes.
   if(!TCPSetupListenSocket(&sd_aux, strport))
   {
      ARQError("%s::Error en llamada a  TCPSetupListenSocket.", argv[0]);
      CleanUp(SIGINT);
      exit(EXIT_FAILURE);
   }

   /*
   ** Funcion que registra un socket asignandole una accion. Es decir cuando
   ** a dicho socket llegue un mensaje se ejecutara la accion que tiene
   ** asignada.
   */
   RegisterSocket(sd_aux, &highest_sd, ACCEPT_CALL);
   ARQLog(BAS_TRACE, "Socket de escucha para clientes establecido: ls = %d, high_sd = %d.",sd_aux, highest_sd);
   
   /* 
   ** Esperar eventos, en los sockets creados, y resolverlos.
   */
   ret = SocketsEventLoop(highest_sd, &interval);

   /*
   ** En un momento dado, el bucle de eventos finalizara devolviendo
   ** un determinado codigo de retorno que sera chequeado.
   */
   switch(ret) 
   {
      case EVENT_LOOP_ERROR:
		ARQError("%s::Error interno del bucle de eventos.", argv[0]);
		break;
	 
      case EVENT_LOOP_STOPPED:
		ARQLog(ALL_TRACE, "%s::Bucle de eventos finalizado con normalidad...", argv[0]);
		break;
	 
      case EXEC_CANNOT_RECEIVE:
		ARQError("%s::Error al recibir un mensaje desde un cliente.", argv[0]);
		break;
	 
      case EXEC_CANNOT_SEND:
		ARQError("%s::Error al mandar un mensaje desde un cliente.", argv[0]);
		break;
	 
      case EXEC_CANNOT_ACCEPT:
		ARQError("%s::Error al aceptar una llamada desde un cliente.", argv[0]);
		break;
	 
      case EXEC_UNKNOWN_ACTION:
		ARQError("%s::Error por accion desconocida.", argv[0]);
		break;
   }
  

   // close database connection
   mysql_close(MySQLConnection);      

   // close open sockets 
   for(i = 0; i<MAX_OPENED_SOCKETS; i++) 
      if(sockets[i].opened) 
         TCPDisconnectSocket(sockets[i].sd);
}



int readConfig(void)
{	     
	char* pEnvVar;
	int iErr = FALSE;

	pEnvVar = getenv ("SUPERVISOR_LISTENPORT");
	if (pEnvVar == NULL)
	{
		ARQError("Variable de entorno SUPERVISOR_LISTENPORT no fijada");
		iErr = TRUE;
	}
	else
	{
		if (!str2int(&config.ListenPort, pEnvVar, 10) == STR2INT_SUCCESS)
	   	{	
			ARQError("Variable de entorno SUPERVISOR_LISTENPORT no tiene un valor valido");
			iErr = TRUE;
		}
	}

	pEnvVar = getenv ("SUPERVISOR_INTERVAL");
	if (pEnvVar == NULL)
	{
		ARQError("Variable de entorno SUPERVISOR_INTERVAL no fijada");
		iErr = TRUE;
	}
	else
	{
		if (!str2int(&config.SupervisorInterval, pEnvVar, 10) == STR2INT_SUCCESS)
	   	{	
			ARQError("Variable de entorno SUPERVISOR_INTERVAL no tiene un valor valido");
			iErr = TRUE;
		}
	}

	pEnvVar = getenv ("SUPERVISOR_DBUSER");
	if (pEnvVar == NULL)
	{
		ARQError("Variable de entorno SUPERVISOR_DBUSER no fijada");
		iErr = TRUE;
	}
	else
		strcpy(config.dbUser, pEnvVar);


	pEnvVar = getenv ("SUPERVISOR_DBPASS");
	if (pEnvVar == NULL)
	{
		ARQError("Variable de entorno SUPERVISOR_DBPASS no fijada");
		iErr = TRUE;
	}
	else
		strcpy(config.dbPassword, pEnvVar);


	pEnvVar = getenv ("SUPERVISOR_DBHOST");
	if (pEnvVar == NULL)
	{
		ARQError("Variable de entorno SUPERVISOR_DBPHOST no fijada");
		iErr = TRUE;
	}
	else
		strcpy(config.dbHost, pEnvVar);


	pEnvVar = getenv ("SUPERVISOR_DBPORT");
	if (pEnvVar == NULL)
	{
		ARQError("Variable de entorno SUPERVISOR_DBPORT no fijada");
		iErr = TRUE;
	}
	else
	{
		if (!str2int(&config.dbPort, pEnvVar, 10) == STR2INT_SUCCESS)
	   	{	
			ARQError("Variable de entorno SUPERVISOR_DBPORT no tiene un valor valido");
			iErr = TRUE;
		}
	}


	pEnvVar = getenv ("SUPERVISOR_DBNAME");
	if (pEnvVar == NULL)
	{
		ARQError("Variable de entorno SUPERVISOR_DBNAME no fijada");
		iErr = TRUE;
	}
	else
		strcpy(config.dbDatabase, pEnvVar);

	return iErr;
}


error_t SocketsEventLoop(int highest_sd, long *interval)
{
   int nfound, exec, i;
   fd_set readmask;
   struct timeval timeout;
   struct timespec spec;  /* tiempo con centesimas de seg */
   time_t tInicioSeg, tActualSeg;

   timeout.tv_sec = *interval;
   /* timeout del select */
   timeout.tv_usec= 0;
   
   /* tomo tiempo */   
   clock_gettime(CLOCK_REALTIME, &spec);
	 tInicioSeg = spec.tv_sec;
   
   while(!stop_loop)
   {
      clock_gettime(CLOCK_REALTIME, &spec);
      tActualSeg = spec.tv_sec;
	  
      /* Actualizamos la mascara de lectura para el select() */
      FD_ZERO(&readmask);
      for(i = 0; i<MAX_OPENED_SOCKETS; i++)
         if(sockets[i].opened)
	    FD_SET(sockets[i].sd, &readmask);
	    
      /*
      ** El bucle permanece bloqueado hasta que llega algun
      ** mensaje a alguno de los sockets, bien de los sockets
      ** abiertos con clientes.
      */

      // ARQLog(INF_TRACE, "SocketsEventLoop::Bloqueo en select, en espera de entrada de mensajes.");
			
      /* 26-nov select con timeout pequeño (y fijo) */
      if((nfound = select(highest_sd+1, &readmask, 0, 0, &timeout)) == -1)
      {
	ARQError("SocketsEventLoop::Error en operacion select(%d - %s).", errno, strerror(errno));
	if (errno != EINTR)
		return EVENT_LOOP_ERROR;
      }
	  
      // ARQLog(INF_TRACE, "SocketsEventLoop::Desbloqueo del select.");

      /*
      ** Si se llega a este punto es p.q. se ha recibido algun
      ** mensaje en alguno de los sockets. Los sockets a los
      ** que han llegado mensajes seran aquellos que cumple lo
      ** siguiente:
      ** sockets[i].opened && FD_ISSET(sockets[i].sd, &readmask).
      ** (esta abierto y ha llegado algun mensaje).
      ** Para aquellos socket a los que ha llegado algun mensaje
      ** se lanzara la accion que tienen asignada.
      */
      /*
      ** Si se decide recorrer la tabla de sockets en orden inverso
      ** para asi asegurar que primero se detectan los mensajes 
      ** procedentes de la aplicacion, el bucle sera como sigue:
      **    for(i = MAX_OPENED_SOCKETS-1; i> = 0; i--)
      ** En caso contrario:
      **    for(i = 0; i<MAX_OPENED_SOCKETS; i++)
      */
      for(i = MAX_OPENED_SOCKETS-1; i >= 0; i--)
	if(sockets[i].opened && FD_ISSET(sockets[i].sd, &readmask))
	{
		ARQLog(ALL_TRACE, "SocketsEventLoop::Ejecutando accion %d sobre el socket %d.",
		sockets[i].action, sockets[i].sd);
		exec = ExecuteAction(sockets[i].sd, sockets[i].action, &highest_sd);
		if(exec != EXEC_OK)
		   return exec;
	}
   } /* while(!stop_loop)*/
   
   return EVENT_LOOP_STOPPED;
}


/*****************************************************************************/
error_t ExecuteAction(int sd, action_t action, int *highest_sd)
/*****************************************************************************
Funcionalidad:
>> Funcion que ejecuta una determinada accion sobre un socket.
*****************************************************************************/
{
   int new_sd;
   int indice;
   char *msg = NULL;
   char salida[MAX_BUFFER];
   char salidaAux[MAX_BUFFER];
  
 
   memset( salida, 0, sizeof(salida) );

   switch(action)
   {
	case ACCEPT_CALL:
		ARQLog(ALL_TRACE, "ExecuteAction::Aceptando llamada desde el socket %d.", sd);
		if(!TCPServerAcceptCall(sd, &new_sd))
			return EXEC_CANNOT_ACCEPT;

		ARQLog(ALL_TRACE, "ExecuteAction::Llamada aceptada en socket %d.", new_sd);
		ARQLog(ALL_TRACE, "ExecuteAction::Registrando socket %d con accion ATTEND_CLIENT.", new_sd);
		RegisterSocket(new_sd, highest_sd, ATTEND_CLIENT);
		break;

	case ATTEND_CLIENT:
		ARQLog(ALL_TRACE, "ExecuteAction::Atendiendo a cliente TCP desde el socket %d.", sd);
		ARQLog(ALL_TRACE, "ExecuteAction::Recibiendo cabecera.");
		msg = (char*)malloc(LEN_MSG); 

		switch(TCPReciveMsg(sd, (char*)msg, LEN_MSG))	
		{
			case TCP_OOB:
				UnRegisterSocket(sd, highest_sd);
				ARQLog(ALL_TRACE, "ExecuteAction::Detectado cierre de socket <%d> origen.", sd);
				return EXEC_OK;

			case TCP_ERROR:
			case TCP_INTR:
				return EXEC_CANNOT_RECEIVE;

			case TCP_OK:
				if( strstr(msg,"GET /?accion") )
				{
				    indice = atoi( substr(msg, 12, 1));
                                    indice--;
				    ARQLog(ALL_TRACE, "ExecuteAction::Detectado actions[%d]", indice);
                                    strcpy( salidaAux, actions[indice] );
				    InsertAudit( substr(msg, 12, 1) );  // register in audit table 
				}
				else
				{
				    ARQError("ExecuteAction::Peticion consulta desconocida.");			
				    strcpy( salidaAux, "Accion no parametrizada." );	
				}
                                sprintf( salida, "%s%s%s", XML_HEADER, salidaAux, XML_TAIL );
		}

		if(!TCPSendMsg(sd, (char *)salida, strlen((char *)salida)))
		{
		    ARQError("ExecuteAction::Error en envio de mensaje.");
		    return 1;
		}						

		free(msg);	
		UnRegisterSocket(sd, highest_sd);
		break;
	 
	default:
		ARQLog(INF_TRACE, "ExecuteAction::Saliendo por accion desconocida <%d>.", action);
		return EXEC_UNKNOWN_ACTION;
   } 
   
   return EXEC_OK;
} 



/*****************************************************************************/
void RegisterSocket(int sd, int *highest_sd, action_t action)
/*****************************************************************************
Funcionalidad:
>> Funcion que registra un socket asignandole una accion. Es decir cuando
>> a dicho socket llegue un mensaje se ejecutara la accion que tiene
>> asignada.
*****************************************************************************/
{
   /*ARQLog(INF_TRACE, "RegisterSocket::Registrando socket <%d> con accion <%d>.", sd, action);*/
   sockets[sd].opened = 1;
   sockets[sd].sd = sd;
   sockets[sd].action = action;
   
   if(sd > *highest_sd)
      *highest_sd = sd;
   
} /* RegisterSocket */


/*****************************************************************************/
void UnRegisterSocket(int sd, int *highest_sd)
/*****************************************************************************
Funcionalidad:
>> Funcion que desactiva un socket no asignandole accion alguna. 
*****************************************************************************/
{
   int is;   /* index socket */
   
   /*ARQLog(INF_TRACE, "UnRegisterSocket::Desregistrando socket <%d>.", sd);*/
   sockets[sd].opened = 0;
   sockets[sd].sd = -1;
   sockets[sd].action = -1;
   
   /*
   ** Cerramos el socket.
   ** close(sd);
   */
   TCPDisconnectSocket(sd);
   
   /*
   ** Si el socket que cerramos es el highest_sd, tenemos que decrementar
   ** hasta que encontremos uno abierto y no hayamos llegado al final de
   ** la tabla 'sockets'.
   */
   
   if(sd == *highest_sd)
   {
      is = *highest_sd - 1;
      while( (!sockets[is].opened)&&(is) )
	is--;

      *highest_sd = is;
   }
} /* UnRegisterSocket */



/*****************************************************************************/
void CleanUp(int sig)
/*****************************************************************************/
{
   ARQLog(ALL_TRACE, "CleanUp::Iniciando proceso de parada del bucle de eventos.");
   stop_loop = 1;
   kill(getpid(), SIGTERM);
} 


str2int_errno str2int(int *out, char *s, int base) 
{
    char *end;

    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}


char *substr(char *s, int a, int b) 
{
    char *r = (char*)malloc(b);
    int m=0, n=0;

    strcpy(r, "");
    while(s[n]!='\0')
    {
        if ( n>=a && m<b ){
            r[m] = s[n];
            m++;
        }
        n++;
    }
    r[m]='\0';

    return r;
}


int readDatabase(void)
{
    MYSQL_ROW row;
    MYSQL_RES *mysqlResult;
    int mysqlStatus = 0;
    int fila = 0;


    mysqlStatus = mysql_query( MySQLConnection, "SELECT action,message FROM webresponse ORDER by action" );

    if (mysqlStatus)
    {
       ARQError("%s\n", mysql_error(MySQLConnection));
       return 1;
    }

    mysqlResult = mysql_store_result(MySQLConnection);
  
    while ((row = mysql_fetch_row(mysqlResult))) 
    { 
        strcpy( actions[fila], row[1] ); 
  	fila++;
    }
  
    mysql_free_result(mysqlResult);
    return 0;
}


void InsertAudit( char *str)
{
    char QueryString[MAX_BUFFER];

    strcpy(QueryString, "INSERT INTO audit (action) VALUES('");
    strcat(QueryString, str);
    strcat(QueryString, "')");
    if( mysql_query(MySQLConnection, QueryString) )
        ARQError("%s\n", mysql_error(MySQLConnection));
}

