CREATE DATABASE dbDatabase;
CREATE USER dbUser;
ALTER USER dbUser IDENTIFIED BY 'dbPassword';
GRANT ALL PRIVILEGES ON dbDatabase.* To dbUser;
USE dbDatabase;
CREATE TABLE audit( action VARCHAR(20), dt datetime NOT NULL DEFAULT CURRENT_TIMESTAMP PRIMARY KEY );
CREATE TABLE webresponse( action VARCHAR(20) PRIMARY KEY, message VARCHAR(200) );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion1', 'estado activo' );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion2', 'MANTENIMIENTO' );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion3', 'operativa especial no.3' );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion4', 'operativa especial no.4' );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion5', 'operativa especial no.5' );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion6', 'operativa especial no.6' );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion7', 'operativa especial no.7' );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion8', 'operativa especial no.8' );
INSERT INTO  webresponse( action, message ) VALUES ( 'accion9', 'operativa especial no.9' );
COMMIT;
