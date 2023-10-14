// ExtractAlcatelStuff.cpp: define el punto de entrada de la aplicación.
//

#include "ExtractAlcatelStuff.h"

using namespace std;


// stolen from https://stackoverflow.com/questions/27642954/create-monochrome-bmp-from-bitset
void Save1BppImage(char* ImageData, const char* filename, long w, long h) {

	int bitmap_dx = w; // Width of image
	int bitmap_dy = h; // Height of Image

	// create file
	std::ofstream file(filename, std::ios::binary | std::ios::trunc);
	if (!file) return;

	// save bitmap file headers
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER* infoHeader;
	infoHeader = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	if (!infoHeader) return;
	RGBQUAD bl = { 0,0,0,0 };  //black color
	RGBQUAD wh = { 0xff,0xff,0xff,0xff }; // white color


	fileHeader.bfType = 0x4d42;
	fileHeader.bfSize = 0;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + (sizeof(BITMAPINFOHEADER)) + 2 * sizeof(RGBQUAD);

	infoHeader->biSize = (sizeof(BITMAPINFOHEADER));
	infoHeader->biWidth = bitmap_dx;
	infoHeader->biHeight = -bitmap_dy;
	infoHeader->biPlanes = 1;
	infoHeader->biBitCount = 1;
	infoHeader->biCompression = BI_RGB; //no compression needed
	infoHeader->biSizeImage = 0;
	infoHeader->biXPelsPerMeter = 0;
	infoHeader->biYPelsPerMeter = 0;
	infoHeader->biClrUsed = 2;
	infoHeader->biClrImportant = 2;

	file.write((char*)&fileHeader, sizeof(fileHeader)); //write bitmapfileheader
	file.write((char*)infoHeader, (sizeof(BITMAPINFOHEADER))); //write bitmapinfoheader
	file.write((char*)&wh, sizeof(wh)); //write RGBQUAD for white
	file.write((char*)&bl, sizeof(bl)); //write RGBQUAD for black


	// convert the bits into bytes and write the file
	int offset, numBytes = ((w + 31) / 32) * 4;
	char* bytes = (char*)malloc(numBytes * sizeof(char));


	int bytes_in = (w + 7) / 8;
	int bytes_out = ((w + 31) / 32) * 4;
	const char zeros[4] = { 0, 0, 0, 0 };
	for (int y = h - 1; y >= 0; --y) {
	//for (int y = 0; y < h; y++) {
		file.write(((const char*)ImageData) + (y * bytes_in), bytes_in);

		if (bytes_out != bytes_in)
			file.write(zeros, bytes_out - bytes_in);
	}

	file.close();
}

const CLSID bmpEncoderClsId = { 0x557cf400, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } };

void Save8BppImage(char* ImageData, const char* filename, long w, long h) {


	int bitmap_dx = w; // Width of image
	int bitmap_dy = h; // Height of Image

	// create file
	std::ofstream file(filename, std::ios::binary | std::ios::trunc);
	if (!file) return;

	// save bitmap file headers
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER* infoHeader;
	infoHeader = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	if (!infoHeader) return;
	RGBQUAD bl = { 0,0,0,0 };  //black color
	RGBQUAD wh = { 0xff,0xff,0xff,0xff }; // white color


	fileHeader.bfType = 0x4d42;
	fileHeader.bfSize = 0;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + (sizeof(BITMAPINFOHEADER)) + 2 * sizeof(RGBQUAD);

	infoHeader->biSize = (sizeof(BITMAPINFOHEADER));
	infoHeader->biWidth = bitmap_dx;
	infoHeader->biHeight = -bitmap_dy;
	infoHeader->biPlanes = 1;
	infoHeader->biBitCount = 4;
	infoHeader->biCompression = BI_RGB; //no compression needed
	infoHeader->biSizeImage = 0;
	infoHeader->biXPelsPerMeter = 0;
	infoHeader->biYPelsPerMeter = 0;
	infoHeader->biClrUsed = 2;
	infoHeader->biClrImportant = 2;

	file.write((char*)&fileHeader, sizeof(fileHeader)); //write bitmapfileheader
	file.write((char*)infoHeader, (sizeof(BITMAPINFOHEADER))); //write bitmapinfoheader
	file.write((char*)&wh, sizeof(wh)); //write RGBQUAD for white
	file.write((char*)&bl, sizeof(bl)); //write RGBQUAD for black


	// convert the bits into bytes and write the file
	int offset, numBytes = ((w + 31) / 32) * 4;
	char* bytes = (char*)malloc(numBytes * sizeof(char));


	int bytes_in = (w + 7) / 8;
	int bytes_out = ((w + 31) / 32) * 4;
	const char zeros[4] = { 0, 0, 0, 0 };
	for (int y = h - 1; y >= 0; --y) {
		//for (int y = 0; y < h; y++) {
		file.write(((const char*)ImageData) + (y * bytes_in), bytes_in);

		if (bytes_out != bytes_in)
			file.write(zeros, bytes_out - bytes_in);
	}

	file.close();
}

int ProcessPic(const char* pic, const char* bmp, const char* bmpmask)
{
	size_t r = 0;

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int bitDepth = 0;
	unsigned int noPictureMask = 0;

	unsigned int rounded_up_thing = 0;

	unsigned int raw_data_size = 0;

	//bitmap bmp;
	//uint8_t* pixelbuffer = nullptr;


	FILE* fh = fopen(pic, "rb");
	//FILE* fout = fopen("test.bmp", "wb");

	if (!fh) {// || !fout) {
		perror("Error opening pic or output");
		return 1;
	}

	fseek(fh, 0, SEEK_END);
	long fsz = ftell(fh);
	fseek(fh, 0, SEEK_SET);

	char* dataStart = nullptr;

	char* contents = (char*)malloc(fsz);
	if (!contents) {
		perror("Error allocating ram for the file contents");
		goto cleanup_close;
	}

	r = fread(contents, fsz, 1, fh);
	if (r != 1) {
		perror("Error reading file");
		goto cleanup_free;
	}

	if (contents[0] == 0) {
		noPictureMask = contents[1];
		width = contents[2];
		height = contents[3];
		bitDepth = 8;
		printf("[INFO] this seems to be a 8bpp file, width is %d, height is %d, noPictureMask %d\n", width, height, noPictureMask);
	}
	else {
		width = contents[0];
		height = contents[1];
		bitDepth = 1;
		printf("[INFO] this seems to be a 1bpp file, width is %d, height is %d\n", width, height);
	}

	rounded_up_thing = width * ((height + 7) >> 3);

	if (bitDepth == 1) {
		raw_data_size = rounded_up_thing + 2;
		dataStart = &contents[2];
	}
	else {
		raw_data_size = (2 * rounded_up_thing) + 4;
		if (!noPictureMask)
			raw_data_size += rounded_up_thing;

		dataStart = &contents[4];
	}

	printf("[INFO] calculated size is %d, actual size is %d (%s)\n", raw_data_size, fsz, raw_data_size == fsz ? "GOOD!" : "damn it");

	if (!noPictureMask)
		Save1BppImage(dataStart, bmp, height, width);
	//else
	//	Save8BppImage(dataStart, bmpmask, height, width);
	/*
	bmp.bitmapinfoheader.width = width;
	bmp.bitmapinfoheader.height = height;
	bmp.bitmapinfoheader.bitsperpixel = bitDepth;

	bmp.bitmapinfoheader.imagesize = bmp.bitmapinfoheader.height * bmp.bitmapinfoheader.width * bmp.bitmapinfoheader.bitsperpixel / 8;

	bmp.fileheader.filesize = bmp.bitmapinfoheader.imagesize + sizeof(bitmap);
	//
	// pixelbuffer = (uint8_t*)malloc(bmp.fileheader.filesize);

	bmp.fileheader.signature[0] = 'B';
	bmp.fileheader.signature[1] = 'M';
	bmp.fileheader.fileoffset_to_pixelarray = sizeof(bitmap);
	bmp.bitmapinfoheader.dibheadersize = sizeof(bitmapinfoheader);
	bmp.bitmapinfoheader.planes = 1;
	bmp.bitmapinfoheader.compression = 0;
	bmp.bitmapinfoheader.ypixelpermeter = _ypixelpermeter;
	bmp.bitmapinfoheader.xpixelpermeter = _xpixelpermeter;
	bmp.bitmapinfoheader.numcolorspallette = 0;
	fwrite(&bmp, 1, sizeof(bitmap), fout);
	if (bitDepth == 1) {
		fwrite(&contents[2], 1, bmp.bitmapinfoheader.imagesize, fout);
	}*/


cleanup_free:
	free(contents);

cleanup_close:
	fclose(fh);
}




// See: https://www.3gpp.org/ftp/tsg_sa/TSG_SA/TSGS_10/Docs/PDF/SP-000690.pdf

#include <pshpack1.h>
typedef struct {
	char magic[4]; // .SEQ
	union {
		uint8_t seq_class_ems_tmode;
		struct {
			uint8_t seq_class : 4;
			uint8_t ems : 1;
			uint8_t rsvd : 1;
			uint8_t tmode : 2;
		};
	};
	uint8_t len_lsb;
	uint16_t len_msb;
	union {
		uint8_t seq_copy_status_countdown;
		struct {
			uint8_t seq_copy_status : 2;
			uint8_t seq_copy_countdown : 6;
		};
	};
	union {
		uint8_t resend_status_countdown;
		struct {
			uint8_t resend_status : 2;
			uint8_t resend_countdown : 6;
		};
	};
	union {
		uint16_t loop1;
		struct {
			uint8_t loop_def1 : 2;
			uint8_t loop_rsvd1 : 3;
			//uint16_t loop_point1 : 11;
		};
	};
	union {
		uint16_t start1;
		struct {
			uint8_t start_nb_repeat1 : 5;
			//uint16_t start_point1 : 11;
		};
	};

	union {
		uint16_t loop2;
		struct {
			uint8_t loop_def2 : 2;
			uint8_t loop_rsvd2 : 3;
			//uint16_t loop_point2 : 11;
		};
	};
	union {
		uint16_t start2;
		struct {
			uint8_t start_nb_repeat2 : 5;
			//uint16_t start_point2 : 11;
		};
	};

	uint16_t absolute_time_base_lsb;
	uint8_t absolute_time_base_msb;
	uint8_t reserved1;
	uint8_t number_of_tracks;
	uint8_t reserved2;
} MSEQ_Header;


typedef struct {
	union {
		uint16_t first_word;
		struct {
			uint8_t note : 5;			/// < NOTE command
			uint8_t channel : 4;		/// < sound channel, from 0 to 15. Most of the time, a single instrument is associated with a channel number. The definition is the same than in MIDI format.
			uint8_t note_number : 7;	/// < this is the note to be played. Definition is the same than in MIDI format (Middle C of an 88 note piano-style keyboard has a reference value of 60).
		};
	};

	union {
		uint16_t second_word;
		struct {
			uint8_t velocity : 6;		/// < this is the volume of the note to be played. Definition is the same than in MIDI format, excepted that the range is reduced to 0-63.
			uint16_t running_time : 10;	/// < defines the length of time the note has to be played. The note will sound during RUNNING_TIME x RTB x ATB (in µS).
		};
	};
} MSEQ_Command_Note;
#include <poppack.h>

typedef enum {
	TMODE_MULTIPROCESSORS = 0,	/// < multiprocessors oriented, no streaming (default) : tracks are Sequentially defined
	TMODE_STREAMING = 1,		/// < streaming oriented : only one multi-purpose track is defined
	TMODE_PACKET = 2,			/// < packet oriented (streaming + multiprocessors) : tracks are Sequentially defined within packets
	TMODE_RESERVED = 3,			/// < reserved for future use
} MSEQ_Header_TMode;

typedef enum {
	SEQ_COPY_STATUS_PROHIBITED = 0,		/// < global copy prohibited. None of the contents can be copied. (in definition of each track, TRK_COPY_STATUS = %10)
	SEQ_COPY_STATUS_COUNTDOWN = 1,		/// < global copy restricted to copy countdown.
	SEQ_COPY_STATUS_NOT_DEFINED = 2,	/// < global copy can not be defined. See each « track copy status » for track-content copy permission.
	SEQ_COPY_STATUS_FREE = 3,			/// < all-tracks copy free. (in definition of each track, TRK_COPY_STATUS = %10)
} MSEQ_Header_SEQ_Copy_Status;

typedef enum {
	LOOP_DEF_NOT_DEFINED = 0,
	LOOP_DEF_DEFINED = 1,
	LOOP_DEF_RESERVED = 2,
	LOOP_DEF_INFINITE = 3,
} MSEQ_Header_Loop_Def;

typedef enum {
	CMD_SYSTEM_TYPE_EOF = 0,				/// < This word is the last of the MSEQ file. It is put after the « Track (close) » command of the last defined track.
	CMD_SYSTEM_TYPE_START_OF_PACKET = 1,	/// < Used in packed mode (T-MODE=2). The packet number are sequential and are modulo 256 (value 0 is reserved).
	CMD_SYSTEM_TYPE_END_OF_PACKET = 2,		/// < Used in packed mode (T-MODE=2). Within a packet, the packet number is the same for STARTing command and ENDing command (value 0 is reserved).
} MSEQ_CMD_System_Types;

typedef enum : char {
	MSEQ_SHORT_TEXT = 0,
	MSEQ_LONG_TEXT = 1,
	MSEQ_NOTE = 2,
	MSEQ_PROGRAM_CHANGE = 3,
	MSEQ_VOLUME = 4,
	MSEQ_SOUND_SPECIAL_EFFECT1 = 5,
	MSEQ_SOUND_SPECIAL_EFFECT2 = 6,
	MSEQ_VIDEO_CHANNEL_EFFECT = 7,
	MSEQ_RESERVED_PICTURE_EFFECT = 8,
	MSEQ_RESERVED_ANIM_EFFECT = 9,
	MSEQ_TEXT_EFFECT = 0xa,
	MSEQ_RESERVED1 = 0xb,
	MSEQ_RESERVED2 = 0xc,
	MSEQ_RESERVED3 = 0xd,
	MSEQ_ANIMATION_DEFINITION = 0xe,
	MSEQ_RESERVED4 = 0xf,
	MSEQ_RESERVED5 = 0x10,
	MSEQ_PICTURE_DEFINITION = 0x11,
	MSEQ_RESERVED6 = 0x12,
	MSEQ_VIDEO_ZONE = 0x13,
	MSEQ_VIDEO_PROPERTIES = 0x14,
	MSEQ_RESERVED7 = 0x15,
	MSEQ_DELAY_DELTA_TIME = 0x16,
	MSEQ_EXTENDED_DELAY = 0x17,
	MSEQ_R_DELAY = 0x18,
	MSEQ_SYNCHRO = 0x19,
	MSEQ_RESERVED8 = 0x1a,
	MSEQ_RELATIVE_TIME_BASE_DEFINITION = 0x1b,
	MSEQ_BULK_DEFINITION = 0x1c,
	MSEQ_USE_OBJECT = 0x1d,
	MSEQ_TRACK_DEFINITION = 0x1e,
	MSEQ_SYSTEM_COMMAND = 0x1f,
} MSEQ_Command_Set;

class MSEQCmd {
public:
	virtual void FeedRemaining(void* const data, size_t sz) {};
	virtual void Dump() const {};

	MSEQ_Command_Set GetKind() const { return _kind; }
	size_t GetRemainingSize() const { return _remaining; }
	bool IsFull() const { return _remaining == 0; }
	size_t Size() const { return _size; }

	static MSEQ_Command_Set GetCmdFromFirstWord(const uint16_t first_word)
	{
		uint16_t cmd_bits = first_word & 0x1f;
		return static_cast<MSEQ_Command_Set>(cmd_bits);
	}
protected:
	MSEQCmd(const MSEQ_Command_Set kind, const size_t size = 2)
		: _kind(kind), _size(size), _remaining(size - 2) { };

	size_t _remaining{ 0 };

private:
	MSEQ_Command_Set _kind{};
	size_t _size{ 1 };
};

class MSEQCmdTrackDefinition : public MSEQCmd {
public:
	MSEQCmdTrackDefinition(uint16_t first_word)
		: MSEQCmd(MSEQ_TRACK_DEFINITION, 8)
	{
		MSEQ_Command_Set cmd = GetCmdFromFirstWord(first_word);
		if (cmd != MSEQ_TRACK_DEFINITION) {
			printf("[ERROR] header not matching (read as 0x%x, expected 0x%x)\n", cmd, MSEQ_TRACK_DEFINITION);
			_ASSERT(true);
		}

		_oc = first_word & 0x20 ? true : false;

		if (_oc) {
			// closing a track is a single command
			_remaining = 0;
		}
		else {
			_track_copy_status = (uint16_t)(first_word & 0x00c0) >> 6;
		}

		_track_number = (uint16_t)(first_word & 0xff00) >> 8;
	}

	void FeedRemaining(void* const data, size_t sz)
	{
		if (sz != _remaining) {
			printf("[ERROR] sz %lld != _remaining %lld\n", sz, _remaining);
			_ASSERT(true);
			return;
		}

		uint16_t* words = (uint16_t*)data;
		if (!words) {
			printf("[ERROR] !data\n");
			_ASSERT(true);
			return;
		}

		_track_type = (uint16_t)(words[0] & 0x003f) >> 0;
		_track_sub_type = (uint16_t)(words[0] & 0x00c0) >> 6;
		_track_length_lsb = (uint16_t)(words[0] & 0xff00) >> 8;

		_track_length_msb = words[1];

		_reserved = words[2];

		_remaining = 0;
	}


	void Dump() const {
		if (_oc) {
			printf("[DBG] CMD TrackHeader, Track %d, OC = close\n", _track_number);
		}
		else {
			printf("[DBG] CMD TrackHeader, Track %d, OC = open, copy status %d, type %d, subtype %d, length %ld\n",
				_track_number, _track_copy_status, _track_type, _track_sub_type, GetTrackLength());
		}
	}

	bool GetOC() const { return _oc; } /// < value 0 means that this track command is applied for track opening. Value 1 is used for closing a track.
	uint8_t GetTrackCopyStatus() const { return _track_copy_status; }
	uint8_t GetTrackNumber() const { return _track_number; } /// < number that identifies the track (track ID). The Track numbers are attributed by the MSEQ builder software, from track 1 to track n (maximum 255 tracks in one MSEQ file). Value 0 is reserved (see §3.2.5). If this command is used for closing a track, then the ID of the track is mentioned.
	uint8_t GetTrackType() const { return _track_type; } /// < this indicates which type of data is included in that track. So if a player or processor is not involved in that type of content, it can read the length of track and jump to read the type of the next track.
	uint8_t GetTrackSubType() const { return _track_sub_type; } /// < defines, for a track type, a particular implementation of this format. This indication is for information only, and must not be used for format decoding.
	uint32_t GetTrackLength() const /// < number of 16-bit words in track. It includes the « opening » TRACK command (3 words) and the « closing » track command (1 word).
	{
		uint32_t l = _track_length_msb;
		l <<= 8;
		l |= _track_length_lsb;
		return l;
	}

private:
	bool _oc{};
	uint8_t _track_copy_status{};
	uint8_t _track_number{};
	uint8_t _track_type{};
	uint8_t _track_sub_type{};
	uint8_t _track_length_lsb{};
	uint16_t _track_length_msb{};
	uint16_t _reserved{};
};

class MSEQCmdNote: public MSEQCmd {
public:
	MSEQCmdNote(uint16_t first_word)
		: MSEQCmd(MSEQ_NOTE, 4)
	{
		MSEQ_Command_Set cmd = GetCmdFromFirstWord(first_word);
		if (cmd != MSEQ_NOTE) {
			printf("[ERROR] header not matching (read as 0x%x, expected 0x%x)\n", cmd, MSEQ_TRACK_DEFINITION);
			_ASSERT(true);
		}

		_channel = (uint16_t)(first_word & 0x01e0) >> 5;
		_note_number = (uint16_t)(first_word & 0xFE00) >> 9;
	}

	void FeedRemaining(void* const data, size_t sz)
	{
		if (sz != _remaining) {
			printf("[ERROR] sz %lld != _remaining %lld\n", sz, _remaining);
			_ASSERT(true);
			return;
		}

		uint16_t* words = (uint16_t*)data;
		if (!words) {
			printf("[ERROR] !data\n");
			_ASSERT(true);
			return;
		}

		_velocity = (uint16_t)(words[0] & 0x003f) >> 0;
		_running_time = (uint16_t)(words[0] & 0xFFC0) >> 6;

		_remaining = 0;
	}


	void Dump() const {
		printf("[DBG] CMD Note, channel %d, note %d, velocity %d, running time %d\n",
			_channel, _note_number, _velocity, _running_time);
	}

	uint8_t GetChannel() const { return _channel; }				/// < sound channel, from 0 to 15. Most of the time, a single instrument is associated with a channel number. The definition is the same than in MIDI format.
	uint8_t GetNoteNumber() const { return _note_number; }		/// < this is the note to be played. Definition is the same than in MIDI format (Middle C of an 88 note piano-style keyboard has a reference value of 60).
	uint8_t GetVelocity() const { return _velocity; }			/// < this is the volume of the note to be played. Definition is the same than in MIDI format, excepted that the range is reduced to 0-63.
	uint16_t GetRunningTime() const { return _running_time; }	/// < defines the length of time the note has to be played. The note will sound during RUNNING_TIME x RTB x ATB (in µS).

private:
	uint8_t _channel{};
	uint8_t _note_number{};
	uint8_t _velocity{};
	uint16_t _running_time{};
};

class MSEQCmdSystem : public MSEQCmd {
public:
	MSEQCmdSystem(uint16_t first_word)
		: MSEQCmd(MSEQ_SYSTEM_COMMAND)
	{
		MSEQ_Command_Set cmd = GetCmdFromFirstWord(first_word);
		if (cmd != MSEQ_SYSTEM_COMMAND) {
			printf("[ERROR] header not matching (read as 0x%x, expected 0x%x)\n", cmd, MSEQ_SYSTEM_COMMAND);
			_ASSERT(true);
		}

		_type = (uint16_t)(first_word & 0x00E0) >> 5;
		_value = (uint16_t)(first_word & 0xff00) >> 8;
	}

	void Dump() const {
		printf("[DBG] CMD System, type %d, value %d\n", _type, _value);
	}

	uint8_t GetType() const { return _type; }		/// < specifies which SYSTEM command is to be applied
	uint16_t GetValue() const { return _value; }	/// < parameter, depends on the command TYPE.
private:
	uint8_t _type{};
	uint16_t _value{};
};

class MSEQCmdDelay : public MSEQCmd {
public:
	MSEQCmdDelay(uint16_t first_word)
		: MSEQCmd(MSEQ_DELAY_DELTA_TIME)
	{
		MSEQ_Command_Set cmd = GetCmdFromFirstWord(first_word);
		if (cmd != MSEQ_DELAY_DELTA_TIME) {
			printf("[ERROR] header not matching (read as 0x%x, expected 0x%x)\n", cmd, MSEQ_DELAY_DELTA_TIME);
			_ASSERT(true);
		}

		_wait = (uint16_t)(first_word & 0xFFE0) >> 5;
	}

	void Dump() const {
		printf("[DBG] CMD Delay, wait %d\n", _wait);
	}

	uint16_t GetWait() const { return _wait; }	/// < time length to wait. This time is always based on ATB. Thus, the total delay is WAIT_DELAY x ATB (in µS). Value 0 is prohibited.
private:
	uint16_t _wait{};
};

class MSEQCmdRTB : public MSEQCmd {
public:
	MSEQCmdRTB(uint16_t first_word)
		: MSEQCmd(MSEQ_RELATIVE_TIME_BASE_DEFINITION)
	{
		MSEQ_Command_Set cmd = GetCmdFromFirstWord(first_word);
		if (cmd != MSEQ_RELATIVE_TIME_BASE_DEFINITION) {
			printf("[ERROR] header not matching (read as 0x%x, expected 0x%x)\n", cmd, MSEQ_RELATIVE_TIME_BASE_DEFINITION);
			_ASSERT(true);
		}

		_value = (uint16_t)(first_word & 0xFFE0) >> 5;
	}

	void Dump() const {
		printf("[DBG] CMD RTB, value %d\n", _value);
	}

	uint16_t GetValue() const { return _value; }	/// < New value for Relative Time Base. Value 0 is prohibited.
private:
	uint16_t _value{};
};

class MSEQCmdPrgChange: public MSEQCmd {
public:
	MSEQCmdPrgChange(uint16_t first_word)
		: MSEQCmd(MSEQ_PROGRAM_CHANGE)
	{
		MSEQ_Command_Set cmd = GetCmdFromFirstWord(first_word);
		if (cmd != MSEQ_PROGRAM_CHANGE) {
			printf("[ERROR] header not matching (read as 0x%x, expected 0x%x)\n", cmd, MSEQ_PROGRAM_CHANGE);
			_ASSERT(true);
		}

		_channel = (uint16_t)(first_word & 0x01E0) >> 5;
		_ambience = (uint16_t)(first_word & 0x0E00) >> 9;
		_instrument_number = (uint16_t)(first_word & 0xF000) >> 12;
	}

	void Dump() const {
		printf("[DBG] CMD PrgChange, channel %d, ambience %d, instrument number %d\n", _channel, _ambience, _instrument_number);
	}

	uint8_t GetChannel() const { return _channel; }						/// < sound channel, from 0 to 15. Most of the time, a single instrument is associated with a channel number. The definition is the same than in MIDI format.
	uint8_t GetAmbience() const { return _ambience; }					/// < sound variation, from 0 to 7 (see Figure 39). VARIATION=7 sets a proprietary kind of sound. So if requested, it could result in an strange variation on other platforms.
	uint8_t GetInstrumentNumber() const { return _instrument_number; }	/// < instrument number to be associated to this channel. values 0 to 15 are possible and must respect the Figure 38 definitions (to be GM compatible).
private:
	uint8_t _channel{};
	uint8_t _ambience{};
	uint8_t _instrument_number{};
};

class MSEQCmdVolume : public MSEQCmd {
public:
	MSEQCmdVolume(uint16_t first_word)
		: MSEQCmd(MSEQ_PROGRAM_CHANGE)
	{
		MSEQ_Command_Set cmd = GetCmdFromFirstWord(first_word);
		if (cmd != MSEQ_VOLUME) {
			printf("[ERROR] header not matching (read as 0x%x, expected 0x%x)\n", cmd, MSEQ_VOLUME);
			_ASSERT(true);
		}

		_channel = (uint16_t)(first_word & 0x01E0) >> 5;
		_volume = (uint16_t)(first_word & 0xFE00) >> 9;
	}

	void Dump() const {
		printf("[DBG] CMD Volume, channel %d, volume %d\n", _channel, _volume);
	}

	uint8_t GetChannel() const { return _channel; }		/// < sound channel, from 0 to 15. The definition is the same than in MIDI format.
	uint16_t GetVolume() const { return _volume; }		/// < ???
private:
	uint8_t _channel{};
	uint16_t _volume{};
};

class MSEQCmdFactory {
public:
	static MSEQCmd* ConstructFromStream(ifstream& stream)
	{
		uint16_t first_word = 0;
		stream.read((char*)&first_word, sizeof(first_word));

		MSEQCmd* cmd = nullptr;

		switch (MSEQCmd::GetCmdFromFirstWord(first_word))
		{
		case MSEQ_NOTE:
		{
			cmd = new MSEQCmdNote(first_word);
			break;
		} // MSEQ_NOTE
		case MSEQ_DELAY_DELTA_TIME:
		{
			cmd = new MSEQCmdDelay(first_word);
			break;
		} // MSEQ_DELAY_DELTA_TIME
		case MSEQ_SYSTEM_COMMAND:
		{
			cmd = new MSEQCmdSystem(first_word);
			break;
		} // MSEQ_SYSTEM_COMMAND
		case MSEQ_TRACK_DEFINITION:
		{
			cmd = new MSEQCmdTrackDefinition(first_word);
			break;
		} // MSEQ_TRACK_DEFINITION
		case MSEQ_RELATIVE_TIME_BASE_DEFINITION:
		{
			cmd = new MSEQCmdRTB(first_word);
			break;
		} // MSEQ_RELATIVE_TIME_BASE_DEFINITION
		case MSEQ_PROGRAM_CHANGE:
		{
			cmd = new MSEQCmdPrgChange(first_word);
			break;
		} // MSEQ_PROGRAM_CHANGE
		case MSEQ_VOLUME:
		{
			cmd = new MSEQCmdVolume(first_word);
			break;
		} // MSEQ_VOLUME

		default:
		{
			printf("[ERR] unhandled command 0x%x\n", first_word);
			return nullptr;
		}
		}

		if (!cmd) { return cmd; }
		if (!cmd->IsFull()) {
			void* mem = malloc(cmd->GetRemainingSize());
			stream.read((char*)mem, cmd->GetRemainingSize());
			cmd->FeedRemaining(mem, cmd->GetRemainingSize());
			free(mem);
		}
		
		cmd->Dump();

		return cmd;
	}
};

int ProcessSeq(const char* seq, const char* mid)
{
	std::ifstream infile(seq, std::ios::binary);
	if (!infile) return -1;

	std::ofstream outfile(mid, std::ios::binary | std::ios::trunc);
	if (!outfile) return -1;

	MSEQ_Header hdr{ };


	infile.read((char*)&hdr, sizeof(hdr));


	uint32_t LengthOfFile = hdr.len_msb;
	LengthOfFile <<= 8;
	LengthOfFile |= hdr.len_lsb;

	uint32_t AbsoluteTimeBase = hdr.absolute_time_base_msb;
	AbsoluteTimeBase <<= 16;
	AbsoluteTimeBase |= hdr.absolute_time_base_lsb;

	printf("[DBG] sz %d, Magic '%c%c%c%c', LengthOfFile %d (0x%x), Tracks %d, TMode %d, EMS %d, SEQ Class %d, ATB %d (0x%x)\n",
		sizeof(hdr),
		hdr.magic[0], hdr.magic[1], hdr.magic[2], hdr.magic[3],
		LengthOfFile, LengthOfFile,
		hdr.number_of_tracks,
		hdr.tmode, hdr.ems, hdr.seq_class,
		AbsoluteTimeBase, AbsoluteTimeBase);

	// sanity checks
	if (hdr.loop_rsvd1 != 7) {
		printf("[WARN] loop_rsvd1 is not 7 (%d)\n", hdr.loop_rsvd1);
	}
	if (hdr.loop_rsvd2 != 7) {
		printf("[WARN] loop_rsvd2 is not 7 (%d)\n", hdr.loop_rsvd2);
	}
	if (hdr.reserved1 != 0x81) {
		printf("[WARN] reserved1 is not 0x81 (0x%x)\n", hdr.reserved1);
	}
	if (hdr.reserved2 != 0xff) {
		printf("[WARN] reserved2 is not 0xff (0x%x)\n", hdr.reserved2);
	}


	printf("[DBG] seq_copy_status %d, seq_copy_countdown %d, resend_status %d, resend_countdown %d\n",
		hdr.seq_copy_status, hdr.seq_copy_countdown, hdr.resend_status, hdr.resend_countdown);

	/*printf("[DBG] loop_point1 %d, loop_def1 %d, start_point1 %d, start_nb_repeat1 %d\n",
		hdr.loop_point1, hdr.loop_def1, hdr.start_point1, hdr.start_nb_repeat1);

	printf("[DBG] loop_point2 %d, loop_def2 %d, start_point2 %d, start_nb_repeat2 %d\n",
		hdr.loop_point2, hdr.loop_def2, hdr.start_point2, hdr.start_nb_repeat2);*/



	bool track_open = false;
	uint32_t track_length = 0;

	// ATB = g_tempo / base_tempo_MIDI;
	// g_tempo = 500000
	uint32_t RTB = 1;
	while (1) {
		auto cmd = MSEQCmdFactory::ConstructFromStream(infile);

		if (!cmd) {
			printf("[ERR] unhandled no cmd\n");
			break;
		}
		if (cmd->GetKind() == MSEQ_SYSTEM_COMMAND) {
			auto cmd_as_system = reinterpret_cast<MSEQCmdSystem*>(cmd);
			if (cmd_as_system->GetType() == CMD_SYSTEM_TYPE_EOF) {
				printf("[INFO] End of file!\n");
				break;
			}
		}
		else if (cmd->GetKind() == MSEQ_RELATIVE_TIME_BASE_DEFINITION) {
			auto cmd_as_rtb = reinterpret_cast<MSEQCmdRTB*>(cmd);
			RTB = cmd_as_rtb->GetValue();
		}
	}
#if 0

	while (1) {
		uint16_t a_word{};

		infile.read((char*)&a_word, sizeof(a_word));


		MSEQ_Command_or_something gen_command; gen_command.word = a_word;

		switch (gen_command.cmd) {
		case MSEQ_TRACK_DEFINITION:
		{
			MSEQ_Track_Header track_header{};
			track_header.first_word = a_word;

			infile.read((char*)&track_header.track_type_subtype, sizeof(track_header) - sizeof(a_word));

			if (track_open) {
				printf("[ERR] opening a new track when one is already open!\n");
				return -2;
			}

			track_length = track_header.track_length_msb;
			track_length <<= 8;
			track_length |= track_header.track_length_lsb;

			printf("[DBG] cmd 0x%x, O/C %d, copy status %d, track number %d\n",
				track_header.track, track_header.oc, track_header.track_copy_status, track_header.track_number);
			printf("[DBG] Track sub type %d, Length of track %d word(s)\n", track_header.track_sub_type, track_length);


			/*for (size_t seq_num = 0; seq_num < track_length; seq_num++) {
				MSEQ_Command_or_something cmd{};
				infile.read((char*)&cmd, sizeof(cmd));
				printf("[DBG] cmd seq_num %d, kind %d\n", seq_num, cmd.cmd);
			}

			MSEQ_Track_End track_end{};
			infile.read((char*)&track_header, sizeof(track_header));
			*/
			break;
		} // MSEQ_TRACK_DEFINITION

		case MSEQ_NOTE:
		{
			MSEQ_Command_Note note{};
			note.first_word = a_word;

			infile.read((char*)&note.second_word, 2);

			printf("[DBG] CMD NOTE, note %d, channel %d, velocity %d, running time %d\n", note.note_number, note.channel, note.velocity, note.running_time);
			break;
		} // MSEQ_NOTE
		default:
			printf("[ERR] unhandled command %d\n", gen_command.cmd);
			return -2;
		}
	}

#endif



	/*

	*/


	infile.close();
	outfile.close();

	return 0;
}

int main()
{

	std::string path = ".";
	for (const auto& entry : filesystem::directory_iterator(path)) {
		if (!entry.is_regular_file())
			continue;
		auto extension = entry.path().extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(),
			[](unsigned char c) { return std::tolower(c); });

		if (!extension.compare(".pic")) {
			std::string outname = entry.path().filename().replace_extension().string() + ".bmp";
			if (filesystem::exists(outname)) {
				std::cout << "[INFO] NOT going to process: " << entry.path() << " (" << outname << " already exists!)" << std::endl;
				continue;
			}
			std::cout << "[INFO] going to process: " << entry.path() << " (into " << outname << ")" << std::endl;
			ProcessPic(entry.path().string().c_str(), outname.c_str(), nullptr);
		}
		else if (!extension.compare(".seq")) {
			std::string outname = entry.path().filename().replace_extension().string() + ".mid";
			//if (filesystem::exists(outname)) {
			//	std::cout << "[INFO] NOT going to process: " << entry.path() << " (" << outname << " already exists!)" << std::endl;
			//	continue;
			//}
			std::cout << "[INFO] going to process: " << entry.path() << " (into " << outname << ")" << std::endl;
			ProcessSeq(entry.path().string().c_str(), outname.c_str());
		}
	}


	//ProcessPic("", "", nullptr);
	//fclose(fout);

	return 0;
}
