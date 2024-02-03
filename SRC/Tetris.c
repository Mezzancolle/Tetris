#include <Tetris_Functions.h>

extern int stage[];

void DrawTetromino(const Color currentColor, const int startOffsetX, const int startOffsetY, const int tetrominoStartX, const int tetrominoStartY, const int *tetromino)
{
    for(int y = 0; y < TETROMINO_SIZE; y++)
    {
        for(int x = 0; x < TETROMINO_SIZE; x++)
        {
            const int offset = y * TETROMINO_SIZE + x;

            // Check if the current cell in the tetromino is filled
            if(tetromino[offset] == 1)
            {
                // Draw a rectangle at the corresponding position on the screen
                DrawRectangle((x + tetrominoStartX) * TILE_SIZE + startOffsetX, (y + tetrominoStartY) * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, currentColor);
            }
        }
    }
}

int CheckCollision(const int tetrominoStartX, const int tetrominoStartY, const int *tetromino)
{
    for(int y = 0; y < TETROMINO_SIZE; y++)
    {
        for(int x = 0; x < TETROMINO_SIZE; x++)
        {
            const int offset = y * TETROMINO_SIZE + x;

            // Check if the current cell in the tetromino is filled
            if(tetromino[offset] == 1)
            {
                // Calculate the corresponding position in the game stage
                const int offset_stage = (y + tetrominoStartY) * STAGE_WIDTH + (x + tetrominoStartX);

                // Check if there is a collision with the existing blocks in the stage
                if (stage[offset_stage] != 0)
                {
                    return 1; // Collision detected
                }
            }
        }
    }

    return 0; // No collision
}

void ResetLines(int startLineY)
{
    for (int y = startLineY; y >= 0; y--)
    {
        for (int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            const int offset = y * STAGE_WIDTH + x;
            const int offset_below = (y + 1) * STAGE_WIDTH + x;

            // Move the block from the current line above to the line below
            if (stage[offset_below] == 0 && stage[offset] > 0)
            {
                stage[offset_below] = stage[offset];
                stage[offset] = 0;
            }
        }
    }
}

void DeleteLines(int *score, float *moveTetrominoDownTimer, float *timeToMoveTetrominoDown, bool *flashActive, int *flashFrameCount, Sound lineClearSound)
{
    for (int y = 0; y < STAGE_HEIGHT - 1; y++)
    {
        int checkLine = 1;

        // Check if the entire line is filled with blocks
        for (int x = 1; x < STAGE_WIDTH - 1; x++)
        {
            const int offset = y * STAGE_WIDTH + x;

            if (stage[offset] == 0)
            {
                checkLine = 0;
                break;
            }
        }

        // If the line is filled, clear it and update the game state
        if (checkLine)
        {
            const int offset = y * STAGE_WIDTH + 1;
            memset(stage + offset, 0, (STAGE_WIDTH - 2) * sizeof(int));

            ResetLines(y);

            *flashActive = true;
            *flashFrameCount = 0;

            *score += 100;

            *moveTetrominoDownTimer -= 0.1f;

            if ((*moveTetrominoDownTimer) < 0.1f)
            {
                (*moveTetrominoDownTimer) = 0.1f;
            }

            (*timeToMoveTetrominoDown) = (*moveTetrominoDownTimer);

            PlaySound(lineClearSound);
        }
    }
}

void DrawFlashEffect(int frameCount, int windowWidth, int windowHeight, bool *flashActive)
{
    // Draw a flashing effect on the screen
    if (frameCount % 2 == 0)
    {
        DrawRectangle(0, 0, windowWidth, windowHeight, Fade(RAYWHITE, 0.5f));
    }

    // Disable the flashing effect after a certain number of frames
    if (frameCount > 10)
    {
        *flashActive = false;
    }
}

void ResetGame(float *moveTetrominoDownTimer, float *timeToMoveTetrominoDown, int *score, bool *flashActive, int *flashFrameCount, bool *gameReset)
{
    // Reset various game parameters and initialize the game stage
    *score = 0;
    *flashActive = false;
    *flashFrameCount = 0;
    *moveTetrominoDownTimer = 1.0f;
    *timeToMoveTetrominoDown = *moveTetrominoDownTimer;
    *gameReset = false;

    // Reset the game stage array to its initial state
    for (int i = 0; i < STAGE_WIDTH * STAGE_HEIGHT; i++)
    {
        stage[i] = (i % STAGE_WIDTH == 0 || i % STAGE_WIDTH == STAGE_WIDTH - 1 || i / STAGE_WIDTH == STAGE_HEIGHT - 1) ? 1 : 0;
    }
}