
#pragma hdrstop
#include <condefs.h>


//---------------------------------------------------------------------------
USEUNIT("..\RIFFFile.cpp");
USEUNIT("..\WAVE.cpp");
//---------------------------------------------------------------------------
#pragma argsused

#include <iostream>
#include <math.h>

#include "WAVE.h"

using namespace std;

const frequency = 440;

int main(int , char **)
{
	WaveFile wf;

	wf.SetupFormat();

	if (!wf.OpenWrite("A440.wav")) {
		cout << "Can't open for writing: " << wf.GetError();
		return 1;
	}

	const dataLength = 1 /*sec*/ * wf.GetSampleRate();  // in samples

	float alpha = 0;
	for (int i = 0; i < dataLength; i++) {
		wf.WriteSample(sin(alpha) / 2);

		alpha += 2 * M_PI * frequency / wf.GetSampleRate();
	}

	return 0;
}
