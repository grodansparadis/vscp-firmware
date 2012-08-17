/* 
 * mouse.c --
 *
 *	Mouse support functions.
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

volatile int mouseX = 320, mouseY = 240, mouseB = 0;


/*
 *----------------------------------------------------------------------
 *
 * MouseInit --
 *
 *	Initialise things to get the mouse going.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	
 *
 *----------------------------------------------------------------------
 */

void
MouseInit(void)
{
	int dummy;

	mouseX = vidMaxX / 2;
	mouseY = vidMaxY / 2;
	mouseB = 0;
	VidShapeCursor(cursorPixmap);
	dummy = MSEDAT;		/* clear interrupt if any */
	IrqInstall(IRQ_MSER, MouseIsr);
	MSECR = 0x08;		/* enable mouse state machine */
	MSEDAT = 0xF4;		/* send mouse enable command */
}

/*
 *----------------------------------------------------------------------
 *
 * MouseIsr --
 *
 *	The mouse interrupt service routine.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A static state variable is updated as each byte is received
 *	from the mouse.  When a complete event message has been
 *	received, the state of the mouse is updated on the screen.
 *
 *----------------------------------------------------------------------
 */

void
MouseIsr(void)
{
	static int state = 2;
	static signed char delta_x, delta_y, old_b;
	char data;

	IRQMSKD &= ~0x01;	/* mask off interrupt */
	data = MSEDAT;		/* acks interrupt */

	switch (state) {
	    case 0:
		old_b = mouseB;
		mouseB = data & 0x07;
		state = 1;
		break;
	    case 1:
		delta_x = data;
		state = 2;
		break;
	    case 2:
		delta_y = data;
		if (delta_x != 0 || delta_y != 0 || mouseB != old_b) {
			mouseX += delta_x;
			mouseY -= delta_y;
			if (mouseX < 0)
				mouseX = 0;
			if (mouseX > 639)
				mouseX = 639;
			if (mouseY < 0)
				mouseY = 0;
			if (mouseY > 479)
				mouseY = 479;
			VidMoveCursor(mouseX, mouseY);
		}
		state = 0;
		break;
	    default:
		state = 0;
	}
	IRQMSKD |= 0x01;	/* mask off interrupt */
}

