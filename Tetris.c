#include <Tetris.h>

int main(int argc, char** argv, char** environ)
{
    TraceLog(LOG_INFO, "Number of arguments : %d", argc);

    for(int i = 0; i < argc; i++)
    {
        TraceLog(LOG_INFO, "Argument : %d = %s", i, argv[i]);
    }

    while(*environ != 0)
    {
        TraceLog(LOG_INFO, "Argument : %s", *environ);
        environ++;
    }

    int score = 0;
    int flashFrameCount = 0;
    bool flashActive = false;
    bool gameReset = false;

    //Stage Variables
    const int windowWidth = 600; 
    const int windowHeight = 700; 

    const int startOffsetX = (windowWidth / 2) - ((STAGE_WIDTH * TILE_SIZE) / 2);
    const int startOffsetY = (windowHeight / 2) - ((STAGE_HEIGHT * TILE_SIZE) / 2);

    const int tetrominoStartX = (STAGE_WIDTH / 2) -1;
    const int tetrominoStartY = 0;

    //SpawnTetrominoPosition
    int currentTetrominoX = tetrominoStartX;
    int currentTetrominoY = tetrominoStartY;

    //RandomTetromino
    time_t unixTime;
    time(&unixTime);
    SetRandomSeed(unixTime);

    int currentTetrominoType = GetRandomValue(0, 6);
    int currentRotation = 0;
    int currentColor = GetRandomValue(0, 7);
    
    //Background Music
    InitAudioDevice(); 
    Music music = LoadMusicStream("Music/Background.mp3");
    PlayMusicStream(music);
    float timePlayed = 0.0f; 

    //Move Tetromino Sound
    Sound MovetetrominoSound = LoadSound("Music/MoveTetromino.mp3");

    //Delete Lines Sound
    Sound lineClearSound = LoadSound("Music/LineClear.mp3");

    //TetrominoSpeed
    float moveTetrominoDownTimer = 1.f;
    float timeToMoveTetrominoDown = moveTetrominoDownTimer;

    //Window
    InitWindow(windowWidth, windowHeight, "Title");
    SetTargetFPS(60);

    GameState currentGameState = MAIN_MENU;
    
    while(!WindowShouldClose())
    {
        if (currentGameState == MAIN_MENU)
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("Press ENTER to Play or ESC to Quit", 100, 200, 20, BLACK);

            EndDrawing();

            if (IsKeyPressed(KEY_ENTER))
            {
                currentGameState = GAMEPLAY;
                gameReset = true;
            }
            else if (IsKeyPressed(KEY_ESCAPE))
            {
                break;
            }
        }
        else if (currentGameState == GAMEPLAY)
        {
            if (gameReset)
            {
                ResetGame(&moveTetrominoDownTimer, &timeToMoveTetrominoDown, &score, &flashActive, &flashFrameCount, &gameReset);
            }

            //Music
            UpdateMusicStream(music);
            timePlayed = GetMusicTimePlayed(music)/GetMusicTimeLength(music);
            if (timePlayed > 1.0f) timePlayed = 1.0f;

            //VelocityTetromino
            timeToMoveTetrominoDown -= GetFrameTime();

            //Inputs
            if (IsKeyPressed(KEY_SPACE))
            {
                PlaySound(MovetetrominoSound);
                const int lastRotation = currentRotation;

                currentRotation++;

                if (currentRotation > 3)
                {
                    currentRotation = 0;
                }

                if (CheckCollision(currentTetrominoX,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
                {
                    currentRotation = lastRotation;
                }
            }

            if (IsKeyPressed(KEY_RIGHT))
            {
                PlaySound(MovetetrominoSound);

                if (!CheckCollision(currentTetrominoX+1,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
                {
                    currentTetrominoX++;
                }
            }
            if (IsKeyPressed(KEY_LEFT))
            {
                PlaySound(MovetetrominoSound);

                if (!CheckCollision(currentTetrominoX-1,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
                {
                    currentTetrominoX--;
                }
            }

            if(timeToMoveTetrominoDown <= 0 || IsKeyPressed(KEY_DOWN))
            {        
                PlaySound(MovetetrominoSound);

                if(!CheckCollision(currentTetrominoX,currentTetrominoY+1,tetrominoTypes[currentTetrominoType][currentRotation]))
                {
                    currentTetrominoY++;
                    timeToMoveTetrominoDown = moveTetrominoDownTimer;
                }

                else
                {
                    for(int y = 0; y < TETROMINO_SIZE; y++)
                    {
                        for(int x = 0; x < TETROMINO_SIZE; x++)
                        {
                            const int offset = y * TETROMINO_SIZE + x;

                            const int *tetromino = tetrominoTypes[currentTetrominoType][currentRotation];

                            if(tetromino[offset] == 1)
                            {
                                const int offset_stage = (y + currentTetrominoY) * STAGE_WIDTH + (x + currentTetrominoX);

                                stage[offset_stage] = currentColor+1;
                            }
                        }
                    }

                    //Score and Reset
                    DeleteLines(&score, &moveTetrominoDownTimer, &timeToMoveTetrominoDown, &flashActive, &flashFrameCount, lineClearSound);
                    
                    currentTetrominoX = tetrominoStartX;
                    currentTetrominoY = tetrominoStartY;

                    currentTetrominoType = GetRandomValue(0, 6);
                    currentRotation = 0;
                    currentColor = GetRandomValue(0, 7);

                    if (CheckCollision(currentTetrominoX, currentTetrominoY, tetrominoTypes[currentTetrominoType][currentRotation]))
                    {
                        currentGameState = GAME_OVER;
                        gameReset = true;
                    }
                }
            }

            BeginDrawing();
            ClearBackground(GRAY);

            //Draw Stage, Tetromino, Text
            for(int y = 0; y < STAGE_HEIGHT; y++)
            {
                for(int x = 0; x < STAGE_WIDTH; x++)
                {
                    const int offset = y * STAGE_WIDTH + x;
                    const int color = stage[offset];

                    if(stage[offset] != 0)
                    {
                        DrawRectangle(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, colorTypes[color-1]);
                    }

                    DrawRectangleLines(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, BLACK);
                }
            }
            
            DrawTetromino(colorTypes[currentColor],startOffsetX, startOffsetY, currentTetrominoX, currentTetrominoY, tetrominoTypes[currentTetrominoType][currentRotation]);

            DrawText(TextFormat("Score: %04d", score), 10, 10, 20, BLACK);

            if (flashActive)
            {
                flashFrameCount++;
                DrawFlashEffect(flashFrameCount, windowWidth, windowHeight, &flashActive);
            }

            EndDrawing();
        }
        else if (currentGameState == GAME_OVER)
        {
            // Schermata "Game Over"
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("Game Over", windowWidth / 2 - MeasureText("Game Over", 40) / 2, windowHeight / 2 - 20, 40, RED);
            DrawText("Press ENTER to Play Again or ESC to Quit", 100, windowHeight - 100, 20, BLACK);

            EndDrawing();

            if (IsKeyPressed(KEY_ENTER))
            {
                currentGameState = MAIN_MENU;
                score = 0;
                flashActive = false;
                flashFrameCount = 0;
            }
            else if (IsKeyPressed(KEY_ESCAPE))
            {
                break;
            }
        }

    }

    UnloadMusicStream(music);
    UnloadSound(MovetetrominoSound);
    UnloadSound(lineClearSound);   

    CloseAudioDevice();

    CloseWindow();

    return 0;
}