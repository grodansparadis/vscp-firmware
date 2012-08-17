/* rifffile.h - Copyright (c) 1996 by Timothy J. Weber */

#ifndef __RIFFFILE_H
#define __RIFFFILE_H

/* Headers required to use this module */
#include <stack>
#include <vector>

#include <stdio.h>

/***************************************************************************
	macros, constants, and enums
***************************************************************************/

/***************************************************************************
	typedefs, structs, classes
***************************************************************************/

class RiffFile;
class RiffChunk {
public:
	char name[5];
	unsigned long size;  // the length, read from the second chunk header entry
	char subType[5];  // valid for RIFF and LIST chunks
	long start;  // the file offset in bytes of the chunk contents
	long after;  // the start of what comes after this chunk

	// initialize at the file's current read position, and mark the file as bad
	// if there's an error.
	RiffChunk()
		{};
	RiffChunk(RiffFile& file);
};

class RiffFile {
#ifndef RWSTD_NO_NAMESPACE
	 using namespace std;
#endif

	FILE* fp;

	unsigned long formSize;

	stack<RiffChunk, vector<RiffChunk> > chunks;

public:
	RiffFile(const char *name);
	~RiffFile();

	bool rewind();
	bool push(const char* chunkType = 0);
	bool pop();
	long chunkSize() const;
	const char* chunkName() const;
	const char* subType() const;
	FILE* filep()
		{ return fp; };
};

/***************************************************************************
	public variables
***************************************************************************/

#ifndef IN_RIFFFILE
#endif

/***************************************************************************
	function prototypes
***************************************************************************/

#endif
/* __RIFFFILE_H */
