//
// Created by dillu24 on 14/04/2017.
//

/**
*The client file , each client will run this program in order to play the game ... the client acts as a terminal display
* and it is important because it sends the client input to the server whom he sends the current game map as output.The
* client is also multithreaded , hence the cleint input is read in a different thread then the other functionalities of the client
**/

#include <ncurses.h>
#include <bits/signum.h>
#include <signal.h>
#include <pthread.h>
#include "connection.h"
//snakes command run bashscript


/**
*This method is used by the client to initialize the ncurses memory and the starting screen , it also , resizes
* the any terminal of the client to a fixed size so that the game can be played properly since there could be
* bugs if diffferent terminal sizes are accepted .. thus for best game experience the terminal should be maximized..
* if for some reason the game is still not p-roperly displayed due to the fact that the screen size is small feel
* free to tweak the screen size from thw wresize function call below
*@param maxheight
*Stores the maximum height of the terminal , this is used as to intitializes the height of the terminal to a fixed size and thus
* this value is passed to the server through TCP/IP
*@param maxwidth
*Stores the maximum height of the terminal , this is used as to initialize the height of the terminal to a fixed size and thus
*this value is passed to the server throguh TCP/IP
**/
void draw_screen(int *maxheight,int *maxwidth){
    initscr(); //create terminal memory
    nodelay(stdscr,true); //do not wait for user input since the snake must travel in the forward direction every periodic time
    wresize(stdscr,43,142);//resize the screen as described
    keypad(stdscr,true);//initialize the keyboard as input
    noecho();//do not display any characters typed by the user
    curs_set(0); //set the curses to be invisible
    getmaxyx(stdscr,*maxheight,*maxwidth); // set the max hieght and width to be sent to the server in the TCP/IP connection
    for(int i=0;i<*maxwidth-1;i++){ //create the arena
        move(0,i);
        addch(EDGE_CHAR);
    }

    for(int i=0;i<*maxheight-1;i++){
        move(i,0);
        addch(EDGE_CHAR);
    }

    for(int i=0;i<*maxwidth-1;i++){
        move(*maxheight-2,i);
        addch(EDGE_CHAR);
    }

    for(int i=0;i<*maxheight-1;i++) {
        move(i, *maxwidth - 2);
        addch(EDGE_CHAR);
    }
    move(*maxheight-1,0); //move to create the intiial points
    addch('0');
    //refresh();
}
/**
 * This method is used to refresh all the spaces in the terminal by setting all too spaces , this so that previous
 * characters of the snake that are no longer part of the snake body will not appear
 * @param maxheight
 * the max height of the terminal
 * @param maxwidth
 * the max width of the terminal
 */
void screen_refresh_all_spaces(int maxheight,int maxwidth){
    for(int i=1;i<maxwidth-1;i++){
        for(int j=1;j<maxheight-2;j++){
            move(j,i);
            addch(' ');
        }
    }
    for(int i=0;i<maxwidth-1;i++){
        move(0,i);
        addch(EDGE_CHAR);
    }

    for(int i=0;i<maxheight-1;i++){
        move(i,0);
        addch(EDGE_CHAR);
    }

    for(int i=0;i<maxwidth-1;i++){
        move(maxheight-2,i);
        addch(EDGE_CHAR);
    }

    for(int i=0;i<maxheight-1;i++) {
        move(i, maxwidth - 1);
        addch(EDGE_CHAR);
    }
}
/**
 * THis method is used to update the screen after the server sends the game struct  , hence it displays the food and
 * snakes present
 * @param gm
 * The game struct to be displayed
 * @param maxheight
 * the max height of the terminal
 * @param maxwidth
 * the max width of the terminal
 */
void screen_update(game *gm,int maxheight,int maxwidth){
    wresize(stdscr,43,142); //resize the screen so that if we have multiple terminal sizes they are set to be the same
                            // to modify the arena size update the characters of this
    screen_refresh_all_spaces(maxheight,maxwidth);
    for(int i=0;i<gm->number_of_players;i++){ //do not display killed snakes
        if(gm->list_of_players[i].killed){
            continue;
        }
        if(gm->list_of_players[i].pid == getpid()){ //printf the respective points of the player
            move(gm->list_of_players[i].terminal_max_height-1,0);
            printw("%d",gm->list_of_players[i].points);
            refresh();
        }
        for(int j=0;j<gm->list_of_players[i].snake.snakesize;j++){
            move(gm->list_of_players[i].snake.snakebody[j].y,gm->list_of_players[i].snake.snakebody[j].x);
            addch(SNAKE_CHAR);
            refresh();
        }
    }
    move(gm->food.y,gm->food.x); //display the food
    addch(FOOD_CHAR);
    refresh();
}
/**
 * This method is used to close the memory set by the ncurses and to close the game accordingly
 * @param gm
 * The game struct to be closed
 */
void stop(game *gm){
    int player_index =0;
    for(int i=0;i<gm->number_of_players;i++){
        if(gm->list_of_players[i].pid == getpid()){
            player_index = i;
            break;
        }
    }
    if(gm->list_of_players[player_index].snake.snakesize==SNAKE_MAX_SIZE){ //if the snake of the player reached full size he won
        move(35,35);
        printw("Congratulations you won , enter any character to exit");
    }else{ //otherwise he lost
        move(35,35);
        printw("You lost ,enter any character to exit");
    }
    nodelay(stdscr,false); //wait for character input in order to terminate
    getch();
    endwin(); //terminate the allocated memory given by ncurses
}
/**
 * This structure is used so that arguments could be passed to the threads
 */
struct params
{
    int index;
};
client_connection cn; //stores all the client information about the connection with the server

/**
 * This functions is executed by the threads and it is used to get write the player input to the server , thus the client
 * is multi threaded
 * @param arg
 * stores the arguments passed to the thread
 * @return
 * The thread id
 */
void *read_thread(void *arg)
{
    int thread_num = (*((struct params *) arg)).index;
    int temp=getch(); //get player input
    flushinp(); //block other characters to not break the game
    unsigned char new_buffer[1024];
    unsigned char *ptr;
    ptr = serialize_int(new_buffer,temp);
    if(write(cn.sockfd,new_buffer,ptr-new_buffer)<0){ //send the input
        perror("error on write");
    }

    int *ret = malloc(sizeof(int));
    *ret = thread_num;
    return ret;
}
game gm; //stores the game struct that is sent by the server each time it is updated
/**
 * This is the handler used when a quit signal is given , this handler is used for the kill however this is done for
 * completion because after searching the man pages it was found that kill functions do not terminate the programs
 * cleanly
 * @param sig
 * the signal integer representation
 */
void quit_handler(int sig){
    close_connection_from_client(&cn); //close the client end of the connection
    stop(&gm); //stop game
    exit(-1);
}


int main(int argc , char *argv[]){
    int server_pid; //stores the server's pid
    pthread_t tids; //stores the thread's id
    signal(SIGINT,quit_handler); //set the handler for each signal that could effect the game
    signal(SIGSTOP,quit_handler);
    signal(SIGQUIT,quit_handler);
    signal(SIGKILL,quit_handler);

    struct params *p = (struct params *)malloc(sizeof(struct params));

    if(argc <3){ //if not required input is given display error
        fprintf(stderr,"usage %s hostname port \n",argv[0]);
        exit(-1);
    }
    char unsigned buffer[1240] , *ptr;
    cn.portno = atoi(argv[2]);
    cn.server = gethostbyname(argv[1]);

    if(cn.server == NULL){ //if the host cannot be found terminate
        fprintf(stderr,"No such host \n");
        exit(-1);
    }

    if(client_create_socket_point(&cn) == -1){ //create socket point
        perror("Error opening socket");
        exit(-1);
    }

    if(connect_to_server(&cn)==-1){ //connect to server
        perror("Error connecting");
        exit(-1);
    }

    sending_client_info cl; //the struct that stores the client info to be sent to the server so that the client could
                            //be identified
    draw_screen(&cl.max_height,&cl.max_width); //initialize screen
    cl.pid = getpid();
    ptr = serialize_sending_connection(buffer,&cl);

    if(write(cn.sockfd,buffer,ptr - buffer)<0){ //write details
        perror("error on write");
    }
    char unsigned achnowledgement_buf[1024];
    int n=0;
    if((n=read(cn.sockfd,achnowledgement_buf,1023))<0){ //get acknowledgement for completion
        perror("error reading");
    }else if(n==0){ //if the connection is lost terminate
        close_connection_from_client(&cn);
        stop(&gm);
        free(p);
        exit(-1);
    }
    ptr = deserialize_int(achnowledgement_buf,&server_pid);
    int temp;
    bool first = true;
    while(1){
        int player_index = 0;
        p->index = 0;
        pthread_create(&tids, NULL,read_thread,p); //create the thread to send input to the server
        void *tret;
        pthread_join(tids, &tret); //wait for the thread
        free(tret);
        unsigned char read_buffer[1024];
        if((n=read(cn.sockfd,read_buffer,1023))<0){ //read the updated game struct
            perror("error on read");
        }else if(n==0){ //if connection is lost terminate the client
            break;
        }
        ptr = deserialize_game(read_buffer,&gm);
        screen_update(&gm,cl.max_height,cl.max_width); //update screen
        if(first){ //if this is the first time we are going to update the screen sleep a bit first in order to give
                   //the client the luxury to know where he's going to start and the direction
            sleep(2);//change
            first = false;
        }
        for(int i=0;i<gm.number_of_players;i++){
            if(gm.list_of_players[i].pid == getpid()){
                player_index = i;
                break;
            }
        }
        if(gm.list_of_players[player_index].snake.snakesize==SNAKE_MAX_SIZE){ //if the player is a winner inform him
            //and set kill to false in order for the client to keep on executing
            move(35,35);
            printw("Congratulations you won , wait until next game continues");
            first =true;
            move(cl.max_height-1,2);
            addch(' ');
            move(cl.max_height-1,1);
            addch(' ');
            refresh();
            sleep(2);
            gm.list_of_players[player_index].killed = 0;
        }
        if(gm.list_of_players[player_index].killed ==1){ //if the snake is killed terminate the client
            break;
        }

    }
    close_connection_from_client(&cn);//close cleanly
    stop(&gm);
    free(p);
    return 0;
}
