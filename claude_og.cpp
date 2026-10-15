/*
 * ============================================================================
 * Program Name  : Assignment4.cpp
 * Description   : Solves Sudoku puzzles using object-oriented programming and
 *                 recursive depth-first search with backtracking. The program
 *                 finds EVERY valid solution for a puzzle (not just the first),
 *                 and reports "No solution found" when none exist.
 * Inputs        : Five text files in the current directory:
 *                 puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, puzzle5.txt
 *                 Each file holds a 9x9 grid. Digits 1-9 are given numbers and
 *                 an underscore (_) is an empty cell. Spaces/newlines between
 *                 characters are ignored, so "5 3 _ ..." and "53_..." both work.
 * Outputs       : For each puzzle (printed to the console):
 *                   - the puzzle's filename
 *                   - the original puzzle
 *                   - every solution found, or "No solution found"
 * Author        : Parker Curl
 * Creation Date : October 15, 2026
 * Collaborators : None
 * AI Sources    : Claude (Anthropic) was used to help draft this program
 *                 structure, the backtracking logic, and the comments.
 *
 * Compile : g++ -std=c++11 -Wall -o Assignment4 Assignment4.cpp
 * Run     : ./Assignment4
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// ----------------------------------------------------------------------------
// Class: SudokuPuzzle
// Represents one 9x9 Sudoku board. It knows how to load itself from a file,
// print itself, and check whether placing a number in a cell follows the rules.
// ----------------------------------------------------------------------------
class SudokuPuzzle {
public:
    static const int SIZE = 9;       // The board is 9 x 9
    static const int BLOCK = 3;      // Each sub-box is 3 x 3
    static const int EMPTY = 0;      // We store empty cells as 0 internally

    // Start with every cell empty
    SudokuPuzzle() {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++)
                grid[r][c] = EMPTY;
    }

    // Read the puzzle from a file. Returns false if the file can't be opened
    // or doesn't contain exactly 81 valid cells.
    bool loadFromFile(const string& filename) {
        ifstream in(filename.c_str());
        if (!in) {
            return false;
        }

        int count = 0;      // How many cells we've read so far
        char ch;
        while (in.get(ch)) {
            if (ch >= '1' && ch <= '9') {
                if (count >= SIZE * SIZE) return false;   // too many cells
                grid[count / SIZE][count % SIZE] = ch - '0';
                count++;
            } else if (ch == '_') {
                if (count >= SIZE * SIZE) return false;   // too many cells
                grid[count / SIZE][count % SIZE] = EMPTY;
                count++;
            }
            // Anything else (spaces, newlines, tabs, '\r') is skipped.
        }
        return count == SIZE * SIZE;
    }

    // Print the board, showing '_' for empty cells
    void print() const {
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (grid[r][c] == EMPTY) cout << '_';
                else cout << grid[r][c];
                if (c < SIZE - 1) cout << ' ';
            }
            cout << endl;
        }
    }

    // Get / set a single cell
    int get(int row, int col) const { return grid[row][col]; }
    void set(int row, int col, int value) { grid[row][col] = value; }

    // Can 'value' be placed at (row, col) without breaking a Sudoku rule?
    // We check the row, the column, and the 3x3 block.
    // (The cell itself is skipped so this also works on already-filled cells.)
    bool isValidPlacement(int row, int col, int value) const {
        // Check the row and the column
        for (int i = 0; i < SIZE; i++) {
            if (i != col && grid[row][i] == value) return false;
            if (i != row && grid[i][col] == value) return false;
        }

        // Check the 3x3 block: find the block's top-left corner first
        int startRow = (row / BLOCK) * BLOCK;
        int startCol = (col / BLOCK) * BLOCK;
        for (int r = startRow; r < startRow + BLOCK; r++) {
            for (int c = startCol; c < startCol + BLOCK; c++) {
                if ((r != row || c != col) && grid[r][c] == value) return false;
            }
        }
        return true;
    }

    // Do the numbers already given in the puzzle follow the rules?
    // If two givens conflict, the puzzle can never be solved.
    bool givensAreValid() const {
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (grid[r][c] != EMPTY &&
                    !isValidPlacement(r, c, grid[r][c])) {
                    return false;
                }
            }
        }
        return true;
    }

    // Find the first empty cell (scanning left-to-right, top-to-bottom).
    // Returns true and fills row/col if one exists; false if the board is full.
    bool findEmptyCell(int& row, int& col) const {
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (grid[r][c] == EMPTY) {
                    row = r;
                    col = c;
                    return true;
                }
            }
        }
        return false;
    }

private:
    int grid[SIZE][SIZE];   // 0 = empty, 1-9 = a number
};

// ----------------------------------------------------------------------------
// Class: SudokuSolver
// Solves a SudokuPuzzle using recursive depth-first search with backtracking
// and prints every solution it finds.
// ----------------------------------------------------------------------------
class SudokuSolver {
public:
    // Solve the given puzzle and print all solutions.
    // We work on a copy so the caller's original puzzle isn't changed.
    void solveAndPrintAll(const SudokuPuzzle& original) {
        solutionCount = 0;
        SudokuPuzzle working = original;

        // If the starting numbers already break the rules, skip the search.
        if (working.givensAreValid()) {
            search(working);
        }

        if (solutionCount == 0) {
            cout << "No solution found" << endl;
        } else if (solutionCount > 1) {
            cout << "Total solutions found: " << solutionCount << endl;
        }
    }

private:
    int solutionCount;   // How many solutions we've found for this puzzle

    // The recursive heart of the program.
    //
    // How it works:
    //   1. Find an empty cell. If there are none, the board is complete,
    //      so we've found a solution: print it.
    //   2. Otherwise try the numbers 1-9 in that cell. For each number that
    //      is legal, place it and recursively try to solve the rest.
    //   3. After the recursive call returns (whether or not it found
    //      anything), REMOVE the number (backtrack) and try the next one.
    //      Because we keep going instead of stopping at the first solution,
    //      we discover every solution.
    void search(SudokuPuzzle& puzzle) {
        int row, col;

        // Base case: no empty cells left -> the puzzle is solved
        if (!puzzle.findEmptyCell(row, col)) {
            solutionCount++;
            cout << "Solution " << solutionCount << ":" << endl;
            puzzle.print();
            cout << endl;
            return;
        }

        // Recursive case: try each digit in the empty cell
        for (int value = 1; value <= 9; value++) {
            if (puzzle.isValidPlacement(row, col, value)) {
                puzzle.set(row, col, value);          // make a choice
                search(puzzle);                        // explore deeper
                puzzle.set(row, col, SudokuPuzzle::EMPTY);  // undo (backtrack)
            }
        }
        // If no digit worked, we simply return, which sends the previous
        // call back to try its next digit.
    }
};

// ----------------------------------------------------------------------------
// Class: SudokuApp
// Controls the overall program: loops over the five puzzle files, loads each
// one, prints it, and asks the solver to solve it.
// ----------------------------------------------------------------------------
class SudokuApp {
public:
    SudokuApp() {
        // The five input files required by the assignment
        files.push_back("puzzle1.txt");
        files.push_back("puzzle2.txt");
        files.push_back("puzzle3.txt");
        files.push_back("puzzle4.txt");
        files.push_back("puzzle5.txt");
    }

    void run() {
        for (size_t i = 0; i < files.size(); i++) {
            processPuzzle(files[i]);
        }
    }

private:
    vector<string> files;   // Names of the puzzle files
    SudokuSolver solver;    // Does the actual solving

    // Handle one puzzle file from start to finish
    void processPuzzle(const string& filename) {
        cout << "==============================" << endl;
        cout << "File: " << filename << endl;
        cout << "==============================" << endl;

        SudokuPuzzle puzzle;
        if (!puzzle.loadFromFile(filename)) {
            cout << "Error: could not read a valid 9x9 puzzle from "
                 << filename << endl << endl;
            return;
        }

        cout << "Original puzzle:" << endl;
        puzzle.print();
        cout << endl;

        solver.solveAndPrintAll(puzzle);
        cout << endl;
    }
};

// ----------------------------------------------------------------------------
// main: create the application object and run it.
// ----------------------------------------------------------------------------
int main() {
    SudokuApp app;
    app.run();
    return 0;
}