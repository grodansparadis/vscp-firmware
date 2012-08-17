
#pragma hdrstop
#include <condefs.h>


//---------------------------------------------------------------------------
#pragma argsused

#include <iostream>

#include "WAVE.h"

USEUNIT("..\RIFFFile.cpp");
USEUNIT("..\WAVE.cpp");
//---------------------------------------------------------------------------
static void reportProblem()
{
	cout << "  *** ERROR: Result incorrect." << endl;
}

static void checkResult(bool got, bool expected)
{
	if (got)
		cout << "success." << endl;
	else 
		cout << "fail." << endl;

	if (got != expected)
		reportProblem();
}

static void pause()
{
	cout << "Press Enter to continue." << endl;
	cin.get();
}

static void ShowErrors(WaveFile& from, WaveFile& to)
{
	bool any = from.GetError() || to.GetError();

	if (from.GetError())
		cout << "Error on input: " << from.GetError() << "." << endl;

	if (to.GetError())
		cout << "Error on output: " << to.GetError() << "." << endl;

	if (!any)
		cout << "Success." << endl;
}

static void ShowFormat(WaveFile& wave)
{
	cout
		<< "Format:           " << wave.GetFormatType()
		<< (wave.IsCompressed()? " (compressed)" : " (PCM)") << endl
		<< "Channels:         " << wave.GetNumChannels() << endl
		<< "Sample rate:      " << wave.GetSampleRate() << endl
		<< "Bytes per second: " << wave.GetBytesPerSecond() << endl
		<< "Bytes per sample: " << wave.GetBytesPerSample() << endl
		<< "Bits per channel: " << wave.GetBitsPerChannel() << endl
		<< "Bytes:            " << wave.GetDataLength() << endl
		<< "Samples:          " << wave.GetNumSamples() << endl
		<< "Seconds:          " << wave.GetNumSeconds() << endl
		<< "File pointer:     " << (wave.GetFile()? "good" : "null") << endl;

	string type, value;
	if (wave.GetFirstExtraItem(type, value)) {
		cout << "Extra data:" << endl;
		do {
			cout << "  " << type << ": " << value << endl;
		} while (wave.GetNextExtraItem(type, value));

		wave.ResetToStart();
	}

	pause();
}

int main(int argc, const char* argv[])
{
	if (argc < 3)
		cout << "Copies one WAVE file to another, in canonical form." << endl;
	else {
		WaveFile From, To;

		cout << "Opening input..." << endl;
		From.OpenRead(argv[1]);
		ShowErrors(From, To);
		ShowFormat(From);

		cout << "Setting formats..." << endl;
		To.CopyFormatFrom(From);
		ShowFormat(To);

		cout << "Opening output..." << endl;
		To.OpenWrite(argv[2]);
		ShowErrors(From, To);

		cout << "Copying..." << endl;
		To.CopyFrom(From);
		ShowErrors(From, To);
		cout << "Resulting format: " << endl;
		ShowFormat(To);
		cout << "Source format: " << endl;
		ShowFormat(From);

		cout << "Closing..." << endl;
		To.Close();
		From.Close();
		ShowErrors(From, To);
	}

	return 0;
}

