#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <cstdio>

using namespace std;

// SETTINGS
const int WIDTH = 800;
const int HEIGHT = 600;
const int GRID = 20;
const int COLS = WIDTH / GRID;
const int ROWS = HEIGHT / GRID;

struct Point
{
    int x;
    int y;
};

enum State
{
    MENU,
    PLAYING,
    GAMEOVER
};

Point snake[500];
Point food;
Point walls[200];

int snakeLength;
int wallCount;
int score;
int highScore;
int level;

int dirX;
int dirY;

bool paused = false;
State gameState = MENU;

// HIGH SCORE
void loadHighScore()
{
    ifstream file("highscore.txt");

    if (file.is_open())
        file >> highScore;
    else
        highScore = 0;

    file.close();
}

void saveHighScore()
{
    ofstream file("highscore.txt");

    if (file.is_open())
        file << highScore;

    file.close();
}

// WALLS
void generateWalls()
{
    wallCount = level * 3;

    for (int i = 0; i < wallCount; i++)
    {
        walls[i].x = rand() % COLS;
        walls[i].y = rand() % ROWS;
    }
}

// FOOD
void generateFood()
{
    bool valid;

    do
    {
        valid = true;

        food.x = rand() % COLS;
        food.y = rand() % ROWS;

        for (int i = 0; i < snakeLength; i++)
        {
            if (snake[i].x == food.x &&
                snake[i].y == food.y)
            {
                valid = false;
            }
        }

        for (int i = 0; i < wallCount; i++)
        {
            if (walls[i].x == food.x &&
                walls[i].y == food.y)
            {
                valid = false;
            }
        }

    } while (!valid);
}

// RESET GAME
void resetGame()
{
    snakeLength = 5;

    snake[0].x = 10;
    snake[0].y = 10;

    for (int i = 1; i < snakeLength; i++)
    {
        snake[i].x = snake[0].x - i;
        snake[i].y = snake[0].y;
    }

    dirX = 1;
    dirY = 0;

    score = 0;
    level = 1;
    paused = false;

    generateWalls();
    generateFood();
}

// INPUT
void input()
{
    if (!kbhit())
        return;

    int key = getch();

    if (key == 0 || key == 224)
        key = getch();

    if (key == 27)
    {
        closegraph();
        exit(0);
    }

    if (key == 'p' || key == 'P')
    {
        paused = !paused;
    }

    if (gameState == MENU)
    {
        if (key == 13)
        {
            resetGame();
            gameState = PLAYING;
        }
        return;
    }

    if (gameState == GAMEOVER)
    {
        if (key == 'r' || key == 'R')
        {
            gameState = MENU;
        }
        return;
    }

    if (gameState != PLAYING)
        return;

    if ((key == 'w' || key == 'W' || key == 72) && dirY != 1)
    {
        dirX = 0;
        dirY = -1;
    }

    if ((key == 's' || key == 'S' || key == 80) && dirY != -1)
    {
        dirX = 0;
        dirY = 1;
    }

    if ((key == 'a' || key == 'A' || key == 75) && dirX != 1)
    {
        dirX = -1;
        dirY = 0;
    }

    if ((key == 'd' || key == 'D' || key == 77) && dirX != -1)
    {
        dirX = 1;
        dirY = 0;
    }
}

// UPDATE
void update()
{
    if (paused)
        return;

    for (int i = snakeLength - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }

    snake[0].x += dirX;
    snake[0].y += dirY;

    if (snake[0].x < 0 ||
        snake[0].x >= COLS ||
        snake[0].y < 0 ||
        snake[0].y >= ROWS)
    {
        gameState = GAMEOVER;
        return;
    }

    for (int i = 1; i < snakeLength; i++)
    {
        if (snake[0].x == snake[i].x &&
            snake[0].y == snake[i].y)
        {
            gameState = GAMEOVER;
            return;
        }
    }

    for (int i = 0; i < wallCount; i++)
    {
        if (snake[0].x == walls[i].x &&
            snake[0].y == walls[i].y)
        {
            gameState = GAMEOVER;
            return;
        }
    }

    if (snake[0].x == food.x &&
        snake[0].y == food.y)
    {
        snakeLength++;
        score++;

        if (score > highScore)
        {
            highScore = score;
            saveHighScore();
        }

        int newLevel = score / 5 + 1;

        if (newLevel > level)
        {
            level = newLevel;
            generateWalls();
        }

        generateFood();
    }
}

// DRAW
void drawSnake()
{
    for (int i = 0; i < snakeLength; i++)
    {
        int x = snake[i].x * GRID;
        int y = snake[i].y * GRID;

        setfillstyle(SOLID_FILL, GREEN);
        bar(x + 1, y + 1,
            x + GRID - 1,
            y + GRID - 1);
    }
}

void drawFood()
{
    int x = food.x * GRID;
    int y = food.y * GRID;

    setfillstyle(SOLID_FILL, RED);

    fillellipse(
        x + GRID / 2,
        y + GRID / 2,
        GRID / 2 - 2,
        GRID / 2 - 2
    );
}

void drawWalls()
{
    setfillstyle(SOLID_FILL, DARKGRAY);

    for (int i = 0; i < wallCount; i++)
    {
        int x = walls[i].x * GRID;
        int y = walls[i].y * GRID;

        bar(
            x + 1,
            y + 1,
            x + GRID - 1,
            y + GRID - 1
        );
    }
}

void drawUI()
{
    char text[100];

    sprintf(
        text,
        "Score: %d   High Score: %d   Level: %d",
        score,
        highScore,
        level
    );

    setcolor(WHITE);
    outtextxy(10, 10, text);

    if (paused)
    {
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
        outtextxy(330, 250, "PAUSED");
    }
}

void drawMenu()
{
    cleardevice();

    setcolor(LIGHTGREEN);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 5);

    outtextxy(200, 150, "MODERN SNAKE");

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);

    outtextxy(250, 270, "Press ENTER to Start");
    outtextxy(250, 320, "WASD or Arrow Keys");
    outtextxy(250, 370, "P = Pause");
    outtextxy(250, 420, "ESC = Exit");
}

void drawGameOver()
{
    cleardevice();

    setcolor(RED);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 5);

    outtextxy(220, 180, "GAME OVER");

    char txt[50];

    sprintf(txt, "Score: %d", score);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);

    outtextxy(300, 280, txt);
    outtextxy(200, 360, "Press R to return");
}

void render()
{
    cleardevice();

    drawWalls();
    drawFood();
    drawSnake();
    drawUI();
}

// MAIN
int main()
{
    srand((unsigned)time(0));

    loadHighScore();

    initwindow(WIDTH, HEIGHT, "Modern Snake");

    while (true)
    {
        input();

        if (gameState == MENU)
        {
            drawMenu();
        }
        else if (gameState == PLAYING)
        {
            update();
            render();

            delay(100);
        }
        else if (gameState == GAMEOVER)
        {
            drawGameOver();
        }
    }

    closegraph();
    return 0;
}