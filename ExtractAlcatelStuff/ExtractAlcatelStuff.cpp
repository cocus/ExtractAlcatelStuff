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
#include <poppack.h>


int ProcessSeq(const char* seq, const char* mid)
{
	std::ifstream infile(seq, std::ios::binary);
	if (!infile) return -1;

	std::ofstream outfile(mid, std::ios::binary | std::ios::trunc);
	if (!outfile) return -1;

	MSEQ_Header hdr{ };


	infile.read((char*) &hdr, sizeof(hdr));


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
