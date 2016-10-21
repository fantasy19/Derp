#include "Sudoku.h"

Sudoku::Sudoku(int basesize, SymbolType stype , CALLBACK callback ) :board(0), cb(callback) {

	sStats.basesize = basesize; 
	sStats.backtracks =
	sStats.moves =
	sStats.placed = 0;

	first = (stype) ? 'A' : '1';
	width = basesize*basesize;
	last = first + (char)width;
	
}

Sudoku::~Sudoku() { delete[] board; }

Sudoku::SudokuStats Sudoku::GetStats() const {
	return sStats;
}

const char * Sudoku::GetBoard() const {
	return board;
}

void Sudoku::SetupBoard(const char *values, size_t size) {

	board = new char[size];

	for (size_t i = 0;i < size;++i) 
		board[i] = (values[i] == '.')?EMPTY_CHAR:values[i];
	
}

bool Sudoku::Solve() {
	SetupBoard(board, sStats.basesize* sStats.basesize);

	return (place_value(1)) ? true : false;
}

bool Sudoku::place_value(size_t place) {
	bool replace = false;
	
	for (char val = first; val <= last; ++val) {

		//if (!cb(*this, board, MSG_ABORT_CHECK, sStats.moves, sStats.basesize, (unsigned)place, board[place]))
		//	return false;

		board[place] = val;
		++moves_;
		++sStats.moves;

		//cb(*this, board, MSG_PLACING, sStats.moves, sStats.basesize, (unsigned)place, board[place]);

		size_t startrow = place - place%width ;
		size_t endrow = startrow + width;

		//row check
		for (size_t i = startrow; i != endrow;++i) {
			if (board[i] == val && i != place) {
				replace = true;
				break;
			}
		}

		size_t startcol = place%width;
		size_t endcol = startcol + (width - 1)*width;

		//column check
		for (size_t i = startcol; i != endcol;i+=width) {
			if (board[i] == val && i != place) {
				replace = true;
				break;
			}
		}

		size_t offset = (sStats.basesize - 1) * sStats.basesize;
		size_t boxstart = place - (place % sStats.basesize);
		//size_t boxend = boxstart + (sStats.basesize - 1) * width + sStats.basesize;

		for (size_t i = boxstart; i < sStats.basesize;i += offset) {
			for (size_t j = 0; j < sStats.basesize; ++j) {
				if (board[i+j] == val && i+j != place) {
					replace = true;
					break;
				}
			}
		}

		if (!replace) {
			++sStats.placed;
			if (place != (width*width) - 1) 
				place_value(++place);
			else
				break;	
		}
		else {
		//	if (!cb(*this, board, MSG_ABORT_CHECK, sStats.moves, sStats.basesize, (unsigned)place, board[place]))
		//		return false;

			board[place] = EMPTY_CHAR;
			//cb(*this, board, MSG_REMOVING, sStats.moves, sStats.basesize, (unsigned)place, board[place]);
			if (val == last) {
				++sStats.backtracks;
				--moves_;
				return replace;
			}
		}

	}

	return replace;
}