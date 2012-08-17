import wave

f= wave.open( 'smoke_short_unsigned_8bit_mono.wav', 'rb' )
sampleRate= f.getframerate()
channels= f.getnchannels()
nframe = f.getnframes()
print "File Parameters:"
tmp ="/* File Parameters:\n"
print "(nchannels, sampwidth, framerate, nframes, comptype, compname)"
tmp +="(nchannels, sampwidth, framerate, nframes, comptype, compname)\n"
tmp +=str(f.getparams())+"*/\n"
print f.getparams()
texti = ""
for i in range(0,nframe):
    data = f.readframes(i)
    if(len(data) !=0):
        texti +=data
##        txt = "Frame"+str(i)+" :"
##        for j in range(0,len(data)):
##            txt +=hex(ord(data[j]))+","
##        print txt
        #print "Frame",i,":",data
f.close()

#Convert the Sampled Data
tmp += "#define PLAY_DATA_SZ "+str(len(texti))+"\n"
tmp += "const unsigned char play_data["+str(len(texti))+"]={\n"
for i in range(0,len(texti)):
    if( ((i%10) == 0) and i>0):
        tmp+="\n"
    #GET Current Data
    dt = ord(texti[i])
    #Make them into Exact Samples
##    if(dt>0x7F):
##        dt=0xFF - dt
##    else:
##        dt+=0x7F
    #put data to stream
    if(i<len(texti)-1):
        tmp+=hex(dt)+","
    else:
        tmp+=hex(dt)    

tmp+="};\n\n\n"
f = open("playdata.h","w")
f.write(tmp);
print "\n\n\n"+tmp
f.close()




