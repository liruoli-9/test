#include<iostream>
#include<vector>
#include<unordered_map>
using namespace std;
class Solution {
public:
    bool isValidSudoku(vector<vector<char>>& board) {
    unordered_map<int,int> harshtable;
        for(int i=0;i<9;i++)
        {
         for(int j=0;j<9;j++)harshtable[board[i][j]]++;
         harshtable['.']=0;   
         for(int k='1';k<='9';k++)
         {
             if(harshtable[k]>1)return false;
             harshtable[k]=0;
         }   
        }

         for(int i=0;i<9;i++)
        {
         for(int j=0;j<9;j++)harshtable[board[j][i]]++;      
         harshtable['.']=0;
         for(int k='1';k<='9';k++)
         {
             if(harshtable[k]>1)return false;
             harshtable[k]=0;
         }   
        }


        for(int m=0;m<3;m++)
            for(int n=0;n<3;n++)
            {
                for(int i=3*m;i<3*m+3;i++)
                    for(int j=3*n;j<3*n+3;j++)harshtable[board[i][j]]++;   
                harshtable['.']=0;
                for(int k='1';k<='9';k++)
                {
                    if(harshtable[k]>1)return false;
                    harshtable[k]=0;
                }
            }
         return true;   
    }
};

int main()
{
     vector<vector<char>> board={{'8','3','.','.','7','.','.','.','.'}
                                ,{'6','4','.','1','9','5','.','.','.'}
                                ,{'.','2','1','.','.','.','.','6','.'}
                                ,{'1','.','.','.','6','.','.','.','3'}
                                ,{'4','.','.','8','.','3','.','.','1'}
                                ,{'7','.','.','.','2','.','.','.','6'}
                                ,{'.','6','.','.','.','.','2','8','.'}
                                ,{'.','.','.','4','1','9','.','.','5'}
                                ,{'.','.','.','.','8','.','.','7','9'}};
    //cout<<board[0][1];
    Solution s;
    cout<<s.isValidSudoku(board);
    return 0;
}