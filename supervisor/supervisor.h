#include <unistd.h> 
#include <ctype.h>
#include <errno.h>
#include <limits.h>


#define SUPERVISOR_VERSION	"1.21" 	/* version 2020-05-13 */

#define LEN_MSG		149+3  /* 2-oct-2013, 3 caracteres mas para que quepa historico */
#define MAX_PATH	256
#define MAX_PID_FILE	250
#define MAX_BUFFER	2000
#define MAX_ACTIONS	9	
#define MAX_FECHAX	8	/* fecha en formato AAAAMMDD para XML*/
#define MAXBUF 		4096   /* buffer de respuesta a historicos*/

#define INTERVALOHISTORICO 	 0		/*intervalo definido para supervisor de historico*/
#define MAX_INTERVAL  		32000		/*maximo valor para supervisorInterval*/
#define DEFAULT_SELECT_TIMEOUT  2		/* tiempo timeout selecting sobre sockets de lectura */

#define CONFIG_FILE	"supervisor.cfg"

#define XML_HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<info>"
#define XML_TAIL   "</info>\n"

#define MAX_OPENED_SOCKETS 24

typedef enum 
{
   ACCEPT_CALL,
   ATTEND_CLIENT
} action_t;

typedef enum 
{
   EVENT_LOOP_ERROR,
   EVENT_LOOP_STOPPED,
   EXEC_CANNOT_RECEIVE,
   EXEC_CANNOT_SEND,
   EXEC_CANNOT_ACCEPT,
   EXEC_ERROR,
   EXEC_UNKNOWN_ACTION,
   EXEC_OK
} error_t;

typedef struct 
{
   int opened;
   int sd;
   int action;
} sockinfo_t;

typedef struct
{
   int  ListenPort;  // range short 
   int  SupervisorInterval;
   char PidFile[MAX_BUFFER];
   int  TraceLevel;
   char dbUser[MAX_BUFFER];
   char dbPassword[MAX_BUFFER];
   char dbHost[MAX_BUFFER];
   int  dbPort;     // range short 
   char dbDatabase[MAX_BUFFER];
} configuration;


#define exists(filename) (!access(filename, F_OK)) /* comprobar si existe un fichero */

/*
** Def de valores logicos.
*/
#define FALSE   0
#define TRUE    1
#define MAXPORT_LEN	5

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;




/*
** ******************* PROTOTIPOS DE FUNCIONES *********************************
*/

str2int_errno str2int(int *out, char *s, int base);
static int handler(void* user, const char* section, const char* name, const char* value);
static int handler_traza(void* user, const char* section, const char* name, const char* value);
int readConfig(void);
error_t SocketsEventLoop(int highest_sd, long *interval);
error_t ExecuteAction(int sd, action_t action, int *highest_sd);
void RegisterSocket(int sd, int *highest_sd, action_t action);
void UnRegisterSocket(int sd, int *highest_sd);
void CleanUp(int sig);
int readDatabase(void);
char *substr(char *s, int a, int b);
void InsertAudit( char * );
