/*
 * nslib.h --
 *
 *	Declarations of public and private features of nslib,
 *	a library for programs running on the ARM7500-based
 *	Netslate Development Board.
 *
 * Copyright (c) 1996 Ben Williamson.
 * All rights reserved.
 *
 * This file is part of nslib, a library used by programs
 * running on the Netslate Development Board.
 *
 * This software is released under the GNU Public License.
 * See the file COPYING for more information.
 */

#ifndef _NSLIB_H
#define _NSLIB_H

#include <ioregs.h>
#include <vidregs.h>


/* constants */

#ifndef EXTERN
# ifdef __cplusplus
#  define EXTERN	extern "C"
# else
#  define EXTERN	extern
# endif
#endif

#define NULL		0

#define IRQ_MAX_NUM	37

#define IRQ_INT2	0		/* INT2 rising edge */
#define IRQ_NINT1	2		/* nINT1 falling edge */
#define IRQ_FLYBACK	3		/* video frame flyback */
#define IRQ_POR		4		/* Power On Reset */
#define IRQ_TIMER0	5		/* 2MHz timer 0 */
#define IRQ_TIMER1	6		/* 2MHz timer 1 */
#define IRQ_ALWAYS	7		/* always active */

#define IRQ_NINT8	8		/* nINT8 active low */
#define IRQ_INT7	9		/* INT7 active high */
#define IRQ_NINT6	10		/* nINT6 active low */
#define IRQ_INT5	11		/* INT5 active high */
#define IRQ_NINT4	12		/* nINT4 active low */
#define IRQ_NINT3	13		/* nINT3 active low */
#define IRQ_KEYBT	14		/* keyboard transmit */
#define IRQ_KEYBR	15		/* keyboard receive */

#define IRQ_IOP0	16		/* IOP[0] active low */
#define IRQ_IOP1	17		/* IOP[1] active low */
#define IRQ_IOP2	18		/* IOP[2] active low */
#define IRQ_IOP3	19		/* IOP[3] active low */
#define IRQ_IOP4	20		/* IOP[4] active low */
#define IRQ_IOP5	21		/* IOP[5] active low */
#define IRQ_IOP6	22		/* IOP[6] active low */
#define IRQ_IOP7	23		/* IOP[7] active low */

#define IRQ_MSER	24		/* mouse receive */
#define IRQ_MSET	25		/* mouse transmit */
#define IRQ_ATOD	26		/* A-to-D comparators */
#define IRQ_NEVENT1	27		/* nEVENT1 active low */
#define IRQ_NEVENT2	28		/* nEVENT2 active low */

#define IRQ_SOUND	36		/* sound DMA */

#define HIBYTE(x)	((unsigned char)((x) >> 8))
#define LOBYTE(x)	((unsigned char)(x))

#define Random(n)	(random() % (n))
#define Suspend()	{SUSMODE = 0x01;}

#define _bottomOfHeapLogical _end

/*
 * Macros to access video dimensions and call video functions
 * through the configuration struct:
 */

#define vidMaxX				(vidConfigPtr->maxX)
#define vidMaxY				(vidConfigPtr->maxY)
#define vidMaxCol			(vidConfigPtr->maxCol)
#define vidBufSize			(vidConfigPtr->bufSize)
#define VidStart()			(vidConfigPtr->startProc)()
#define VidClear()			(vidConfigPtr->clearProc)()
#define VidDrawPage(p)			(vidConfigPtr->drawPageProc)(p)
#define VidDisplayPage(p)		(vidConfigPtr->displayPageProc)(p)
#define VidFrameIsr			(vidConfigPtr->frameIsrProc)
#define VidSync()			(vidConfigPtr->syncProc)()
#define VidShapeCursor(p)		(vidConfigPtr->shapeCursorProc) \
						((p))
#define VidMoveCursor(x, y)		(vidConfigPtr->moveCursorProc) \
						((x), (y))
#define VidGetCol(r, g, b)		(vidConfigPtr->getColProc) \
						((r), (g), (b))
#define VidSetRGB(c, r, g, b)		(vidConfigPtr->setRGBProc) \
						((c), (r), (g), (b))
#define VidPixel(x, y, c)		(vidConfigPtr->pixelProc) \
						((x), (y), (c))
#define VidHLine(x1, x2, y, c)		(vidConfigPtr->hlineProc) \
						((x1), (x2), (y), (c))
#define VidVLine(x, y1, y2, c)		(vidConfigPtr->vlineProc) \
						((x), (y1), (y2), (c))
#define VidLine(x1, y1, x2, y2, c)	(vidConfigPtr->lineProc) \
						((x1), (y1), (x2), (y2), (c))
#define VidRect(x1, y1, x2, y2, c)	(vidConfigPtr->rectProc) \
						((x1), (y1), (x2), (y2), (c))
#define VidTriangle(x1,y1,x2,y2,x3,y3,c) \
					(vidConfigPtr->triangleProc) \
						((x1), (y1), (x2), (y2), \
						(x3), (y3), (c))
#define VidCircle(x, y, r, c)		(vidConfigPtr->circleProc) \
						((x), (y), (r), (c))
#define VidFillRect(x1,y1,x2,y2,c)	(vidConfigPtr->fillRectProc) \
						((x1), (y1), (x2), (y2), (c))
#define VidFillTriangle(x1,y1,x2,y2,x3,y3,c) \
					(vidConfigPtr->fillTriangleProc) \
						((x1), (y1), (x2), (y2), \
						(x3), (y3), (c))
#define VidFillCircle(x, y, r, c)	(vidConfigPtr->fillCircleProc) \
						((x), (y), (r), (c))

#define VidChar(c, x, y, f, b)		(vidConfigPtr->charProc) \
						((c), (x), (y), (f), (b))
#define VidText(s, x, y, f, b)		(vidConfigPtr->textProc) \
						((s), (x), (y), (f), (b))



typedef void (VidStartProc)(void);
typedef void (VidClearProc)(void);
typedef void (VidDrawPageProc)(int page);
typedef void (VidDisplayPageProc)(int page);
typedef void (VidFrameIsrProc)(void);
typedef int  (VidSyncProc)(void);
typedef void (VidShapeCursorProc)(char *xpmPtr[]);
typedef void (VidMoveCursorProc)(int x, int y);
typedef int  (VidGetColProc)(int red, int green, int blue);
typedef void (VidSetRGBProc)(int col, int red, int green, int blue);
typedef void (VidPixelProc)(int x, int y, int col);
typedef void (VidHLineProc)(int x1, int x2, int y, int col);
typedef void (VidVLineProc)(int x, int y1, int y2, int col);
typedef void (VidLineProc)(int x1, int y1, int x2, int y2, int col);
typedef void (VidRectProc)(int x1, int y1, int x2, int y2, int col);
typedef void (VidTriangleProc)(int x1, int y1, int x2, int y2,
					int x3, int y3, int col);
typedef void (VidCircleProc)(int cx, int cy, int r, int col);
typedef void (VidFillRectProc)(int x1, int y1, int x2, int y2, int col);
typedef void (VidFillTriangleProc)(int x1, int y1, int x2, int y2,
					int x3, int y3, int col);
typedef void (VidFillCircleProc)(int cx, int cy, int r, int col);
typedef void (VidCharProc)(unsigned char ch, int x, int y,
                                        int fgcol, int bgcol);
typedef void (VidTextProc)(char *s, int x, int y,
                                        int fgcol, int bgcol);


typedef struct {
	int 			maxX;
	int 			maxY;
	int 			maxCol;
	int			bufSize;
	VidStartProc		*startProc;
	VidClearProc		*clearProc;
	VidDrawPageProc		*drawPageProc;
	VidDisplayPageProc	*displayPageProc;
	VidFrameIsrProc		*frameIsrProc;
	VidSyncProc		*syncProc;
	VidShapeCursorProc	*shapeCursorProc;
	VidMoveCursorProc	*moveCursorProc;
	VidGetColProc		*getColProc;
	VidSetRGBProc		*setRGBProc;
	VidPixelProc		*pixelProc;
	VidHLineProc		*hlineProc;
	VidVLineProc		*vlineProc;
	VidLineProc		*lineProc;
	VidRectProc		*rectProc;
	VidTriangleProc		*triangleProc;
	VidCircleProc		*circleProc;
	VidFillRectProc		*fillRectProc;
	VidFillTriangleProc	*fillTriangleProc;
	VidFillCircleProc	*fillCircleProc;
	VidCharProc		*charProc;
	VidTextProc		*textProc;
} VidConfig;

/* data structures */

typedef unsigned int uint;
typedef unsigned long int size_t;
typedef volatile unsigned int ureg;
typedef unsigned char byte;

/*
 * Pointer to function with no arguments returning int
 */
typedef int (*PFI)(void);

/*
 * Pointer to function with no arguments returning void 
 */
typedef void (*PFV)(void);

/*
 * Pointer to function with a pointer argument returning void 
 */
typedef void (*PTV)(void *);


/*
 * A block on the heap
 */

typedef struct _HeapBlock {
	uint length;			/* bytes including header */
	struct _HeapBlock *next;	/* next block in heap */
	struct _HeapBlock *prev;	/* previous block in heap */
} _HeapBlock;


/*
 * Declarations of memory map constants from memmap.S:
 */

EXTERN int	_topOfStackPhysical;
EXTERN int	_l1TableStartPhysical;
EXTERN int	_bottomOfHeapLogical;
EXTERN int	_topOfHeapLogical;
EXTERN int	_bottomOfStackLogical;
EXTERN int	_topOfStackLogical;
EXTERN int	_l1TableStartLogical;
EXTERN int	_topOfMemoryLogical;
EXTERN int	_l1TableStart;
EXTERN int	_romStartPhysical;
EXTERN int	_memStartPhysical;
EXTERN int	_ram0StartPhysical;
EXTERN int	_ram1StartPhysical;
EXTERN int	_ram2StartPhysical;
EXTERN int	_ram3StartPhysical;
EXTERN int	_pcioStartPhysical;
EXTERN int	_armioStartPhysical;
EXTERN int	_sioStartPhysical;
EXTERN int	_vidStartPhysical;
EXTERN int	_memStartLogical;
EXTERN int	_ram0StartLogical;
EXTERN int	_ram1StartLogical;
EXTERN int	_ram2StartLogical;
EXTERN int	_ram3StartLogical;
EXTERN int	_pcioStartLogical;
EXTERN int	_armioStartLogical;
EXTERN int	_sioStartLogical;
EXTERN int	_vidStartLogical;
EXTERN int	_romStartLogical;

EXTERN _HeapBlock *_heapStart;
EXTERN _HeapBlock *_heapLimit;

/*
 * Video buffers, cursors and fonts:
 */
EXTERN char _vid1Display0Logical;
EXTERN char _vid1Display1Logical;
EXTERN char _vid1Display0Physical;
EXTERN char _vid1Display1Physical;

EXTERN char _vid2Display0Logical;
EXTERN char _vid2Display1Logical;
EXTERN char _vid2Display0Physical;
EXTERN char _vid2Display1Physical;

EXTERN char _vid3Display0Logical;
EXTERN char _vid3Display1Logical;
EXTERN char _vid3Display0Physical;
EXTERN char _vid3Display1Physical;

EXTERN char _vid4Display0Logical;
EXTERN char _vid4Display1Logical;
EXTERN char _vid4Display0Physical;
EXTERN char _vid4Display1Physical;

EXTERN char _vidCursorLogical;
EXTERN char _vidCursorPhysical;
 
EXTERN char *cursorPixmap[];
EXTERN unsigned char fixedFont[];

/*
 * Other global variables and constants
 */
EXTERN VidConfig *vidConfigPtr;

EXTERN VidConfig vid1Config;
EXTERN VidConfig vid2Config;
EXTERN VidConfig vid3Config;
EXTERN VidConfig vid4Config;

EXTERN volatile int mouseX, mouseY, mouseB;

EXTERN uint const mapTbl[];
EXTERN uint const unmapTbl[];


/* procedures */


/*
 * From debug/
 */

EXTERN uint	_getcpsr(void);
EXTERN void	_setcpsr(uint cpsr);
EXTERN uint	_getsp(void);
EXTERN void	halt(void);
EXTERN char *	number(unsigned int n);
EXTERN void	print(const char *s);
EXTERN void	putchar(char c);



/*
 * From intrpt/
 */
 
EXTERN void	IntEnable(void);
EXTERN void	IntDisable(void);
EXTERN void	IrqInit(void);
EXTERN void	IrqEnable(int irqNum);
EXTERN void	IrqDisable(int irqNum);
EXTERN PFV	IrqInstall(int irqNum, PFV isrProc);
EXTERN void	IrqDispatch(void);


/*
 * From mem/
 */

EXTERN void	_MmuInitTable(void);
EXTERN void	HeapInit(void);
EXTERN void *	malloc(size_t size);
EXTERN void	free(void *ptr);


/*
 * From misc/
 */
 
EXTERN void	_main(void);
EXTERN uint	random(void);

/*
 * From mouse/
 */

EXTERN void	MouseInit(void);
EXTERN void	MouseIsr(void);

 

/*
 * From serial/
 */

EXTERN void	SerialInit(int baud);
EXTERN void	SerialSend(char ch);
EXTERN char  	SerialRecv(void);
EXTERN int	SerialReady(void);
EXTERN void	SerialIsr(void);


 

/*
 * From string/
 */
 
EXTERN void	_clear(void *start, int len);
EXTERN void	bzero(void *s, int n);


/*
 * From video/
 */
 
EXTERN void	VidInit(VidConfig *configPtr);


#endif /* _NSLIB_H */
