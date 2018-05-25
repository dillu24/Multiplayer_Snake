//
// Created by dillu24 on 14/04/2017.
//
/**
 * Connection header file along with all the necessary structures
 */

#ifndef SYSTEMS_CONNECTION_H //used against multiple definition
#define SYSTEMS_CONNECTION_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include "game.h"
/**
 * Server connection struct i.e all the necessary TCP/IP connection details and variables that the server requires ,
 * this struct was created in order to not let many variables running around in the server
 */
typedef struct server_connection{
    int sockfd,newsockfd,portno,clilen;
    struct sockaddr_in serv_addr,cli_addr;
}server_connection;

/**
 * The server connection struct i.e. alll the necessary TCP/IP connection details and variables that the client requires,
 * this struct was created in order to not let many variables running around in the client
 */
typedef struct client_connection{
    int sockfd,portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
}client_connection;

/**
 * This method creates a TCP/IP socket for the server
 * @param cn
 * The connection struct created in the server to be passed to this method to store the necessary values.
 * @return -1 on error and 0 on success
 */
int create_server_socket(server_connection *cn);
/**
 * This method binds the host address to the TCP/IP socket
 * @param
 * cn The connection struct created in the server to be passed to this method to store the necessary values.
 * @return -1 on error and 0 on success
 */
int bind_host_address(server_connection *cn);
/**
 * The method that accepts a given TCP/IP connection and sets all values to the variables as required
 * @param cn
 * The connection struct created in the server to be passed to this method to store the necessary values.
 * @return -1 on error and 0 on success
 */
int accept_connection_from_client(server_connection *cn);
/**
 * The method that closes a TCP/IP connection from the server end , note that this closure only effects one connection
 * from the socket because as it could be seen from the server.c file an array of newsockfd's will be created to
 * represent different connections , and hence the only connection that will be closed here is the most recent connection
 * , the other connections should be closed in the server.c file.
 * @param cn
 * The connection struct created in the server to be passed to this method to store the necessary values
 * @return -1 on error and 0 on success
 */
int close_connection_from_server (server_connection *cn);
/**
 * The method that creates the client TCP/IP socket point
 * @param cn
 * THe connection struct created in the client to be passed to this method to store the necessary values
 * @return -1 on error 0 on success
 */
int client_create_socket_point(client_connection *cn);
/**
 * The method that connects to the server and hence create a TCP/IP connection
 * @param cn
 * The connection struct created in the client to be passed to this method to store the necessary values
 * @return -1 on error 0 on success
 */
int connect_to_server(client_connection *cn);
/**
 * The method that closes the client's end of the TCP/IP connection
 * @param cn
 * The connection struct created in the client to be passed to this method to store the necessary values
 * @return
 * -1 on error 0 on success
 */
int close_connection_from_client(client_connection *cn);
/**
 * This method is used to convert an integer into a series of bytes in order to be sent over tcp/ip connection , this
 * was done because structs cannot be sent as a whole over connections hence they must be serialized at the sender
 * and deserialized at the receiver
 * @param buffer
 * The placeholder where the bytes will be placed
 * @param value
 * The integer value that is going to be converted
 * @return
 * The next free slot in the buffer in order to send more data.
 */
unsigned char * serialize_int(unsigned char *buffer, int value);
/**
 * This method is used to convert a series of bytes into an integer so that the sent data in bytes over the TCP/IP connection
 * could be converted back to the original integer value , this was done because structs cannot be sent as a whole over
 * connections and hence they must be deserialized and serialized at the receiver and server respectively
 * @param buffer
 * THe placeholder where the received bytes are placed
 * @param value
 * THe integer variable where the new converted value will be placed
 * @return
 * The next slot in the buffer that was not deserialized
 */
unsigned char * deserialize_int(unsigned char *buffer, int *value);
/**
 * This method is used to convert a sending_client_info struct into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * THe placeholder where the bytes will be placed
 * @param value
 * The struct value that will be converted;
 * @return
 * THe next free slot in the buffer in order to send more data
 */
unsigned char * serialize_sending_connection(unsigned char *buffer, struct sending_client_info *value);
/**
 * This method is used to convert a series of bytes into the original sent sending_client_info struct over TCP/IP
 * connection , this was done because structs cannot be sent as a whole over connections and hence they must be
 * deserialized and serialized at the receiver and server respectively
 * @param buffer
 * The placeholder where the received bytes are placed
 * @param value
 * The sending_client_info struct where the new converted struct will be placed
 * @return
 * The next slot in the buffer that was not deserialized
 */
unsigned char * deserialize_sending_connection(unsigned char *buffer, struct sending_client_info *value);
/**
 * This method is used to convert a character value  into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * THe placeholder where the bytes will be placed
 * @param value  The character value that will be converted
 * @return
 * The next free slot in the buffer in order to send more data
 */
unsigned char * serialize_char(unsigned char *buffer, char value);
/**
 * This method is used to convert a series of bytes into the original sent character data over TCP/IP connection , this
 * was done because structs cannot be sent as a whole over connections and hence they must be deserialized and serialized
 * at the receiver and server respectively
 * @param buffer
 * The placeholder where the received bytes are placed
 * @param value
 * The character variable where the new converted character will be placed
 * @return
 * The next slot in the buffer that was not deserialized
 */
unsigned char * deserialize_char(unsigned char *buffer, char *value);
/**
 * This method is used to convert a short value  into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * The placeholder where the bytes will be placed
 * @param value
 * The short value that will be converted
 * @return
 * The next free slot in the buffer in order to send more data
 */
unsigned char * serialize_short(unsigned char *buffer, short value);
/**
 * This method is used to convert a struct element value  into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * The placeholder where the bytes will be placed
 * @param value
 * The struct that will be converted
 * @return
 * The next free slot in the buffer in order to send more data
 */
unsigned char* serialize_element(unsigned char *buffer,element *value);
/**
 * This method is used to convert a struct snakeobject value  into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * The placeholder where the bytes will be placed
 * @param snake
 * The struct that will be converted
 * @return
 * The next free slot in the buffer in order to send more data
 */
unsigned char* serialize_snakeobject(unsigned char*buffer,snakeobject *snake);
/**
 * This method is used to convert a series of bytes into the original sent snakeobject struct over TCP/IP connection , this
 * was done because structs cannot be sent as a whole over connections and hence they must be deserialized and serialized
 * at the receiver and server respectively
 * @param buffer
 * The placeholder where the received bytes are placed
 * @param snake
 * The snakeobject variable where the new converted snakeobject will be placed
 * @return
 * The next slot in the buffer that is still not deserialized
 */
unsigned char* deserialize_snakeobject(unsigned char* buffer,snakeobject *snake);
/**
 * This method is used to convert a struct element array into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * The placeholder where the bytes will be placed
 * @param array
 * The array that will be converted
 * @param size
 * The size of the array that will be converted
 * @return The next free slot in the buffer in order to send more data
 */
unsigned char* serialize_element_array(unsigned char* buffer, element *array,int size);
/**
 * This method is used to convert a series of bytes into the original sent struct element array over TCP/IP connection , this
 * was done because structs cannot be sent as a whole over connections and hence they must be deserialized and serialized
 * at the receiver and server respectively
 * @param buffer
 * The placeholder where the received bytes are placed
 * @param array
 * The element array where the new converted element array will be placed
 * @param size
 * The size of the converted element array
 * @return
 * The next slot in the buffer that is still not deserialized
 */
unsigned char *deserialize_element_array(unsigned char* buffer, element *array,int size);
/**
 * This method is used to convert a series of bytes into the original sent short value over TCP/IP connection , this
 * was done because structs cannot be sent as a whole over connections and hence they must be deserialized and serialized
 * at the receiver and server respectively
 * @param buffer
 * The placeholder where the received bytes are placed
 * @param value
 * The short variable where the new converted element will be placed
 * @return
 * The next slot in the buffer that is still not deserialized
 */
unsigned char * deserialize_short(unsigned char *buffer, short *value);
/**
 * This method is used to convert a series of bytes into the original sent struct element value over TCP/IP connection , this
 * was done because structs cannot be sent as a whole over connections and hence they must be deserialized and serialized
 * at the receiver and server respectively
 * @param buffer
 * The placeholder where the received bytes are placed
 * @param value
 * The element value where the new converted element will be placed
 * @return
 * The next slot in the buffer that is still not deserialized
 */
unsigned char* deserialize_element(unsigned char *buffer,element *value);
/**
 * * This method is used to convert a struct player into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * The placeholder where bytes will be placed
 * @param value
 * The struct player value that will be converted
 * @return
 * The next next free slot in the buffer in order to send more data
 */
unsigned char* serialize_player(unsigned char*buffer,player *value);
/**
 * This method is used to convert a series of bytes into the original sent struct player value over TCP/IP connection , this
 * was done because structs cannot be sent as a whole over connections and hence they must be deserialized and serialized
 * at the receiver and server respectively
 * @param buffer
 * THe placeholder where the received bytes are placed
 * @param value
 * The player value where the new converted element will be placed
 * @return
 * The next slot in the buffer that is still not deserialized
 */
unsigned char* deserialize_player(unsigned char*buffer,player *value);
/**
 * This method is used to convert a struct player array into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * THe placeholder where bytes will be placed
 * @param value
 * The struct player array that will be converted
 * @param size
 * The size of the player array
 * @return
 * The next free slot in the buffer in order to send more data
 */
unsigned char* serialize_player_array(unsigned char* buffer, player *value,int size);
/**
 * This method is used to convert a series of bytes into the original sent struct player array value over TCP/IP connection , this
 * was done because structs cannot be sent as a whole over connections and hence they must be deserialized and serialized
 * at the receiver and server respectively
 * @param buffer
 * The placeholder where the received bytes are placed
 * @param value
 * The player array value where the new converted element will be placed
 * @param size
 * The size of the converted array
 * @return
 * The next slot in the buffer that is still not deserialized
 */
unsigned char* deserialize_player_array(unsigned char* buffer,player *value,int size);
/**
 * This method is used to convert a game struct into  a series of bytes in order to be sent over tcp/ip
 * connection, this was done because structs cannot be sent as a whole over connections and hence they must be serialized
 * at the sender and deserialized at the receiver
 * @param buffer
 * The placeholder where bytes will be placed
 * @param game
 * The game struct that will be converted
 * @return
 * The next free slot in the buffer in order to send more data
 */
unsigned char* serialize_game(unsigned char*buffer,game *game);
/**
 * This method is used to convert a series of bytes into the original sent game struct value over TCP/IP connection , this
 * was done because structs cannot be sent as a whole over connections and hence they must be deserialized and serialized
 * at the receiver and server respectively
 * @param buffer
 * The placeholder where the received bytes are placed
 * @param game
 * THe game struct variable where the new converted element will be placed
 * @return
 * The next slot in the buffer that is still not deserialized
 */
unsigned char* deserialize_game(unsigned char*buffer,game *game);

#endif //SYSTEMS_CONNECTION_H
