//
// Created by dillu24 on 14/04/2017.
//
/**
 * The connection.c file that contains the connection.h header file implementation
 */

#include <sys/socket.h>
#include "connection.h"


int create_server_socket(server_connection *cn) {
    cn->sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(cn->sockfd<0){
        perror("Error opening socket");
        return -1;
    }
    bzero((char*) &cn->serv_addr,sizeof(cn->serv_addr));
    cn->portno = 2007;
    cn->serv_addr.sin_family = AF_INET;
    cn->serv_addr.sin_addr.s_addr = INADDR_ANY;
    cn->serv_addr.sin_port = htons(cn->portno);
    return 0;
}

int bind_host_address(server_connection *cn) {
    if(bind(cn->sockfd,(struct sockaddr*) &cn->serv_addr,sizeof(cn->serv_addr))<0){
        return -1;
    }
    return 0;
}

int accept_connection_from_client(server_connection *cn) {
    cn->clilen = sizeof(cn->cli_addr);
    cn->newsockfd = accept(cn->sockfd, (struct sockaddr*)&cn->cli_addr,&cn->clilen);

    if(cn->newsockfd <0){
        return -1;
    }
    return 0;
}

int close_connection_from_server(server_connection *cn) {
    close(cn->newsockfd);
    close(cn->sockfd);
    return 0;
}

int client_create_socket_point(client_connection *cn) {
    cn->sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(cn->sockfd <0){
        return -1;
    }
    bzero((char *) &cn->serv_addr,sizeof(cn->serv_addr));
    cn->serv_addr.sin_family = AF_INET;
    bcopy((char *) cn->server->h_addr,(char *) &cn->serv_addr.sin_addr.s_addr,cn->server->h_length);
    cn->serv_addr.sin_port = htons(cn->portno);
    return 0;
}

int connect_to_server(client_connection *cn) {
    if(connect(cn->sockfd,(struct sockaddr*)&cn->serv_addr, sizeof(cn->serv_addr)) <0){
        return -1;
    }
    return 0;
}

int close_connection_from_client(client_connection *cn) {
    close(cn->sockfd);
    return 0;
}

unsigned char *serialize_int(unsigned char *buffer, int value) { //since an integer is 4 bytes
    buffer[0] = (unsigned char) (value >> 24);
    buffer[1] = (unsigned char) (value >> 16);
    buffer[2] = (unsigned char) (value >> 8);
    buffer[3] = (unsigned char) value;
    return buffer + 4;
}

unsigned char * deserialize_int(unsigned char *buffer, int *value) //since an integer is 4 bytes
{
    int tempvalue = 0;
    tempvalue |= buffer[0] << 24;
    tempvalue |= buffer[1] << 16;
    tempvalue |= buffer[2] << 8;
    tempvalue |= buffer[3];
    *value = tempvalue;
    return buffer + 4;
}

unsigned char * serialize_sending_connection(unsigned char *buffer, struct sending_client_info *value)
{   //convert each and every part of the struct to bytes using the basic serialization converters
    buffer = serialize_int(buffer, value->pid);
    buffer = serialize_int(buffer,value->max_height);
    buffer = serialize_int(buffer,value->max_width);
    buffer = serialize_char(buffer, value->player_input);
    return buffer;
}

unsigned char * deserialize_sending_connection(unsigned char *buffer, struct sending_client_info *value)
{   //convert each and every part of the struct to bytes using the basic serialization converters
    buffer = deserialize_int(buffer, &value->pid);
    buffer = deserialize_int(buffer,&value->max_height);
    buffer = deserialize_int(buffer,&value->max_width);
    buffer = deserialize_char(buffer, &value->player_input);
    return buffer;
}

unsigned char * serialize_char(unsigned char *buffer, char value)
{   //a character is 1 byte long
    buffer[0] = (unsigned char)value;
    return buffer + 1;
}

unsigned char * deserialize_char(unsigned char *buffer, char *value)
{
    *value = buffer[0];
    return buffer + 1;
}

unsigned char * serialize_short(unsigned char *buffer, short value) {
    //short is 2 bytes long
    buffer[0] = (unsigned char) (value >> 8);
    buffer[1] = (unsigned char) value;
    return buffer + 2;
}

unsigned char* serialize_element(unsigned char *buffer,element *value){
    //convert each and every part of the struct to bytes using the basic serialization converters
    buffer = serialize_int(buffer,value->x);
    buffer = serialize_int(buffer,value->y);
    return buffer;
}

unsigned char *serialize_snakeobject(unsigned char *buffer, snakeobject *snake) {
    //convert each and every part of the struct to bytes using the basic serialization converters
    buffer = serialize_short(buffer, snake->ate_food);
    buffer = serialize_char(buffer,snake->direction);
    buffer = serialize_int(buffer,snake->delay);
    buffer = serialize_int(buffer,snake->snakesize);
    buffer = serialize_element_array(buffer,snake->snakebody,snake->snakesize);
    return buffer;
}

unsigned char *deserialize_snakeobject(unsigned char *buffer, snakeobject *snake) {
    buffer = deserialize_short(buffer,(short*)&snake->ate_food);
    buffer = deserialize_char(buffer, &snake->direction);
    buffer = deserialize_int(buffer,&snake->delay);
    buffer = deserialize_int(buffer,&snake->snakesize);
    buffer = deserialize_element_array(buffer,snake->snakebody,snake->snakesize);

    return buffer;
}

unsigned char *serialize_element_array(unsigned char *buffer, element *array,int size) {
    //convert each and every part of the array by calling the basic converter on each placeholder
    for(int i=0;i<size;i++){
        buffer = serialize_element(buffer,&array[i]);
    }
    return buffer;
}

unsigned char *deserialize_element_array(unsigned char *buffer, element *array,int size){
    //same as above for deserialization
    for(int i=0;i<size;i++){
        buffer = deserialize_element(buffer,&array[i]);
    }
    return buffer;
}

unsigned char* deserialize_element(unsigned char *buffer,element *value){
    //calling the deserialization methods for each part of the struct in order to convert it from bytes
    buffer = deserialize_int(buffer,&value->x);
    buffer = deserialize_int(buffer,&value->y);
    return buffer;
}


unsigned char * deserialize_short(unsigned char *buffer, short *value) {
    uint16_t temp = 0;

    temp |= (uint16_t) buffer[0] << 8;
    temp |= (uint16_t) buffer[1];
    *value = temp;
    return buffer + 2;
}

unsigned char *serialize_player(unsigned char *buffer, player *value) {
    //convert each and every part of the struct to bytes using the serialization converters required and indicate
    //the next free slot afterwords by following the buffer pointer
    buffer = serialize_int(buffer,value->pid);
    buffer = serialize_int(buffer,value->terminal_max_height);
    buffer = serialize_int(buffer,value->terminal_max_width);
    buffer = serialize_short(buffer,value->refresh);
    buffer = serialize_char(buffer,value->player_input);
    buffer = serialize_snakeobject(buffer,&value->snake);
    buffer = serialize_short(buffer,value->killed);
    buffer = serialize_int(buffer,value->points);
    return buffer;
}

unsigned char *deserialize_player(unsigned char *buffer, player *value) {
    //converted everything to the original struct by deserialziaing each and every slot accordingly untill we return
    //the next free unused buffer space
    buffer = deserialize_int(buffer,&value->pid);
    buffer = deserialize_int(buffer,&value->terminal_max_height);
    buffer = deserialize_int(buffer,&value->terminal_max_width);
    buffer = deserialize_short(buffer,(short*)&value->refresh);
    buffer = deserialize_char(buffer,&value->player_input);
    buffer = deserialize_snakeobject(buffer,&value->snake);
    buffer = deserialize_short(buffer,(short*)&value->killed);
    buffer = deserialize_int(buffer,&value->points);
    return buffer;
}


unsigned char *serialize_player_array(unsigned char *buffer, player *value,int size) {
    //convert each and every part of the array by calling the basic converter on each placeholder
    for(int i=0;i<size;i++){
        buffer = serialize_player(buffer,&value[i]);
    }
    return buffer;
}

unsigned char *deserialize_player_array(unsigned char *buffer, player *value,int size) {
    //same as desribed for the serialization but instead calling the deserialization converted
    for(int i=0;i<size;i++){
        buffer = deserialize_player(buffer,&value[i]);
    }
    return buffer;
}

unsigned char *serialize_game(unsigned char *buffer, game *game) {
    //convert each and every part of the struct accordingly to bytes and then return the next free placeholder in order
    //to send more data
    buffer = serialize_element(buffer,&game->food);
    buffer = serialize_int(buffer,game->number_of_players);
    buffer = serialize_int(buffer,game->server_pid);
    buffer = serialize_player_array(buffer,game->list_of_players,game->number_of_players);

    return buffer;
}

unsigned char *deserialize_game(unsigned char *buffer, game *game) {
    //same for deserialization but calling the basic methods for deseralization on each struct element.
    buffer = deserialize_element(buffer,&game->food);
    buffer = deserialize_int(buffer,&game->number_of_players);
    buffer = deserialize_int(buffer,&game->server_pid);
    buffer = deserialize_player_array(buffer,game->list_of_players,game->number_of_players);
    return buffer;
}

