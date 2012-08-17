
#pragma hdrstop
#include <condefs.h>


//---------------------------------------------------------------------------
#pragma argsused

#include <iostream>

#include "WAVE.h"


USEUNIT("..\RIFFFile.cpp");
USEUNIT("..\WAVE.cpp");
//---------------------------------------------------------------------------
int main(int argc, char **argv)
{
	if (argc != 3) {
		cout << "Makes a new copy of a file with echo." << endl;
		cout << "Usage: " << argv[0] << " input.wav output.wav" << endl;
		return 1;
	}

	WaveFile in;
	if (!in.OpenRead(argv[1])) {
		cout << "Can't open " << argv[1] << " for reading." << endl;
		cout << in.GetError() << endl;
		return 1;
	}

	WaveFile out;
	out.SetupFormat(in.GetSampleRate(), in.GetBitsPerChannel());

	if (!in.FormatMatches(out)) {
		cout << "Input file format needs to be mono." << endl;
		return 1;
	}

	if (!out.OpenWrite(argv[2])) {
		cout << "Can't open " << argv[2] << " for writing." << endl;
		cout << out.GetError() << endl;
		return 1;
	}

	const size_t delayLength = size_t(0.2 /*sec*/ * out.GetSampleRate());  // in samples

	float* delayBuffer = new float[delayLength];

	for (size_t i = 0; i < in.GetNumSamples() + delayLength; i++) {
		float outputSample = 0;

		if (i < in.GetNumSamples()) {
			in.ReadSample(outputSample);

			outputSample *= 0.6;
		}

		if (i > delayLength)
			outputSample += delayBuffer[i % delayLength] * 0.5;

		out.WriteSample(outputSample);

		delayBuffer[i % delayLength] = outputSample;
	}

	return 0;

}
