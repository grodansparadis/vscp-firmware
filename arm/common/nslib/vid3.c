/* 
 * vid3.c --
 *
 *	Video display functions for video mode 3 - LCD 640x480x12bit
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

#define VID3_WIDTH	640
#define VID3_HEIGHT	480
#define VID3_COLS	(1 << 12)
#define VID3_SIZE	(VID3_WIDTH * VID3_HEIGHT * 3 / 2)

/*
 * Static functions in this file:
 */

static void	Vid3Start();
static void	Vid3Clear();
static void	Vid3DrawPage(int page);
static void	Vid3DisplayPage(int page);
static void	Vid3FrameIsr(void);
static int	Vid3Sync(void);
static void	Vid3ShapeCursor(char *xpmPtr[]);
static void	Vid3MoveCursor(int x, int y);  
static int	Vid3GetCol(int red, int green, int blue);
static void	Vid3SetRGB(int col, int red, int green, int blue);
static void	Vid3Pixel(unsigned int x, unsigned int y, unsigned int col);
static void	Vid3HLine(int x1, int x2, int y, int col);
static void	Vid3VLine(int x, int y1, int y2, int col);
static void	Vid3Line(int x1, int y1, int x2, int y2, int col);
static void	Vid3Rect(int x1, int y1, int x2, int y2, int col);
static void	Vid3Triangle(int x1, int y1, int x2, int y2,
						int x3, int y3, int col);
static void	Vid3Circle(int cx, int cy, int r, int col);
static void	Vid3FillRect(int x1, int y1, int x2, int y2, int col);
static void	Vid3FillTriangle(int x1, int y1, int x2, int y2,
						int x3, int y3, int col);
static void	Vid3FillCircle(int cx, int cy, int r, int col);
static void	Vid3Char(unsigned char ch, int x, int y, int fgcol, int bgcol);
static void	Vid3Text(char *s, int x, int y, int fgcol, int bgcol);



VidConfig vid3Config = {
	VID3_WIDTH,
	VID3_HEIGHT,
	VID3_COLS,
	VID3_SIZE,
	Vid3Start,
	Vid3Clear,
	Vid3DrawPage,
	Vid3DisplayPage,
	Vid3FrameIsr,
	Vid3Sync,
	Vid3ShapeCursor,
	Vid3MoveCursor,
	Vid3GetCol,
	Vid3SetRGB,
	Vid3Pixel,
	Vid3HLine,
	Vid3VLine,
	Vid3Line,
	Vid3Rect,
	Vid3Triangle,
	Vid3Circle,
	Vid3FillRect,
	Vid3FillTriangle,
	Vid3FillCircle,
	Vid3Char,
	Vid3Text
};



static char *drawPages[2] = {&_vid3Display0Logical, &_vid3Display1Logical};
static char *displayPages[2] = {&_vid3Display0Physical, &_vid3Display1Physical};

static char *drawPage = &_vid3Display0Logical;
static char *displayPage = &_vid3Display0Physical;

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
 * Vid3Start --
 *
 *	Initialise the LCD display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The ARM video registers are set up for display on
 *	the LCD display, in 640x480x4096 mode.  Internally to
 *	the ARM, this is actually (640*3)x480x16.
 *
 *----------------------------------------------------------------------
 */

static void
Vid3Start()
{
	int i, j;

	/*
	 * Set up display format for (640*3)x480x16
	 */

	VID(CONREG, CONREG_VCLK|CONREG_CK_DIV_1|CONREG_BPP_4|CONREG_FIFO_20);

	/*
	 * Reset the phase comparator   9-14, 11-2, E-2
	 * Want dot clock of 90 MHz for 640x480
	 */

#define VID3_VMOD	35 /*45 */
#define VID3_RMOD	16
#define VID3_VR		(((VID3_VMOD-1) << 8) | (VID3_RMOD - 1))


	VID(FrequencySynthesisRegister, 0x80C0 | VID3_VR);
	VID(FrequencySynthesisRegister, 0x0040 | VID3_VR);
	for (i = 0; i < 1000000; i++)
		;
	VID(FrequencySynthesisRegister, VID3_VR);
	for (i = 0; i < 1000000; i++)
		;
	
	/*
	 * Set up the video macrocell for colour LCD display
	 * See 11-8, 11-10, 13-16
	 */

	VIDMUX = 0x00;
	
	VID(EREG, EREG_ESEL_LCD|EREG_LCDGREY_ON|EREG_ECLK_ON
					|EREG_HSYNC|EREG_VSYNC);
	VID(DCTL, DCTL_NORMAL|DCTL_HDWR(VID3_WIDTH * 3 / 8));
	
	VID(BorderColourRegister, 0xFF0000);	/* blue border, 0xBBGGRR */
	VID(CursorPalette1, 0x000000);		/* cursor colour 1 is black */
	VID(CursorPalette2, 0x888888);		/* cursor colour 2 is grey */
	VID(CursorPalette3, 0xFFFFFF);		/* cursor colour 3 is white */

	/*
	 * Horizontal Timings in Pixel Units 
	*/
#define WID_SCREEN	(640*3)			/* Width of Screen */
#define WID_HSYNC 	8			/* Width of HSYNC pulse */
#define WID_HFPORCH	64 			/* Width of Horizontal porch(s) */
#define WID_HBPORCH	64			/* 128 for 90 MHz clock */
#define WID_HBORDER 	0 			/* Width of Border */

	VID(HCR,  (WID_HSYNC + WID_SCREEN + 2*WID_HBORDER + WID_HFPORCH
		   + WID_HBPORCH) -  8);	/* 1976, 1944 */

	VID(HSWR, (WID_HSYNC) -  8);	/* 12, 12 */

	VID(HBSR, (WID_HSYNC + WID_HBPORCH) - 12);	/* 36, 20 */

	VID(HDSR, (WID_HSYNC + WID_HBPORCH + WID_HBORDER) - 18);	/* 36, 20 */

	VID(HDER, (WID_HSYNC + WID_HBPORCH + 2*WID_HBORDER
		   + WID_SCREEN) - 18);	/* 1956, 1940 */

	VID(HBER, (WID_HSYNC + WID_HBPORCH + WID_HBORDER
		   + WID_SCREEN) - 12);	/* 1956, 1940 */

	VID(HCSR,  (WID_HSYNC + WID_HBPORCH + WID_HBORDER) - 17
		   + (320 * 3)); 			/* Start Cursor in Centre */
	
	/*
	 * Vertical Timings ... (unit = raster)
	 */
#define H_VSYNC		2 			/* Vertical Sync pulse width */
#define H_SCREEN	480			/* Vertical Size of screen */
#define H_VBORDER	0
#define H_BPORCH	0
#define H_FPORCH	0

	VID(VCR,  (H_SCREEN + H_VSYNC + 2*H_VBORDER + H_BPORCH + H_FPORCH) - 2); /* 484 */
	VID(VSWR, (H_VSYNC) - 2);
	VID(VBSR, (H_VSYNC + H_BPORCH) - 1);
	VID(VDSR, (H_VSYNC + H_BPORCH + H_VBORDER) - 1);
	VID(VDER, (H_VSYNC + H_BPORCH + H_VBORDER + H_SCREEN) - 1);
	VID(VBER, (H_VSYNC + H_BPORCH + H_VBORDER + H_SCREEN + H_VBORDER) - 1);

	/*
	 * Virtical Positioning of the mouse cursor
	 */
	VID(VCSR, (H_VSYNC + H_BPORCH + H_VBORDER) - 1 + mouseY);
	VID(VCER, (H_VSYNC + H_BPORCH + H_VBORDER) - 1 + mouseY + 32);

	/*
	 * Set up display buffer
	 */
	CURSINIT = (uint)&_vidCursorPhysical;
	VIDSTART = (uint)displayPage;
	VIDEND   = (uint)displayPage + VID3_SIZE;
	VIDINITA = (uint)displayPage;
	VIDINITB = 0;

	/*
	 * Set up the LCD offset registers.  What do these do?
	 * I dunno, but they need to be set up before ECLK
	 * is disabled during horizontal and vertical flyback.
	 * I have no idea why these are the way they are,
	 * see the datasheet, page 9-5.
	 */

#define VID3_OFF_15	((3 * VID3_HEIGHT + 8) % 15)
#define VID3_OFF_9	((7 * VID3_HEIGHT + 4) % 9)
#define VID3_OFF_5	((1 * VID3_HEIGHT + 3) % 5)
#define VID3_OFF_2	0
	 
	VID(LCDOffsetRegister0, (VID3_OFF_5  << 1) | VID3_OFF_2);
	VID(LCDOffsetRegister1, (VID3_OFF_15 << 4) | VID3_OFF_9);

	/*
	 * Turn on the video system and set DISP to the panel high
	 */

	VIDCR = 0x70;
	IOCR = 0x01;
	
	/*
	 * Set up the video palette for 16 greyscales per subpixel
	 */

	VID(VideoPaletteAddressRegister, 0);
	for (j = 0; j < 16; j++) {
		for (i = 0; i < 16; i++) {
			VID(VideoPalette, i | (i << 8) | (i << 16) | (i << 24));
		}
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3Clear --
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
Vid3Clear()
{
/*	bzero(drawPage, VID3_SIZE);	*/
	_clear(drawPage, VID3_SIZE);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3DrawPage --
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
Vid3DrawPage(int page)
{
	drawPage = drawPages[page];
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3DisplayPage --
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
Vid3DisplayPage(int page)
{
	nextPage = page;
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3FrameIsr --
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
Vid3FrameIsr(void)
{
	frameCount++;

	displayPage = displayPages[nextPage];
	VIDSTART = (uint)displayPage;
	VIDEND   = (uint)displayPage + vidBufSize;
	VIDINITA = (uint)displayPage;

	/*
	 * Cursor position (also setup on line 168 ...)
	 */
	VID(HCSR, (WID_HSYNC + WID_HBPORCH + WID_HBORDER) - 17
		   + (mouseX * 3));
	VID(VCSR, (H_VSYNC + H_BPORCH + H_VBORDER) - 1 + mouseY);
	VID(VCER, (H_VSYNC + H_BPORCH + H_VBORDER) - 1 + mouseY + 32);

	IRQRQA = 0x08;		/* ack interrupt */
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3Sync --
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
Vid3Sync(void)
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
 * Vid3ShapeCursor --
 *
 *	Change the shape of the cursor in LCD mode.
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
Vid3ShapeCursor(char *xpmPtr[])
{
	int	i, j, k, mask;
	char	*dest;

	_clear(&_vidCursorLogical, 32 * 8);
	dest = (&_vidCursorLogical);

	k = 0;
	mask = 0;
	for (j=0;j<32;j++) {
		for (i=0;i<32;i++) {
			switch (xpmPtr[j][i]) {
				case '.': mask |= 0x00; break;
				case ' ': mask |= 0x15; break;
				case 'X': mask |= 0x3F; break;
				default: mask |= 0x00; break;
			}
			if (k==3) {
				*(int *)dest |= 0xffff; 
				mask = 0;
				dest += 3;
				k = 0;
			} else {
				mask <<= 6; 
				k++;
			}
		}
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3MoveCursor --
 *
 *	Position the hardware cursor in LCD mode.  The actual
 *      repositioning is done in the refresh ISR.
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
Vid3MoveCursor(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3GetCol --
 *
 *	Convert a set of RGB values to a colour integer for LCD mode.
 *
 * Results:
 *	Returns an integer colour number that represents the colour
 *	closest to the given RGB values in LCD mode.  In LCD mode,
 *	colours are on a 6 * 6 * 6 RGB colour cube, ie a 216 colour palette.
 *	Each parameter is treated as an 8 bit R, G or B value.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
Vid3GetCol(int red, int green, int blue)
{
	return ( ((red & 0xF0) << 4) | (blue & 0xF0) | ((green & 0xF0) >> 4));
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3SetRGB --
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
Vid3SetRGB(int col, int red, int green, int blue)
{

}

/*
 *----------------------------------------------------------------------
 *
 * Vid3Pixel --
 *
 *	Draw a pixel, in LCD mode.
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
Vid3Pixel(unsigned int x, unsigned int y, unsigned int col)
{
	char *addr;

	if ((x < VID3_WIDTH) && (y < VID3_HEIGHT)) {
		addr = (char *) (drawPage + ((y * 960) + (x * 3 / 2)));
		
		/*
		 * col = 0xRBG
		 *
		 * If x is even:	0xRB 0x-G
		 * If x is odd:		     0xB- 0xGR
		 */
		if (x & 1) {
			*addr = (*addr & 0x0F) | (char)(col & 0xF0);
			++addr;
			*addr = (char)(((col << 4) & 0xF0) | (col >> 8));
		} else {
			*addr = (char)(col >> 4);
			++addr;
			*addr = (*addr & 0xF0) | (char)(col);
		}
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3HLine --
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
Vid3HLine(int x1, int x2, int y, int col)
{
	int x, tmp;

	if (y < 0 || y >= VID3_HEIGHT)
		return;
	if (x2 < x1) {
		SWAP(x1, x2);
	}
	if (x2 < 0)
		return;
	if (x1 >= VID3_WIDTH)
		return;
	if (x1 < 0)
		x1 = 0;
	if (x2 >= VID3_WIDTH)
		x2 = VID3_WIDTH - 1;

	for (x = x1; x <= x2; x++) {
		Vid3Pixel(x, y, col);
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3VLine --
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
Vid3VLine(int x, int y1, int y2, int col)
{
	int y, tmp;

	if (y2 < y1) {
		SWAP(y1, y2);
	}
	
	if (x < 0 || x >= VID3_WIDTH)
		return;
	if (y2 < 0)
		return;
	if (y1 >= VID3_HEIGHT)
		return;
	if (y1 < 0)
		y1 = 0;
	if (y2 >= VID3_HEIGHT)
		y2 = VID3_HEIGHT - 1;

	for (y = (y1); y <= (y2); y++) {
		Vid3Pixel(x, y, col);
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3Line --
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
Vid3Line(int x1, int y1, int x2, int y2, int col)
{
	int x, y, tmp;		/* counters and a temporary variable */
	int xf, yf, sf;		/* 16.16 fixed point values */

	if (x1 < 0 && x2 < 0)
		return;
	if (x1 >= 640 && x2 >= 640)
		return;
	if (y1 < 0 && y2 < 0)
		return;
	if (y1 >= 480 && y2 >= 480)
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
			Vid3Pixel(x1, y, col);
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
			Vid3Pixel(x, y1, col);
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
			Vid3Pixel(xf >> 16, y, col);
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
			Vid3Pixel(x, yf >> 16, col);
			yf += sf;
		}
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3Rect --
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
Vid3Rect(int x1, int y1, int x2, int y2, int col)
{
	Vid3HLine(x1, x2, y1, col);
	Vid3HLine(x1, x2, y2, col);
	Vid3VLine(x1, y1, y2, col);
	Vid3VLine(x2, y1, y2, col);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3Triangle --
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
Vid3Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int col)
{
	Vid3Line(x1, y1, x2, y2, col);
	Vid3Line(x2, y2, x3, y3, col);
	Vid3Line(x3, y3, x1, y1, col);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3Circle --
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
Vid3Circle(int cx, int cy, int r, int col)
{
	int x, y, d;

	x = 0;
	y = r;
	d = 3 - 2 * r;
	while (x <= y) {
		Vid3Pixel(cx + x, cy + y, col);	/* XXX slow */
		Vid3Pixel(cx - x, cy + y, col);
		Vid3Pixel(cx + x, cy - y, col);
		Vid3Pixel(cx - x, cy - y, col);
		Vid3Pixel(cx + y, cy + x, col);
		Vid3Pixel(cx - y, cy + x, col);
		Vid3Pixel(cx + y, cy - x, col);
		Vid3Pixel(cx - y, cy - x, col);
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
 * Vid3FillRect --
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
Vid3FillRect(int x1, int y1, int x2, int y2, int col)
{
	int y;
	
	for (y = y1; y < y2; y++)
		Vid3HLine(x1, x2, y, col);
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3FillTriangle --
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
Vid3FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int col)
{
	int x2f, x3f, s2f, s3f;		/* 16.16 fixed point values */
	int y;				/* counters */
	int tmp;

	if (x1 < 0 && x2 < 0 && x3 < 0)
		return;
	if (x1 >= VID3_WIDTH && x2 >= VID3_WIDTH && x3 >= VID3_WIDTH)
		return;
	if (y1 < 0 && y2 < 0 && y3 < 0)
		return;
	if (y1 >= VID3_HEIGHT && y2 >= VID3_HEIGHT && y3 >= VID3_HEIGHT)
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
			Vid3HLine(x2f >> 16, x3f >> 16, y, col);
			x2f += s2f;
			x3f += s3f;
			y++;
		}
	} else {
		while (y < y2) {
			Vid3HLine(x3f >> 16, x2f >> 16, y, col);
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
	if (y3 >= VID3_HEIGHT) {
		y3 = VID3_HEIGHT - 1;
	}

	if (x2f <= x3f) {
		while (y <= y3) {
			Vid3HLine(x2f >> 16, x3f >> 16, y, col);
			x2f += s2f;
			x3f += s3f;
			y++;
		}
	} else {
		while (y <= y3) {
			Vid3HLine(x3f >> 16, x2f >> 16, y, col);
			x2f += s2f;
			x3f += s3f;
			y++;
		}
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3FillCircle --
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
Vid3FillCircle(int cx, int cy, int r, int col)
{
	int x, y, d;

	x = 0;
	y = r;
	d = 3 - 2 * r;
	while (x <= y) {
		Vid3HLine(cx + x, cx - x, cy + y, col);
		Vid3HLine(cx + x, cx - x, cy - y, col);
		Vid3HLine(cx + y, cx - y, cy + x, col);
		Vid3HLine(cx + y, cx - y, cy - x, col);
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
 * Vid3Char --
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
Vid3Char(unsigned char ch, int x, int y, int fgcol, int bgcol)
{
	int i, j;
	unsigned char *f;
	
	f = fixedFont + (((unsigned char)ch) * 12);
	for (j = 0; j < 12; j++) {
		for (i = 0; i < 6; i++) {
			Vid3Pixel(x + i, y + j,
				(*f & (0x20 >> i)) ? fgcol : bgcol);
		}
		f++;
	}
}

/*
 *----------------------------------------------------------------------
 *
 * Vid3Text --
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
Vid3Text(char *s, int x, int y, int fgcol, int bgcol)
{
	while (*s != '\0') {
		Vid3Char(*s, x, y, fgcol, bgcol);
		s++;
		x += 6;					/* XXX fixed */
	}
}
