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


#Instrucciones para ejecurla en Kubernetes
------------------------------------------
Desde el directorio k8s ejecutar los siguientes comandos:

cd k8s

sudo kubectl apply -f app-service.yaml

sudo kubectl apply -f database-service.yaml

sudo kubectl apply -f app-deployment.yaml

sudo kubectl apply -f variables-env-configmap.yaml

sudo kubectl apply -f database-deployment.yaml

sudo kubectl apply -f database-claim0-persistentvolumeclaim.yaml



#Instrucciones para hacer uso de secretos para los datos sensibles
..................................................................
El único dado sensible es la contraseña de usuario de base de datos que se encuentra almacenada
en claro en la variable de entorno SUPERVISOR_DBPASS
Para ocultar esta informacion codificaría el contenido de esta variable en Base64:

echo -n 'dbPassword' | base64

ZGJQYXNzd29yZA==

y ese valor sería el que asignaría al contenido siguiente (fichero: secret.yaml)

apiVersion: v1
kind: Secret
metadata:
  name: mysecret
type: Opaque
data:
  password: ZGJQYXNzd29yZA==

Después crear el secreto ----> sudo kubectl apply -f ./mysecret.yaml
Y modificar la definicion del contenedor de la aplicacion (app-deployment.yaml) para que tome el valor de la variable de entorno SUPERVISOR_DBPASS mediante la instruccion: "env[].valueFrom.secretKeyRef."

Modificando el fichero de definición del contenedor de la aplicación de la siguiente manera:

  containers:
  - name: app
    env:
      - name: SUPERVISOR_DBPASS
        valueFrom:
          secretKeyRef:
            name: mysecret
            key: password
