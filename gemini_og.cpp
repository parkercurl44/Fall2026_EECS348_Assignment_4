/**
 * ============================================================================
 * Program Name: Sudoku Solver (Assignment 4)
 * Description:  This program reads five Sudoku puzzles from text files 
 *               (puzzle1.txt through puzzle5.txt), solves them using an 
 *               object-oriented approach with recursive depth-first search and 
 *               backtracking, and prints all valid solutions or a "No solution 
 *               found" message.
 * Inputs:       Five text files (puzzle1.txt, puzzle2.txt, puzzle3.txt, 
 *               puzzle4.txt, puzzle5.txt) containing 9x9 grids with digits 
 *               1-9 and underscores ('_') for empty cells.
 * Outputs:      Console output displaying each puzzle's filename, the original 
 *               puzzle grid, and all solved solutions (or an indication if no 
 *               solution exists).
 * Author:       [Your Name]
 * Creation Date: October 15, 2026
 * Collaborators: [None / List any collaborators]
 * AI Sources:    [OpenAI / Gemini used for architectural guidance and debugging]
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/**
 * @class SudokuSolver
 * @brief Encapsulates a 9x9 Sudoku board, file loading, validation rules, 
 *        and the recursive backtracking solving algorithm.
 */
class SudokuSolver {
private:
    int grid[9][9]; // 9x9 grid where 0 represents an empty cell

    /**
     * Checks if placing a number at a specific row and column violates 
     * standard Sudoku rules (row, column, and 3x3 subgrid check).
     */
    bool isValid(int row, int col, int num) const {
        // Check row and column constraints
        for (int i = 0; i < 9; ++i) {
            if (grid[row][i] == num) return false;
            if (grid[i][col] == num) return false;
        }

        // Check 3x3 subgrid constraints
        int startRow = row - (row % 3);
        int startCol = col - (col % 3);
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (grid[startRow + r][startCol + c] == num) return false;
            }
        }

        return true;
    }

    /**
     * Recursive backtracking helper function that explores valid number placements 
     * and counts/prints all valid solutions.
     */
    void solveHelper(int row, int col, int &solutionCount) {
        // Base case: If we have successfully moved past the last row, a solution is found
        if (row == 9) {
            solutionCount++;
            cout << "Solution " << solutionCount << ":\n";
            printGrid();
            cout << "---------------------\n";
            return; // Return to continue searching for other possible solutions
        }

        // Determine the coordinates of the next cell
        int nextRow = (col == 8) ? row + 1 : row;
        int nextCol = (col == 8) ? 0 : col + 1;

        // If the current cell is already filled, skip to the next cell
        if (grid[row][col] != 0) {
            solveHelper(nextRow, nextCol, solutionCount);
            return;
        }

        // Try placing digits 1 through 9 in the empty cell
        for (int num = 1; num <= 9; ++num) {
            if (isValid(row, col, num)) {
                grid[row][col] = num;           // Temporarily place number
                solveHelper(nextRow, nextCol, solutionCount); // Recurse
                grid[row][col] = 0;             // Backtrack
            }
        }
    }

public:
    /**
     * Constructor initializes the grid to all zeros.
     */
    SudokuSolver() {
        clear();
    }

    /**
     * Resets the grid.
     */
    void clear() {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                grid[i][j] = 0;
            }
        }
    }

    /**
     * Loads a 9x9 Sudoku puzzle from a text file, parsing underscores as 0 
     * and digits 1-9 accordingly.
     */
    bool loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        char ch;
        int count = 0;
        // Read up to 81 valid puzzle characters, skipping whitespace automatically
        while (count < 81 && file >> ch) {
            if (ch == '_' || (ch >= '1' && ch <= '9')) {
                int r = count / 9;
                int c = count % 9;
                grid[r][c] = (ch == '_') ? 0 : (ch - '0');
                count++;
            }
        }

        file.close();
        return (count == 81);
    }

    /**
     * Prints the Sudoku grid with 3x3 block separators for readability.
     */
    void printGrid() const {
        for (int i = 0; i < 9; ++i) {
            if (i > 0 && i % 3 == 0) {
                cout << "------+------+------\n";
            }
            for (int j = 0; j < 9; ++j) {
                if (j > 0 && j % 3 == 0) cout << "|";
                if (grid[i][j] == 0) {
                    cout << "_ ";
                } else {
                    cout << grid[i][j] << " ";
                }
            }
            cout << "\n";
        }
    }

    /**
     * Initiates the solving process and outputs results or notifications if no solution exists.
     */
    void solve() {
        int solutionCount = 0;
        solveHelper(0, 0, solutionCount);

        if (solutionCount == 0) {
            cout << "No solution found.\n";
        } else {
            cout << "Total solutions found: " << solutionCount << "\n";
        }
    }
};

int main() {
    // Array of the five required puzzle files
    string puzzleFiles[5] = {
        "puzzle1.txt", 
        "puzzle2.txt", 
        "puzzle3.txt", 
        "puzzle4.txt", 
        "puzzle5.txt"
    };

    // Process each puzzle sequentially
    for (int i = 0; i < 5; ++i) {
        cout << "========================================\n";
        cout << "Processing: " << puzzleFiles[i] << "\n";
        cout << "========================================\n";

        SudokuSolver solver;
        if (!solver.loadFromFile(puzzleFiles[i])) {
            cout << "Error: Could not open or read " << puzzleFiles[i] << ".\n\n";
            continue;
        }

        cout << "Original Puzzle:\n";
        solver.printGrid();
        cout << "\nSolving...\n";

        solver.solve();
        cout << "\n";
    }

    return 0;
}