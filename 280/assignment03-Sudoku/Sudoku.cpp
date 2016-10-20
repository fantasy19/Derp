#include "Sudoku.h"

Sudoku::Sudoku(int basesize, SymbolType stype = SYM_NUMBER, CALLBACK callback = 0) :board(new char[basesize*basesize]), cb(callback) {
	sStats.basesize = basesize; 
	sStats.backtracks =
	sStats.moves =
	sStats.placed = 0;
}

Sudoku::~Sudoku() { delete[] board; }

Sudoku::SudokuStats Sudoku::GetStats() const {
	return sStats;
}

const char * Sudoku::GetBoard() const {
	return board;
}

void Sudoku::SetupBoard(const char *values, size_t size) {
	sStats.basesize =
		width = size;

	for (int i = 0;i < size;++i) 
		board[i] = (values[i] == '.')?EMPTY_CHAR:values[i];
	
}

bool Sudoku::Solve() {

}