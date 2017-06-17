/*************************************************************************//**
 * @file
 *
 * @mainpage Program 3 - Mazes
 *
 * @section course_section Course Information
 *
 * @author Hunter Comer
 *
 * @date April 13, 2017
 *
 * @par Professor:
 *         Roger Schrader
 *
 * @par Course:
 *         CSC 250 - Section 1 - 1:00 pm
 *
 * @par Location:
 *         McLaury - Room 313
 *
 * @section program_section Program Information
 *
 * @details This program inputs a rectangular maze along with its size and 
 *          the starting position of where the player will start at. The 
 *          maze is marked with the character '*' around the edges. There are
 *          also other obstacles such as lava, represented as 'L', and water,
 *          represented as 'W'. The object of the game is to get to the end of 
 *          the maze, represented by the character 'E', finding the shortest
 *          possible path. The character '-' marks where the player stepped as 
 *          well as the character '+'. However the plus character represents 
 *          when the player stepped in water. It is marked differently because 
 *          when the player walks in water, it is counted as two steps, rather
 *          than just one. 
 *
 * @section compile_section Compiling and Usage
 *
 * @par Compiling Instruction
 *	None
 *
 * @par Usage
   @verbatim
   c:\> prog3.exe maze.maz solutions.txt
        maze.maz - an input text file that contains mazes consisting of '*',
                   'L', 'W', and 'E' symbols as well as the size of the maze
                   and the starting position
        solutions.txt - an output text file that will be used to print the 
                        shortest possible path to solving the maze
   @endverbatim
 *
 * @section todo_bugs_modification_section Todo, Bugs, and Modifications
 *
 * @bug none
 *
 * @todo none
 *
 * @par Modifications and Development Timeline:
 * <a href=
 * "https://gitlab.mcs.sdsmt.edu/7343972/csc250s17programs/commits/master">
 * Please see Gitlab Repository</a>
 *
 *****************************************************************************/
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

/*******************************************************************************
*                         Function Prototypes
******************************************************************************/
void allocate2d(char **&ptr2d, int num_rows, int num_cols);
void free2d(char **&ptr2d, int num_rows);
void read_in_row_col(ifstream &fin, int &row_col);
void read_in_maze(ifstream &fin, int num_rows, int num_cols, char **&ptr2d);
void output_maze(ofstream &fout, int num_rows, int num_cols, char **&ptr2d,
    int min_count, int start_row, int start_col, int maze_count);
int check_files(ifstream &fin, ofstream &fout);
void solve_maze(char **&ptr2d, int count, int &min_count, int row, int col,
    char **&sol2d, int num_rows, int num_cols);


/**************************************************************************//** 
 * @author Hunter Comer
 * 
 * @par Description: 
 * This is the main function which calls the other functions in the program.
 * 
 * @param[in]      argc - count for number of command arguements
 * @param[in]      argv - array to hold the exact names of the command line 
 *                        arguements
 * 
 * @returns 0 program ran successful, -1 if file error.
 * 
 *****************************************************************************/

int main(int argc, char *argv[])
{
    ifstream fin;
    ofstream fout;
    int num_rows = 0, num_cols = 0;
    int start_row = 0, start_col = 0;
    char **ptr2d = nullptr;
    char **sol2d = nullptr;
    int count = 0, min_count, maze_count = 1;

    //check to see if all files needed are in the command line
    if (argc != 3)
    {
        cout << "Not enough files in command line. Must have input file with "
            << "a maze and an output file to show the solutions. Exiting" 
            << endl;
        return -1;
    }

    //open input and output files
    fin.open(argv[1]);
    fout.open(argv[2]);

    //check if files opened, exit if not
    if (check_files(fin, fout) == -1)
    {
        return -1;
    }


    while (fin >> num_rows)
    {
        //read in number of columns, number of rows, and the spot to start on 
        //the maze
        read_in_row_col(fin, num_cols);
        read_in_row_col(fin, start_row);
        read_in_row_col(fin, start_col);

        //ingore new line
        fin.ignore();

        //dynamicaly allocate storage for 2d arrays and check for success
        allocate2d(ptr2d, num_rows, num_cols);
        allocate2d(sol2d, num_rows, num_cols);

        //read in the maze layout and place it in the ptr2d array
        read_in_maze(fin, num_rows, num_cols, ptr2d);

        //set default count
        min_count = num_rows * num_cols;

        //call function to solve the maze
        solve_maze(ptr2d, count, min_count, start_row, start_col, sol2d, 
            num_rows, num_cols);

        //output shortest possible soloution
        output_maze(fout, num_rows, num_cols, sol2d, min_count, start_row, 
            start_col, maze_count);

        //add one to output count
        maze_count++;

        //free up all memory
        free2d(ptr2d, num_rows);
        free2d(sol2d, num_rows);
    }
    //close files
    fin.close();
    fout.close();

    return 0;
}

/**************************************************************************//** 
 * @author Hunter Comer
 * 
 * @par Description: 
 * This function checks if the input file and output file were opened 
 * correctly. If not, it returns -1 and will exit the program.
 * 
 * @param[in,out]  fin - input file varaible
 * @param[in,out]  fout - output file variable
 * 
 * @returns 0 program ran successful, -1 if otherwise.
 * 
 *****************************************************************************/
int check_files(ifstream &fin, ofstream &fout)
{
    //check if input file opened
    if (!fin)
    {
        cout << "Problem opening input file. Exiting." << endl;
        return -1;
    }

    //check if output file opened
    if (!fout)
    {
        cout << "Problem opening output file. Exiting." << endl;
        return -1;
    }
    return 0;
}

/**************************************************************************//** 
 * @author Hunter Comer
 * 
 * @par Description: 
 * This function allocates a two-dimensional pointer array. It will check if
 * the memory was allocated correctly or not.
 * 
 * @param[in,out]  ptr2d - two-dimentional pointer array
 * @param[in]      num_rows - the total number of rows in the maze
 * @param[in]      num_cols - the total number of columns in the maze
 *
 * @returns nothing, void function
 * 
 *****************************************************************************/
void allocate2d(char **&ptr2d, int num_rows, int num_cols)
{
    int i, j;

    //dynamically allocate a 1d array of pointers
    ptr2d = new (nothrow) char * [num_rows];

    //check for success
    if (ptr2d == nullptr)
    {
        cout << "Problem allocating memory." << endl;
        return;
    }

    //dynamically allocate every single pointer in each row
    for (i = 0; i < num_rows; i++)
    {
        ptr2d[i] = new (nothrow) char [num_cols];

        //check for success
        if (ptr2d[i] == nullptr)
        {
            //if didn't work, delete space 
            for (j = 0; j < i; j++)
            {
                delete[] ptr2d[j];
            }

            delete[] ptr2d;

            cout << "Problem allocating memory. Exiting." << endl;
            return;
        }
    }
}

/**************************************************************************//** 
 * @author Hunter Comer
 * 
 * @par Description: 
 * this function will free up memory from a two-dimensional pointer array.
 * 
 * @param[in,out]  ptr2d - two-dimentional pointer array
 * @param[in]      num_rows - the total number of rows in the maze
 * 
 * @returns nothing, void function
 * 
 *****************************************************************************/
void free2d(char **&ptr2d, int num_rows)
{
    int i;

    if (ptr2d == nullptr)
    {
        return;
    }

    //deletes every pointer in each row
    for (i = 0; i < num_rows; i++)
    {
        delete[] ptr2d[i];
    }

    //deletes 1d array
    delete[] ptr2d;
}

/**************************************************************************//** 
 * @author Hunter Comer
 * 
 * @par Description: 
 * This function will read in the needed number from the input file. The 
 * exact variable the number will be stored in is called from main. 
 * 
 * @param[in,out]  fin - input file variable
 * @param[in,out]  row_col - the desired variable to be storing into
 * 
 * @returns nothing, void function
 * 
 *****************************************************************************/
void read_in_row_col(ifstream &fin, int &row_col)
{
    int num = 0;

    //read in number
    fin >> num;

    //set equal to correct variable called into function
    row_col = num;
}

/**************************************************************************//** 
 * @author Hunter Comer
 * 
 * @par Description: 
 * This function reads in a single maze and puts each character in an element
 * of a dynamically allocated 2d array.
 * 
 * @param[in,out]  fin - input file variable
 * @param[in]      num_rows - total number of rows in the maze
 * @param[in]      num_cols - total number of columns in the maze
 * @param[in,out]  ptr2d - 2d pointer array to store maze into
 * 
 * @returns nothing, void function
 * 
 *****************************************************************************/
void read_in_maze(ifstream &fin, int num_rows, int num_cols, char **&ptr2d)
{
    int i, j;

    //walk through and add maze to 2d array from input file
    for (i = 0; i < num_rows; i++)
    {
        for (j = 0; j < num_cols; j++)
        {
            //inserts into the 2d array
            ptr2d[i][j] = fin.get();
        }

        //ignore the new line character
        fin.ignore();
    }
}

/**************************************************************************//** 
 * @author Hunter Comer
 * 
 * @par Description: 
 * This function is the output function. It will output the maze number, the
 * size of the maze, the starting location, the number of steps in the shortest
 * path, and the maze solution. 
 * 
 * @param[in,out]  fout - output file variable
 * @param[in]      num_rows - total number of rows in the maze
 * @param[in]      num_cols - total number of columns in the maze
 * @param[in,out]  ptr2d - dynamically 2d pointer array that stors the maze
 * @param[in]      min_count - the minimum number of steps to solve the maze
 * @param[in]      start_row - the row the starting position is on
 * @param[in]      start_col - the column the starting position is on
 * @param[in]      maze_count - the maze number
 * 
 * @returns nothing, void function
 * 
 *****************************************************************************/
void output_maze(ofstream &fout, int num_rows, int num_cols, char **&ptr2d,
    int min_count, int start_row, int start_col, int maze_count)
{
    int i, j; 

    //output maze information
    fout << "Maze " << maze_count << ": " << endl;

    fout << "\tSize: " << num_rows << "x" << num_cols << endl;
    fout << "\tStart loc: " << start_row << " " << start_col << endl;
    fout << "\tShortest Path: " << min_count << " steps" << endl;

    fout << "\t";

    //output maze from 2d array
    for (i = 0; i < num_rows; i++)
    {
        for (j = 0; j < num_cols; j++)
        {
            fout << ptr2d[i][j];
            
            //end line after every row
            if (j == num_cols - 1)
            {
                fout << endl;
                fout << "\t";
            }
        }
    }

    fout << "\n";
}

/**************************************************************************//** 
 * @author Hunter Comer
 * 
 * @par Description: 
 * A detailed description of the function.  It can
 * extend to multiple lines
 * 
 * @param[in]      num_rows - total number of rows in the maze
 * @param[in]      num_cols - total number of columns in the maze
 * @param[in,out]  ptr2d - dynamically 2d pointer array that stors the maze
 * @param[in,out]  min_count - the minimum number of steps to solve the maze
 * @param[in]      row - current row location
 * @param[in]      col - current column location
 * @param[in]      count - the counting variable for how many steps have been
 *                         taken
 * @param[in,out]  sol2d - dynamically allocated 2d pointer array to store the
 *                         the shortest solution
 *
 * @returns nothing, void function
 * 
 *****************************************************************************/
void solve_maze(char **&ptr2d, int count, int &min_count, int row, int col,
    char **&sol2d, int num_rows, int num_cols)
{
    int i, j;

    //if space already occupied, return
    if (ptr2d[row][col] == '*' || ptr2d[row][col] == 'L' || 
        ptr2d[row][col] == '-' || ptr2d[row][col] == '+')
        return;

    //found the end of maze
    if (ptr2d[row][col] == 'E')
    {
        //check for shortest route
        if (count < min_count)
        {
            //set comparision count to new count found
            min_count = count;

            //if found, copy to solution
            for (i = 0; i < num_rows; i++)
            {
                for (j = 0; j < num_cols; j++)
                {
                    sol2d[i][j] = ptr2d[i][j];
                }
            }
        }
        return;
    }
    //check if space is water
    if (ptr2d[row][col] == 'W')
    {
        //make space +
        ptr2d[row][col] = '+';
        //add two to count
        count += 2;
    }
    //if space isnt water or end, add -
    else
    {
        //make space -
        ptr2d[row][col] = '-';
        //add one to count
        count++;
    }

    //move right
    solve_maze(ptr2d, count, min_count, row, col + 1, sol2d, num_rows,
        num_cols);
    //move left
    solve_maze(ptr2d, count, min_count, row, col - 1, sol2d, num_rows,
            num_cols);
    //move down
    solve_maze(ptr2d, count, min_count, row + 1, col, sol2d, num_rows,
            num_cols);
    //move up
    solve_maze(ptr2d, count, min_count, row - 1, col, sol2d, num_rows,
            num_cols);
    
    //if not found but is water
    if (ptr2d[row][col] == '+')
    {
        ptr2d[row][col] = 'W';

        //subtract two from count
        count -= 2;
    }
    else
    {
        //if not found back up and clear space
        ptr2d[row][col] = ' ';
        //subtract from count since backing up
        count--;
    }
}