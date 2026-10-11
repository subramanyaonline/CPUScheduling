
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;
using Grid = vector<vector<int>>;

mt19937 rng(random_device{}());

// Check whether a number can be placed in a cell
bool isSafe(const Grid& grid, int row, int col, int num) {
    for (int i = 0; i < 9; i++) {
        if (grid[row][i] == num || grid[i][col] == num)
            return false;
    }

    int startRow = (row / 3) * 3;
    int startCol = (col / 3) * 3;

    for (int r = startRow; r < startRow + 3; r++) {
        for (int c = startCol; c < startCol + 3; c++) {
            if (grid[r][c] == num)
                return false;
        }
    }

    return true;
}

// Print the Sudoku grid
void printGrid(const Grid& grid) {
    for (int row = 0; row < 9; row++) {
        if (row > 0 && row % 3 == 0)
            cout << "------+-------+------\n";

        for (int col = 0; col < 9; col++) {
            if (col > 0 && col % 3 == 0)
                cout << "| ";

            if (grid[row][col] == 0)
                cout << ". ";
            else
                cout << grid[row][col] << ' ';
        }

        cout << '\n';
    }
}

// Generate a complete, randomized Sudoku grid
bool generateSolvedGrid(Grid& grid) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (grid[row][col] != 0)
                continue;

            vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            shuffle(nums.begin(), nums.end(), rng);

            for (int num : nums) {
                if (isSafe(grid, row, col, num)) {
                    grid[row][col] = num;

                    if (generateSolvedGrid(grid))
                        return true;

                    grid[row][col] = 0;
                }
            }

            return false;
        }
    }

    return true;
}

// Find an empty cell with the fewest possible candidates (MRV)
bool findBestCell(const Grid& grid, int& bestRow, int& bestCol,
                  vector<int>& bestCandidates) {
    int fewest = 10;
    bool found = false;

    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (grid[row][col] != 0)
                continue;

            vector<int> candidates;

            for (int num = 1; num <= 9; num++) {
                if (isSafe(grid, row, col, num))
                    candidates.push_back(num);
            }

            if (candidates.empty()) {
                bestCandidates.clear();
                bestRow = row;
                bestCol = col;
                return true;
            }

            if ((int)candidates.size() < fewest) {
                fewest = candidates.size();
                bestRow = row;
                bestCol = col;
                bestCandidates = candidates;
                found = true;

                if (fewest == 1)
                    return true;
            }
        }
    }

    return found;
}

// Solve the Sudoku using MRV and backtracking
bool solveSudoku(Grid& grid) {
    int row = -1, col = -1;
    vector<int> candidates;

    if (!findBestCell(grid, row, col, candidates))
        return true;

    if (candidates.empty())
        return false;

    for (int num : candidates) {
        grid[row][col] = num;

        if (solveSudoku(grid))
            return true;

        grid[row][col] = 0;
    }

    return false;
}

// Count solutions, stopping after finding two
int countSolutions(Grid& grid) {
    int row = -1, col = -1;
    vector<int> candidates;

    if (!findBestCell(grid, row, col, candidates))
        return 1;

    if (candidates.empty())
        return 0;

    int count = 0;

    for (int num : candidates) {
        grid[row][col] = num;

        count += countSolutions(grid);

        grid[row][col] = 0;

        if (count >= 2)
            return 2;
    }

    return count;
}

// Remove cells while preserving a unique solution
void generatePuzzle(Grid& puzzle, int targetClues) {
    vector<vector<bool>> visited(9, vector<bool>(9, false));
    uniform_int_distribution<int> dist(0, 8);

    int clues = 81;
    int attempts = 0;

    while (clues > targetClues && attempts < 81) {
        int row = dist(rng);
        int col = dist(rng);

        if (visited[row][col])
            continue;

        visited[row][col] = true;
        attempts++;

        int original = puzzle[row][col];
        puzzle[row][col] = 0;

        Grid test = puzzle;

        if (countSolutions(test) == 1) {
            clues--;
        } else {
            puzzle[row][col] = original;
        }
    }

    cout << "\nClues remaining: " << clues << '\n';
}

int main() {
    Grid solution(9, vector<int>(9, 0));

    cout << "Generating complete Sudoku...\n";

    if (!generateSolvedGrid(solution)) {
        cout << "Failed to generate Sudoku.\n";
        return 1;
    }

    Grid puzzle = solution;

    generatePuzzle(puzzle, 30);

    cout << "\nGenerated Sudoku Puzzle:\n";
    printGrid(puzzle);

    Grid solvedPuzzle = puzzle;

    cout << "\nSolving Sudoku using MRV...\n";

    if (solveSudoku(solvedPuzzle)) {
        cout << "\nSolved Sudoku:\n";
        printGrid(solvedPuzzle);
    } else {
        cout << "No solution exists.\n";
    }

    return 0;
}
