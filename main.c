#include "raylib.h"
#include <math.h>
#include "raymath.h"
#include <stdio.h>
#include <kos.h>
#include <adx/adx.h>
#include <adx/snddrv.h>
#include <dc/sound/sfxmgr.h>
#include <dc/sound/sound.h>
#include <dc/sound/stream.h>
#include <stdint.h>

#define PLAYER_LIVES 5
#define BRICKS_LINES 5
#define BRICKS_PER_LINE 18
#define BRICKS_POSITION_Y 20

#define screenWidth 640
#define screenHeight 480

typedef enum GameScreen { LOGO=0, TITLE, GAMEPLAY, ENDING} GameScreen;

static sfxhnd_t sfx_bounce;
static sfxhnd_t sfx_hit;
static sfxhnd_t sfx_lose;
static sfxhnd_t sfx_win;

void playBounce()
{
    snd_sfx_play(sfx_bounce, 12, 128);
}
void playHit()
{
    snd_sfx_play(sfx_hit, 12, 128);
}
void playLose()
{
    snd_sfx_play(sfx_lose, 12, 128);
} 
   void playWin()
{
    snd_sfx_play(sfx_win, 12, 128);
}   

typedef struct Paddle
{
    Vector2 position;
    Vector2 speed;
    Vector2 size;
    Rectangle bounds;
    int lives;
} Paddle;

typedef struct Ball
{
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
} Ball;

typedef struct Brick
{
    Vector2 position;
    Vector2 size;
    Rectangle bounds;
    Color brickcolor;
    int resistance;
    bool active;
} Brick;

Color First;
Color Second; 
Color FinalLerp;
Color RedOrangeLerp;
Color OrangeRedLerp;
Color LifeLerp;

float clSin = 0.0f;
void colorLerping()
{
    clSin = (sinf(GetTime()) + 1.0f) / 2.0f;
    RedOrangeLerp = ColorLerp(RED, ORANGE, clSin);
    OrangeRedLerp = ColorLerp(ORANGE, RED, clSin);
    LifeLerp = ColorLerp(LIME, GREEN, clSin);
    First = ColorLerp(BLUE, SKYBLUE, clSin);
    Second = ColorLerp(SKYBLUE, PURPLE, clSin);
    FinalLerp = ColorLerp(First, Second, clSin);
}  

int main(void)
{
    snd_stream_init();
    sfx_bounce = snd_sfx_load("rd/bounce.wav");
    sfx_hit = snd_sfx_load("rd/hit.wav");
    sfx_lose = snd_sfx_load("rd/lose.wav");
    sfx_win = snd_sfx_load("rd/win.wav");
    InitWindow(screenWidth, screenHeight, "DevBall");
    GameScreen CurrentScreen = LOGO;
    int framesCounter = 0;
    int gameresult = -1;
    bool pause = false;

    Texture2D BallT = LoadTexture("rd/DevBall.png");
    Texture2D PaddleT = LoadTexture("rd/DevPaddle.png");
    Texture2D LogoT = LoadTexture("rd/DevBallLogo.png");
    Texture2D Test = LoadTexture("rd/test.png");

    Paddle paddle = {0};
    Ball ball = {0};
    Brick bricks[BRICKS_LINES][BRICKS_PER_LINE] = {0};
    int brickCount = 0;

    paddle.position = (Vector2){(screenWidth/2)-32, screenHeight - 40};
    paddle.speed = (Vector2){8.0f, 0.0f};
    paddle.size = (Vector2){64, 16};
    paddle.lives = PLAYER_LIVES;

    ball.radius = 16.0f; // Half of the texture size (64 / 2)
    ball.active = false;
    ball.position = (Vector2){paddle.position.x+(paddle.size.x/2), paddle.position.y - (ball.radius*2)};
    ball.speed = (Vector2){4.0f, 4.0f};

    SetTargetFPS(60);

    colorLerping(); // Initialize the lerp colors before using them

    for (int i = 0; i < BRICKS_LINES; i++)
    {
        for (int j = 0; j < BRICKS_PER_LINE; j++)
        {
            bricks[i][j].position = (Vector2){(j*((screenWidth)/BRICKS_PER_LINE))+3, i*BRICKS_POSITION_Y+10};
            bricks[i][j].size = (Vector2){(screenWidth/BRICKS_PER_LINE)-1, BRICKS_POSITION_Y-1};
            bricks[i][j].bounds = (Rectangle){bricks[i][j].position.x, bricks[i][j].position.y, bricks[i][j].size.x, bricks[i][j].size.y};
            bricks[i][j].resistance = 1;
            bricks[i][j].active = true;
            brickCount += bricks[i][j].resistance;
        }
    }

    while(!WindowShouldClose())
    {
        //update
        switch(CurrentScreen)
        {
            case LOGO:
            {
                framesCounter++;
                if (framesCounter > 120)
                {
                    CurrentScreen = TITLE;
                    framesCounter = 0;
                }
            }break;
            case TITLE:
            {
                framesCounter++;
                if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) CurrentScreen = GAMEPLAY;
            }break;
            case GAMEPLAY:
            {
                framesCounter++;
                if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)||IsKeyPressed('P')) pause = !pause;
                
                if (!pause)
                {
                    if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)||IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) paddle.position.x -= paddle.speed.x;
                    if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)||IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) paddle.position.x += paddle.speed.x;

                    if (paddle.position.x <= 0) paddle.position.x = 0;
                    if (paddle.position.x >= screenWidth - paddle.size.x) paddle.position.x = screenWidth - paddle.size.x;

                    paddle.bounds = (Rectangle){paddle.position.x, paddle.position.y, paddle.size.x, paddle.size.y};

                    if (ball.active)
                    {
                        ball.position.x += ball.speed.x;
                        ball.position.y += ball.speed.y;

                        if ((ball.position.x-ball.radius <= 0 && ball.speed.x < 0) || (ball.position.x+ball.radius >= screenWidth && ball.speed.x > 0)) 
                        {
                            ball.speed.x *= -1;
                            playBounce();
                        }
                        if (ball.position.y-ball.radius <= 0) 
                        {
                            ball.speed.y *= -1;
                            playBounce();
                        }
                        if (CheckCollisionCircleRec(ball.position, ball.radius, paddle.bounds) && ball.speed.y > 0)
                        {
                            ball.speed.y *= -1;
                            ball.speed.x = 5*(ball.position.x - (paddle.position.x + paddle.size.x/2))/paddle.size.x;
                            playBounce();
                        }
                        for (int j = 0; j < BRICKS_LINES; j++)
                        {
                            for (int i = 0; i < BRICKS_PER_LINE; i++)
                            {
                                if (bricks[j][i].active && (CheckCollisionCircleRec(ball.position, ball.radius, bricks[j][i].bounds)))
                                {
                                    playHit();
                                    bricks[j][i].active = false;
                                    if(ball.position.y <= bricks[j][i].position.y-8 || ball.position.y >= bricks[j][i].position.y+8 + bricks[j][i].size.y)
                                    {
                                        ball.speed.y *= -1;
                                    }
                                    else
                                    {
                                        ball.speed.x *= -1;
                                    }
                                    brickCount--;
                                    break;
                                }
                            }
                        }
                        if(ball.position.y+ball.radius*2 >= screenHeight) 
                        {
                            paddle.position = (Vector2){(screenWidth/2)-32, screenHeight - 40};
                            ball.active = false;
                            ball.speed = (Vector2){0,0};
                            paddle.lives--;
                        }
                        if (paddle.lives <= 0)
                        {
                            gameresult = 0;
                            playLose();
                            CurrentScreen = ENDING;
                            paddle.lives = PLAYER_LIVES;
                            framesCounter = 0;
                        }
                    }else
                    {
                        ball.position = (Vector2){paddle.position.x+(paddle.size.x/2), paddle.position.y - (ball.radius*2)};
                        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)||IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
                        {
                            ball.active = true;
                            ball.speed = (Vector2){4.0f, -4.0f};
                        }
                    }
                    if (brickCount <= 0)
                    {
                        gameresult = 1;
                        playWin();
                        CurrentScreen = ENDING;
                        framesCounter = 0;
                    }
                }
            }break;
            case ENDING:
            {
                framesCounter++;
                if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                {
                    CurrentScreen = TITLE;
                }
            }break;
            default: break;
        }
    
        BeginDrawing();
        ClearBackground(FinalLerp);
        switch (CurrentScreen)
        {
            //draw
            case LOGO:
            {
                DrawTexture(LogoT, 200, 120, WHITE);
                DrawText("A thing.", 200, 400, 20, DARKGRAY);
            }break;
            case TITLE:
            {
                DrawTexture(Test, 0, 0, WHITE);
                if ((framesCounter/30)%5 < 3) DrawText("Press A to start", GetScreenWidth()/2 - MeasureText("Press A to start", 20)/2, GetScreenHeight()/2 + 60, 20, DARKGRAY);
            }break;
            case GAMEPLAY:
            {
            // Draw GUI: player lives
                char frameCounterText[10];
                sprintf(frameCounterText, "%d", framesCounter);
                DrawText(frameCounterText, 0, 0, 10, DARKGRAY);
                for (int i = 0; i < paddle.lives; i++) DrawRectangle(20 + 40*i, screenHeight - 10, 35, 10, LifeLerp);
                if(pause) 
                {
                    if ((framesCounter/30)%5 < 3) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
                }
                DrawTexture(BallT, ball.position.x-16, ball.position.y-16, WHITE);
                DrawTexture(PaddleT, paddle.position.x, paddle.position.y, WHITE);
                for(int j = 0; j < BRICKS_LINES; j++)
                {
                    for(int i = 0; i < BRICKS_PER_LINE; i++)
                    {
                        if (bricks[j][i].active)
                        {
                            colorLerping();
                            if ((i+j)%2==0)
                            {
                               bricks[j][i].brickcolor = RedOrangeLerp;
                            }else 
                            {
                                bricks[j][i].brickcolor = OrangeRedLerp;
                            }
                            DrawRectangle(bricks[j][i].position.x, bricks[j][i].position.y, bricks[j][i].size.x, bricks[j][i].size.y, bricks[j][i].brickcolor);
                        }
                    }
                }
            }break;
            case ENDING:
            {
                DrawText("Thanks for playin'", 20, 20, 20, DARKGRAY);
                if (gameresult == 0) DrawText("YOU LOSE", GetScreenWidth()/2 - MeasureText("YOU LOSE", 40)/2, GetScreenHeight()/2 - 40, 40, GRAY);
                else DrawText("YOU WIN", GetScreenWidth()/2 - MeasureText("YOU WIN", 40)/2, GetScreenHeight()/2 - 40, 40, GRAY);
                if ((framesCounter/30)%2 == 0) DrawText("PRESS A TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS A TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 + 80, 20, GRAY);
            }break;
            default: break;
        }
        EndDrawing();
    }
    return 0;
}