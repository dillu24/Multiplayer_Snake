//
// Created by dillu24 on 14/04/2017.
//
/**
 * Contains the game definition file along with the game logic
 */
#ifndef SYSTEMS_GAME_H
#define SYSTEMS_GAME_H

#include <stdbool.h>
#include <utmpx.h>
/**
 * The constants that will be used throughout the game
 */
#define SNAKE_CHAR '8'
#define FOOD_CHAR '*'
#define SNAKE_INITIAL_SIZE 3
#define SNAKE_MAX_SIZE 15
#define FOOD_POINTS 10
#define MAX_PLAYERS 400
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'
#define EDGE_CHAR (char)219
#define QUIT 'q'
/**
 * The element struct used to define an element on the screen
 */
typedef struct element{
    int x,y; //co_ordinates
}element;
/**
 * The snake object struct , used to define the snake of the player
 */
typedef struct snakeobject{
    bool ate_food; //a variable that indicates if the snake has eaten food or not
    char direction; //a variable that stores the direction the snake is going
    int delay,snakesize; //The delay variable which stores the ammount of delay t be seen on screen in order to modify
                         //the snakes' speed as it is growing , snakesize stores the size of the snake
    element snakebody[SNAKE_MAX_SIZE];//the array that stores the x-y co ordinates of the body of the snake
}snakeobject;
/**
 * The player struct that contains important information about the player
 */
typedef struct player{
    int pid; //the process ID of the client used to identify the players
    int terminal_max_width,terminal_max_height; //used to store the terminal of the player's max width and height ,
                                                //this was done for completion as the size of the screen is resized
    bool refresh; //done as to indicate that the client needs to refresh his terminal , done for completion
    char player_input; //stores the player input from keyboard
    snakeobject snake; //the snakeobject of the player
    int points; //each player gets points when it eats food
    bool killed; //a variable that indicates whether a snake is killed or not i.e out of the game or not
}player;

/**
 * The game struct which stores relative information about the game
 */

typedef struct game{
    player list_of_players[MAX_PLAYERS]; //The players that are playing the game and those that have lost till the server ends
                                        //this game was made so that when all clients have lost the server stops
    element food;//stores the food element on the screen
    int number_of_players; //stores the number of players that are involved in the game
    int server_pid; //stores the process ID of the server,was done for completion
}game;

/**
 * The struct that stores the initial information sent from the client to the server when connecting, this information
 * will be used in order to initialize the player values
 */

typedef struct sending_client_info{
    int pid; //the client's process id
    char player_input; //the client's input from keyboard
    int max_height; //the client's max height of the terminal
    int max_width; //the client's max width of the terminal
}sending_client_info;

/**
 * Used to initialize the snake variables
 * @param sn
 * THe snakeobject to be initialized
 * @param starting_x
 * The starting x-value of the snake altough not needed in this method because it was optimized in other part of the code
 * @param starting_y
 * The starting y-value of the snake altough not needed in this method because it was optimized in other part of the code
 * @param starting_direction
 * The starting direction of the snake
 */
void initialize_snake(snakeobject *sn,int starting_x,int starting_y,char starting_direction);
/**
 * Used to initialize the created player
 * @param pl
 * The player that will be initialized
 * @param term_width
 * The terminal width of the player to be given as value
 * @param term_height
 * THe terminal height of the player to be given as value
 * @param pid
 * The pid of the player to be stored in the player's pid variable
 */
void initialize_player(player *pl,int term_width,int term_height,int pid);
/**
 * used to initialize the created game
 * @param gm
 * The game structure to be initialized
 */
void initialize_game(game *gm);
/**
 * The method used to put the food according to the terminal width and height given in order to always display legal
 * co ordinates within the terminal
 * @param gm
 * The game struct
 * @param over_max_width
 * The terminal max width limit
 * @param ovr_max_height
 * The terminal max height limit
 * @return
 * The food element struct
 */
element* putfood(struct game *gm, int over_max_width,int ovr_max_height);
/**
 * A method done for completion in order to refresh all screens of the client's terminals , this was done for completion
 * so that different implementations of the client could use it altough not directly used in this implementation
 * @param gm
 * The game struct
 */
void refresh_terminals(game *gm);
/**
 * A method used to create a new player and call the initializer to inititlize the new player
 * @param gm
 * the game struct variable
 * @param term_width
 * the passed value player's terminal width
 * @param term_height
 * the passed player's terminal height
 * @param pid
 * the passed value player's process ID
 * @return
 * returns the newly created player
 */
player *createplayer(game *gm,int term_width, int term_height ,int pid);
/**
 * A method used to create a new game struct and calls the initializer to intiialize it
 * @return
 * The newly created game struct
 */
game *creategame();
/**
 * A method used to create a new snake and call the initializer to initialize the newly created snake
 * @param starting_x
 * the starting x co-ordinate of the snake passed as parameter
 * @param starting_y
 * the starting y co-ordinate of the snake passed as parameter
 * @param direction
 * the direction of the snake passed as parameter
 * @return
 * The newly created and initialized snake
 */
snakeobject *createsnake(int starting_x,int starting_y,char direction); //done for completion since this was done
                                                                        //differently in the client
/**
 * Used to check whether a snake hit another snake
 * @param sn1
 * the snake that will be checked if it hit another snake
 * @param sn2
 * the snake that sn1 will be compared with for collisions
 * @return
 * 2 if snakes hit head on , 1 if sn1 hit snake 2 in other parts rather than the head ,0 if both snakes did not collide
 */
int snake_hit_snake(snakeobject *sn1,snakeobject *sn2);
/**
 * The method used to move different snakes in the game
 * @param sn
 * The snake that will be moved
 */
void move_snake(snakeobject *sn);
/**
 * The method that will check if all different types of collisions occured
 * @param gm
 * The game struct that will be checked
 * @return
 * 1 if a collision occured 0 otherwise
 */
bool collision(struct game *gm);
/**
 * A method that will shift all snakebody parts to the right in order to make space in the head to add a new body element
 * @param s
 * The snake whose body will be modified
 */
void move_array_when_eat(snakeobject *s);
/**
 * A method that will shift all snakebody parts to the right except the last one since that will be lost for the snake
 * to move in it's direction
 * @param s
 * The snake whose body will be modified
 */
void move_array_when_not_eat(snakeobject *s);
/**
 * A method that will check whether a snake hit the border
 * @param sn
 * The snake that will be compared for collisions
 * @param max_x
 * The terminal max width
 * @param max_y
 * The terminal max height
 * @return 0 if no collision , 1 if collision
 */
bool snake_hit_border(snakeobject *sn,int max_x,int max_y);
/**
 * A method that will check if a snake collided with itself
 * @param sn
 * The snake object that will be checked for collisions
 * @return
 * 1 if collided , 0 if not
 */
bool snake_hit_himself(snakeobject *sn);
/**
 * A method that will determine the starting position of the snake
 * @param gm
 * The game struct where the snake will be starting int
 * @param p
 * The player struct whose snake will be used to determine his position
 */
void determineSnakePosition(game *gm,player *p);

#endif //SYSTEMS_GAME_H
