#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "Map.hpp"

Map::Map(){}

Map::~Map(){
	stbi_image_free(tileImageData);
}

void Map::initMap(const char * filename) {
	tileImageData = stbi_load(filename, &imgWidth, &imgHeight, &channels, 3);
	createMap();
}

void Map::saveMap(const char * filename) {
	const int tileSize = 11;
	unsigned char imageData[3 * mapSize * tileSize * mapSize * tileSize];
	for (int i = 0, mapx = 0, mapy = 0, piecex = 0, piecey = 0; 
			i < mapSize * tileSize * mapSize * tileSize; 
			++i, 
			mapx = (i / tileSize) % mapSize, 
			mapy = (i / (tileSize * tileSize * mapSize)), 
			piecex = (i % tileSize), 
			piecey = (i / (tileSize * mapSize) % tileSize ) ) {
		Pixel p = ((Piece *)map[mapy][mapx])->imageIndex(piecex, piecey);
		imageData[i*3] = p.r;
		imageData[i*3+1] = p.g;
		imageData[i*3+2] = p.b;
	}
	stbi_write_png(filename, mapSize*tileSize, mapSize*tileSize, 3, &imageData, 3*sizeof(char)*mapSize*tileSize);
}

void Map::createMap() {
	std::vector<Piece> pieces;
	for (int i = 0; i < imgHeight / imgWidth; ++i) {
		pieces.push_back(Piece{ i, tileImageData });
	}
	for (int i = 0; i < mapSize; ++i) {
		map.push_back({});
		for (int j = 0; j < mapSize; ++j) {
			map[i].push_back(nullptr);
		}
	}
	map[(int)(mapSize / 2)][(int)(mapSize / 2)] = new Piece(1,tileImageData);
	std::vector<std::tuple<int,int,double>> allIndexes;
	for (int x = 0; x < mapSize; ++x) {
		for (int y = 0; y < mapSize; ++y) {
			allIndexes.push_back({x,y,sqrt(pow(x-mapSize/2,2) + pow(y-mapSize/2,2))});
		}
	}
	std::sort(allIndexes.begin(), allIndexes.end(), [](std::tuple<int, int, double> a, std::tuple<int, int, double> b) { return std::get<2>(a) < std::get<2>(b); });
	for (auto tuple : allIndexes) {
		int x = std::get<0>(tuple), y = std::get<1>(tuple);
		if (map[y][x] != nullptr) continue;
		Piece::Side connectors[4];
		for (auto list : std::vector<std::vector<int>>{ {0,-1,0},{1,0,1},{2,1,0},{3,0,-1} }) {
			int s = list[0], j = list[1], i = list[2];
			if (y + j < 0 || x + i >= mapSize || y + j >= mapSize || x + i < 0) {
				connectors[s] = Piece::Side::wall;
				continue;
			}
			Piece * neighbor = (Piece *)map[y + j][x + i];
			if (neighbor == nullptr) connectors[s] = Piece::Side::null;
			else connectors[s] = neighbor->getSide((s + 2) % 4);
		}
		std::vector<Piece> potentialPieces;
		for (int index = 0; index < pieces.size(); ++index) {
			Piece * checkPiece = new Piece(pieces[index]);
			int flips = checkPiece->xsymm || checkPiece->ysymm || checkPiece->xysymm || checkPiece->yxsymm ? 1 : 2;
			int rots = checkPiece->xsymm && checkPiece->ysymm ? 1 : 4;
			for (checkPiece->flp = 0; checkPiece->flp < flips; ++checkPiece->flp) {
				for (checkPiece->rot = 0; checkPiece->rot < rots; ++checkPiece->rot) {
					auto add = true;
					auto sides = checkPiece->getSides();
					for (int i = 0; i < 4; ++i) {
						if (!(connectors[i] == Piece::Side::null || (connectors[i] != Piece::Side::left_green && connectors[i] != Piece::Side::right_green && connectors[i] == sides[i]) || (connectors[i] == Piece::Side::left_green && sides[i] == Piece::Side::right_green) || (connectors[i] == Piece::Side::right_green && sides[i] == Piece::Side::left_green))) {
							add = false;
							break;
						}
					}
					if (add) potentialPieces.push_back(*checkPiece);
				}
			}
		}
		if (potentialPieces.size() == 0) continue;
		std::default_random_engine eng;
		//eng.seed(0);
		std::uniform_int_distribution<int> rand(0, potentialPieces.size()-1);
		map[y][x] = new Piece(potentialPieces[rand(eng)]);
	}

	struct pair { 
		int x, y; 

		bool operator==(pair rhs) {
			return this->x == rhs.x && this->y == rhs.y;
		}
	};
	
	std::vector<pair> keepList = { pair{ (int)(mapSize / 2), (int)(mapSize / 2) } };
	int index = 0;
	while(index < keepList.size()){
		auto x = keepList[index].x, y = keepList[index].y;
		index++;
		auto curr_piece = map[y][x];
		for (auto list : std::vector<std::vector<int>>{ { 0,-1,0 },{ 1,0,1 },{ 2,1,0 },{ 3,0,-1 } }) {
			int s = list[0], j = list[1], i = list[2];
			if (y + j < 0 || x + i >= mapSize || y + j >= mapSize || x + i < 0) continue;
			Piece * neighbor = (Piece *)map[y + j][x + i];
			if (std::find(keepList.begin(), keepList.end(), pair{ x + i, y + j }) != keepList.end()) continue;
			if (neighbor == nullptr) continue;
			else {
				if (((Piece*)map[y][x])->getSide(s) == Piece::Side::empty) continue;
				keepList.push_back(pair{ x + i, y + j });
			}
		}
	}
	for (int x = 0; x < mapSize; ++x) {
		for (int y = 0; y < mapSize; ++y) {
			if (std::find(keepList.begin(), keepList.end(), pair{ x, y }) == keepList.end()) map[y][x] = new Piece(pieces[0]);
		}
	}
}

Map::Piece::Piece(int pieceInd, unsigned char * imageData){
	this->imageData = imageData;
	this->pieceInd = pieceInd;
	for (int y = 0; y < pieceSize; ++y) {
		for (int x = 0; x < pieceSize; ++x) {
			if (xsymm && index(x, y) != index(pieceSize - x - 1,y)) xsymm = false;
			if (ysymm && index(x, y) != index(x, pieceSize - y - 1)) ysymm = false;
			if (xysymm && index(x, y) != index(y, x)) xysymm = false;
			if (yxsymm && index(x, y) != index(pieceSize - y - 1, pieceSize - x - 1)) yxsymm = false;
		}
	}
	for (rot; rot < 4; ++rot) {
		sides[rot] = Side::empty;
		for (int p = 1; p < pieceSize; ++p) {
			Pixel pix = index(p, 0);
			if (pix.r > 0 && pix.g == 0 && pix.b == 0) {
				sides[rot] = Side::red;
				break;
			}
			if (pix.r == 0 && pix.g == 0 && pix.b > 0) {
				sides[rot] = Side::blue;
				break;
			}
			if (pix.r == 0 && pix.g > 0 && pix.b == 0) {
				if(p == 1) sides[rot] = Side::left_green;
				else sides[rot] = Side::right_green;
				break;
			}
		}
	}
	rot = 0;
}

Map::Piece::~Piece() {

}

Map::Pixel Map::Piece::index(int x, int y) {
	if (rot % 4 == 0) return Pixel{ &((&imageData[pieceInd*pieceSize*pieceSize *3])[3 * (y*pieceSize + x)]) };
	if (rot % 4 == 1) return Pixel{ &((&imageData[pieceInd*pieceSize*pieceSize *3])[3 * (x*pieceSize + (pieceSize - y - 1))]) };
	if (rot % 4 == 2) return Pixel{ &((&imageData[pieceInd*pieceSize*pieceSize *3])[3 * ((pieceSize - y - 1)*pieceSize + (pieceSize - x - 1))]) };
	if (rot % 4 == 3) return Pixel{ &((&imageData[pieceInd*pieceSize*pieceSize *3])[3 * ((pieceSize - x - 1)*pieceSize + y)]) };
}

Map::Pixel Map::Piece::imageIndex(int x, int y) {
	if (flp % 2 == 1) {
		if (rot % 4 == 0) return Pixel{ &((&imageData[pieceInd*pieceSize*pieceSize * 3])[3 * (y*pieceSize + (pieceSize - x - 1))]) };
		if (rot % 4 == 1) return Pixel{ &((&imageData[pieceInd*pieceSize*pieceSize * 3])[3 * (x*pieceSize + y)]) };
		if (rot % 4 == 2) return Pixel{ &((&imageData[pieceInd*pieceSize*pieceSize * 3])[3 * ((pieceSize - y - 1)*pieceSize + x)]) };
		if (rot % 4 == 3) return Pixel{ &((&imageData[pieceInd*pieceSize*pieceSize * 3])[3 * ((pieceSize - x - 1)*pieceSize + (pieceSize - y - 1))]) };
	}
	else return index(x, y);
}

Map::Piece::Side * Map::Piece::flipGreen(Side * sides) {
	Side res[4];
	for (int i = 0; i < 4; ++i) {
		if (sides[i] == Side::left_green)
			res[i] = Side::right_green;
		else if (sides[i] == Side::right_green)
			res[i] = Side::left_green;
		else
			res[i] = sides[i];
	}
	return res;
}

Map::Piece::Side * Map::Piece::getSides() {
	Side s[4];
	for (int i = 0; i < 4; ++i) s[i] = sides[i];
	if (flp % 2 == 1) {
		auto f = flipGreen(s);
		s[0] = f[0];
		s[1] = f[3];
		s[2] = f[2];
		s[3] = f[1];
	}
	Side res[4];
	for (int i = 0; i < 4; ++i) {
		if(i+rot<4) res[i] = s[i+rot];
		else res[i] = s[(i+rot)%4];
	}
	return res;
}

Map::Piece::Side Map::Piece::getSide(int side) {
	return getSides()[side%4];
}

Map::Pixel::Pixel(unsigned char * rgb) {
	this->r = rgb[0];
	this->g = rgb[1];
	this->b = rgb[2];
}

Map::Pixel::~Pixel() {

}
