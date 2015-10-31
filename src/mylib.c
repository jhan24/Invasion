#include <stdlib.h>
#include "gamebg.h"

typedef unsigned short u16;
typedef unsigned int u32;
u16 *videoBuffer = (u16*) 0x6000000;

#define SCANLINECOUNTER *(volatile unsigned short *)0x4000006

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

#define OFFSET(r, c) ((c) * 240 + (r))
void setPixel(int, int, u16);
void drawRect(int, int, int, int, u16);
void drawBGRect(int, int, int, int);
void drawHollowRect(int, int, int, int, u16);
void drawImage3(int, int, int, int, const u16*);
void waitForVblank();
int playerLimitCheck(int, int, int, int, int*);
int enemyLimitCheck(int, int, int, int, int, int, int, int);
void delay(int);

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

ENEMY createEnemy(int, int, int);


// A function to set pixel (r, c) to the color passed in.
void setPixel(int x, int y, u16 color) {
	videoBuffer[OFFSET(x, y)] = color;
}

// A function to draw a FILLED rectangle starting at (r, c)
void drawRect(int r, int c, int width, int height, u16 color) {
	for (int y = 0; y < height; y++) {
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[(c + y) * 240 + r];
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
		// for (int x = 0; x < height; x++) {
		// 	setPixel(y + r, x + c, color);
		// }
	}
}

void drawBGRect(int r, int c, int width, int height) {
	for (int y = 0; y < height; y++) {
		DMA[3].src = &gamebg[(c + y) * 240 + r];
		DMA[3].dst = &videoBuffer[(c + y) * 240 + r];
		DMA[3].cnt = width | DMA_ON;
	}
}

// A function to draw a HOLLOW rectangle starting at (r,c)
// NOTE: It has to run in O(w+h) time.
void drawHollowRect(int r, int c, int width, int height, u16 color) {
	for (int x = 0; x < width; x++) {
		setPixel(r + x, c, color);
		setPixel(r + x, c + height - 1, color);

	}
	for (int y = 0; y < height - 1; y++) {
		setPixel(r, c + y, color);
		setPixel(r + width - 1, c + y, color);
	}
}

/* drawimage3
 * A function that will draw an arbitrary sized image
 * onto the screen
 * @param r row to draw the image
 * @param c column to draw the image
 * @param width width of the image
 * @param height height of the image
 * @param image Pointer to the first element of the image.
 */
void drawImage3(int r, int c, int width, int height, const u16* image)
{
	int i;
	int j;
	int k = 0;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			videoBuffer[((c + i) * 240) + (r + j)] = image[k];
			k++;
		}
	}
}

// Waits for SCANLINECOUNTER to hit the vblank area before drawing.
void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

// Limit Check for the Player Ship
int playerLimitCheck(int x, int width, int lowerLimit, int upperLimit, int *whichLimit) {
	if ((x + (width / 2) > upperLimit)) {
		*whichLimit = 1;
		return 1;
	} else if ((x - (width / 2) < lowerLimit)) {
		*whichLimit = 0;
		return 1;
	}
	*whichLimit = 2;
	return 0;
}

// Limit Check for the Enemy Ship
int enemyLimitCheck(int x, int y, int width, int height, int leftLimit, int rightLimit, int lowerLimit, int upperLimit) {
	int xLimit = 0;
	int yLimit = 0;
	if ((x + (width) > rightLimit)) {
		xLimit = 1;
	} else if ((x < leftLimit)) {
		xLimit = 2;
	}

	if ((y < upperLimit)) {
		yLimit = 1;
	} else if ((y + (height) > lowerLimit)) {
		yLimit = 2;
	}
	if (xLimit == 0) {
		if (yLimit == 1) {
			return 3;
		} else if (yLimit == 2) {
			return 4;
		}
	}
	if (yLimit == 0) {
		return xLimit;
	}
	return 0;
}

ENEMY createEnemy(int type, int x, int y) {
	ENEMY temp;
	temp.speedX = 1;
	temp.speedY = 0;
	temp.speedXChange = 0;
	temp.speedYChange = 0;
	temp.height = 9;
	temp.width = 8;
	temp.leftLimit = 2;
	temp.rightLimit = 238;
	temp.lowerLimit = 140;
	temp.upperLimit = 5;
	temp.color = RED;
	temp.x = x;
	temp.y = y;
	temp.fireRate = (rand() % 200) + 5;
	temp.baseFireRate = 200;
	temp.oldFireRate = temp.fireRate;
	temp.arrayPosition = 987654;

	if (type == 1) {
		return temp;
	} else if (type == 2) {
		temp.color = BLUE;
		return temp;
	} else if (type == 3) {
		temp.color = GREEN;
		temp.fireRate = temp.fireRate + 5;
		return temp;
	} else if (type == 4) {
		temp.color = MAGENTA;
		temp.baseFireRate = 100;
		return temp;
	} else if (type == 5) {
		temp.color = YELLOW;
		temp.baseFireRate = 50;
		return temp;
	} else if (type == 6) {
		temp.color = HELL;
		temp.baseFireRate = 25;
		return temp;
	} else if (type == 7) {
		temp.color = CYAN;
		temp.baseFireRate = 100;
	} else if (type == 8) {
		temp.color = WHITE;
		temp.fireRate = temp.fireRate + 5;
		temp.baseFireRate = 100;
	} else if (type == 9) {
		temp.color = PURPLE;
		temp.baseFireRate = 50;
	} else if (type == 10) {
		temp.color = ORANGE;
		temp.fireRate = temp.fireRate + 5;
		temp.baseFireRate = 50;
	}

	return temp;

}

void delay(int n) {
	volatile int x = 0;
	for (int i = 0; i < 1000 * n; i++) {
		x++;
		if (x > 999999) {
			x = 0;
		}
	}
}
