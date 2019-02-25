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
#define BIRDH 10
#define BIRDW 7
//Height and width of the pipes
#define PIPEH 14
#define PIPEW 7
//Number of pipes
#define NROFPIPS 5
//Height and width of the enemies
#define ENEMYH 3
#define ENEMYW 3
//Number of enemies
#define NOFENEMY 5

//Coordinates for the bird. Global variables
double xpos = 0; //this is the left-most point of the bird
double ypos = 20; //this is the top-most point of the bird

uint8_t screen[128*4] = {0}; //Display

//variables for keeping track of the score
int score = 0; //score counter
//Top highest scores
int score1 = 0;
int score2 = 0;

//boolean variable that would be use to acces highest scores from the menu
int check_high_score = 0;

//creates a two dimensional array: first dimension is to count each of the enemies and second has two cells: [][0] for x coordinate and [][1] for y coordinate of each of the enemies
int enemies[NOFENEMY][2] = {0};

//Converts x and y coordinates so it easier to draw on the screen
//SHOULD WE MAKE THE SCREEN A CONST??????
void add_pixel(int x, int y, uint8_t *array){
//This prevents the addition of pixels out of bounds
    if(x <= 0 || x >= SCREEN_WIDTH || y <= 0 || y >= SCREEN_HEIGHT)
        return;
    
//Defines in which page the pixel should go. 0 < y < 32 because of screen.
    int page= y / 8;
    
//page * 128 gives the page. Plus x gives the index inside that page. Same for y coordinate.
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

//draw an enemy at a set position
void draw_enemy(int x, int y) {
    int i,j;
    for(i = 0; i < ENEMYH; i++) {
        for(j = 0; j < ENEMYW; j++) {
            add_pixel(x + j, y + i, screen);
        }
    }
}

//adds the enemies created to the global array for enemies. Parameters are the x and y coordinates plus the number of the enemy which acts as its identification
void addEnemy(int x, int y, int num) {
    enemies[num][0] = x;
    enemies[num][1] = y;
}


//Set every byte to 0 in order to clear the screen so the bird, pipes, and enemies can move properly
void clear() {
    int i;
    for(i = 0; i < sizeof(screen); i++) {
        screen[i] = 0;
    }
}



//this function allows the enemies to move to the left and down
void move_enemy(int xspeed,int yspeed) {
    int i;
    for(i = 0; i < NOFENEMY; i++) {
        enemies[i][0] = enemies[i][0] - xspeed;
        enemies[i][1] = enemies[i][1] - yspeed;
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

//Checks which switch was toggled: change speed
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



//moves the bird
void move_bird () {
    //to move bird to the left
    if(getbtn(4)) {
        if(xpos >= 0)
            xpos -= 0.25;
    }
    //to move the bird up
    if(getbtn(3)) {
        if(ypos <= (31-BIRDH))
            ypos -= 0.25;
    }
    //to move bird to the right
    if(getbtn(2)) {
        if(xpos <= (127-BIRDW))
            xpos += 0.25;
    }
}



//checks if the bird has collided with: a pipe, a enemy, the borders of the display
int collisions() {
    int i;

//borders for the bird
    int bird_top = ypos;
    int bird_bottom = ypos + BIRDH;
    int bird_left = xpos;
    int bird_right = xpos + BIRDW;
    
    for(i = 0; i < NOFENEMY; i++) {
        int enemy_top = enemies[i][1];
        int enemy_bottom = enemy_top + ENEMYH;
        int enemy_left = enemies[i][0];
        int enemy_right = enemy_left + ENEMYW;

        if ( bird_top == enemy_bottom &&
             bird_bottom == enemy_top &&
             bird_right == enemy_left &&
             bird_left == enemy_right) {
            return 1;
        }
    }

    return 0;
}

///*
// * itoa
// *
// * Simple conversion routine
// * Converts binary to decimal numbers
// * Returns pointer to (static) char array
// *
// * The integer argument is converted to a string
// * of digits representing the integer in decimal format.
// * The integer is considered signed, and a minus-sign
// * precedes the string of digits if the number is
// * negative.
// *
// * This routine will return a varying number of digits, from
// * one digit (for integers in the range 0 through 9) and up to
// * 10 digits and a leading minus-sign (for the largest negative
// * 32-bit integers).
// *
// * If the integer has the special value
// * 100000...0 (that's 31 zeros), the number cannot be
// * negated. We check for this, and treat this as a special case.
// * If the integer has any other value, the sign is saved separately.
// *
// * If the integer is negative, it is then converted to
// * its positive counterpart. We then use the positive
// * absolute value for conversion.
// *
// * Conversion produces the least-significant digits first,
// * which is the reverse of the order in which we wish to
// * print the digits. We therefore store all digits in a buffer,
// * in ASCII form.
// *
// * To avoid a separate step for reversing the contents of the buffer,
// * the buffer is initialized with an end-of-string marker at the
// * very end of the buffer. The digits produced by conversion are then
// * stored right-to-left in the buffer: starting with the position
// * immediately before the end-of-string marker and proceeding towards
// * the beginning of the buffer.
// *
// * For this to work, the buffer size must of course be big enough
// * to hold the decimal representation of the largest possible integer,
// * and the minus sign, and the trailing end-of-string marker.
// * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
// * 64-bit quantities; however, the size of an int on your current compiler
// * may not allow this straight away.
// */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
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

//some type of string concatination to store the scores needed



////Game's main loop
void play() {
    score = 0; //resets the score

//variables needed to set up the clock
    int period = 15;
    int periodcounter = 0;
    int timeoutcounter = 0;
    int scoreperiod = 0;
    
//variables needed for the enemies to move
//WE HAVE TO DO SOMETHING WITH THE SWITCHES HERE
    int xspeed = 1;
    int yspeed = 1;

//    fillenemies();

int playing = 1; //boolean to known when you are playing or not
    
    while(playing) {
 
//movement of bird, clear necessary so moves works properly
        move_bird();
        clear();

//Do stuff with IFS here, timeout, period, scoreperiod, timeout==period move enemy and reset the timeout stuff like that


        draw_enemy(32,2);
        draw_bird();
        render(screen);
        playing = !collisions(); //This is the way to stop the playing
    }
}



//game over screen, here is where you check if you should update your high score, display the player their score, and allow the player to go back to the menu
void game_over() {
    //checks if the score should replace the second place
    if(score < score1 && score > score2) {
        score2 = score;
    }
    
    //checks if the score should replace the highest score
    if(score > score1) {
        score2 = score1;
        score1 = score;
    }
   
    int gameover = 1; // boolean for game over
    
    //reset the position of the bird to initial position
    xpos = 0;
    ypos = 0;
    
//what you would see on your screen
    display_string(0, "WASTED!");
    display_string(1, "your score:");
//    display_string(2, ); //find a way to display the score here
    display_string(3, "BTN1 to go to menu");
    display_update();

//goes back to the menu if btn 1 is pressed
    while(gameover) {
        if(getbtn(1)) {
            gameover = 0;
        }
    }
}

void high_score() {
  //check what to do with the text buffer
    
    
    display_string(0, "--HIGH SCORES--");
//    display_string(2, ); //find a way to put the highest score here
//    display_string(1, ); //find a way to put the second highest score here
    display_string(3, "Start: BTN1");
    display_update();
    
    while(1) {
        if(getbtn(1)) {
            break;
        }
    }
    //sets the variable that checks the high score from the menu to zero
    check_high_score = 0;
}



//this is the menu, when the program starts the first thing you see it's this
void menu() {
    int run_menu = 1; //boolean to know when to run menu and when to stop

//what you see on the display
    display_string(0, "****************");
    display_string(1, "---FLAPPYBIRD---");
    display_string(2, "SWITCH4: Start");
    display_string(3, "BTN1: High Score");
    display_update();

//stops running the menu
    while(run_menu) {
         if(getsw(4)) {
            run_menu = 0;
        }
//stops running the menu and sets boolean to check high score to true
        if(getbtn(1)) {
            run_menu = 0;
            check_high_score = 1;
        }
    }
}



//Infinite loop that switches between the menu, high score, game, and game over
void run() {
    while (1) {
        menu();
//Once you pressed a btn you turn off the menu, if you pressed btn 4 then you also set check high score to true if that is the case, then you will jump to the high score
        if(check_high_score) {
            high_score();
        }
//You don't need to turn on a variable to play the game because you have this as your default
        play();
//Once you collide with something you will go to the game over screen
        game_over();
    }
}
