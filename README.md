#Práctica modulo contenedores (KeepCoding school)
=================================================
Práctica final del módulo denominado "contenedores" del 2º Bootcamp DevOps de Keepcoding 


#Descripcion de la aplicacion
-----------------------------
Se trata de un servidor de sockets codificado en C que sirve peticiones TCP (HTTP implementado parcialmente) 
por un puerto configurable mediante variable de entorno SUPERVISOR_LISTENPORT.
En función del parámetro web recibido configura una respuesta (XML) extraida de una tabla (WEBRESPONSE) de una 
BB.DD MySQL a la que se conecta mediante los parámetros definidos en las variables de entorno:
SUPERVISOR_DBUSER
SUPERVISOR_DBPASS
SUPERVISOR_DBHOST
SUPERVISOR_DBPORT
SUPERVISOR_DBNAME

La aplicacion toma en cuenta mas variables de entorno para configurar el intervalo de la SELECT (de sockets) 
a través de SUPERVISOR_INTERVAL y el nivel de trazado (formato json) a mostrar segun valor numerico (mayor 
valor --> mas trazas) en SUPERVISOR_NIVELTRAZA.

Las peticiones (validas) registradas en la tabla webresponse son registradas en la tabla audit junto con una
marca de tiempo (timestamp).

Esta aplicación se compila al vuelo durante el despliegue del contenedor mediante el uso de un makefile que no
tiene dependencias con la version de la librería cliente de MySQL.


#Instrucciones para ejecutarla en local
---------------------------------------
Para ejecutar esta aplicación es necesario tener instalada la última versión de Docker y Minicube.

La forma más rápida de ponerla en funcionamiento es utilizar docker-compose en la siguiente secuencia de 
comandos siendo 'myapp' el directorio donde se ha descargado este repositorio de github

   cd $HOME/myapp
   sudo docker-compose up -d --build


#Verficación de su funcionalidad
-------------------------------------------------
Una vez levantados los contenedores de aplicacion (kcapp_c) y de base de datos (kcdb_c), verificar que levantan bien mediante los comandos: 

sudo docker-compose ps
sudo docker-compose logs

sudo docker network inspect myapp_lognet 
---> de aquí tomar la IP del contenedor kcapp_c (IPv4Address) p.e 172.19.0.3

curl http://172.19.0.3:8080/?misc
---> mensaje para acción no parametrizada

curl http://172.19.0.3:8080/?accion1
-->  <?xml version="1.0" encoding="UTF-8"?>
     <info>estado activo</info>

curl http://172.19.0.3:8080/?accion2
-->  <?xml version="1.0" encoding="UTF-8"?>
     <info>MANTENIMIENTO</info>


#Instrucciones para ejecurla en Minicube
----------------------------------------
En curso
