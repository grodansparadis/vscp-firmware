@echo off
echo .
echo Regression test for WAVE file utilities.
echo .

echo Duplicating a test file.
pause
del wavetest-out.wav
wavetest wavetest.wav wavetest-out.wav
start /wait wavetest-out.wav

echo Generating and playing a 440 Hz sine wave.
pause
a440
start /wait a440.wav

echo .
echo Panning the word "test" to left, middle, and right.
pause
pan wavetest.wav leftpan.wav 0
start /wait leftpan.wav
pan wavetest.wav midpan.wav 50
start /wait midpan.wav
pan wavetest.wav rightpan.wav 100
start /wait rightpan.wav

echo .
echo Putting reverb on "test".
pause
reverb wavetest.wav reverb.wav
start /wait reverb.wav


