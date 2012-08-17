/* 
 * vid2.c --
 *
 *	Video display functions for video mode 1 - CRT 320x240x24bit
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

#include <nslib.h>

#define VID2_WIDTH	320
#define VID2_HEIGHT	240
#define VID2_COLS	(256 * 256 * 256)
#define VID2_SIZE	(VID2_WIDTH * VID2_HEIGHT * 4)

/*
 * Static functions in this file:
 */

static void	Vid2Start();
static void	Vid2Clear();
static void	Vid2DrawPage(int page);
static void	Vid2DisplayPage(int page);
static void	Vid2FrameIsr(void);
static int	Vid2Sync(void);
static void	Vid2ShapeCursor(char *xpmPtr[]);
static void	Vid2MoveCursor(int x, int y);  
static int	Vid2GetCol(int red, int green, int blue);
static void	Vid2SetRGB(int col, int red, int green, int blue);
static void	Vid2Pixel(int x, int y, int col);
static void	Vid2HLine(int x1, int x2, int y, int col);
static void	Vid2VLine(int x, int y1, int y2, int col);
static void	Vid2Line(int x1, int y1, int x2, int y2, int col);
static void	Vid2Rect(int x1, int y1, int x2, int y2, int col);
static void	Vid2Triangle(int x1, int y1, int x2, int y2,
						int x3, int y3, int col);
static void	Vid2Circle(int cx, int cy, int r, int col);
static void	Vid2FillRect(int x1, int y1, int x2, int y2, int col);
static void	Vid2FillTriangle(int x1, int y1, int x2, int y2,
						int x3, int y3, int col);
static void	Vid2FillCircle(int cx, int cy, int r, int col);
static void	Vid2Char(unsigned char ch, int x, int y, int fgcol, int bgcol);
static void	Vid2Text(char *s, int x, int y, int fgcol, int bgcol);



VidConfig vid2Config = {
	VID2_WIDTH,
	VID2_HEIGHT,
	VID2_COLS,
	VID2_SIZE,
	Vid2Start,
	Vid2Clear,
	Vid2DrawPage,
	Vid2DisplayPage,
	Vid2FrameIsr,
	Vid2Sync,
	Vid2ShapeCursor,
	Vid2MoveCursor,
	Vid2GetCol,
	Vid2SetRGB,
	Vid2Pixel,
	Vid2HLine,
	Vid2VLine,
	Vid2Line,
	Vid2Rect,
	Vid2Triangle,
	Vid2Circle,
	Vid2FillRect,
	Vid2FillTriangle,
	Vid2FillCircle,
	Vid2Char,
	Vid2Text
};



static char *drawPages[2] = {&_vid2Display0Logical, &_vid2Display1Logical};
static char *displayPages[2] = {&_vid2Display0Physical, &_vid2Display1Physical};

static int *drawPage = (int *)&_vid2Display0Logical;
static char *displayPage = &_vid2Display0Physical;

volatile int frameCount = 0;

static int nextPage = 0;


#define SWAP(a,b)	{		\
	tmp = (a);			\
	(a) = (b);			\
	(b) = tmp;			\
}
	



/*
 *----------------------------------------------------------------------
 *
 * Vid2Start --
 *
 *	Initialise the CRT display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The ARM video registers are set up for display on
 *	an attached PC VGA monitor, in 320x240x256 mode.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Start()
{
	int i;

	/*
	 * Set up display format for 320x240x24bit
	 */

	VID(CONREG, CONREG_VCLK|CONREG_CK_DIV_1|CONREG_BPP_32|CONREG_FIFO_28);

	/*
	 * Reset the phase comparator   9-14, 11-2, E-2
	 * Want dot clock of 8 ??? MHz for 320x240
	 */

#define VID2_VMOD	8
#define VID2_RMOD	32
#define VID2_VR		(((VID2_VMOD-1) << 8) | (VID2_RMOD - 1))


	VID(FrequencySynthesisRegister, 0x80C0 | VID2_VR);
	VID(FrequencySynthesisRegister, 0x0040 | VID2_VR);
	for (i = 0; i < 1000000; i++)
		;
	VID(FrequencySynthesisRegister, VID2_VR);
	for (i = 0; i < 1000000; i++)
		;
	
	/*
	 * The HSYNC and VSYNC signals get inverted before they 
	 * get to the monitor, so this is actually negative sync
	 * signal polarity:
	 */
	VID(EREG, EREG_DACS_ON|EREG_NHSYNC|EREG_VSYNC|EREG_ECLK_ON);
	VID(DCTL, DCTL_NORMAL|DCTL_HDWR(VID2_WIDTH));
	
	VID(BorderColourRegister, 0x000000);	/* blue border, 0xBBGGRR */
	VID(CursorPalette1, 0x000000);		/* cursor colour 1 is black */
	VID(CursorPalette2, 0x888888);		/* cursor colour 2 is grey */
	VID(CursorPalette3, 0xFFFFFF);		/* cursor colour 3 is white */

	VID(HCR,  444 -  8);		/* 8 MHz / 18 kHz = 444 */
	VID(HSWR,  20 -  8);
	VID(HBSR,  50 - 12);
	VID(HDSR,  60 - 18);
	VID(HDER, 380 - 18);
	VID(HBER, 410 - 12);
	VID(HCSR,  60 - 17 + 160);
#if 0
	VID(HCR,  796 -  8);		/* 25 MHz / 31.5 kHz = 794 */
	VID(HSWR,  38 -  8);
	VID(HBSR, 110 - 12);
	VID(HDSR, 120 - 18);
	VID(HDER, 760 - 18);
	VID(HBER, 770 - 12);
	VID(HCSR, 120 - 17 + 320);
#endif

	VID(VCR,  300 - 2);		/* 18 kHz / 60 Hz = 300 */
	VID(VSWR,  10 - 2);
	VID(VBSR,  10 - 1);
	VID(VDSR,  30 - 1);
	VID(VDER, 270 - 1);
	VID(VBER, 290 - 1);
	VID(VCSR,  30 - 1 + 120);
	VID(VCER,  30 - 1 + 120 + 32);
#if 0
	VID(VCR,  525 - 2);		/* 31.5 kHz / 60 Hz = 525 */
	VID(VSWR,  20 - 2);
	VID(VBSR,  20 - 1);
	VID(VDSR,  30 - 1);
	VID(VDER, 510 - 1);
	VID(VBER, 520 - 1);
	VID(VCSR,  30 - 1 + 240);
	VID(VCER,  30 - 1 + 240 + 32);
#endif
	/*
	 * Set up display buffer
	 */
	CURSINIT = (uint)&_vidCursorPhysical;
	VIDSTART = (uint)displayPage;
	VIDEND   = (uint)displayPage + VID2_SIZE;
	VIDINITA = (uint)displayPage;
	VIDINITB = 0;
	VIDCR    = 0x70;	/* on */

	/*
	 * Set up the video palette for flat translation
	 */
	VID(VideoPaletteAddressRegister, 0);
	for (i = 0; i < 256; i++) {
		VID(VideoPalette, i | (i << 8) | (i << 16));
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Clear --
 *
 *	Clear the display buffer
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The video DMA buffer is filled with zeros.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Clear()
{
/*	bzero(drawPage, VID2_SIZE);	*/
	_clear(drawPage, VID2_SIZE);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2DrawPage --
 *
 *	Select video page 0 or 1 for drawing.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Subsequent drawing actions will be performed on the
 *	specified page.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2DrawPage(int page)
{
	drawPage = (int *)drawPages[page];
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2DisplayPage --
 *
 *	Select video page 0 or 1 for display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The specified page is displayed.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2DisplayPage(int page)
{
	nextPage = page;
#if 0
	displayPage = displayPages[page];
	VIDSTART = (uint)displayPage;
	VIDEND   = (uint)displayPage + vidBufSize;
	VIDINITA = (uint)displayPage;
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2FrameIsr --
 *
 *	A routine to handle video retrace interrupts.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A frame counter is incremented.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2FrameIsr(void)
{
	frameCount++;

	displayPage = displayPages[nextPage];
	VIDSTART = (uint)displayPage;
	VIDEND   = (uint)displayPage + vidBufSize;
	VIDINITA = (uint)displayPage;

	IRQRQA = 0x08;		/* ack interrupt */
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Sync --
 *
 *	Synchronise the frame rate to the retrace counter.
 *
 * Results:
 *	Returns the number of frame flybacks that have occured
 *	since VidSync was last called.
 *
 * Side effects:
 *	The function waits until the next frame flyback occurs
 *	before returning.
 *
 *----------------------------------------------------------------------
 */

static int
Vid2Sync(void)
{
	int i;
	
	IntDisable();
	i = frameCount;
	while (frameCount == i) {
		IntEnable();
		Suspend();
		IntDisable();
	}
	IntEnable();
	i = frameCount;
	frameCount = 0;
	return i;
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2ShapeCursor --
 *
 *	Change the shape of the cursor in CRT mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	
 *
 *----------------------------------------------------------------------
 */

static void
Vid2ShapeCursor(char *xpmPtr[])
{
	int i, j;
	char mask, *dest;

	bzero(&_vidCursorLogical, 32 * 8);
	for (j = 0; j < 32; j++) {
		for (i = 0; i < 32; i++) {
			switch (xpmPtr[j][i]) {
			    case '.':
			    	mask = 0;
			    	break;
			    case ' ':
			    	mask = 1;
			    	break;
			    case 'X':
			    	mask = 3;
			    	break;
			    default:
			    	mask = 0;
			}
			dest = (&_vidCursorLogical + j * 8 + i / 4);
			*dest |= (mask << ((i % 4) * 2));
			/* *dest = 0xFF; */
		}
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2MoveCursor --
 *
 *	Position the hardware cursor in CRT mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The hardware cursor is moved to the screen coordinates given
 *
 *----------------------------------------------------------------------
 */

static void
Vid2MoveCursor(int x, int y)
{
	VID(HCSR, 120 - 17 + x);
	VID(VCSR, 30 - 1 + y);
	VID(VCER, 30 - 1 + y + 32);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2GetCol --
 *
 *	Convert a set of RGB values to a colour integer for CRT mode.
 *
 * Results:
 *	Returns an integer colour number that represents the colour
 *	closest to the given RGB values in CRT mode.  In CRT mode,
 *	colours are on a 6 * 6 * 6 RGB colour cube, ie a 216 colour palette.
 *	Each parameter is treated as an 8 bit R, G or B value.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
Vid2GetCol(int red, int green, int blue)
{
	return (((red / 43) * 6 * 6)	/* 43 = 256 / 6 + 1 */
		+ ((green / 43) * 6)
		+ (blue / 43));
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2SetRGB --
 *
 *	Set the red, green and blue intensities for a colour.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Colour col gets the attributes red, green and blue.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2SetRGB(int col, int red, int green, int blue)
{
#if 0
	VID(VideoPaletteAddressRegister, col);
	VID(VideoPalette, red | ((int)green << 8) | ((int)blue << 16));
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Pixel --
 *
 *	Draw a pixel, in CRT mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A pixel of colour col is drawn at screen coordinates (x, y).
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Pixel(int x, int y, int col)
{
	if ((x >= 0) && (x < VID2_WIDTH) && (y >= 0) && (y < VID2_HEIGHT)) {
		*(drawPage + (y * VID2_WIDTH) + x) = col;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2HLine --
 *
 *	Draw a horizontal line
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A horizontal line from (x1,y) to (x2,y) is clipped to the screen
 *	and drawn in colour col.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2HLine(int x1, int x2, int y, int col)
{
	int x, tmp;

	if (y < 0 || y >= VID2_HEIGHT)
		return;
	if (x2 < x1) {
		SWAP(x1, x2);
	}
	if (x2 < 0)
		return;
	if (x1 >= VID2_WIDTH)
		return;
	if (x1 < 0)
		x1 = 0;
	if (x2 >= VID2_WIDTH)
		x2 = VID2_WIDTH - 1;

	for (x = x1; x <= x2; x++) {
		*(drawPage + (y * VID2_WIDTH) + x) = col;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2VLine --
 *
 *	Draw a vertical line
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A vertical line from (x,y1) to (x,y2) is clipped to the screen
 *	and drawn in colour col.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2VLine(int x, int y1, int y2, int col)
{
	int y, tmp;

	if (y2 < y1) {
		SWAP(y1, y2);
	}
	
	if (x < 0 || x >= VID2_WIDTH)
		return;
	if (y2 < 0)
		return;
	if (y1 >= VID2_HEIGHT)
		return;
	if (y1 < 0)
		y1 = 0;
	if (y2 >= VID2_HEIGHT)
		y2 = VID2_HEIGHT - 1;

	for (y = (y1); y <= (y2); y++) {
		*(drawPage + (y * VID2_WIDTH) + x) = col;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Line --
 *
 *	Draw a line, in any video mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A line is drawn from (x1, y1) to (x2, y2) in colour col,
 *	using the Digital Differential Algorithm.
 *	This is implemented using 16.16 bit fixed point values.  Tests
 *	are made for the special cases x1=x2 and y1=y2, for speed.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Line(int x1, int y1, int x2, int y2, int col)
{
	int x, y, tmp;		/* counters and a temporary variable */
	int xf, yf, sf;		/* 16.16 fixed point values */

	if (x1 < 0 && x2 < 0)
		return;
	if (x1 >= VID2_WIDTH && x2 >= VID2_WIDTH)
		return;
	if (y1 < 0 && y2 < 0)
		return;
	if (y1 >= VID2_HEIGHT && y2 >= VID2_HEIGHT)
		return;
	
 	/*
	 * Test for vertical line special case
	 */
	if (x1 == x2) {
		/*
		 * If neccessary, swap y1 and y2 so that y1
		 * is above y2.  Then draw the line.
		 */
		if (y1 > y2) {
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		for (y = y1; y <= y2; y++)
			Vid2Pixel(x1, y, col);
	/*
	 * Test for horizontal line special case
	 */
	} else if (y1 == y2) {
		/*
		 * If neccessary, swap x1 and x2 so that x1
		 * is to the left of x2.  Then draw the line.
		 */
		if (x1 > x2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		for (x = x1; x <= x2; x++)
			Vid2Pixel(x, y1, col);
	/*
	 * Sloped line with delta y greater than delta x,
	 * need to draw one pixel per y value.
	 */
	} else if (abs(y2 - y1) > abs(x2 - x1)) {
		/*
		 * If neccessary, swap the two points so that
		 * y1 is above y2.
		 */
		if (y1 > y2) {
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		/*
		 * Calculate the step (dx/dy) as a 16.16 bit fixed
		 * point value, then step along the line drawing one
		 * pixel for each y value.
		 */
		sf = ((x2 - x1) << 16) / (y2 - y1);
		xf = (x1 << 16) + 0x8000;
		for (y = y1; y <= y2; y++) {
			Vid2Pixel(xf >> 16, y, col);
			xf += sf;
		}
	/*
	 * Sloped line with delta x greater than delta y,
	 * need to draw one pixel per x value.
	 */
	} else {
		/*
		 * If neccessary, swap the two points so that
		 * x1 is to the left of x2.
		 */
		if (x1 > x2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		/*
		 * Calculate the step (dy/dx) as a 16.16 bit fixed
		 * point value, then step along the line drawing one
		 * pixel for each x value.
		 */
		sf = ((y2 - y1) << 16) / (x2 - x1);
		yf = (y1 << 16) + 0x8000;
		for (x = x1; x <= x2; x++) {
			Vid2Pixel(x, yf >> 16, col);
			yf += sf;
		}
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Rect --
 *
 *	Draw a rectangle, in any mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A rectangle with corners at (x1, y1) and (x2, y2) is
 *	drawn in colour col.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Rect(int x1, int y1, int x2, int y2, int col)
{
	Vid2HLine(x1, x2, y1, col);
	Vid2HLine(x1, x2, y2, col);
	Vid2VLine(x1, y1, y2, col);
	Vid2VLine(x2, y1, y2, col);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Triangle --
 *
 *	Draw a triangle
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A triangle with corners at (x1,y1),(x2,y2),(x2,y3) is clipped
 *	to the screen and drawn in colour col.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int col)
{
	Vid2Line(x1, y1, x2, y2, col);
	Vid2Line(x2, y2, x3, y3, col);
	Vid2Line(x3, y3, x1, y1, col);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Circle --
 *
 *	Draw a circle, in any video mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Draws a circle of colour col with centre (cx, cy) and radius r,
 *	using Bresenham's circle algorithm.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Circle(int cx, int cy, int r, int col)
{
	int x, y, d;

	x = 0;
	y = r;
	d = 3 - 2 * r;
	while (x <= y) {
		Vid2Pixel(cx + x, cy + y, col);	/* XXX slow */
		Vid2Pixel(cx - x, cy + y, col);
		Vid2Pixel(cx + x, cy - y, col);
		Vid2Pixel(cx - x, cy - y, col);
		Vid2Pixel(cx + y, cy + x, col);
		Vid2Pixel(cx - y, cy + x, col);
		Vid2Pixel(cx + y, cy - x, col);
		Vid2Pixel(cx - y, cy - x, col);
		if (d < 0)
			d = d + 4 * x + 6;
		else {
			d = d + 4 * (x - y) + 10;
			y = y - 1;
		}
		x++;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2FillRect --
 *
 *	Draw a filled rectangle, in any mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The rectangular region with corners at (x1, y1) and (x2, y2)
 *	is filled with the colour col.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2FillRect(int x1, int y1, int x2, int y2, int col)
{
	int y;
	
	for (y = y1; y < y2; y++)
		Vid2HLine(x1, x2, y, col);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2FillTriangle --
 *
 *	Draw a filled triangle.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A filled triangle with points (x1, y1), (x2, y2), (x3, y3)
 *	is drawn with colour col.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int col)
{
	int x2f, x3f, s2f, s3f;		/* 16.16 fixed point values */
	int y;				/* counters */
	int tmp;

	if (x1 < 0 && x2 < 0 && x3 < 0)
		return;
	if (x1 >= VID2_WIDTH && x2 >= VID2_WIDTH && x3 >= VID2_WIDTH)
		return;
	if (y1 < 0 && y2 < 0 && y3 < 0)
		return;
	if (y1 >= VID2_HEIGHT && y2 >= VID2_HEIGHT && y3 >= VID2_HEIGHT)
		return;

	/*
	 * First sort points so that y1 <= y2 <= y3:
	 */
	 
	if (y2 < y1) {
		SWAP(y1, y2);
		SWAP(x1, x2);
	}
	if (y3 < y1) {
		SWAP(y1, y3);
		SWAP(x1, x3);
	}
	if (y3 < y2) {
		SWAP(y2, y3);
		SWAP(x2, x3);
	}

	y = y1;
	x2f = x1 << 16;
	x3f = x1 << 16;

	if (y1 != y2) {
		s2f = ((x2 - x1) << 16) / (y2 - y1);
	} else {
		s2f = 0;	/* XXX */
	}
	if (y1 != y3) {
		s3f = ((x3 - x1) << 16) / (y3 - y1);
	} else {
		s3f = 0;
	}
	if (x2 <= x3) {
		while (y < y2) {
			Vid2HLine(x2f >> 16, x3f >> 16, y, col);
			x2f += s2f;
			x3f += s3f;
			y++;
		}
	} else {
		while (y < y2) {
			Vid2HLine(x3f >> 16, x2f >> 16, y, col);
			x2f += s2f;
			x3f += s3f;
			y++;
		}
	}

	if (y2 == y3) {
		return;
	}

	s2f = ((x3 - x2) << 16) / (y3 - y2);
	x2f = x2 << 16;
	if (y3 >= VID2_HEIGHT) {
		y3 = VID2_HEIGHT - 1;
	}

	if (x2f <= x3f) {
		while (y <= y3) {
			Vid2HLine(x2f >> 16, x3f >> 16, y, col);
			x2f += s2f;
			x3f += s3f;
			y++;
		}
	} else {
		while (y <= y3) {
			Vid2HLine(x3f >> 16, x2f >> 16, y, col);
			x2f += s2f;
			x3f += s3f;
			y++;
		}
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2FillCircle --
 *
 *	Draw a filled circle, in any video mode.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Draws a filled circle of colour col with centre (cx, cy) and
 *	radius r, using Bresenham's circle algorithm.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2FillCircle(int cx, int cy, int r, int col)
{
	int x, y, d;

	x = 0;
	y = r;
	d = 3 - 2 * r;
	while (x <= y) {
		Vid2HLine(cx + x, cx - x, cy + y, col);
		Vid2HLine(cx + x, cx - x, cy - y, col);
		Vid2HLine(cx + y, cx - y, cy + x, col);
		Vid2HLine(cx + y, cx - y, cy - x, col);
		if (d < 0)
			d = d + 4 * x + 6;
		else {
			d = d + 4 * (x - y) + 10;
			y = y - 1;
		}
		x++;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Char --
 *
 *	Draw a character
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The character ch is drawn at (x, y) in colours fgcol and bgcol.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Char(unsigned char ch, int x, int y, int fgcol, int bgcol)
{
	int i, j;
	unsigned char *f;
	
	f = fixedFont + (((unsigned char)ch) * 12);
	for (j = 0; j < 12; j++) {
		for (i = 0; i < 6; i++) {
			Vid2Pixel(x + i, y + j,
				(*f & (0x20 >> i)) ? fgcol : bgcol);
		}
		f++;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid2Text --
 *
 *	Write a string to the display
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The string s is written at (x, y) in colours fgcol and bgcol.
 *
 *----------------------------------------------------------------------
 */

static void
Vid2Text(char *s, int x, int y, int fgcol, int bgcol)
{
	while (*s != '\0') {
		Vid2Char(*s, x, y, fgcol, bgcol);
		s++;
		x += 6;					/* XXX fixed */
	}
}
