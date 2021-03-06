//
//  header.h
//
//
//  Created by Victoria Torres on 2/23/19.
//

/* Declare display-related functions from mipslabfunc.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);
uint8_t* pages [];

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

//game
void addPixel(int x, int y, uint8_t *array); //for display
void clear();
void play(void);
void run(void);

//configuration
void configbtn(void);
void configswitch(void);
void configscreen(void);
void user_isr(void);
void configtimer(void);
void configLed(void);
void clearbuffer(void);



