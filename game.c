/*
This file contains the logic for the game
Written by Victoria Torres and Manuel Rydholm.
*/
#include <stdint.h>
#include <pic32mx.h>
#include "header.h"

//Height and width of the screen
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 128
//Height and width of the bird
#define BIRDH 4
#define BIRDW 7
//Width of the pipes
#define PIPEW 7
//Height and width of the enemies
#define ENEMYH 3
#define ENEMYW 3

//variables needed for random function
int randomSeed;
int finalRandomSeed = 0;
int randomSetterVarPipe;
int randomSetterVarEnemy;
//int randomSetterVar;

//Coordinates for the bird. Global variables
double xpos = 0; //this is the left-most point of the bird
double ypos = 2; //this is the top-most point of the bird

double xposp = 100; // same as above^ but for pipe
double yposp = 20;

double xpose=120; //same as above^bur for enemy
double ypose=1;

uint8_t screen[128*4] = {0}; //Display

//variables for keeping track of the score
int score = 0; //score counter
//Top highest scores
int score1 = 0;
int score2 = 0;
int score3 = 0;

//boolean variable that would be use to acces highest scores from the menu
int high_score_screen = 0;

//Converts x and y coordinates so it easier to draw on the screen
void add_pixel(int x, int y, uint8_t *array){
//This prevents the addition of pixels out of bounds
    if(x <= 0 || x >= SCREEN_WIDTH || y <= 0 || y >= SCREEN_HEIGHT)
        return;

//Defines in which page the pixel should go. 0 < y < 32 because of screen.
    int page= y / 8;

//page * 128 gives the page. Plus x gives the index inside that page. y position in the page.
    array[page * 128 + x] |= (1 << (y - page * 8));
}

//Draw the bird at current position
void draw_bird() {
    int i,j;
    for(i = 0; i < BIRDH; i++) {
        for(j = 0; j < BIRDW; j++) {
            add_pixel((int) xpos + j,(int) ypos + i, screen);
        }
    }
}


//Checks which button was pressed: for movement of bird, and navegation through the menus
volatile int getbtn(int btn) {
    int value = 0;
    switch (btn) {
        case 1:
            value = (PORTF)&2;
            break;
        case 2:
            value = ((PORTD>>5)&7) & 1;
            break;
        case 3:
            value = ((PORTD>>5)&7) & 2;
            break;
        case 4:
            value = ((PORTD>>5)&7) & 4;
            break;
        default:
            value = 0;
    }
    return value;
}

//Copies the current value of TMR4 whenever btn3 is pressed
int randGenerator() {
  if (getbtn(3)) {
    int randTemp = TMR4; //RANDOM SEED 0-9. Gets defined when user presses up btn 3
    randomSeed = randTemp;
  }
  return randomSeed;
}

//moves the bird
void move_bird () {
    //to move bird to the left
    if(getbtn(4)) {
        if(xpos >= 0)
            xpos -= 0.25;
    }
    //to move the bird up
    if(getbtn(3)) {     //FIX SO THE BIRD DOESNT LEAVE THE SCREEN
      //int randTemp = TMR4; //RANDOM SEED 0-9. Gets defined when user presses up btn
      //  randomSeed = randTemp;
      //randomSeed = TMR4;
        if(ypos >= 0)
            ypos -= 0.25;
    }

    //to move bird to the right
    if(getbtn(2)) {
        if(xpos <= (127-BIRDW))
            xpos += 0.25;
    }
}

//THIS FUNCTION INCLUDES CODE FROM https://www.sanfoundry.com/c-program-binary-number-into-decimal/
// WHICH IS USED TO CONVERT A BINARY NUMBER OBTAINED FROM randGenerator() TO DECIMAL AND STORE IT IN finalRandomSeed
void random_seed_generator() {
    int decimal_randomSeed = 0;
    int base = 1;
    int rem;
    int seed = randGenerator();

    while (seed > 0)
      {
          rem = seed % 10;
          decimal_randomSeed = decimal_randomSeed + rem * base;
          seed = seed / 10 ;
          base = base * 2;
        }

        finalRandomSeed = (decimal_randomSeed % 10); //value from 0 to 9
      }


//Obtains the value from random_seed_generator and stores it to feed it to draw_pipe once
//the pipe has left the screen. This results in a randomization of the height of the pipe
int randomSetterPipe() {
  if (xposp == 127) {
    randomSetterVarPipe = finalRandomSeed;
  }
  return randomSetterVarPipe;
}

int randomSetterEnemy() {
  if (ypose == 0) {
    randomSetterVarEnemy = finalRandomSeed;
  }
  return randomSetterVarEnemy;
}

void draw_pipe() {
  int localtemp;
  if (xposp == 0) {
    xposp = 127;
    //random_seed_generator();
  }
   int i,j;
   random_seed_generator();
   localtemp = randomSetterPipe();
    yposp = ( (2 * localtemp) + 12 ); //height at which pipe gets drawn
    for (i=0; i < (31-yposp); i++) {
      for (j = 0; j < PIPEW; j++) {
          add_pixel((int) xposp + j,(int) yposp + i, screen);
    }
  }
}



void draw_enemy() {
  int localtemp;
//checks if the enemy has touched the end of the screen and resets it to the initial position
  if (ypose == (31)) { //(31-ENEMYH)) {
    ypose = 0;
    // random_seed_generator();
    // localtemp = randomSetterPipe();
    // xpose = 65 + (5 * localtemp);
  }
    int i,j;
    random_seed_generator();
    localtemp = randomSetterEnemy();
    xpose = 65 + (5 * localtemp);
    for(i = 0; i < ENEMYH; i++) {
        for(j = 0; j < ENEMYW; j++) {
            add_pixel(xpose + j, ypose + i, screen);
        }
    }
}



//Set every byte to 0 in order to clear the screen so movement works properly
void clear() {
    int i;
    for(i = 0; i < sizeof(screen); i++) {
        screen[i] = 0;
    }
}



//Checks which switch was toggled: 1,2,3 change speed. 4 to start the game
volatile int getsw(int sw) {
    int value = 0;
    switch (sw) {
        case 1:
            value = ((PORTD>>8)&15) & 1;
            break;
        case 2:
            value = ((PORTD>>8)&15) & 2;
            break;
        case 3:
            value = ((PORTD>>8)&15) & 4;
            break;
        case 4:
            value = ((PORTD>>8)&15) & 8;
            break;
        default:
            value = 0;
    }
    return value;
}



//checks if the bird has collided with: a pipe, an enemy, or the borders of the display
int collisions() {
    int i;

//borders for the screen
  int screen_top = 0;
  int screen_bottom = SCREEN_HEIGHT;
  int screen_left = 0;

//borders for the bird
    int bird_top = ypos;
    int bird_bottom = ypos + BIRDH;
    int bird_left = xpos;
    int bird_right = xpos + BIRDW;

//borders for the pipes
    int pipe_top = yposp;
    int pipe_left = xposp;
    int pipe_right = xposp + PIPEW;

//borders for the enemies
    int enemy_top = ypose;
    int enemy_bottom = ypose + ENEMYH;
    int enemy_left = xpose;
    int enemy_right = xpose + ENEMYW;

//collisions with the screen borders
    if ( bird_top == screen_top ||
         bird_bottom == screen_bottom ||
         bird_left == screen_left) {
      return 1;
    }

//collisions with the pipes. no bird top needed.
    else if ( bird_bottom == pipe_top ||
              bird_right == pipe_left ||
              bird_left == pipe_right) {
      return 1;
    }

//collisions with the enemies
    else if ( bird_top == enemy_bottom ||
              bird_bottom == enemy_top ||
              bird_right == enemy_left ||
              bird_left == enemy_right) {
      return 1;
    }

 else
    return 0;
}

// * Simple conversion routine
// * Converts binary to decimal numbers
// * Returns pointer to (static) char array
//This function was taken from mipslabfunction written 2015 by F Lundevall. This is used for the score.
#define ITOA_BUFSIZ ( 24 )
char* itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}



//Game's main loop
void play() {
//variables needed for the score
    score = 0; //resets the score
    int speriod = 0;

int playing = 1; //boolean to known when you are playing or not
    while(playing) {
//movement of bird, clear necessary so moves works properly
        move_bird();
        clear();

// https://i.gyazo.com/1f8f50bea099f1b70eef51e3c39463a7.png <---- IFS0 BITS
//////////////GRAVITY//////////////////
		if (IFS(0) & 0x1000) {
            if (ypos >= 0) {
            ypos += 0.25;
			}
		IFSCLR(0) = 0x1000; //reset
		}


////////////  PIPE MOVEMENT  ////////////
		if (IFS(0) & 0x10000) {
            speriod++;
            if(xposp > 0) {
				xposp -= 0.25;
			}
		IFSCLR(0) = 0x10000; //reset
		}

//the speriod is associated with the
        if(speriod == 100) {
            score++;
            PORTE += 1;
            speriod = 0;
        }

////////////  ENEMY MOVEMENT ////////////
		if (IFS(0) & 0x100) {
            if(xpose > 0 && ypose < (31)) { //ypose < (31-ENEMYH))
                xpose -= 0.25;
                ypose += 0.25;
            }
		IFSCLR(0) = 0x10000; //reset     <------------ OJO 10000?
		}

        draw_enemy();
        draw_pipe();
        draw_bird();
        render(screen);
    //    playing = !collisions(); //This is the way to stop the playing
    }
}



//game over screen, here is where you check if you should update your high score, display the player their score, and allow the player to go back to the menu
void game_over() {
    //checks if the score should replace the third place
    if(score < score2 && score > score3){
      score3 = score;
    }

    //checks if the score should replace the second place
    if(score < score1 && score > score2) {
        score2 = score;
    }

    //checks if the score should replace the highest score
    if(score > score1) {
        score3 = score2;
        score2 = score1;
        score1 = score;
    }

    int gameover_screen = 1; // boolean for game over

    //reset the position of the bird to initial position
    xpos = 0;
    ypos = 0;

//what you would see on your screen
    display_string(0, "WASTED!");
    display_string(1, "score:");
    display_string(2, itoaconv(score));
    display_string(3, "BTN1 menu");
    display_update();

//goes back to the menu if btn 1 is pressed
    while(gameover_screen) {
        if(getbtn(1)) {
            gameover_screen = 0;
        }
    }
}

void high_score() {
//you need to clear the text buffer so you dont get stuff from the menu screen
  clear_text();
  display_update();

    display_string(0, itoaconv(score1));
    display_string(1, itoaconv(score2));
    display_string(2, itoaconv(score3));
    display_string(3, "Menu: BTN1");
    display_update();

    while(1) {
        if(getbtn(1)) {
            break;
        }
    }
    //sets the variable that checks the high score from the menu to zero
    high_score_screen = 0;
}

//choose the difficulty of the game
void choose_difficulty() {
    clear_text();
    display_update();

    display_string(0,"Difficulty");
    display_string(1,"SW3: Easy");
    display_string(2,"SW2: Medium");
    display_string(3,"SW1: Hard");
    display_update();

    while(1) {
        if(getsw(3)) {
            difficulty = 100;
            break;
        }
        if(getsw(2)) {
            difficulty = 1000;
            break;
        }
        if(getsw(1)) {
             difficulty = 10000;
            break;
        }
    }

}

//this is the menu, when the program starts the first thing you see it's this
void menu() {
    int run_menu = 1; //boolean to know when to run menu and when to stop

//what you see on the display
    display_string(0, "****************");
    display_string(1, "---FLAPPYBIRD---");
    display_string(2, "SWITCH4 Play!");
    display_string(3, "BTN1 High Scores");
    display_update();

//stops running the menu
    while(run_menu) {
         if(getsw(4)) {
            run_menu = 0;
        }
//stops running the menu and sets boolean to check high score to true
        if(getbtn(1)) {
            run_menu = 0;
            high_score_screen = 1;
        }
    }
}



//Infinite loop that switches between the menu, high score, game, and game over
void run() {
    while (1) {
        menu();
//Once you pressed a btn you turn off the menu, if you pressed btn 4 then you also set check high score to true if that is the case, then you will jump to the high score
        if(high_score_screen) {
            high_score();
        }
        choose_difficulty();
        configtimer2();
        configtimer4();
//You don't need to turn on a variable to play the game because you have this as your default
        play();
//Once you collide with something you will go to the game over screen
        game_over();
    }
}
