#pragma once

#include <string.h>
#include <vector>

using namespace std;

namespace Maze {
	enum {
		CELL_WALL_NORTH = 0x01,
		CELL_WALL_SOUTH = 0x02,
		CELL_WALL_EAST = 0x04,
		CELL_WALL_WEST = 0x08,
		CELL_VISITED = 0x10
	};

	class Backtracking {
	public:
		unsigned char* cells;
		int width;
		int height;

		Backtracking(int _width, int _height) : width(_width), height(_height) {
			cells = new unsigned char[width * height];
			memset(cells, 0x0f, width * height * sizeof(unsigned char));
		}

		void createHelper(char cell, int pos) {
			vector<char> directions;

			// East
			if ((pos + 1) % width != 0 && (cells[pos + 1] & CELL_VISITED) == 0) {
				directions.push_back(0);
			}
			// West
			if (pos % width != 0 && (cells[pos - 1] & CELL_VISITED) == 0) {
				directions.push_back(1);
			}
			// North
			if (pos - width > -1 && (cells[pos - width] & CELL_VISITED) == 0) {
				directions.push_back(2);
			}
			// South
			if (pos + width < height * width && (cells[pos + width] & CELL_VISITED) == 0) {
				directions.push_back(3);
			}

			while (!directions.empty()) {
				int idx = rand() % directions.size() + 0;

				char currDirection = directions[idx];
				directions.erase(directions.begin() + idx);

				switch (currDirection) {
				case 0:  // East
					if ((cells[pos + 1] & CELL_VISITED) == 0) {
						cells[pos + 1] &= ~CELL_WALL_WEST;
						cells[pos] &= ~CELL_WALL_EAST;
						cells[pos + 1] |= CELL_VISITED;
						createHelper(cells[pos + 1], pos + 1);
					}
					break;
				case 1:  // West
					if ((cells[pos - 1] & CELL_VISITED) == 0) {
						cells[pos - 1] &= ~CELL_WALL_EAST;
						cells[pos] &= ~CELL_WALL_WEST;
						cells[pos - 1] |= CELL_VISITED;
						createHelper(cells[pos - 1], pos - 1);
					}
					break;
				case 2:  // North
					if ((cells[pos - width] & CELL_VISITED) == 0) {
						cells[pos - width] &= ~CELL_WALL_SOUTH;
						cells[pos] &= ~CELL_WALL_NORTH;
						cells[pos - width] |= CELL_VISITED;
						createHelper(cells[pos - width], pos - width);
					}
					break;
				case 3:  // South
					if ((cells[pos + width] & CELL_VISITED) == 0) {
						cells[pos + width] &= ~CELL_WALL_NORTH;
						cells[pos] &= ~CELL_WALL_SOUTH;
						cells[pos + width] |= CELL_VISITED;
						createHelper(cells[pos + width], pos + width);
					}
					break;
				default:
					break;
				}
			}
		}

		void create() {
			cells[0] |= CELL_VISITED;

			srand(time(NULL));

			createHelper(cells[0], 0);
		}
	};
}  // namespace Maze
