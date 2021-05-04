#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <cmath>
#include "board.h"

static std::map<std::pair<int, int>, int> g_board;
static int g_rowNum = 5;
static int g_colNum = 5;

void InitBoard(int player)
{
	g_board[std::make_pair(0, 0)] = player;
	g_board[std::make_pair(0, 1)] = player;
	g_board[std::make_pair(0, 2)] = player;
	g_board[std::make_pair(0, 3)] = player;
	g_board[std::make_pair(0, 4)] = player;
	g_board[std::make_pair(4, 0)] = (BLACK + WHITE) - player;
	g_board[std::make_pair(4, 1)] = (BLACK + WHITE) - player;
	g_board[std::make_pair(4, 2)] = (BLACK + WHITE) - player;
	g_board[std::make_pair(4, 3)] = (BLACK + WHITE) - player;
	g_board[std::make_pair(4, 4)] = (BLACK + WHITE) - player;
}

int CheckPostion(int row, int col)
{
	if (row >= g_colNum || col >= g_colNum || row < 0 || col < 0) {
		return -1;
	}
	return 0;
}

bool IsVaildPiece(int fromRow, int fromCol, int toRow, int toCol, int player)
{
	if (CheckPostion(fromRow, fromCol) || CheckPostion(toRow, toCol)) {
		return false;
	}

	auto it = g_board.find(std::make_pair(fromRow, fromCol));
	if (it == g_board.end() || it->second != player) {
		return false;
	}

	it = g_board.find(std::make_pair(toRow, toCol));
	if (it != g_board.end()) {
		return false;
	}

	// 前后只能移动一个格子
	if (abs(fromCol - toCol) == 0 && abs(fromRow - toRow) == 1) {
		return true;
	}

	if (abs(fromCol - toCol) == 1 && abs(fromRow - toRow) == 0) {
		return true;
	}

	return false;
}

int GetPieceNum(int player)
{
	int pieceNum = 0;

	for (auto &i : g_board) {
		if (i.second == player) {
			pieceNum++;
		}
	}
	return pieceNum;
}

void AddPiece(int row, int col, int player)
{
	if (CheckPostion(row, col)) {
		std::cout << "此位置不在棋盘上："<< row << ", " << col << std::endl;
		assert(0);
		return;
	}

	auto it = g_board.find(std::make_pair(row, col));
	if (it != g_board.end()) {
		std::cout << "此位置已有其他棋子：" << row << ", " << col << std::endl;
		assert(0);
		return;
	}

	g_board[std::make_pair(row, col)] = player;
}

void DeletePiece(int row, int col, int player)
{
	if (CheckPostion(row, col)) {
		std::cout << "此位置不在棋盘上：" << row << ", " << col << std::endl;
		assert(0);
		return;
	}

	auto it = g_board.find(std::make_pair(row, col));
	if (it == g_board.end()) {
		std::cout << "此位置没有棋子：" << row << ", " << col << std::endl;
		assert(0);
		return;
	}

	if (it->second != player) {
		std::cout << "此位置的棋子不是当前玩家的棋子：" << row << ", " << col << std::endl;
		assert(0);
		return;
	}

	g_board.erase(it);
}

void GenMoveWays(int player, std::vector<std::tuple<int, int, int, int>>& ways)
{
	int direct[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

	for (auto &i : g_board) {
		if (i.second != player) {
			continue;
		}
		for (int k = 0; k < 4; k++) {
			int nextRow = i.first.first + direct[k][0];
			int nextCol = i.first.second + direct[k][1];

			if (CheckPostion(nextRow, nextCol)) {
				continue;
			}
			auto nextIt = g_board.find(std::make_pair(nextRow, nextCol));
			if (nextIt != g_board.end()) {
				continue;
			}
			ways.push_back(std::make_tuple(i.first.first, i.first.second, nextRow, nextCol));
		}
	}
}
// 一行或者一列两个己方棋子可以吃对方一个棋子
// _○○●_：OK
// ○○●__：OK
// __○○●：OK
// ○○●_●：这种情况不能吃子
// ○○○●_：这种情况不能吃子
// _○○○●：这种情况不能吃子
// _●○○●：这种情况不能吃子
// ●○○●_：这种情况不能吃子
int GetPieceNumByDirect(int row, int col, int direct[2])
{
	int pieceNum = 0;

	if (direct[0] != 0 && direct[1] != 0) {
		std::cout << "错误的方向" << std::endl;
		assert(0);
		return 0;
	}

	if (direct[0] != 0) {
		for (int i = 0; i < g_rowNum; i++) {
			auto it = g_board.find(std::make_pair(i, col));
			if (it != g_board.end()) {
				pieceNum++;
			}
		}
		return pieceNum;
	}

	for (int i = 0; i < g_colNum; i++) {
		auto it = g_board.find(std::make_pair(row, i));
		if (it != g_board.end()) {
			pieceNum++;
		}
	}
	return pieceNum;
}

void EatPiece(int row, int col, int currPlayer, std::vector<std::pair<int, int>> &eats)
{
	int direct[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
	int nextPlayer = (WHITE + BLACK) - currPlayer;

	for (int i = 0; i < 4; i++) {
		int secondPieceRow = row + direct[i][0];
		int secondPieceCol = col + direct[i][1];

		// 左/下/右/上四个方向搜索连续两个己方棋子
		auto secondPieceIt = g_board.find(std::make_pair(secondPieceRow, secondPieceCol));
		if (secondPieceIt == g_board.end() || secondPieceIt->second != currPlayer) {
			continue;
		}

		// 一行或者一列只能有3个棋子
		if (GetPieceNumByDirect(row, col, direct[i]) != 3) {
			continue;
		}

		// 分别搜索两边
		int thirdPieceRow = secondPieceRow + direct[i][0];
		int thirdPieceCol = secondPieceCol + direct[i][1];
		auto thirdPieceIt = g_board.find(std::make_pair(thirdPieceRow, thirdPieceCol));
		if (thirdPieceIt != g_board.end() && thirdPieceIt->second == nextPlayer) {
			DeletePiece(thirdPieceRow, thirdPieceCol, nextPlayer);
			eats.push_back(std::make_pair(thirdPieceRow, thirdPieceCol));
			continue;
		}

		thirdPieceRow = row - direct[i][0];
		thirdPieceCol = col - direct[i][1];
		thirdPieceIt = g_board.find(std::make_pair(thirdPieceRow, thirdPieceCol));
		if (thirdPieceIt != g_board.end() && thirdPieceIt->second == nextPlayer) {
			DeletePiece(thirdPieceRow, thirdPieceCol, nextPlayer);
			eats.push_back(std::make_pair(thirdPieceRow, thirdPieceCol));
		}
	}
}

std::string GenBoard(int row, int col, int colNum, int rowNum, std::map<std::pair<int, int>, int> board)
{
	std::string str;
	auto it = board.find(std::make_pair(row, col));

	if (it != board.end()) {
		if (it->second == WHITE) {
			return "○";
		}
		if (it->second == BLACK) {
			return "●";
		}
	}

	if (row == 0 && col == 0) {
		str = "┌";
	}
	else if (row == 0 && col == colNum - 1) {
		str = "┐";
	}
	else if (row == rowNum - 1 && col == 0) {
		str = "└";
	}
	else if (row == rowNum - 1 && col == colNum - 1) {
		str = "┘";
	}
	else if (row == 0) {
		str = "┬";
	}
	else if (col == colNum - 1) {
		str = "┤";
	}
	else if (col == 0) {
		str = "├";
	}
	else if (row == rowNum - 1) {
		str = "┴";
	}
	else {
		str = "┼";
	}
	
	return str;
}

void GenBorderLine(std::string& topBorderLine, std::string& bottomBorderLine, int colNum)
{
	topBorderLine += " ┌";
	bottomBorderLine += " └";
	for (int i = 0; i < 2 * colNum - 1; i++) {
		topBorderLine += "─";
		bottomBorderLine += "─";
	}
	topBorderLine += "┐";
	bottomBorderLine += "┘";
}

void DrawBoard()
{
	std::string posCol;
	std::string topBorderLine;
	std::string bottomBorderLine;

	posCol += "  ";
	for (int i = 0; i < g_colNum; i++) {
		posCol += std::to_string(i) + " ";
	}

	GenBorderLine(topBorderLine, bottomBorderLine, g_colNum);
	std::cout << posCol << std::endl;
	std::cout << topBorderLine << std::endl;
	for (int rowIdx = 0; rowIdx < g_rowNum; rowIdx++) {
		std::cout << std::to_string(rowIdx) << "│";
		for (int colIdx = 0; colIdx < g_colNum; colIdx++) {
			std::cout << GenBoard(rowIdx, colIdx, g_rowNum, g_colNum, g_board);
			if (colIdx != g_colNum - 1) {
				std::cout << "─";
			}
		}
		std::cout << "│" << std::endl;
	}
	std::cout << bottomBorderLine << std::endl;
}
