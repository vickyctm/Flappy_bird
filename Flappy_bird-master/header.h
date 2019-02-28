/*
Programs's header file
Written by Victoria Torres and Manuel Rydholm
*/

//all configurations needed
void configbtn(void);
void configscreen(void);
volatile int difficulty;
void configtimer2(void);
void configtimer3(void);
void configtimer4(void);
void user_isr(void);

//all display related functions
uint8_t spi_send_recv(uint8_t data);
void display_init(void);
void display_update(void);
void display_string(int line, char *s);

void display_characters(int line, char *s);

extern const uint8_t const font[128*8];
extern char textbuffer[4][16];

//all game related functions
void addPixel(int x, int y, uint8_t *array);
void clear_text(void);
void clear();
void play(void);
void run(void);
