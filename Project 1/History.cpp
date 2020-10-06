#include "History.h"
#include "globals.h"

#include <iostream>

using namespace std;

History::History(int nRows, int nCols)
{
    m_rows = nRows;
    m_cols = nCols;
    
    int i, j;
    for (i = 0; i < nRows; i++){
        for (j = 0; j < nCols; j++){
            m_status[i][j] = '.';} //Initialize to all dot position
    }
}

bool History::record(int r, int c)
{
    if (r > m_rows || c > m_cols || r < 1 || c < 1)
        return false;
    
    switch(m_status[r-1][c-1]){
            case '.':  m_status[r-1][c-1] = 'A'; break; // first appear
            case 'Z':  break; // continue display 'Z' if larger than 26
            default:   m_status[r-1][c-1]++; break; // add one palce
    }
    
    return true;
}

void History::display() const
{
    clearScreen();
    int r, c;
    for (r = 1; r <= m_rows; r++) // loop through each row and column to display the grid
    {
        for (c = 1; c <= m_cols; c++)
            cout << m_status[r-1][c-1];
        cout << endl;
    }
    
    cout << endl;
}

