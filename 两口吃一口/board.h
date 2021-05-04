#pragma once
#include <set>
#include <vector>
enum {
	WHITE = 1,
	BLACK = 2,
};
void InitBoard(int player);
void GenMoveWays(int player, std::vector<std::tuple<int, int, int, int>>& ways);
void AddPiece(int row, int col, int player);
void DeletePiece(int row, int col, int player);
void EatPiece(int row, int col, int currPlayer, std::vector<std::pair<int, int>> &eats);
int GetPieceNum(int player);
bool IsVaildPiece(int fromRow, int fromCol, int toRow, int toCol, int player);
void DrawBoard();