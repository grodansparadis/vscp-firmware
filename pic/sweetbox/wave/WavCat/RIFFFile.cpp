/* rifffile.cpp

	Copyright (c) 1996 by Timothy J. Weber.

	See rifffile.txt for documentation.
*/

#include <string.h>

#include "rifffile.h"

/***************************************************************************
	macros and constants
***************************************************************************/

// define REVERSE_ENDIANISM if the endianism of the host platform is not Intel
// (Intel is little-endian)
#ifdef REVERSE_ENDIANISM
 #define SWAP_32(int32) (  \
	((((DWORD) int32) & 0x000000FFL) << 24) +  \
	((((DWORD) int32) & 0x0000FF00L) << 8) +  \
	((((DWORD) int32) & 0x00FF0000L) >> 8) +  \
	((((DWORD) int32) & 0xFF000000L) >> 24))
#endif

/***************************************************************************
	typedefs and class definitions
***************************************************************************/

/***************************************************************************
	prototypes for static functions
***************************************************************************/

/***************************************************************************
	static variables
***************************************************************************/

/***************************************************************************
	member functions for RiffFile
***************************************************************************/

RiffFile::RiffFile(const char *name):
	fp(fopen(name, "rb"))
{
	if (fp && !rewind()) {
		fclose(fp);
		fp = 0;
	}
}

RiffFile::~RiffFile()
{
	if (fp)
		fclose(fp);
}

bool RiffFile::rewind()
{
	// clear the chunk stack
	while (!chunks.empty())
		chunks.pop();

	// rewind to the start of the file
	if (fseek(fp, 0, SEEK_SET))
		return false;

	// look for a valid RIFF header
	RiffChunk topChunk(*this);

	if (feof(fp) || strcmp(topChunk.name, "RIFF"))
		return false;

	// found; push it on the stack, and leave the put pointer in the same place
	// as the get pointer.
	formSize = topChunk.size;
	chunks.push(topChunk);
	return true;
}

bool RiffFile::push(const char* chunkType)
{
	// can't descend if we haven't started out yet.
	if (chunks.empty())
		return false;

	// first, go to the start of the current chunk, if we're looking for a named
	// chunk.
	if (chunkType)
		if (fseek(fp, chunks.top().start, SEEK_SET))
			return false;

	// read chunks until one matches or we exhaust this chunk
	while (!feof(fp) && ftell(fp) < chunks.top().after) {
		RiffChunk chunk(*this);

		if (!feof(fp)) {
			// see if the subchunk type matches
			if (!chunkType || strcmp(chunk.name, chunkType) == 0) {
				// found; synchronize the put pointer, push the chunk, and succeed
				chunks.push(chunk);
				return true;
			} else {
				// not found; go to the next one.
				if (fseek(fp, chunk.after, SEEK_SET))
					return false;
			}
		}
	}

	// couldn't find it; synchronize the put pointer and return error.
	fseek(fp, chunks.top().start, SEEK_SET);
	return false;
}

bool RiffFile::pop()
{
	// if we've only got the top level chunk (or not even that), then we can't
	// go up.
	if (chunks.size() < 2)
		return false;

	// Position the get and put pointers at the end of the current subchunk.
	fseek(fp, chunks.top().after, SEEK_SET);

	// Pop up the stack.
	chunks.pop();
	return true;
}

long RiffFile::chunkSize() const
{
	if (!chunks.empty())
		return chunks.top().size;
	else
		return 0;
}

const char* RiffFile::chunkName() const
{
	if (!chunks.empty())
		return chunks.top().name;
	else
		return 0;
}

const char* RiffFile::subType() const
{
	if (!chunks.empty() && chunks.top().subType[0])
		return chunks.top().subType;
	else
		return 0;
}

/***************************************************************************
	member functions for RiffChunk
***************************************************************************/

RiffChunk::RiffChunk(RiffFile& parent)
{
	// read the chunk name
	fread(name, 1, 4, parent.filep());
	name[4] = '\0';

	// read the chunk size
	fread(&size, 4, 1, parent.filep());

#ifdef REVERSE_ENDIANISM
	// reverse the endianism of the chunk size.
	size = SWAP_32(size);
#endif

	// if this is a RIFF or LIST chunk, read its subtype.
	if (strcmp(name, "RIFF") == 0
		|| strcmp(name, "LIST") == 0)
	{
		fread(subType, 1, 4, parent.filep());
		subType[4] = '\0';
	} else
		*subType = '\0';

	// the chunk starts after the name and size.
	start = ftell(parent.filep());

	// the next chunk starts after this one, but starts on a word boundary.
	after = start + size;
	if (after % 2)
		after++;
}

/***************************************************************************
	main()
***************************************************************************/

#ifdef TEST_RIFFFILE

#include <iostream.h>

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

static void showChunk(RiffFile& file, int indent, bool expandLists)
{
	for (int i = 0; i < indent; i++)
		cout << ' ';

	cout << "Chunk type: " << file.chunkName();

	if (file.subType())
		cout << " (" << file.subType() << ")";

	cout << ", " << file.chunkSize() << " bytes" << endl;

	// show all subchunks
	if (strcmp(file.chunkName(), "RIFF") == 0
		|| (expandLists && strcmp(file.chunkName(), "LIST") == 0))
	{
		while (file.push()) {
			showChunk(file, indent + 2, expandLists);
			file.pop();
		}
	}
}

int main(int argc, const char* argv[])
{
	if (argc == 1)
		cout << "Lists the chunks in a named RIFF file." << endl;
	else {
		RiffFile file(argv[1]);

		if (!file.filep()) {
			cout << "No RIFF form in file " << argv[1] << "." << endl;
		} else {
			cout << "Without expanding lists:" << endl;
			showChunk(file, 0, false);

			cout << endl << "Expanding lists:" << endl;
			showChunk(file, 0, true);
		}
	}

	return 0;
}

#endif
