#include "Sudoku.h"
#include <iostream>

Sudoku::Sudoku(int basesize, SymbolType stype , CALLBACK callback ) :board(0), cb(callback) {

	sStats.basesize = basesize; 
	sStats.backtracks =
	sStats.moves =
	sStats.placed = 0;

    boxWidth = 0;

	first = (stype) ? 'A' : '1';
	width = basesize*basesize;
	last = first + static_cast<char>( (stype)?width:(width-1) );
	
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

    for (size_t i = 0; i < size; ++i) {
        
        board[i] = (values[i] == '.') ? EMPTY_CHAR : values[i];
       // std::cout << board[i] << std::endl;
    }
   // dumpboard();
}

bool Sudoku::Solve() {
	//SetupBoard(board, sStats.basesize* sStats.basesize);
    
    size_t init_val = 0;
	return (place_value(init_val)) ? true : false;
}

bool Sudoku::place_value(size_t place) {
	bool replace = false;
	
    if (place == (width*width))
        return true;

    if (board[place] != EMPTY_CHAR) 
        place_value(place+1);
    

	for (char val = first; val <= last; ++val) {
		//if (!cb(*this, board, MSG_ABORT_CHECK, sStats.moves, sStats.basesize, (unsigned)place, board[place]))
		//	return false;
        if (place == (width*width))
            return true;

		board[place] = val;
		++moves_;
		++sStats.moves;
        
		//cb(*this, board, MSG_PLACING, sStats.moves, sStats.basesize, (unsigned)place, board[place]);
        
        if (ConflictCheck(place, val)) {
            ++sStats.placed;
            if (place == (width*width) - 1) 
                return true;
            else 
                return place_value(place+1);
            
        }
        else 
            board[place] = EMPTY_CHAR;
        
	}
    board[place] = EMPTY_CHAR;
    ++sStats.backtracks;
    --moves_;
	return false;
}

bool Sudoku::ConflictCheck(size_t place, char val) {
    bool maintain = true;

    size_t startrow = place - place%width;
    size_t endrow = startrow + width;
    
    //row check
    for (size_t i = startrow; i < endrow; ++i) {
        if (board[i] == val && i != place) {
            maintain = false;
            break;
        }
    }

    size_t startcol = place%width;
    size_t endcol = startcol + (width - 1)*width;
    
    //column check
    for (size_t i = startcol; i <= endcol; i += width) {
        if (board[i] == val && i != place) {
            maintain = false;
            break;
        }
    }

    size_t boxstart = place - (place % sStats.basesize); // row 
    size_t rowoffset = place / width;
    
    if (rowoffset % width)
        boxstart -= rowoffset * width; // column

    size_t boxend = boxstart + (sStats.basesize - 1) * width;

    // box check
    for (size_t i = boxstart; i <= boxend; i += width) {
        bool brake = true;
        for (size_t j = 0; j <= sStats.basesize - 1; ++j) {
            if (board[i + j] == val && (i + j) != place) {
                brake =
                maintain = false;
                break;
            }
        }
        if (!brake) break;
    }

    std::cout << "row: " << startrow << " " << endrow << std::endl;
    std::cout << "--" << std::endl;
    std::cout << "column: " << startcol << " " << endcol << std::endl;
    std::cout << "--" << std::endl;
    std::cout << "box: " << boxstart << " " << boxend << std::endl;
    std::cout << "--" << std::endl;
    dumpboard();
    return maintain;
}