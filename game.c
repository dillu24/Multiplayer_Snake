//
// Created by dillu24 on 14/04/2017.
//
/**
 * The game.h method implementation file
 */
#include <zconf.h>
#include "game.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void initialize_snake(snakeobject *sn, int starting_x, int starting_y, char starting_direction) {
    //set all values to default
    sn->snakesize = SNAKE_INITIAL_SIZE;
    sn->ate_food = 0;
    sn->delay = 110000;
    sn->direction = starting_direction;
}

void initialize_player(player *pl, int term_width, int term_height,int pid) {
    //set all values to default
    pl->killed = 0;
    pl->pid = pid;
    pl->player_input = 'z';//set to z so as to not move immediately with a dummy value
    pl->points = 0;
    pl->refresh = 1;
    pl->terminal_max_height = term_height;
    pl->terminal_max_width = term_width;
}

void initialize_game(game *gm) {
    //set all value to default
    gm->food.x = 0;
    gm->food.y = 0;
    gm->number_of_players = 0;
    gm->server_pid = getpid();
}

element *putfood(struct game *gm, int over_max_width, int ovr_max_height) {
    srand(time(NULL));
    refresh_terminals(gm);
    while(1){
        bool food_on_snake_body = false; //a variable used to indicate that the new food was generated on a snake
        int tmpx = rand()% over_max_width+1; //get a random x co-ordinate value
        int tmpy = rand()% ovr_max_height+1; //get a random y co-ordinate value
        for(int i=0;i<gm->number_of_players;i++){ //check if the generated co ordinate lies on any snake
            for(int j=0;j<gm->list_of_players[i].snake.snakesize;j++){
                if(gm->list_of_players[i].snake.snakebody[j].x == tmpx &&
                   gm->list_of_players[i].snake.snakebody[j].y ==tmpy){
                    food_on_snake_body=true;
                    break;
                }
            }
            if(food_on_snake_body){ //if it lies on a snake break from the inner loop
                break;
            }
        }
        if(food_on_snake_body){ //and continue to generate another co ordinate
            continue;
        }
        if(tmpx>= over_max_width-2 || tmpy>= ovr_max_height-3){ //if food is generated on terminal continue finding another one
            continue;
        }
        gm->food.x = tmpx;
        gm->food.y = tmpy;
        break;
    }
    return &gm->food;
}

void determineSnakePosition(game *gm,player *p){
    while(1) {
        bool wrong_guess = false; //used to store if a co ordinate is badly or good generated
        int tmpx = rand() % p->terminal_max_width + 1; //generate random co ordinates
        int tmpy = rand() % p->terminal_max_height + 1;
        element tmpsnakebody[3]; //stores the termporary snake starting body
        if(tmpx ==0){ //if an x co ordinate in generated on the left upper terminal choose another co ordinate... this
                      //was done because for weird reasons co ordinates were still being genrerated on the terminal
                      //altough they were checked below
            continue;
        }

        if (p->snake.direction == LEFT) { //According to the direction generated the reaining co ordinates
            for (int i = 0; i < 3; i++) {
                tmpsnakebody[i].x = tmpx + i;
                tmpsnakebody[i].y = tmpy;
            }
        } else if (p->snake.direction == RIGHT) {
            for (int i = 0; i < 3; i++) {
                tmpsnakebody[i].x = tmpx - i;
                tmpsnakebody[i].y = tmpy;
            }
        } else if (p->snake.direction == UP) {
            for (int i = 0; i < 3; i++) {
                tmpsnakebody[i].x = tmpx;
                tmpsnakebody[i].y = tmpy - i;
            }
        } else if (p->snake.direction == DOWN) {
            for (int i = 0; i < 3; i++) {
                tmpsnakebody[i].x = tmpx;
                tmpsnakebody[i].y = tmpy + 1;
            }
        }


        for (int i = 0; i < gm->number_of_players; i++) { //if any generated co ordinate lies on another snake generate another
            for (int j = 0; j < gm->list_of_players[i].snake.snakesize; j++) {
                for (int k = 0; k < 3; k++) {
                    if (gm->list_of_players[i].snake.snakebody[j].x ==tmpsnakebody[k].x &&
                        gm->list_of_players[i].snake.snakebody[j].y ==tmpsnakebody[k].y){
                        wrong_guess = true;
                        break;
                    }
                }
                if(wrong_guess){
                    break;
                }
            }
            if(wrong_guess){
                break;
            }
        }
        if(wrong_guess){
            continue;
        }
        bool guess_on_terminal = false;
        for(int i=0;i<3;i++){ //else if either parts of the co ordinates lie on the terminal generate another set of co ordinates
            if(tmpsnakebody[i].x >= p->terminal_max_width-2 || tmpsnakebody[i].x == 0 || tmpsnakebody[i].y == 0
               ||tmpsnakebody[i].y >= p->terminal_max_height-3){
                guess_on_terminal = true;
                break;
            }
        }
        if(guess_on_terminal){
            continue;
        }

        bool guess_on_food = false;
        for(int i=0;i<3;i++){ //else if the generated co ordinate is on the food , generate another set of co ordinates
            if(tmpsnakebody[i].x == gm->food.x && tmpsnakebody[i].y == gm->food.y){
                guess_on_food = true;
                break;
            }
        }
        if(guess_on_food){
            continue;
        }

        for(int i=0;i<3;i++){
            p->snake.snakebody[i].x = tmpsnakebody[i].x;
            p->snake.snakebody[i].y = tmpsnakebody[i].y;
        }
        break;
    }
}

void refresh_terminals(game *gm) { //done for completion
    for(int i=0;i<gm->number_of_players;i++){
        gm->list_of_players[i].refresh = 1;
    }
}

player *createplayer(game *gm,int term_width, int term_height,int pid) {
    player *p1 = &(gm->list_of_players[gm->number_of_players]);
    gm->number_of_players++;
    initialize_player(p1,term_width,term_height,pid);
    return p1;
}

game *creategame() {
    game *snake_game = (game*)malloc(sizeof(game));
    initialize_game(snake_game);
    return snake_game;
}

snakeobject *createsnake(int starting_x,int starting_y,char direction) {
    snakeobject *snake = (snakeobject*)malloc(sizeof(snake));
    initialize_snake(snake,starting_x,starting_y,direction);
    return snake;
}

int snake_hit_snake(snakeobject *sn1, snakeobject *sn2) {
    //if the snakes hit on their heads return their respective value
    if(sn1->snakebody[0].x == sn2->snakebody[0].x && sn1->snakebody[0].y == sn2->snakebody[0].y){
        return 2;
    }
    //if the snakes hit on their heads and the second element is also hit then those snakes must be hit head one also
    //this was done because there could be a case that if one snake is moved to the left and the other to the right
    //where the snakes were seperated with no spaces it could lead to only one snake to be terminated due the fact
    //that the server is giving each client the chance to move once .. it is also important to note that in this implmentation
    // no IPC structures were used because the server waits for each movement thread to finish prior to continue serving
    //and hence no race conditions are created that way .. this is because it was understood that each snake must move
    //first in order to check for collisions
    if((sn1->snakebody[1].x == sn2->snakebody[0].x && sn1->snakebody[1].y == sn2->snakebody[0].y)&&
            (sn1->snakebody[0].x == sn2->snakebody[1].x && sn1->snakebody[0].y == sn2->snakebody[1].y)){
        return 2;
    }
    //for each part of the second snake check if the head of the first snake has collided with it
    for(int i=1;i<sn2->snakesize;i++){
        if(sn1->snakebody[0].x == sn2->snakebody[i].x && sn1->snakebody[0].y == sn2->snakebody[i].y){
            return 1;
        }
    }
    return 0;
}

void move_snake(snakeobject *sn) {
    //if the snake did not eat food move the array accordingly and generate the new co ordinates
    if(!sn->ate_food) {
        move_array_when_not_eat(sn);
        if (sn->direction == LEFT) {
            sn->snakebody[0].x = sn->snakebody[1].x-1;
            sn->snakebody[0].y = sn->snakebody[1].y;
        } else if (sn->direction == RIGHT) {
            sn->snakebody[0].x = sn->snakebody[1].x+1;
            sn->snakebody[0].y = sn->snakebody[1].y;
        } else if (sn->direction == UP) {
            sn->snakebody[0].x =sn->snakebody[1].x;
            sn->snakebody[0].y =sn->snakebody[1].y-1;
        } else if (sn->direction == DOWN) {
            sn->snakebody[0].x = sn->snakebody[1].x;
            sn->snakebody[0].y = sn->snakebody[1].y+1;
        }
    }else{
        //if the snake ate food move the array accordingly and gerenate the new co ordinates
        sn->snakesize++;
        move_array_when_eat(sn);
        if (sn->direction == LEFT) {
            sn->snakebody[0].x = sn->snakebody[1].x-1;
            sn->snakebody[0].y = sn->snakebody[1].y;
        } else if (sn->direction == RIGHT) {
            sn->snakebody[0].x = sn->snakebody[1].x+1;
            sn->snakebody[0].y = sn->snakebody[1].y;
        } else if (sn->direction == UP) {
            sn->snakebody[0].x =sn->snakebody[1].x;
            sn->snakebody[0].y =sn->snakebody[1].y-1;
        } else if (sn->direction == DOWN) {
            sn->snakebody[0].x = sn->snakebody[1].x;
            sn->snakebody[0].y = sn->snakebody[1].y+1;
        }
    }
}

bool collision(struct game *gm) {
    bool collisions = 0; //stores whether a collision occured
    for(int i=0;i<gm->number_of_players;i++) { //if a snake is already killed do not check for it's collision
        if (gm->list_of_players[i].killed) {
            continue;
        }
        if (snake_hit_himself(&gm->list_of_players[i].snake)) { //if the snake hit himself kill it
            collisions = 1;
            gm->list_of_players[i].killed = 1;
            continue;
        }
        if (snake_hit_border(&gm->list_of_players[i].snake, gm->list_of_players[i].terminal_max_width,
                             gm->list_of_players[i].terminal_max_height)) {
            //if the snake hit  hit the border kill it
            gm->list_of_players[i].killed = 1;
            collisions = 1;
            continue;
        }
        for (int j = 0; j < gm->number_of_players; j++) {
            if(j ==i){ //compare each snake with another and ignore if it is going to be compared with itself
                continue;
            }
            if(gm->list_of_players[j].killed){ //if the other is already killed skip it
                continue;
            }
            if (snake_hit_snake(&gm->list_of_players[i].snake, &gm->list_of_players[j].snake)==1) {
                collisions = 1; //if the first snake hit the second kill the first
                gm->list_of_players[i].killed = 1;
                refresh_terminals(gm);
            } else if (snake_hit_snake(&gm->list_of_players[j].snake, &gm->list_of_players[i].snake)==1) {
                gm->list_of_players[j].killed = 1; //if the first snake hit the second kill the first
                refresh_terminals(gm);
                collisions = 1;
            } else if (snake_hit_snake(&gm->list_of_players[i].snake, &gm->list_of_players[j].snake) == 2) {
                gm->list_of_players[i].killed = 1; //if both collide head on kill them both
                gm->list_of_players[j].killed = 1;
                refresh_terminals(gm);
                collisions = 1;
            } else { //if no collisions occured consider other snakes
                continue;
            }
        }

        if(gm->list_of_players[i].snake.snakebody[0].x == gm->food.x &&
           gm->list_of_players[i].snake.snakebody[0].y ==gm->food.y){ //if a snake ate food generate a new position
            //for the food and increase it's points and decrease the delay in order to increase the speed in the game
            gm->list_of_players[i].snake.ate_food =1;
            int min_x =9999,min_y=9999;
            for(int k=0;k<gm->number_of_players;k++){
                if(gm->list_of_players[k].terminal_max_width<min_x){
                    min_x = gm->list_of_players[k].terminal_max_width;
                }
                if(gm->list_of_players[k].terminal_max_height<min_y){
                    min_y = gm->list_of_players[k].terminal_max_height;
                }
            }
            putfood(gm,min_x,min_y);
            gm->list_of_players[i].points+= FOOD_POINTS;
            refresh_terminals(gm);
            if(!(gm->list_of_players[i].points%100)){
                gm->list_of_players[i].snake.delay -=30000;
            }
        }else{
            gm->list_of_players[i].snake.ate_food=0;
        }
    }

    return collisions;
}

void move_array_when_eat(snakeobject *s) {
    for(int i=s->snakesize-1;i>=0;i--){
        s->snakebody[i+1]= s->snakebody[i];
    }
}

void move_array_when_not_eat(snakeobject *s) {
    for(int i=s->snakesize-2;i>=0;i--){
        s->snakebody[i+1]= s->snakebody[i];
    }
}

bool snake_hit_border(snakeobject *sn, int max_x, int max_y) {
    if(sn->snakebody[0].x ==0 || sn->snakebody[0].x ==(max_x-1) || sn->snakebody[0].y ==0 || sn->snakebody[0].y == max_y-2){
        return 1;
    }
    return 0;
}

bool snake_hit_himself(snakeobject *sn) {
    for(int i=2;i<sn->snakesize;i++){
        if(sn->snakebody[0].x == sn->snakebody[i].x && sn->snakebody[i].y == sn->snakebody[0].y){
            return 1;
        }
    }
    return 0;
}
