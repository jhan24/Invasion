// Made by Jinsong Han for CS2110
#include <stdlib.h>
#include "title.h"
#include "gameover.h"
#include "levelcomplete.h"
#include "victory.h"
#include "gamebg.h"
#include "playerimg.h"
#include "playerlarge.h"
#include "playersmall.h"
#include "instruction1.h"
#include "instruction2.h"
#include "instruction3.h"

//# typedef for unsigned short
typedef unsigned short u16;
typedef unsigned int u32;
// Display count register
#define REG_DISPCNT *(u16*) 0x4000000

/* DMA */

#define REG_DMA0SAD         *(const volatile u32*)0x40000B0 // source address
#define REG_DMA0DAD         *(volatile u32*)0x40000B4       // destination address
#define REG_DMA0CNT         *(volatile u32*)0x40000B8       // control register

// DMA channel 1 register definitions
#define REG_DMA1SAD         *(const volatile u32*)0x40000BC // source address
#define REG_DMA1DAD         *(volatile u32*)0x40000C0       // destination address
#define REG_DMA1CNT         *(volatile u32*)0x40000C4       // control register

// DMA channel 2 register definitions
#define REG_DMA2SAD         *(const volatile u32*)0x40000C8 // source address
#define REG_DMA2DAD         *(volatile u32*)0x40000CC       // destination address
#define REG_DMA2CNT         *(volatile u32*)0x40000D0       // control register

// DMA channel 3 register definitions
#define REG_DMA3SAD         *(const volatile u32*)0x40000D4 // source address
#define REG_DMA3DAD         *(volatile u32*)0x40000D8       // destination address
#define REG_DMA3CNT         *(volatile u32*)0x40000DC       // control register

typedef struct
{
	const volatile void *src;
	const volatile void *dst;
	u32                  cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

// Defines
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

// Colors
#define RGB(r, g, b) ((r) | (g) << 5 | (b) << 10)
#define BLACK   RGB(0,0,0)
#define WHITE   RGB(31, 31, 31)
#define RED     RGB(31,0,0)
#define GREEN   RGB(0, 31, 0)
#define BLUE    RGB(0,0,31)
#define YELLOW  RGB(31,31,0)
#define CYAN    RGB(0,31,31)
#define MAGENTA RGB(31,0,31)
#define GRAY    RGB(25, 25, 25)
#define PURPLE RGB(15, 15, 31)
#define ORANGE RGB(31, 15, 0)
#define HELL RGB(10, 0, 10)

// Display
#define MODE_3 3
#define BG2_EN (1 << 10)
#define BG1_EN (1 << 9)
#define BG3_EN (1 << 11)

// Buttons
#define BUTTON_A		(1<<0)
#define BUTTON_B		(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT		(1<<5)
#define BUTTON_UP		(1<<6)
#define BUTTON_DOWN		(1<<7)
#define BUTTON_R		(1<<8)
#define BUTTON_L		(1<<9)
#define BUTTONS (*(unsigned int *)0x4000130)
#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)

// Sound
// Defines for the interrupt handler
#define REG_IE        *(volatile unsigned short*) 0x4000200
#define REG_IF        *(volatile unsigned short*) 0x4000202
#define REG_IME       *(volatile unsigned short*) 0x4000208
#define IRQ_ENABLE    1
#define IRQ_VBLANK    (1 << 0)
#define IRQ_HBLANK    (1 << 1)
#define IRQ_VCOUNT    (1 << 2)
#define IRQ_TIMER(n)  (1 << (3 + (n)))
#define IRQ_COM       (1 << 7)
#define IRQ_DMA(n)    (1 << (8 + (n)))
#define IRQ_KEYPAD    (1 << 12)
#define IRQ_CARTRIDGE (1 << 13)
typedef void (*irqptr)(void);
#define REG_ISR_MAIN *(irqptr*) 0x3007FFC

// Defines for the timer
#define REG_TMD(n)   *(volatile unsigned short*) (0x4000100 + ((n) << 2))
#define REG_TMCNT(n) *(volatile unsigned short*) (0x4000102 + ((n) << 2))
#define TM_FREQ_1    0
#define TM_FREQ_64   1
#define TM_FREQ_256  2
#define TM_FREQ_1024 3
#define TM_CASCADE   (1 << 2)
#define TM_IRQ       (1 << 6)
#define TM_ENABLE    (1 << 7)

// Defines for sound control
#define REG_SND1SWP   *(volatile unsigned short*) 0x4000060 
#define REG_SND1ENV   *(volatile unsigned short*) 0x4000062
#define REG_SND1FRQ   *(volatile unsigned short*) 0x4000064
#define REG_SND2ENV   *(volatile unsigned short*) 0x4000068
#define REG_SND2FRQ   *(volatile unsigned short*) 0x400006C
#define REG_SND4ENV   *(volatile unsigned short*) 0x4000078
#define REG_SND4FRQ   *(volatile unsigned short*) 0x400007C
#define REG_SNDDMGCNT *(volatile unsigned short*) 0x4000080
#define REG_SNDSTAT   *(volatile unsigned short*) 0x4000084
#define MASTER_SND_EN (1 << 7)
#define MASTER_VOL25  0
#define MASTER_VOL50  1
#define MASTER_VOL100 2
#define LEFT_VOL(n)   (n)
#define RIGHT_VOL(n)  ((n) << 4)
#define CHAN_EN_L(n)  (1 << ((n) + 7))
#define CHAN_EN_R(n)  (1 << ((n) + 11))

// Function Prototypes
void setPixel(int, int, u16);
void drawRect(int, int, int, int, u16);
void drawBGRect(int, int, int, int);
void drawHollowRect(int, int, int, int, u16);
void drawImage3(int, int, int, int, const u16*);
void waitForVblank();
int playerLimitCheck(int, int, int, int, int*);
int enemyLimitCheck(int, int, int, int, int, int, int, int);
void delay(int);

// Global Variables
extern u16 *videoBuffer;

// Structs
typedef struct {
	int x;
	int y;
	int height;
	int width;
	int leftLimit;
	int rightLimit;
	u16 color;
} PLAYER;

typedef struct {
	int x;
	int y;
	int speedX;
	int speedY;
	int speedXChange;
	int speedYChange;
	int height;
	int width;
	int leftLimit;
	int rightLimit;
	int upperLimit;
	int lowerLimit;
	int fireRate;
	int oldFireRate;
	int baseFireRate;
	int arrayPosition;
	u16 color;
} ENEMY;

typedef struct {
	int x;
	int y;
	int height;
	int width;
	int arrayPosition;
	int speedY;
	int speedX;
	int speedYChange;
	int speedXChange;
	u16 color;
} PLAYERBULLET;

typedef struct {
	int x;
	int y;
	int height;
	int width;
	int arrayPosition;
	int speedY;
	int speedX;
	int speedYChange;
	int speedXChange;
	u16 color;
} ENEMYBULLET;

typedef struct {
	u16 color;
	int flag;
	int x;
	int y;
} POWERUP;

ENEMY createEnemy(int, int, int);

enum GBAState {
	START,
	START_NODRAW,
	START2,
	START3,
	START4,
};

#define drawPlayerRect (drawImage3(player.x - (player.width / 2), player.y, player.width, player.height, playerimg))
#define destroyPlayerRect (drawBGRect(oldPlayer.x - (oldPlayer.width / 2), oldPlayer.y, oldPlayer.width, oldPlayer.height))
// Beginning of actul program
int main(void) {
	REG_SNDSTAT = MASTER_SND_EN; // Enables sound

	// Enables channels and volume
	REG_SNDDMGCNT = CHAN_EN_L(1) | CHAN_EN_R(1) | CHAN_EN_L(2) | CHAN_EN_R(2) | CHAN_EN_L(4) | CHAN_EN_R(4) | LEFT_VOL(7) | RIGHT_VOL(7);

	REG_DISPCNT = MODE_3 | BG2_EN;

	int level = 0;

	// Player ship initialization
	PLAYER player;
	player.x = 120;
	player.y = 140;
	player.color = CYAN;
	player.height = 8;
	player.width = 8;
	player.leftLimit = 0;
	player.rightLimit = 240;

	// Game initialization
	while (1) {
		int returnStart;
		int starting = 1;
		int variable = 0;
		int startOK = 1;
		enum GBAState state = START;
		enum GBAState nextState = START;

		// Title Screen, as well as Instruction screens are here as a state configuration.
		if (level == 0) {
			while(starting) {
		switch(state) {
		case START:
			drawImage3(0, 0, 240, 160, title);
			state = START_NODRAW;
			nextState = START2;
			startOK = 1;
			break;
		case START_NODRAW:
			if (KEY_DOWN_NOW(BUTTON_A) && variable == 0) {
				state = nextState;
				variable = 1;
			} else if (KEY_DOWN_NOW(BUTTON_START) && startOK == 1) {
				starting = 0;
				level = 1;
			} else if (KEY_DOWN_NOW(BUTTON_SELECT)) {
				state = START;
			}
			break;
		case START2:
			drawImage3(0, 0, 240, 160, instruction1);
			state = START_NODRAW;
			startOK = 0;
			variable = 1;
			nextState = START3;
			break;
		case START3:
			drawImage3(0, 0, 240, 160, instruction2);
			state = START_NODRAW;
			nextState = START4;
			variable = 1;
			break;
		case START4:
			drawImage3(0, 0, 240, 160, instruction3);
			state = START_NODRAW;
			nextState = START4;				
			variable = 1;
			break;
		// TODO add more cases (states) to your switch statement!
		}

		if (!KEY_DOWN_NOW(BUTTON_A)) {
			variable = 0;
		}

		// TODO you should probably do something here with one of your variables
	}
		}
		// if (level == 0) {
		// 	drawImage3(0, 0, TITLE_WIDTH, TITLE_HEIGHT, title);
		// }
		// while (level == 0) {
		// 	returnStart = 0;
		// 	if (KEY_DOWN_NOW(BUTTON_START)) {
		// 		level = 1;
		// 		player.color = CYAN;
		// 	} else if (KEY_DOWN_NOW(BUTTON_A)) {
		// 		drawImage3(0, 0, 240, 160, instruction1);
		// 		while
		// 	}
		// }
		drawRect(0, 0, 240, 160, BLACK);

		// Player ship reset all but health
		player.x = 120;
		player.y = 140;
		player.height = 8;
		player.width = 8;
		player.leftLimit = 0;
		player.rightLimit = 240;
		PLAYER oldPlayer;

		// Draws the ship and the ship line for movement
		drawRect(0, 142, 240, 1, MAGENTA);
		drawPlayerRect;
		int whichPlayerLimit = 2;

		int test = 0;

		// Creates the Player Bullet array
		PLAYERBULLET playerBullets[10];
		int playerBulletInitCounter = 0;
		// Fills array with PLAYERBULLET elements
		// arrayPosition for each of the ten is initalized to 987654
		// If this value is 987654, the game knows that there's nothing important in that element
		// And can skip over that element entirely. 
		while (playerBulletInitCounter < 10) {
			PLAYERBULLET placeholder;
			placeholder.arrayPosition = 987654;
			playerBullets[playerBulletInitCounter] = placeholder;
			playerBulletInitCounter++;
		}

		// Counter for the bullet array. Each time a new bullet is fired, it increments by 1.
		int nextPlayerBulletEmpty = 0;

		int enemyCount = 0;
		int enemyBulletLimit = 0;

		// Level generation begins here. Random levels wouldn't have been as fun as these...
		int row1Start = 0;
		int row2Start = 0;
		int row3Start = 0;
		int row4Start = 0;
		int row5Start = 0;
		int row6Start = 0;
		if (level == 1) {
			enemyCount = 4;
			enemyBulletLimit = 25;
			row1Start = 0;
			row2Start = 4;
		} else if (level == 2) {
			enemyCount = 8;
			enemyBulletLimit = 35;
			row1Start = 0;
			row2Start = 8;
		} else if (level == 3) {
			enemyCount = 12;
			enemyBulletLimit = 50;
			row1Start = 0;
			row2Start = 8;
			row3Start = 12;
		} else if (level == 4) {
			enemyCount = 12;
			enemyBulletLimit = 50;
			row1Start = 0;
			row2Start = 8;
			row3Start = 12;
		} else if (level == 8) {
			enemyCount = 16;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 8;
			row3Start = 16;
		} else if (level == 11) {
			enemyCount = 11;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 6;
			row3Start = 11;
		} else if (level == 20) {
			enemyCount = 15;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 5;
			row3Start = 9;
			row4Start = 12;
			row5Start = 14;
			row6Start = 15;
		} else if (level == 6) {
			enemyCount = 10;
			enemyBulletLimit = 100;
			row1Start = 0;
			row2Start = 6;
			row3Start = 10;
		} else if (level == 5) {
			enemyCount = 8;
			enemyBulletLimit = 80;
			row1Start = 0;
			row2Start = 8;
		} else if (level == 7) {
			enemyCount = 12;
			enemyBulletLimit = 120;
			row1Start = 0;
			row2Start = 6;
			row3Start = 12;
		} else if (level == 9) {
			enemyCount = 12;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 5;
			row3Start = 9;
			row4Start = 12;
		} else if (level == 10) {
			enemyCount = 12;
			enemyBulletLimit = 120;
			row1Start = 0;
			row2Start = 6;
			row3Start = 12;
		} else if (level == 12) {
			enemyCount = 13;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 4;
			row3Start = 9;
			row4Start = 13;
		} else if (level == 13) {
			enemyCount = 13;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 4;
			row3Start = 9;
			row4Start = 13;
		} else if (level == 14) {
			enemyCount = 13;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 4;
			row3Start = 9;
			row4Start = 13;
		} else if (level == 15) {
			enemyCount = 12;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 5;
			row3Start = 9;
			row4Start = 12;
		} else if (level == 16) {
			enemyCount = 12;
			enemyBulletLimit = 120;
			row1Start = 0;
			row2Start = 6;
			row3Start = 12;
		} else if (level == 17) {
			enemyCount = 12;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 5;
			row3Start = 9;
			row4Start = 12;
		} else if (level == 18) {
			enemyCount = 13;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 4;
			row3Start = 9;
			row4Start = 13;
		} else if (level == 19) {
			enemyCount = 14;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 5;
			row3Start = 11;
			row4Start = 14;
		} else if (level == 21) {
			enemyCount = 15;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 5;
			row3Start = 9;
			row4Start = 12;
			row5Start = 14;
			row6Start = 15;
		} else if (level == 22) {
			enemyCount = 18;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 5;
			row3Start = 10;
			row4Start = 13;
			row5Start = 17;
			row6Start = 18;
		} else if (level == 23) {
			enemyCount = 18;
			enemyBulletLimit = 150;
			row1Start = 0;
			row2Start = 5;
			row3Start = 10;
			row4Start = 13;
			row5Start = 17;
			row6Start = 18;
		}

		int keepLooping = 1;

		// Creates the Enemy array
		ENEMY enemies[enemyCount];
		// Fills array with 4 enemy elements
		int e = 0;
		if (level == 1) {
			while (e < enemyCount) {
				enemies[e] = createEnemy(1, 40 + (e * 50), 12);
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 2) {
			while (e < enemyCount) {
				if (e != 3 && e != 6) {
					enemies[e] = createEnemy(1, 40 + (e * 25), 12);
				} else {
					enemies[e] = createEnemy(2, 40 + (e * 25), 12);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
			
		} else if (level == 3) {
			while (e < enemyCount) {

				if (e % 2 == 0 && e < row2Start) {
					enemies[e] = createEnemy(1, 40 + (e * 25), 12);
				} else if (e < row2Start) {
					enemies[e] = createEnemy(2, 40 + (e * 25), 12);
				} else {
					enemies[e] = createEnemy(1, 52 + ((e - row2Start) * 50), 32);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 4) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(1, 40 + (e * 25), 12);
				} else {
					enemies[e] = createEnemy(3, 65 + ((e - row2Start) * 35), 32);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 8) {
			while (e < enemyCount) {
				if (e % 2 == 0 && e < row2Start) {
					enemies[e] = createEnemy(1, 28 + (e * 25), 12);
				} else if (e < row2Start) {
					enemies[e] = createEnemy(2, 28 + (e * 25), 12);
				} else if (e % 2 == 0) {
					enemies[e] = createEnemy(3, 40 + ((e - row2Start) * 25), 32);
				} else {
					enemies[e] = createEnemy(1, 40 + ((e - row2Start) * 25), 32);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 11) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(8, 25 + (e * 30), 12);
				} else if (e < row3Start) {
					enemies[e] = createEnemy(3, 40 + ((e - row2Start) * 30), 32);
				}				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 20) {
			while (e < enemyCount) {
				if (e < row2Start) {
						enemies[e] = createEnemy(6, 25 + (e * 36), 12);
					} else if (e < row3Start) {
						enemies[e] = createEnemy(9, 43 + ((e - row2Start) * 36), 32);
					} else if (e < row4Start){
						enemies[e] = createEnemy(10, 61 + ((e - row3Start) * 36), 52);
					} else if (e < row5Start) {
						enemies[e] = createEnemy(9, 79 + ((e - row4Start) * 36), 72);
					} else if (e < row6Start) {
						enemies[e] = createEnemy(6, 97, 92);
					}
					enemies[e].arrayPosition = e;
					e++;
			}
		} else if (level == 6) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(4, 40 + (e * 30), 12);
				} else {
					enemies[e] = createEnemy(1, 65 + ((e - row2Start) * 34), 32);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 5) {
			while (e < enemyCount) {
				enemies[e] = createEnemy(2, 40 + (e * 25), 12);
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 7) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(4, 40 + (e * 26), 12);
				} else {
					enemies[e] = createEnemy(2, 27 + ((e - row2Start) * 30), 32);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 9) {
			while (e < enemyCount) {
				if (e < row2Start) {
						enemies[e] = createEnemy(5, 25 + (e * 36), 12);
					} else if (e < row3Start) {
						enemies[e] = createEnemy(4, 43 + ((e - row2Start) * 36), 32);
					} else if (e < row4Start){
						enemies[e] = createEnemy(3, 61 + ((e - row3Start) * 36), 52);
					}
					enemies[e].arrayPosition = e;
					e++;
			}
		} else if (level == 10) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(7, 40 + (e * 30), 12);
				} else {
					enemies[e] = createEnemy(2, 25 + ((e - row2Start) * 35), 32);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 12) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(7, 40 + (e * 30), 12);
				} else if (e < row3Start) {
					enemies[e] = createEnemy(5, 21 + ((e - row2Start) * 32), 31);
				} else {
					enemies[e] = createEnemy(7, 40 + ((e - row3Start) * 30), 52);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 13) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(6, 40 + (e * 30), 12);
				} else if (e < row3Start) {
					enemies[e] = createEnemy(5, 21 + ((e - row2Start) * 32), 31);
				} else {
					enemies[e] = createEnemy(4, 40 + ((e - row3Start) * 30), 52);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 14) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(9, 40 + (e * 30), 12);
				} else if (e < row3Start) {
					enemies[e] = createEnemy(7, 21 + ((e - row2Start) * 32), 31);
				} else {
					enemies[e] = createEnemy(7, 40 + ((e - row3Start) * 30), 52);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 15) {
			while (e < enemyCount) {
				if (e < row2Start) {
						enemies[e] = createEnemy(3, 25 + (e * 36), 12);
					} else if (e < row3Start) {
						enemies[e] = createEnemy(8, 43 + ((e - row2Start) * 36), 32);
					} else if (e < row4Start){
						enemies[e] = createEnemy(10, 61 + ((e - row3Start) * 36), 52);
					}
					enemies[e].arrayPosition = e;
					e++;
			}
		} else if (level == 16) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(6, 40 + (e * 30), 12);
				} else {
					enemies[e] = createEnemy(6, 25 + ((e - row2Start) * 35), 32);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 17) {
			while (e < enemyCount) {
				if (e < row2Start) {
						enemies[e] = createEnemy(5, 25 + (e * 36), 12);
					} else if (e < row3Start) {
						enemies[e] = createEnemy(10, 43 + ((e - row2Start) * 36), 32);
					} else if (e < row4Start){
						enemies[e] = createEnemy(10, 61 + ((e - row3Start) * 36), 52);
					}
					enemies[e].arrayPosition = e;
					e++;
			}
		} else if (level == 18) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(9, 40 + (e * 30), 12);
				} else if (e < row3Start) {
					enemies[e] = createEnemy(10, 21 + ((e - row2Start) * 32), 31);
				} else {
					enemies[e] = createEnemy(9, 40 + ((e - row3Start) * 30), 52);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 19) {
			while (e < enemyCount) {
				if (e < row2Start) {
					enemies[e] = createEnemy(6, 30 + (e * 30), 12);
				} else if (e < row3Start) {
					enemies[e] = createEnemy(6, 23 + ((e - row2Start) * 27), 31);
				} else {
					enemies[e] = createEnemy(6, 50 + ((e - row3Start) * 40), 52);
				}
				enemies[e].arrayPosition = e;
				e++;
			}
		} else if (level == 24) {
			drawImage3(0, 0, VICTORY_WIDTH, VICTORY_HEIGHT, victory);
			while(level == 23) {
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					returnStart = 1;
					keepLooping = 0;
					level = 0;
				}
			}
		} else if (level == 21) {
			while (e < enemyCount) {
				if (e < row2Start) {
						enemies[e] = createEnemy(10, 25 + (e * 36), 12);
					} else if (e < row3Start) {
						enemies[e] = createEnemy(9, 43 + ((e - row2Start) * 36), 32);
					} else if (e < row4Start){
						enemies[e] = createEnemy(6, 61 + ((e - row3Start) * 36), 52);
					} else if (e < row5Start) {
						enemies[e] = createEnemy(9, 79 + ((e - row4Start) * 36), 72);
					} else if (e < row6Start) {
						enemies[e] = createEnemy(6, 97, 92);
					}
					enemies[e].arrayPosition = e;
					e++;
			}
		} else if (level == 22) {
			while (e < enemyCount) {
				if (e < row2Start) {
						enemies[e] = createEnemy(10, 25 + (e * 36), 12);
					} else if (e < row3Start) {
						enemies[e] = createEnemy(9, 25 + ((e - row2Start) * 36), 32);
					} else if (e < row4Start){
						enemies[e] = createEnemy(6, 43 + ((e - row3Start) * 55), 52);
					} else if (e < row5Start) {
						enemies[e] = createEnemy(10, 31 + ((e - row4Start) * 45), 72);
					} else if (e < row6Start) {
						enemies[e] = createEnemy(6, 97, 92);
					}
					enemies[e].arrayPosition = e;
					e++;
			}
		} else if (level == 23) {
			while (e < enemyCount) {
				if (e < row2Start) {
						enemies[e] = createEnemy(10, 25 + (e * 36), 42);
					} else if (e < row3Start) {
						enemies[e] = createEnemy(9, 25 + ((e - row2Start) * 36), 62);
					} else if (e < row4Start){
						enemies[e] = createEnemy(6, 43 + ((e - row3Start) * 55), 82);
					} else if (e < row5Start) {
						enemies[e] = createEnemy(10, 31 + ((e - row4Start) * 45), 102);
					} else if (e < row6Start) {
						enemies[e] = createEnemy(6, 97, 118);
					}
					enemies[e].arrayPosition = e;
					e++;
			}
		}
 
 
		// Creates the enemy bullet array
		ENEMYBULLET enemyBullets[enemyBulletLimit];
		// Fills array with PLAYERBULLET elements
		// arrayPosition for each of the ten is initalized to 987654
		// If this value is 987654, the game knows that there's nothing important in that element
		// And can skip over that element entirely. 
		int enemyBulletInitCounter = 0;
		while (enemyBulletInitCounter < enemyBulletLimit) {
			ENEMYBULLET placeholder;
			placeholder.arrayPosition = 987654;
			enemyBullets[enemyBulletInitCounter] = placeholder;
			enemyBulletInitCounter++;
		}

		// Counter for the bullet array. Each time a new bullet is fired, it increments by 1.
		int nextEnemyBulletEmpty = 0;

		int loopCounter = 1;
		int gameOver = 0;
		int nextLevel = 0;
		POWERUP power;
		power.color = BLACK;
		power.flag = 0;
		power.x = 0;
		power.y = 0;
		int fireRateLimiter = 20;
		int powerCounter = 0;
		int lagCooldown = 0;
		int heavyLagCooldown = 0;
		DMA[3].src = gamebg;
		DMA[3].dst = videoBuffer;
		DMA[3].cnt = DMA_ON | 38400;

		//------------------------------------------------- Game loop begins here ---------------------------------------------------------------------------------

		while(keepLooping) {
			// Stores current player value into an old object
			int destroyBullet = -1;
			int destroyEnemy = -1;
			int destroyPlayer = -1;
			int destroyEnemyBullet = -1;
			int powerCollision = 0;
			// int row1Reverse = 0;
			// int row2Reverse = 0;
			// int row3Reverse = 0;
			int rowReverse = 0;
			oldPlayer = player;

			// Powerup expires after 750 cycles
			if (powerCounter > 750) {
				powerCounter = 0;
				player.width = 8;
				fireRateLimiter = 20;
			}

			// Movement of player Left and Right, hold X to slow movement.
			if (KEY_DOWN_NOW(BUTTON_LEFT)) {
				if (KEY_DOWN_NOW(BUTTON_B)) {
					player.x = player.x - 1;
				} else {
					player.x = player.x - 3;
				}
			} else if (KEY_DOWN_NOW(BUTTON_RIGHT)){
				if (KEY_DOWN_NOW(BUTTON_B)) {
					player.x = player.x + 1;
				} else {
					player.x = player.x + 3;
				}
			}

			if (KEY_DOWN_NOW(BUTTON_SELECT)) {
				returnStart = 1;
				keepLooping = 0;
				level = 0;
			}

			// Update enemy position
			e = 0;
			int deadEnemies = 0;
			while (e < enemyCount) {
				if (enemies[e].arrayPosition != 987654) {
					enemies[e].x = enemies[e].x + enemies[e].speedX;
					enemies[e].y = enemies[e].y + enemies[e].speedY;
					enemies[e].speedX = enemies[e].speedX + enemies[e].speedXChange;
					enemies[e].speedY = enemies[e].speedY + enemies[e].speedYChange;
					int enemyLimit = enemyLimitCheck(enemies[e].x, enemies[e].y, enemies[e].width, enemies[e].height, enemies[e].leftLimit, enemies[e].rightLimit, enemies[e].lowerLimit, enemies[e].upperLimit);
					if (enemyLimit == 1 || enemyLimit == 2) {
						rowReverse = 1;
						// if (e >= row1Start && e < row2Start) {
						// 	row1Reverse = 1;
						// }
						// if (e >= row2Start && e < row3Start) {
						// 	row2Reverse = 1;
						// }
						// if (e >= row3Start && e < row4Start) {
						// 	row3Reverse = 1;
						// }
					}

					// If the enemies have managed to invade the base, immediate game over.
					if (enemies[e].y > 120) {
						keepLooping = 0;
						gameOver = 1;
					}
				} else {
					deadEnemies++;
					if (deadEnemies == enemyCount) {
						keepLooping = 0;
						nextLevel = 1;
					}
				}
				e++;
			}
			e = 0;
			if (rowReverse == 1) {
				while (e < enemyCount) {
					enemies[e].speedX = enemies[e].speedX * -1;
			 		e++;
				}
			}
			// if (row1Reverse == 1) {
			// 	while (e < row2Start) {
			// 		enemies[e].speedX = enemies[e].speedX * -1;
			// 		e++;
			// 	}
			// }
			// if (row2Reverse == 1) {
			// 	e = row2Start;
			// 	while (e < row3Start) {
			// 		enemies[e].speedX = enemies[e].speedX * -1;
			// 		e++;
			// 	}
			// }
			// if (row3Reverse == 1) {
			// 	e = row3Start;
			// 	while (e < row4Start) {
			// 		enemies[e].speedX = enemies[e].speedX * -1;
			// 		e++;
			// 	}
			// }
	        // Updating Player's Bullets
			int i = 0;
			while (i < enemyBulletLimit) {
				// Makes sure element is valid
				if (playerBullets[i].arrayPosition != 987654) {
					// Updates acceleration and velocity
					playerBullets[i].x = playerBullets[i].x + playerBullets[i].speedX;
					playerBullets[i].y = playerBullets[i].y - playerBullets[i].speedY;
					playerBullets[i].speedX = playerBullets[i].speedX + playerBullets[i].speedXChange;
					playerBullets[i].speedY = playerBullets[i].speedY + playerBullets[i].speedYChange;
					// If the bullet is already off screen, set velocity/accel to 0 and set arrayPosition to 987654
					if (playerBullets[i].y < -50) {
						playerBullets[i].arrayPosition = 987654;
						playerBullets[i].speedY = 0;
						playerBullets[i].speedXChange = 0;
						playerBullets[i].speedYChange = 0;
					}
				}
				i++;
			}
			//Updating Enemy Bullets
			i = 0;
			while (i < enemyBulletLimit) {
				// Makes sure elemnt is valid
				if (enemyBullets[i].arrayPosition != 987654) {
					// Updates acceleration and velocity
					enemyBullets[i].x = enemyBullets[i].x + enemyBullets[i].speedX;
					enemyBullets[i].y = enemyBullets[i].y + enemyBullets[i].speedY;
					enemyBullets[i].speedX = enemyBullets[i].speedX + enemyBullets[i].speedXChange;
					enemyBullets[i].speedY = enemyBullets[i].speedY + enemyBullets[i].speedYChange;
					//If the bullet is already off screen, set velocity/accel to 0 and set arrayPosition to 987654
					if (enemyBullets[i].y > 170) {
						enemyBullets[i].arrayPosition = 987654;
						enemyBullets[i].speedY = 0;
						enemyBullets[i].speedXChange = 0;
						enemyBullets[i].speedYChange = 0;
					}
				}
				i++;
			}

			i = 0;

			// Creates a new player bullet if button A is pressed
			if (KEY_DOWN_NOW(BUTTON_A) && test > fireRateLimiter) {
				REG_SND2ENV = 0xF1B9;
				REG_SND2FRQ = 0x8106;
				PLAYERBULLET newPlayerBullet;
				newPlayerBullet.width = 1;
				newPlayerBullet.height = 5;
				newPlayerBullet.x = player.x + (newPlayerBullet.width / 2);
				newPlayerBullet.y = player.y - newPlayerBullet.height;
				newPlayerBullet.color = YELLOW;
				newPlayerBullet.speedY = 4;
				newPlayerBullet.speedX = 0;
				newPlayerBullet.speedYChange = 0;
				newPlayerBullet.speedXChange = 0;
				if (newPlayerBullet.x < 1) {
					newPlayerBullet.x = 1;
				} else if (newPlayerBullet.x > 239) {
					newPlayerBullet.x = 239;
				}
				newPlayerBullet.arrayPosition = nextPlayerBulletEmpty;
				playerBullets[nextPlayerBulletEmpty] = newPlayerBullet;
				// Increments so next created bullet goes into next spot of array
				nextPlayerBulletEmpty++;
				// If we're at the end of the array, loop back. By this point, the previous spots should
				// contain off-screen bullets.
				if (nextPlayerBulletEmpty > 9) {
					nextPlayerBulletEmpty = 0;
				}
				test = 0;
			} 

			// Loops through enemies and see if new bullet needs to be created
			e = 0;
			while (e < enemyCount) {
				if (loopCounter % enemies[e].fireRate == 0 && enemies[e].arrayPosition != 987654) {
					ENEMYBULLET newEnemyBullet;
					newEnemyBullet.width = 1;
					newEnemyBullet.height = 5;
					newEnemyBullet.x = enemies[e].x + enemies[e].width / 2;
					newEnemyBullet.y = enemies[e].y + newEnemyBullet.height;
					newEnemyBullet.color = GREEN;
					newEnemyBullet.speedY = 3;
					newEnemyBullet.speedX = 0;
					if (enemies[e].color == BLUE || enemies[e].color == CYAN || enemies[e].color == PURPLE) {
						newEnemyBullet.color = MAGENTA;
						newEnemyBullet.speedY = 5;
					} else if (enemies[e].color == GREEN || enemies[e].color == ORANGE || enemies[e].color == WHITE) {
						newEnemyBullet.width = 3;
						newEnemyBullet.height = 8;
						newEnemyBullet.speedY = 2;
						newEnemyBullet.color = WHITE;
						newEnemyBullet.speedX = (newEnemyBullet.x - player.x) / ((newEnemyBullet.y - player.y) / newEnemyBullet.speedY);
					}
					newEnemyBullet.speedYChange = 0;
					newEnemyBullet.speedXChange = 0;
					newEnemyBullet.arrayPosition = nextEnemyBulletEmpty;
					enemyBullets[nextEnemyBulletEmpty] = newEnemyBullet;
					// Increments so next created bullet goes into next spot of array
					nextEnemyBulletEmpty++;
					// If we're at the end of the array, loop back. By this point, the previous spots should
					// contain off-screen bullets.
					if (nextEnemyBulletEmpty > enemyBulletLimit - 1) {
						nextEnemyBulletEmpty = 0;
					}
				}
				e++;
			}

			// Checking if player is trying to move out of bounds. Can't let that happen!
			if (playerLimitCheck(player.x, player.width, player.leftLimit, player.rightLimit, &whichPlayerLimit)) {
				if (whichPlayerLimit == 0) {
					player.x = 0 + (player.width / 2);
				} else {
					player.x = 240 - (player.width / 2);
				}
			}

			int fireRateChanged = -1;
			// Prevent loop counter overflow, also uses this as a chance to change the pattern!
			loopCounter++;
			if (loopCounter > (25 + (rand() % 175))) {
				loopCounter = 1;
				e = 0;
				fireRateChanged = 1;
				while (e < enemyCount) {
					enemies[e].oldFireRate = enemies[e].fireRate;
					if (enemies[e].color == GREEN || enemies[e].color == ORANGE || enemies[e].color == WHITE) {
						enemies[e].fireRate = rand() % enemies[e].baseFireRate + 10;
					} else {
						enemies[e].fireRate = rand() % enemies[e].baseFireRate + 5;
					}
					e++;
				}
			}
			// ---------------------------------------------------COLLISIONS!------------------------------------------------------------------------------------------------

			// Player Bullet vs. Enemy
			int ee = 0;
			int pb = 0;
			while (ee < enemyCount) {
				if (enemies[ee].arrayPosition != 987654) {
					pb = 0;
					while (pb < 10) {
						if (playerBullets[pb].arrayPosition != 987654) {
							if (enemies[ee].x < playerBullets[pb].x + playerBullets[pb].width &&
								enemies[ee].x + enemies[ee].width > playerBullets[pb].x &&
								enemies[ee].y + enemies[ee].height > playerBullets[pb].y &&
								enemies[ee].y < playerBullets[pb].y + playerBullets[pb].height) {
								REG_SND4ENV = 0xF21F;
								REG_SND4FRQ = 0x8061;
								destroyBullet = pb;
								destroyEnemy = ee;	

								if (power.color == BLACK) {
									int chance = rand() % 50;
									if (chance <= 2) {
										power.color = YELLOW;
									} else if (chance <= 5) {
										power.color = RED;
									} else if (chance <= 8) {
										power.color = CYAN;
									} else if (chance <= 11) {
										power.color = PURPLE;
									} else if (chance <= 15) {
										power.color = GREEN;
									}
									power.x = enemies[ee].x;
									power.y = enemies[ee].y;
								}
								
							}
						}
						pb++;
					}
				}
				ee++;
			}

			// Enemy Bullet vs. Player
			int eb = 0;
			while (eb < enemyBulletLimit) {
				if (enemyBullets[eb].arrayPosition != 987654) {
					if (player.x - (player.width / 2) < enemyBullets[eb].x + enemyBullets[eb].width &&
							player.x + (player.width / 2) > enemyBullets[eb].x &&
							player.y + player.height > enemyBullets[eb].y &&
							player.y < enemyBullets[eb].y + enemyBullets[eb].height) {
								REG_SND4ENV = 0xF21F;
								REG_SND4FRQ = 0x8081;
								destroyEnemyBullet = eb;
								destroyPlayer = 1;	
					}
				}
				eb++;
			}


			// Powerup vs. Player
			if (power.color != BLACK) {
				if (player.x - (player.width / 2) < power.x + 5 &&
					player.x + (player.width / 2) > power.x &&
					player.y + player.height > power.y &&
					player.y < power.y + 5) {
					powerCollision = 1;
				} 
				if (power.y >= 165) {
					power.x = 0;
					power.y = 0;
					power.color = BLACK;
				}
			}

			test++;
			if (powerCounter >= 1) {
				powerCounter++;
			}


			// --------------------------------------------------------- DRAWING -----------------------------------------------------------------------
			// All Drawing Functions come AFTER this line!
			waitForVblank();
			i = 0;
			// DMA[3].src = gamebg;
			// DMA[3].dst = videoBuffer;
			// DMA[3].cnt = DMA_ON | 38400;
			// Draw Powerup
			if (powerCollision == 1) {
				// Covers original hollow rect
				drawBGRect(power.x, power.y, 5, 5);

				// Can't have two powerups at the same time.
				fireRateLimiter = 20;
				player.width = 8;

				// Conditional checking
				if (power.color == YELLOW) {
					player.width = 4;
					REG_SND1SWP = 0x75;
					REG_SND1ENV = 0xF29F;
					REG_SND1FRQ = 0x860C;
				} else if (power.color == RED) {
					player.width = 12;
					REG_SND1SWP = 0x7D;
					REG_SND1ENV = 0xF29F;
					REG_SND1FRQ = 0x860C;
				} else if (power.color == CYAN) {
					fireRateLimiter = 10;
					REG_SND1SWP = 0x75;
					REG_SND1ENV = 0xF29F;
					REG_SND1FRQ = 0x860C;
				} else if (power.color == PURPLE) {
					fireRateLimiter = 30;
					REG_SND1SWP = 0x7D;
					REG_SND1ENV = 0xF29F;
					REG_SND1FRQ = 0x860C;
				} else if (power.color == GREEN) {
					if (player.color == GREEN) {
						player.color = CYAN;
					} else if (player.color == YELLOW) {
						player.color = GREEN;
					} else if (player.color == ORANGE) {
						player.color = YELLOW;
					} else if (player.color == RED) {
						player.color = ORANGE;
					}
					REG_SND1SWP = 0x75;
					REG_SND1ENV = 0xF29F;
					REG_SND1FRQ = 0x860C;
				}
				// Cleanup
				power.color = BLACK;
				power.x = 0;
				power.y = 0;
				powerCollision = 0;
				powerCounter = 1;
			}

			// Draw enemies
			e = 0;

			while (e < enemyCount) {
				if (enemies[e].arrayPosition != 987654) {
					if (rowReverse == 0) {
						drawBGRect(enemies[e].x - (enemies[e].speedX - enemies[e].speedXChange), enemies[e].y + (enemies[e].speedY - enemies[e].speedYChange), enemies[e].width, enemies[e].height);
			 			drawRect(enemies[e].x, enemies[e].y, enemies[e].width, enemies[e].height, enemies[e].color);
					} else {
						drawBGRect(enemies[e].x - (-enemies[e].speedX - enemies[e].speedXChange), enemies[e].y + (enemies[e].speedY - enemies[e].speedYChange), enemies[e].width, enemies[e].height);
			 			enemies[e].y = enemies[e].y + 1;
						drawRect(enemies[e].x, enemies[e].y, enemies[e].width, enemies[e].height, enemies[e].color);
					}
				}
				e++;
			}
			// int row1Y = -1;
			// int row2Y = -1;
			// while (e < enemyCount) {
			// 	while (e >= row1Start && e < row2Start) {
			// 		if (enemies[e].arrayPosition != 987654) {
			// 			if (row1Reverse == 0) {
			// 				drawRect(enemies[e].x - (enemies[e].speedX - enemies[e].speedXChange), enemies[e].y + (enemies[e].speedY - enemies[e].speedYChange), enemies[e].width, enemies[e].height, BLACK);
			// 				drawRect(enemies[e].x, enemies[e].y, enemies[e].width, enemies[e].height, enemies[e].color);
			// 			} else {
			// 				drawRect(enemies[e].x - (-enemies[e].speedX - enemies[e].speedXChange), enemies[e].y + (enemies[e].speedY - enemies[e].speedYChange), enemies[e].width, enemies[e].height, BLACK);
			// 				enemies[e].y = enemies[e].y + 1;
			// 				drawRect(enemies[e].x, enemies[e].y, enemies[e].width, enemies[e].height, enemies[e].color);
			// 			}
			// 			row1Y = enemies[e].y;
			// 		}
			// 		e++;
			// 	}
			// 	while (e >= row2Start && e < row3Start) {
			// 		if (enemies[e].arrayPosition != 987654) {
			// 			if (row2Reverse == 0) {
			// 				drawRect(enemies[e].x - (enemies[e].speedX - enemies[e].speedXChange), enemies[e].y + (enemies[e].speedY - enemies[e].speedYChange), enemies[e].width, enemies[e].height, BLACK);
			// 				drawRect(enemies[e].x, enemies[e].y, enemies[e].width, enemies[e].height, enemies[e].color);
			// 			} else {
			// 				drawRect(enemies[e].x - (-enemies[e].speedX - enemies[e].speedXChange), enemies[e].y + (enemies[e].speedY - enemies[e].speedYChange), enemies[e].width, enemies[e].height, BLACK);
			// 				if (row1Y != -1) {
			// 					if (row1Y - enemies[e].y < -20) {
			// 						enemies[e].y = enemies[e].y;
			// 					} else if (row1Y - enemies[e].y < 20) {
			// 						enemies[e].y = enemies[e].y + 3;
			// 					}
			// 				}
			// 				drawRect(enemies[e].x, enemies[e].y, enemies[e].width, enemies[e].height, enemies[e].color);
			// 			}
			// 		}
			// 		e++;
			// 	}
			// }
			i = 0;
			// Drawing player's bullets
			while (i < 10) {
				if (playerBullets[i].arrayPosition != 987654) {
					PLAYERBULLET temp = playerBullets[i];
					//PHYSICS!
					drawBGRect(temp.x - (temp.speedX - temp.speedXChange), temp.y + (temp.speedY - temp.speedYChange), temp.width, temp.height);
					drawRect(temp.x, temp.y, temp.width, temp.height, YELLOW);
				}
				i++;
			}

			i = 0;
			// Drawing enemy bullets
			while (i < enemyBulletLimit) {
				if (enemyBullets[i].arrayPosition != 987654) {
					ENEMYBULLET temp = enemyBullets[i];
					//PHYSICS!
					drawBGRect(temp.x - (temp.speedX - temp.speedXChange), temp.y - (temp.speedY - temp.speedYChange), temp.width, temp.height);
					drawRect(temp.x, temp.y, temp.width, temp.height, temp.color);
				}
				i++;
			}

			// Drawing powerup
			if (power.color != BLACK) {
				drawBGRect(power.x, power.y, 5, 5);
				power.y = power.y + 1;
				drawHollowRect(power.x, power.y, 5, 5, power.color);
			}

			// Destroy enemy if hit
			if (destroyEnemy != -1) {
				e = destroyEnemy;
				drawBGRect(enemies[e].x, enemies[e].y, enemies[e].width, enemies[e].height);
				enemies[e].speedX = 0;
				enemies[e].speedY = 0;
				enemies[e].speedXChange = 0;
				enemies[e].speedYChange = 0;
				PLAYERBULLET temp = playerBullets[destroyBullet];
				drawBGRect(temp.x, temp.y, temp.width, temp.height);
				playerBullets[destroyBullet].speedY = 4;
				playerBullets[destroyBullet].speedX = 0;
				playerBullets[destroyBullet].speedYChange = 0;
				playerBullets[destroyBullet].speedXChange = 0;
				enemies[e].arrayPosition = 987654;
				playerBullets[destroyBullet].arrayPosition = 987654;
			}

			// Game over if player is hit
			if (destroyPlayer == 1) {
				if (player.color == CYAN) {
					player.color = GREEN;
				} else if (player.color == GREEN) {
					player.color = YELLOW;
				} else if (player.color == YELLOW) {
					player.color = ORANGE;
				} else if (player.color == ORANGE) {
					player.color = RED;
				} else if (player.color == RED) {
					gameOver = 1;
					keepLooping = 0;
				}

				ENEMYBULLET temp = enemyBullets[destroyEnemyBullet];
				drawBGRect(temp.x, temp.y, temp.width, temp.height);
				enemyBullets[destroyEnemyBullet].speedY = 0;
				enemyBullets[destroyEnemyBullet].speedX = 0;
				enemyBullets[destroyEnemyBullet].speedYChange = 0;
				enemyBullets[destroyEnemyBullet].speedXChange = 0;
				enemyBullets[destroyEnemyBullet].arrayPosition = 987654;

			}
			destroyPlayerRect;
			drawRect(0, 143, 240, 1, player.color);
			if (player.width == 8) {
				drawImage3(player.x - (player.width / 2), player.y, 8, 8, playerimg);
			} else if (player.width == 4) {
				drawImage3(player.x - (player.width / 2), player.y, 4, 8, playersmall);
			} else {
				drawImage3(player.x - (player.width / 2), player.y, 12, 8, playerlarge);
			}


			
			if (KEY_DOWN_NOW(BUTTON_L) && lagCooldown < 250) {
				delay(8);
				lagCooldown = lagCooldown + 4;
				if (lagCooldown > 240) {
					lagCooldown = 600;
				}
			} else if (KEY_DOWN_NOW(BUTTON_R) && lagCooldown < 250) {
				delay(4);
				lagCooldown = lagCooldown + 2;
				if (lagCooldown > 240) {
					lagCooldown = 600;
				}
			} else {
				if (lagCooldown != 0) {
					lagCooldown--;
					heavyLagCooldown--;
				}
			}




			
		}

		if (gameOver == 1) {
			drawImage3(0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, gameover);
			player.color = CYAN;
			while(gameOver) {
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					gameOver = 0;
					level = 0;
				} else if (KEY_DOWN_NOW(BUTTON_START)) {
					gameOver = 0;
				}
			}
		}
		if (nextLevel == 1) {
			drawImage3(0, 0, LEVELCOMPLETE_WIDTH, LEVELCOMPLETE_HEIGHT, levelcomplete);
			while(nextLevel) {
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					nextLevel = 0;
					level = 0;
				} else if (KEY_DOWN_NOW(BUTTON_START)) {
					level++;
					nextLevel = 0;
				}
			}
		}
	}
}

