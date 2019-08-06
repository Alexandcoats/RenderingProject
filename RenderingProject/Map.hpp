#pragma once
#include <stb_image.h>
#include <stb_image_write.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <functional>
#include <random>

class Map {
public:
	Map();
	~Map();

	void initMap(const char * filename);
	void saveMap(const char * filename);

	struct MinimalPiece {
		int pieceInd;
		int flp = 0;
		int rot = 0;
	};

private:
	int imgWidth, imgHeight, channels;
	unsigned char * tileImageData;
	static const int mapSize = 10;

	struct Pixel {
		unsigned char r;
		unsigned char g;
		unsigned char b;

		Pixel(unsigned char * rgb);
		~Pixel();

		bool operator==(const Pixel rhs) {
			return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b;
		}

		bool operator!=(const Pixel rhs) {
			return !(*this == rhs);
		}
	};

	struct Piece : MinimalPiece {
		const static int pieceSize = 11;
		unsigned char * imageData;
		bool xsymm = true;
		bool ysymm = true;
		bool xysymm = true;
		bool yxsymm = true;
		enum Side { empty /* empty connector */, red, blue, left_green, right_green, wall /* edge of map */, null /* missing tile */ };
		Side sides[4];

		Piece(int pieceInd, unsigned char * imageData);
		~Piece();

		Pixel index(int x, int y);

		Pixel imageIndex(int x, int y);

		Side * flipGreen(Side * sides);

		Side * getSides();

		Side getSide(int side);
	};

	void createMap();

public:
	std::vector<std::vector<MinimalPiece*>> map;
};

