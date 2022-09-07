#include <iostream>
#include <vector>
#include <math.h>
#include <queue>
#include <algorithm>
#include <time.h>
#include <numeric>
using namespace std;

/**************************************************************
 * The Sudoku class is used to reference many member variables
 * without having to pass them into every function. It can
 * create a puzzle or solve a puzzle.
 **************************************************************/
class Sudoku{
  public:
  //public functions
  /**************************************************************
   * Class Constructor: Sets member variables
   **************************************************************/
  Sudoku(int size){
    boardsize = size*size;
    blocksize = size;
    numberOfCellsRemoved = 0;
    totalNumberOfCells = boardsize*boardsize;
    if (size < 4) length = 2 * blocksize * (blocksize + 1) - 3;
    else          length = 53;
    board.resize(boardsize, vector<int>(boardsize, 0));
  }

/**************************************************************
 * Simply prints the board in the termial window. It can draw
 * 4x4, 9x9, and 16x16.
 **************************************************************/
  void drawBoard(){
    drawBoard(board);
    cout << endl;
  }

  /**************************************************************
   * Public function that calls the private function with designated
   * input perameters. Starts at cell (0,0) beccause we don't where
   * or even if there is an empty cell yet.
   **************************************************************/
  void solve(){
    solveSudoku(board, 0, 0);
  }

  /**************************************************************
   * The purpose of this function is to create a unique puzzle 
   * that has exactly one solution.
   * Gridpos - struct - This function contains a struct that holds
   * a row and a col which represents the position of a cell (not 
   * the value within the cell)
   **************************************************************/
  void createUnique(){
    struct Gridpos{
      int row;
      int col;
      Gridpos(int col, int row){
        this->col = col;
        this->row = row;
      }
    };

    // first solve the current board
    solveSudoku(board, 0, 0);
    numberOfCellsRemoved = 0;

    //generate a list of grid positions to check
    vector<Gridpos> cells;
    for(int row = 0; row < boardsize; row++){
      for(int col = 0; col < boardsize; col++){
        cells.push_back(Gridpos(col,row));
      }
    }

    //shuffle the list
    random_shuffle(cells.begin(), cells.end());

    //test each position in the board randomly (cells list)
    while (cells.size() > 0){
      //save val in case we need to put it back
      int temp = board[cells.back().row][cells.back().col];

      //set value to 0
      board[cells.back().row][cells.back().col] = 0;
      
      //run solver and count solutions on a copy of the board
      vector<vector<int>> boardcpy = board;
      int numberOfSolutions = 0;
      solveSudokuSolutionCheck(boardcpy, cells.back().row, cells.back().col, numberOfSolutions);
      
      //if we found more than one solution then we need to put it back (we can't remove it)
      if (numberOfSolutions != 1){
        board[cells.back().row][cells.back().col] = temp;
      }else{
        numberOfCellsRemoved++;
      }

      cells.pop_back();
      drawBoard(board);
      printf("\nSolutions: %i \t %i cells removed out of %i\n", numSolutions, numberOfCellsRemoved, totalNumberOfCells);

      cout << "\n=====================\n" << endl;
    }
  }

  /**************************************************************
   * A get function that returns the current board state
   * return - vector<vector<int>> - current board state
   **************************************************************/
  vector<vector<int>> getBoard(){
    return board;
  }

  /**************************************************************
   * A fucnction that will take a board state and apply it 
   * to the current object if it is formated properly.
   * brd - vector<vector<int>>& - exteral board state
   * return - bool - returns if the board is a valid board state
   **************************************************************/
  bool assignBoard(vector<vector<int>>& brd){
    if(brd.size() == brd[0].size() && brd.size() % brd.size() == 0) {
      board = brd;
      boardsize = brd.size();
      blocksize = sqrt(brd.size());
      totalNumberOfCells = boardsize*boardsize;
      
      for(int i = 0; i < boardsize; i++){
        for(int j = 0; j < boardsize; j++){
          if (brd[i][j] == 0) numberOfCellsRemoved++;
        }
      }

      if (boardsize < 10) length = 2 * blocksize * (blocksize + 1) - 3;
      else                length = 53;

      return true;
    }

    return false;
  }

  private:
  //private variables
  int numSolutions = 0;
  int boardsize;
  int blocksize;
  int length;
  int totalNumberOfCells;
  int numberOfCellsRemoved = 0;
  vector<vector<int>> board;

  //private functions
  /**************************************************************
   * Simply prints the board in the termial window. It can draw
   * 4x4, 9x9, and 16x16.
   **************************************************************/
  void drawBoard(vector<vector<int>>& brd){
    //if the numbers only contain one digit
    if(boardsize < 10){
      for (int i = 0; i < boardsize; i++)  {
        if (i % blocksize == 0 && i > 0){
          for (int k = 0; k < length; k++){ cout << "-"; }
          cout << endl;
        }
        for (int j = 0; j < boardsize; j++){
          if (j % blocksize == 0 && j > 0)  { cout << "| "; }
          if (brd[i][j] == 0)               { cout << "  "; }
          else                              { cout << brd[i][j] << " "; }
        }
        cout << endl;
      }

    //if the numbers contain 2 digits
    }else{
      for (int i = 0; i < boardsize; i++)  {
        if (i % blocksize == 0 && i > 0){
          for (int k = 0; k < length; k++){ cout << "-"; }
          cout << endl;
        }
        for (int j = 0; j < boardsize; j++){
          if (j % blocksize == 0 && j > 0) cout << "| ";
          if (brd[i][j] == 0)     { cout << "   "; }
          else if(brd[i][j] > 9)  { cout << brd[i][j] << " "; }
          else                    { cout << brd[i][j] << "  "; }
        }
        cout << endl;
      }
    }
  }

  /**************************************************************
   * Checks to see if we can place the 'val' at the current
   * location (row, col). It will check the row, column, and
   * block to see if it can be placed.
   * brd - vector<vector<int>>& - board state
   * row - int - row of the current cell
   * col - int - column of the current cell
   * val - int - the value that we are trying to place
   * return - bool - whether or not the value is valid
   **************************************************************/
  bool isValidValue(vector<vector<int>>& brd, int row, int col, int val){
    //if the cell is already filled then we cant place anything
    if (brd[row][col] != 0) return false;

    //Check corresponding row, col, and block for tartget value
    int gridx = (col / blocksize) * blocksize;
    int gridy = (row / blocksize) * blocksize;
    for (int i = 0; i < boardsize; i++){
      //check if the these cells already have the target value
      if (brd[row][i] == val || brd[i][col] == val) return false;

      //check of the block already has the target value (found this trick online)
      if (brd[gridy + i / blocksize][gridx + i % blocksize] == val) return false;
    }
    //return true is the target value is valid
    return true;
  }

  /**************************************************************
   * A function that tries to find an empty cell in the board.
   * If there isn't one, then the board is solved
   * brd - vector<vector<int>>& - board state
   * nextRow - int& - row of the empty cell
   * nextCol - int& - column of the empty cell
   * return - bool - whether or not we found an empty cell
   **************************************************************/
  bool findEmptyCell(vector<vector<int>>& brd, int& nextRow, int& nextCol){
    //search from top to bottom and left to right
    for (int i = 0; i < boardsize; i++){
      for (int j = 0; j < boardsize; j++){
        if (brd[i][j] == 0){
          nextRow = i;
          nextCol = j;
          return true;
        }
      }
    }
    return false;
  }

  /**************************************************************
   * A function that creates a list of every possible value at an 
   * empty cell.
   * brd - vector<vector<int>>& - board state
   * row - int - row of the current cell
   * col - int - column of the current cell
   * return - vector<vector<int>>& - list of possible values
   **************************************************************/
  vector<int> findPossibleValues(vector<vector<int>>& brd, int row, int col)
  {
    vector<int> possibleValues = {};
    for (int i = 1; i <= boardsize; i++)
      if (isValidValue(brd, row, col, i))
        possibleValues.push_back(i);
    random_shuffle(possibleValues.begin(), possibleValues.end());
    return possibleValues;
  }

  /**************************************************************
   * A recursive function that that will solve any board that
   * has a solution. If the board has no solution then the board
   * state should not change.
   * brd - vector<vector<int>>& - board state
   * row - int - row of the current cell
   * col - int - column of the current cell
   * return - bool - whether or not the board state is solvable
   **************************************************************/
  bool solveSudoku(vector<vector<int>>& brd, int row, int col)
  {
    //if the current row and column already has a value
    if (brd[row][col] != 0){
      int nextRow, nextCol;
      if(findEmptyCell(brd, nextRow, nextCol))
        return solveSudoku(brd, nextRow, nextCol);
      else
        return true;
    }

    //check if the number of possible cells are placable
    vector<int> p_val = findPossibleValues(brd, row, col);
    if (p_val.size() == 0) return false;

    for (int i = 0; i < p_val.size(); i++)
    {
      vector<vector<int>> boardcp = brd;

      boardcp[row][col] = p_val[i];
      int nextRow = row;
      int nextCol = col;
      findEmptyCell(boardcp, nextRow, nextCol);
      if (solveSudoku(boardcp, nextRow, nextCol))
      {
        brd = boardcp;
        return true;
      }
    }
    return false;
  }

  /**************************************************************
   * A modified version of solveSudoku where we need to track the
   * number of solutions. If there is more than one solution then
   * we return false, as if we never found a solution in the first
   * place.
   * brd - vector<vector<int>>& - board state
   * row - int - row of the current cell
   * col - int - column of the current cell
   * numSol - int& - the number of solutions currently found
   * return - bool - whether or not there is exacty one solution
   **************************************************************/
  bool solveSudokuSolutionCheck(vector<vector<int>>& brd, int row, int col, int& numSol)
  {
    //if the current row and column already has a value
    //then look for the next empty spot
    if (brd[row][col] != 0){
      int nextRow, nextCol;
      if(findEmptyCell(board, nextRow, nextCol)){
        return solveSudokuSolutionCheck(brd, nextRow, nextCol, numSol);
      }else{
        numSol++;
        //return true;
      }
    }

    //check if the number of possible cells are placable
    vector<int> p_val = findPossibleValues(brd, row, col);
    if (p_val.size() == 0) return false;

    //This is where we check for every possible solution for the values in placable
    bool solutionFound = false;
    for (int i = 0; i < p_val.size(); i++)
    {
      //Makes it so that we don't need to check a solution for every falue
      if (numSol > 1) return true;

      //copy the board since we done want to modify the original
      vector<vector<int>> boardcpy = brd;

      //assign the possible value to the cell and attempt to solve it.
      boardcpy[row][col] = p_val[i];
      int rowNext = row;
      int colNext = col;
      if (findEmptyCell(boardcpy, rowNext, colNext)){
        //if solvable then copy the new board to the original
        if (solveSudokuSolutionCheck(boardcpy, rowNext, colNext, numSol)){
          brd = boardcpy;
          solutionFound = true;
          numSol++;
          //numSolutions += 10;
        }
      }else{
        numSol++;
        numSolutions++;
      }
    }
    return solutionFound;
  }
};

struct TestCases{
  vector<vector<int>> uniqueBoard{      //has only one solution
    {3,0,0,0,0,5,0,0,0},
    {0,9,1,0,0,0,3,4,0},
    {0,0,0,3,0,1,9,0,6},
    {0,0,0,0,0,2,0,0,0},
    {0,8,0,0,0,0,0,0,0},
    {0,4,2,7,6,0,1,0,9},
    {9,0,0,6,0,0,0,0,4},
    {8,0,0,0,0,0,0,0,0},
    {0,0,4,1,0,0,8,3,0}};

  vector<vector<int>> filledBoard{      //should just return same board
    {3,2,6,4,9,5,7,1,8},
    {7,9,1,8,2,6,3,4,5},
    {4,5,8,3,7,1,9,2,6},
    {1,7,9,5,8,2,4,6,3},
    {6,8,3,9,1,4,5,7,2},
    {5,4,2,7,6,3,1,8,9},
    {9,1,7,6,3,8,2,5,4},
    {8,3,5,2,4,7,6,9,1},
    {2,6,4,1,5,9,8,3,7}};

  vector<vector<int>> emptyBoard{       //many solutions
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0}};

  vector<vector<int>> largeUniqueBoard{ //unique solution
    { 0, 0,12, 6,  8, 7, 0,11, 16, 0, 0,10,  5, 0, 9, 0},
    { 2, 8, 0, 3,  0,13, 1, 0, 11, 0, 0, 0,  0, 6, 0, 0},
    {15, 9, 0, 0,  0, 3, 2, 0,  0, 0, 0, 0, 14, 0,10, 4},
    {10, 0, 0, 0,  0, 9, 6, 0,  0, 4, 0, 0,  0,13, 0, 1},

    { 0, 0, 9, 0,  0,15, 0, 3,  5, 0, 8, 0,  7,12, 0,13},
    { 0, 0, 1, 0,  0,12, 8, 0,  0, 0,14, 0,  0, 0, 4, 0},
    { 0, 0,11, 0, 10, 5, 0, 0,  1, 7, 0, 6,  0,15, 0, 0},
    { 0,14, 0,10,  0, 0, 0, 6, 15, 2, 0, 0,  0, 1,11,16},

    { 0, 0, 0, 0, 11, 1,10, 8,  0, 6,15, 0,  0, 0,12, 0},
    { 0, 0,14, 7,  0, 0,15,12,  0, 0, 0, 1,  0, 8, 0, 5},
    { 1, 0, 6, 5,  0, 0, 3,13,  0,12, 0, 0,  0, 0,15, 0},
    { 0, 0,15, 0,  6, 0, 0, 0,  9, 0, 0, 3, 13, 7, 0, 2},

    { 0,16, 0,14,  0, 0, 0, 0,  6, 0, 0,11,  0, 0,13, 3},
    { 8, 0, 0, 0, 15, 0, 9, 7, 14, 0, 0, 0,  0,16, 0, 0},
    { 6, 5, 0, 4,  3, 0,14,16,  0, 9,13, 0,  1, 0, 0, 7},
    {12, 0, 7, 0,  4, 0, 0, 1,  0,16, 0, 0,  2,14, 0, 9}};

  vector<vector<int>> largeFilledBoard{ //should return the smae board
    {14,13,12, 6,  8, 7, 4,11, 16, 3, 1,10,  5, 2, 9,15},
    { 2, 8, 4, 3, 14,13, 1,10, 11,15, 5, 9, 16, 6, 7,12},
    {15, 9,16, 1, 12, 3, 2, 5, 13, 8, 6, 7, 14,11,10, 4},
    {10, 7, 5,11, 16, 9, 6,15,  2, 4,12,14,  8,13, 3, 1},

    { 4, 6, 9, 2,  1,15,11, 3,  5,10, 8,16,  7,12,14,13},
    { 7,15, 1,16,  2,12, 8, 9,  3,11,14,13, 10, 5, 4, 6},
    {13, 3,11,12, 10, 5,16,14,  1, 7, 4, 6,  9,15, 2, 8},
    { 5,14, 8,10, 13, 4, 7, 6, 15, 2, 9,12,  3, 1,11,16},

    {16, 2,13, 9, 11, 1,10, 8,  7, 6,15, 5,  4, 3,12,14},
    { 3,10,14, 7,  9, 2,15,12,  4,13,11, 1,  6, 8,16, 5},
    { 1, 4, 6, 5,  7,14, 3,13,  8,12,16, 2, 11, 9,15,10},
    {11,12,15, 8,  6,16, 5, 4,  9,14,10, 3, 13, 7, 1, 2},

    { 9,16,10,14,  5, 8,12, 2,  6, 1, 7,11, 15, 4,13, 3},
    { 8, 1, 3,13, 15,10, 9, 7, 14, 5, 2, 4, 12,16, 6,11},
    { 6, 5, 2, 4,  3,11,14,16, 12, 9,13,15,  1,10, 8, 7},
    {12,11, 7,15,  4, 6,13, 1, 10,16, 3, 8,  2,14, 5, 9}};

  vector<vector<int>> largeEmptyBoard{  //many solutions
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},

    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},

    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},

    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
    { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0}};
};

//============ MAIN =============
int main()
{

  /******************** TEST CASES ******************************
   * Running the test cases are fairly simple. Once you run the 
   * program, you will promted to make selections. One of those
   * selections are the test cases. When you select one, it will 
   * display it so that you can see the current board state, then
   * just type 1 to solve it. It will then print the result and
   * go back to the main menu. There are no manual inputs to 
   * create a board state ourside of the program.
   **************************************************************/

  srand(time(NULL));
  int input = 1;
  Sudoku* s = NULL;
  TestCases t;

  while(input){
    cout << "What would you like to do?" << endl;
    cout << "[1] Create (new) empty Sudoku" << endl;
    cout << "[2] Create (new) unique Sudoku" << endl;
    cout << "[3] Run a test case" << endl;
    if (s){
      cout << "[4] Solve (current) Sudoku" << endl;
      cout << "[5] Create unique board from (current) Sudoku" << endl;
      cout << "[6] Remove (current) Sudoku" << endl;
    }

    //get input
    cout << "\n\tSelection: ";
    cin >> input;
    cout << endl;

    int size;
    switch (input){
      case (1): 
        cout << "\tBlock size: ";
        cin >> size;
        s = new Sudoku(size);
        s->drawBoard();
        break;

      case(2):
        cout << "\tBlock size: ";
        cin >> size;
        s = new Sudoku(size);
        s->createUnique();
        break;

      case (3):
        cout << "Pick a test case! (solver only)" << endl;
        cout << "[1] 9x9 empty board" << endl;
        cout << "[2] 9x9 filled board" << endl;
        cout << "[3] 9x9 unique board" << endl;
        cout << "[4] 16x16 empty board" << endl;
        cout << "[5] 16x16 filled board" << endl;
        cout << "[6] 16x16 unique board" << endl;
        cout << "\n\tSelection: ";
        cin >> input;

        switch (input){
          case (1):
            s = new Sudoku(3);
            s->assignBoard(t.emptyBoard);
            break;
          case (2):
            s = new Sudoku(3);
            s->assignBoard(t.filledBoard);
            break;
          case (3):
            s = new Sudoku(3);
            s->assignBoard(t.uniqueBoard);
            break;
          case (4):
            s = new Sudoku(4);
            s->assignBoard(t.largeEmptyBoard);
            break;
          case (5):
            s = new Sudoku(4);
            s->assignBoard(t.largeFilledBoard);
            break;
          case (6):
            s = new Sudoku(4);
            s->assignBoard(t.largeUniqueBoard);
            break;
          }
          s->drawBoard();
          cout <<"[1] To solve the board: ";
          cin >> input;
          cout << endl;
          s->solve();
          s->drawBoard();
        break;

      case (4):
        s->solve();
        s->drawBoard();
        break;

      case (5):
        s = NULL;
        break;

      default:
        cout << "--Please enter a valid number--" << endl;
    }
  }
  return 0;
}
