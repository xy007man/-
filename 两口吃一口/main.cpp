#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "board.h"
#include "play.h"
#define MAN_MACHINE_GAME 1

int main()
{
	int fromRow;
	int fromCol;
	int toRow;
	int toCol;

	InitPlayer(WHITE);
	DrawBoard();
	
	while (!IsCurrentPlayLose()) {
#if MAN_MACHINE_GAME
		do {
			std::cout << "请输入要走棋子的起点和终点的行号和列号(4个数字):";
			std::cin >> fromRow;
			std::cin >> fromCol;
			std::cin >> toRow;
			std::cin >> toCol;
		}while (PersonOneStep(fromRow, fromCol, toRow, toCol));
#endif
		ComputerOneStep();
		DrawBoard();
	}

	return 0;
}