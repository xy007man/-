#include "play.h"
#include "board.h"
#include <map>
#include <vector>

static int g_currentPlayer = WHITE;
constexpr int INFINITY_VALUE = 100;
constexpr int MAX_SEARCH_DEPTH = 5;

void InitPlayer(int player)
{
	InitBoard(player);
	g_currentPlayer = player;
}

void ChangePlayer()
{
	g_currentPlayer = (WHITE + BLACK) - g_currentPlayer;
}

bool IsCurrentPlayLose()
{	
	return GetPieceNum(g_currentPlayer) < 2;
}

int EvaluatePosition()
{
	return GetPieceNum(WHITE) - GetPieceNum(BLACK);
}

int GenOneStep(int fromRow, int fromCol, int toRow, int toCol, std::vector<std::pair<int, int>> &eats)
{
	std::set<std::tuple<int, int, int, int>> ways;

	if (!IsVaildPiece(fromRow, fromCol, toRow, toCol, g_currentPlayer)) {
		return -1;
	}

	// 走一步棋
	DeletePiece(fromRow, fromCol, g_currentPlayer);
	AddPiece(toRow, toCol, g_currentPlayer);
	
	// 吃子
	EatPiece(toRow, toCol, g_currentPlayer, eats);

	// 交换走棋方
	ChangePlayer();

	//DrawBoard();
	return 0;
}

void UndoOneStep(int fromRow, int fromCol, int toRow, int toCol, std::vector<std::pair<int, int>>& eats)
{
	// 还原被吃掉的对方棋子
	for (auto& i : eats) {
		AddPiece(i.first, i.second, g_currentPlayer); // 当前走棋方为对方
	}
	eats.clear();

	// 交换走棋方
	ChangePlayer();
	
	// 撤销上一步棋
	DeletePiece(toRow, toCol, g_currentPlayer);
	AddPiece(fromRow, fromCol, g_currentPlayer);
}

int Search(int depth, int& fromRow, int& fromCol, int &toRow, int &toCol)
{
	int bestValue;
	int value;
	if (depth == 0) {
		return EvaluatePosition();
	}

	if (g_currentPlayer == BLACK) {
		bestValue = INFINITY_VALUE;  // bestValue越小对黑方越有利
	}
	else {
		bestValue = -INFINITY_VALUE; // bestValue越大对白方越有利
	}

	std::vector<std::tuple<int, int, int, int>> ways;
	std::vector<std::pair<int, int>> eats;

	GenMoveWays(g_currentPlayer, ways);
	for (auto& i : ways) {
		if (GenOneStep(std::get<0>(i), std::get<1>(i), std::get<2>(i), std::get<3>(i), eats)) {
			continue;
		}
		value = Search(depth - 1, fromRow, fromCol, toRow, toCol);
		UndoOneStep(std::get<0>(i), std::get<1>(i), std::get<2>(i), std::get<3>(i), eats);
		if (g_currentPlayer == BLACK) {
			if (value < bestValue) {
				bestValue = value;
				if (depth == MAX_SEARCH_DEPTH) {
					fromRow = std::get<0>(i);
					fromCol = std::get<1>(i);
					toRow = std::get<2>(i);
					toCol = std::get<3>(i);
				}
			}
		}
		else {
			if (value > bestValue) {
				bestValue = value;
				if (depth == MAX_SEARCH_DEPTH) {
					fromRow = std::get<0>(i);
					fromCol = std::get<1>(i);
					toRow = std::get<2>(i);
					toCol = std::get<3>(i);
				}
			}
		}
	}

	// 当前方已经无棋可走了
	if (g_currentPlayer == BLACK && bestValue == INFINITY_VALUE) {
		// 黑方value越小越好，即搜索深度越深到达无路可走越好
		return INFINITY_VALUE - (MAX_SEARCH_DEPTH - depth);
	}

	if (g_currentPlayer == WHITE && bestValue == -INFINITY_VALUE) {
		return (MAX_SEARCH_DEPTH - depth) - INFINITY_VALUE;
	}

	return bestValue;
}

void ComputerOneStep()
{
	int fromRow;
	int fromCol;
	int toRow; 
	int toCol;

	std::vector<std::pair<int, int>> eats;

	Search(MAX_SEARCH_DEPTH, fromRow, fromCol, toRow, toCol);

	GenOneStep(fromRow, fromCol, toRow, toCol, eats);
}

int PersonOneStep(int fromRow, int fromCol, int toRow, int toCol)
{
	std::vector<std::pair<int, int>> eats;
	return GenOneStep(fromRow, fromCol, toRow, toCol, eats);
}