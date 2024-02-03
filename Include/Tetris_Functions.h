#ifndef TETRIS_FUNCTIONS_H
#define TETRIS_FUNCTIONS_H

#include <raylib.h>
#include <time.h>
#include <string.h>

#define STAGE_WIDTH 12
#define STAGE_HEIGHT 22
#define TILE_SIZE 24
#define TETROMINO_SIZE 4

typedef enum { MAIN_MENU, GAMEPLAY, GAME_OVER } GameState;

void DrawTetromino(const Color currentColor, const int startOffsetX, const int startOffsetY, const int tetrominoStartX, const int tetrominoStartY, const int *tetromino);
int CheckCollision(const int tetrominoStartX, const int tetrominoStartY, const int *tetromino);
void ResetLines(int startLineY);
void DeleteLines(int *score, float *moveTetrominoDownTimer, float *timeToMoveTetrominoDown, bool *flashActive, int *flashFrameCount, Sound lineClearSound);
void DrawFlashEffect(int frameCount, int windowWidth, int windowHeight, bool *flashActive);
void ResetGame(float *moveTetrominoDownTimer, float *timeToMoveTetrominoDown, int *score, bool *flashActive, int *flashFrameCount, bool *gameReset);

#endif 