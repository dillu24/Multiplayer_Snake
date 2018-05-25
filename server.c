//
// Created by dillu24 on 14/04/2017.
//
/**
 * This is the game server and here is where all the game logic is incorporated.We must also mention that the server
 * is multi threaded and is capable of doing multiple request at the same time by the server , and it also listens
 * for new connections.Thus locks were implemented to avoid race conditions . A race condition could appear when a thread
 * has just completed a new connection , but the player snake is not initialized and since the connection is created the
 * server tries to move the unitialized snake thus this would break the game , that's why locks were used.The server
 * also never starts the game without having any connection hence initialy the server is blocked waiting for a new connection.
 * One must also mention that this server was implemented so that when everybody looses it does not need to keep on working and
 * thus stop running for simplicity. This could have been done so that the server never stops , but this was done in order
 * to check that all connections are closed cleanly etc.
 */
#include <fcntl.h>
#include "connection.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ncurses.h"
bool write_last_struct[MAX_PLAYERS]; //this is used as an indicator so that if the snake is killed , the client is not
//blocked waiting for the server to write the last game struct , thus the server
//can write the last game struct to the client so that he can terminate after a check

pthread_mutex_t lock; //the lock mentioned above
/**
 * The struct used in order to pass arguments to the threads
 */
struct params
{
    int index;
    int *cn;
};
pthread_t tids[MAX_PLAYERS+1];//stores the thread id's , MAX_PLAYERS+1 because the first thread is used to listen for new connections
sending_client_info cl[MAX_PLAYERS]; //stores the client info
int cn[MAX_PLAYERS]; //stores the client connection info
int number_of_connections; //stores the number of connections
struct params p[MAX_PLAYERS+1];
game *gm; //the game currently played
server_connection server_socket; //the socket of the server
/**
 * This method is used to check whether everyone is killed , in which case if this is true the server stops
 * @param gm
 * THe game struct to be checked
 * @return
 * false if not everyone is killed , true otherwise
 */
bool is_everyone_killed(game *gm){
    for(int i=0;i<gm->number_of_players;i++){
        if(!gm->list_of_players[i].killed){
            return false;
        }
    }
    return true;
}
/**
 * This method is used to check if there is a game winner , in which case the server restarts with all the participants
 * that are still not killed
 * @param gm
 * The game struct to be checked
 * @return
 * true if there is a winner , false otheriwse
 */
bool there_is_a_winner(game *gm){
    for(int i=0;i<gm->number_of_players;i++){
        if(gm->list_of_players[i].snake.snakesize==15){
            return true;
        }
    }
    return false;
}
bool thread_connection_ready = false; // variable used to check if a connection was accepted and hence indicate
                                      //that the thread must execute again to listen for more connections
/**
 * This is the function executed by the connection thread , a thread is blocked listening for a new connection untill
 * a connection is accepted , in which case in the loop then the same thread is made to listen for more connections
 * @param arg
 * @return
 */
void *connection_thread(void *arg)
{
    int thread_num = (*((struct params *) arg)).index;
    unsigned char *ptr,buffer[1240];

    if(accept_connection_from_client(&server_socket)== -1){ //accept new client connection
        perror("Error on accept");
        exit(-1);
    }
    pthread_mutex_lock(&lock); //get the lock to avoid conflicts
    int cnt = server_socket.newsockfd;
    cn[number_of_connections] = cnt ;
    p[number_of_connections].cn = (int*)malloc(sizeof(int));
    p[number_of_connections].cn = &cn[number_of_connections];
    number_of_connections++;
    if(read(cn[number_of_connections-1],buffer,1239)<0){ //read the client info
        printf("connection \n");
        perror("error on read");
    }
    unsigned char akn[1024];
    ptr = serialize_int(akn,getpid());
    if(write(cn[number_of_connections-1],akn,50)<0){//give server pid as aknowledgement
        printf("connection \n");
        perror("error writing");
    }
    ptr = deserialize_sending_connection(buffer,&cl[number_of_connections-1]);
    int input = (int)'a'; //the snake by default moves to the left
    player *pl = createplayer(gm,cl[number_of_connections-1].max_width,cl[number_of_connections-1].max_height,
                              cl[number_of_connections-1].pid);
    pl->snake.direction = 'a';
    determineSnakePosition(gm,pl); //get snake position
    initialize_snake(&pl->snake,pl->snake.snakebody[0].x,pl->snake.snakebody[0].y,'a'); //create a snake for the new player

    int *ret = malloc(sizeof(int));
    *ret = thread_num;
    thread_connection_ready = true; //indicate that the thread is ready
    pthread_mutex_unlock(&lock); //unlock so that other operations could be done
    return ret;
}
/**
 * This method is used by the different threads to get different client inputs
 * @param arg
 * The arguments passed to each thread
 * @return
 * The thread id
 */
void *read_user_input_thread(void *arg)
{
    int thread_num = (*((struct params *) arg)).index;
    if(!gm->list_of_players[thread_num-1].killed) { //if the snake is not killed get it's input otherwise ignore
        int n=0;

        int input = (char) 'a';
        unsigned char read_buffer[1024], write_buffer[1024], *ptr;
        if ((n=read(cn[thread_num - 1], read_buffer, 1023)) < 0) { //read input , threadnum-1 because the first is blocked listening for connections
            printf("reading \n");
            perror("error on read");
        }else if(n==0){ //if connection is lost the snake is killed
            gm->list_of_players[thread_num-1].killed = 1;
            write_last_struct[thread_num-1]=1;
        }
        ptr = deserialize_int(read_buffer, &input);
        gm->list_of_players[(*((struct params *) arg)).index - 1].player_input = (char) input;
        switch (gm->list_of_players[(*((struct params *) arg)).index - 1].player_input) {
            case LEFT: //set direction according to player input
                if (gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction != RIGHT) {
                    gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction = LEFT;
                }
                break;
            case UP:
                if (gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction != DOWN) {
                    gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction = UP;
                }
                break;
            case DOWN:
                if (gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction != UP) {
                    gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction = DOWN;
                }
                break;
            case RIGHT:
                if (gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction != LEFT) {
                    gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction = RIGHT;
                }
                break;
            case QUIT:
                gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction = QUIT;
                break;
            default:
                gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction
                        = gm->list_of_players[(*((struct params *) arg)).index - 1].snake.direction;
        }
    }


    int *ret = malloc(sizeof(int));
    *ret = thread_num;
    return ret;
}
/**
 * This is a function used by a thread to move a snake
 * @param arg
 * The arguments passed to the thread
 * @return
 * The thread id
 */
void *move_thread(void *arg)
{
    int thread_num = (*((struct params *) arg)).index;
    if(!gm->list_of_players[(*((struct params *) arg)).index-1].killed) { //if snake is killed ignore it
        char unsigned write_buffer[1024], *ptr;
        move_snake(&gm->list_of_players[(*((struct params *) arg)).index - 1].snake); //move snake
        usleep(gm->list_of_players[(*((struct params *) arg)).index - 1].snake.delay); //sleep for a bit so the game is playable
        ptr = serialize_game(write_buffer, gm);

        if (write(cn[(*((struct params *) arg)).index - 1], write_buffer, ptr - write_buffer) < 0) { //write the new updates to the client
            printf("move \n");
            perror("error in writing");
        }
    }
    int *ret = malloc(sizeof(int));
    *ret = thread_num;
    return ret;

}
/**
 * This handler is used whenever the client is signaled to be killed , it is important to note that after searchning the
 * man pages some kill commands are not handled cleanly so effectively the only signals caught by this handler is the
 * SIGINT signal
 * @param sig
 */
void quit_handler(int sig){
    for(int i=0;i<gm->number_of_players;i++) {
        if(!gm->list_of_players[i].killed){ //if the snake is not killed write the last game strict
            unsigned char write_buffer1[1240],*ptr;
            gm->list_of_players[i].killed = 1;
            ptr = serialize_game(write_buffer1, gm);
            if (write(cn[i], write_buffer1, ptr - write_buffer1) < 0) {
                printf("quit ahndler \n");
                perror("error in writing");
            }
        }
    }
    for(int i=0;i<gm->number_of_players;i++){ //close all connections
        close(cn[i]);
    }
    close_connection_from_server(&server_socket); //close the socket
    pthread_mutex_destroy(&lock); //destroy the lock
    exit(-1);
}

int main(){
    for(int i=0;i<MAX_PLAYERS;i++){ //initially set the array all to 0
        write_last_struct[i]=0;
    }
    signal(SIGINT , quit_handler);//set the handler
    signal(SIGSTOP,quit_handler); //done for completion altough cannot be caught
    signal(SIGKILL,quit_handler);
    signal(SIGQUIT,quit_handler);
    int n=0;
    if(pthread_mutex_init(&lock,NULL)!=0){ //initialize the lock
        perror("error creating lock \n");
        exit(-1);
    }

    for(int i = 0; i < MAX_PLAYERS+1; i++) //set the thread id parameters accordingly
    {
        p[i].index = i;
    }
    number_of_connections = 0;
    unsigned char *ptr,buffer[1240];
    if(create_server_socket(&server_socket)==-1){ //create socket
        perror("Error opening socket");
        exit(-1);
    }

    if(bind_host_address(&server_socket)==-1){ //bind address
        perror("Error binding");
        exit(-1);
    }

    listen(server_socket.sockfd,MAX_PLAYERS); //listen for connections
    if(accept_connection_from_client(&server_socket)== -1){ //accept first connection
        perror("Error on accept");
        exit(-1);
    }
    cn[number_of_connections] = server_socket.newsockfd; //set the values accordingly
    p[0].cn = (int*)malloc(sizeof(int));
    p[0].cn = &cn[0];
    number_of_connections++;
    if((n=read(cn[0],buffer,1239))<0){ //read the client information
        printf("connection main \n");
        perror("error on read");
    }else if(n==0){ //if connection is lost there are no further connections therefore close server
        close(cn[0]);
        close_connection_from_server(&server_socket);
        exit(-1);
    }
    unsigned char akn[1024];
    ptr = serialize_int(akn,getpid());
    if(write(cn[0],akn,50)<0){ //write acknowledgement by sending the server pid
        printf("connection main \n");
        perror("error writing");
    }
    ptr = deserialize_sending_connection(buffer,&cl[0]);
    int input = (int)'a';
    gm = creategame(); //create a new game and initialize the first player and first snake which is set to have by default
                       //a left direction in this game
    putfood(gm,cl[0].max_width,cl[0].max_height);//put new food
    player *pl = createplayer(gm,cl[0].max_width,cl[0].max_height,cl[0].pid);
    pl->snake.direction = 'a';
    determineSnakePosition(gm,pl);

    initialize_snake(&pl->snake,pl->snake.snakebody[0].x,pl->snake.snakebody[0].y,'a');
    pthread_create(&tids[0], NULL, connection_thread, p + number_of_connections);//create a thread to listen for connections seperately
    while(1){

        if(thread_connection_ready){ //if a connection has been accepted create a new thread to listen for more connections
            thread_connection_ready = false;
            pthread_create(&tids[0], NULL, connection_thread, p);
        }
        pthread_mutex_lock(&lock); //lock in order to not have conflicts with the connection thread
        for(int i=1;i<gm->number_of_players+1;i++){ //create threads to get different client input
            pthread_create(&tids[i], NULL, read_user_input_thread, p + i);
        }
        pthread_mutex_unlock(&lock);
        for(int i = 1; i < gm->number_of_players+1; i++) //wait for threads in order to not check for collisions while a thread
                                                         //is moving so that to give opportunity to each snake to move first
        {
            void *tret;
            pthread_join(tids[i], &tret);
        }
        pthread_mutex_lock(&lock); //lock to not have conflicts with other thread
        collision(gm); //check for collisions
        pthread_mutex_unlock(&lock);

        for(int i=0;i<gm->number_of_players;i++){
            if(gm->list_of_players[i].snake.snakesize==15 ||
               gm->list_of_players[i].snake.direction ==QUIT){ //if someone quit kill him
                gm->list_of_players[i].killed = 1;
            }
            if(gm->list_of_players[i].snake.snakesize==15){ //if someone won do not kill him so to restart the game
                gm->list_of_players[i].killed = 0;
            }
        }

        if(is_everyone_killed(gm)){ //if everyone is killed stop server
            break;
        }
        if(there_is_a_winner(gm)){ //if somebody won , set all not killed snakes to initial values and restart game
            for(int i=0;i<gm->number_of_players;i++){
                char unsigned *ptr,write_buffer[1024];
                if(!gm->list_of_players[i].killed){
                    gm->list_of_players[i].points =0;
                    gm->list_of_players[i].snake.snakesize =3;
                    gm->list_of_players[i].snake.delay = 110000;
                    gm->list_of_players[i].snake.direction = LEFT;
                    gm->list_of_players[i].snake.ate_food = 0;
                    determineSnakePosition(gm,&gm->list_of_players[i]);
                    ptr = serialize_game(write_buffer, gm);
                }
            }
        }
        pthread_mutex_lock(&lock);//lock to avoid conflicts with other threads
        for (int i = 1; i < gm->number_of_players + 1; i++) { //create threads to move snakes at the same time
            pthread_create(&tids[i], NULL, move_thread, p + i);
        }
        pthread_mutex_unlock(&lock);
        for (int i = 1; i < gm->number_of_players + 1; i++) { //wait for threads in order to not have race conditions
            void *tret;
            pthread_join(tids[i], &tret);
        }
        for (int i=0;i<gm->number_of_players;i++){
            char unsigned write_buffer[1024];
            if(gm->list_of_players[i].killed && !write_last_struct[i]){ //if a snake is killed and the last game update was
                //not written to him , write it in order for the client to terminate the program and does not block indefinetly
                ptr = serialize_game(write_buffer, gm);
                if (write(cn[i], write_buffer, ptr - write_buffer) < 0) {
                    perror("error in writing");
                }
                write_last_struct[i] = 1;
            }
        }
    }

    pthread_cancel(tids[0]); //if server stops close cleanly , and close waiting thread
    for(int i=0;i<gm->number_of_players;i++){
        close(cn[i]);
    }
    close_connection_from_server(&server_socket);
    pthread_mutex_destroy(&lock); //destory lock
    exit(0);

}

