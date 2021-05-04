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

	// ��һ����
	DeletePiece(fromRow, fromCol, g_currentPlayer);
	AddPiece(toRow, toCol, g_currentPlayer);
	
	// ����
	EatPiece(toRow, toCol, g_currentPlayer, eats);

	// �������巽
	ChangePlayer();

	//DrawBoard();
	return 0;
}

void UndoOneStep(int fromRow, int fromCol, int toRow, int toCol, std::vector<std::pair<int, int>>& eats)
{
	// ��ԭ���Ե��ĶԷ�����
	for (auto& i : eats) {
		AddPiece(i.first, i.second, g_currentPlayer); // ��ǰ���巽Ϊ�Է�
	}
	eats.clear();

	// �������巽
	ChangePlayer();
	
	// ������һ����
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
		bestValue = INFINITY_VALUE;  // bestValueԽС�Ժڷ�Խ����
	}
	else {
		bestValue = -INFINITY_VALUE; // bestValueԽ��԰׷�Խ����
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

	// ��ǰ���Ѿ����������
	if (g_currentPlayer == BLACK && bestValue == INFINITY_VALUE) {
		// �ڷ�valueԽСԽ�ã����������Խ�����·����Խ��
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