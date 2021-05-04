#pragma once
void InitPlayer(int player);
void ComputerOneStep();
int PersonOneStep(int fromRow, int fromCol, int toRow, int toCol);
bool IsCurrentPlayLose();