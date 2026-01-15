#include "raylib.h"
#include <cstdlib>
#include <ctime>

char board[3][3];
const char PLAYER = 'X';
const char BOT    = 'O';

int gameState = 0; 
int difficulty = 0; 
int selected = 0;
char winner = ' ';

bool soundPlayed = false;

Sound sClick, sWin, sLose;

char currentTurn = PLAYER; 


void ResetBoard()
{
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            board[i][j]=' ';
}

char CheckWinner()
{
    for(int i=0;i<3;i++)
    {
        if(board[i][0]==board[i][1] && board[i][0]==board[i][2] && board[i][0]!=' ')
            return board[i][0];
        if(board[0][i]==board[1][i] && board[0][i]==board[2][i] && board[0][i]!=' ')
            return board[0][i];
    }
    if(board[0][0]==board[1][1] && board[0][0]==board[2][2] && board[0][0]!=' ')
        return board[0][0];
    if(board[0][2]==board[1][1] && board[0][2]==board[2][0] && board[0][2]!=' ')
        return board[0][2];
    return ' ';
}

bool BoardFull()
{
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            if(board[i][j]==' ') return false;
    return true;
}


void BotRandom()
{
    int x,y;
    do{
        x=rand()%3;
        y=rand()%3;
    }while(board[x][y]!=' ');
    board[x][y]=BOT;
    PlaySound(sClick);
}

int WinningMove(char p)
{
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            if(board[i][j]==' ')
            {
                board[i][j]=p;
                if(CheckWinner()==p)
                {
                    board[i][j]=' ';
                    return i*3+j;
                }
                board[i][j]=' ';
            }
    return -1;
}

void BotRule()
{
    int m = WinningMove(BOT);
    if(m!=-1){ board[m/3][m%3]=BOT; PlaySound(sClick); return; }

    m = WinningMove(PLAYER);
    if(m!=-1){ board[m/3][m%3]=BOT; PlaySound(sClick); return; }

    if(board[1][1]==' '){ board[1][1]=BOT; PlaySound(sClick); return; }

    BotRandom();
}

int Minimax(bool bot)
{
    char w = CheckWinner();
    if(w==BOT) return 10;
    if(w==PLAYER) return -10;
    if(BoardFull()) return 0;

    int best = bot ? -1000 : 1000;

    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            if(board[i][j]==' ')
            {
                board[i][j]= bot ? BOT : PLAYER;
                int s = Minimax(!bot);
                board[i][j]=' ';
                best = bot ? (s>best?s:best) : (s<best?s:best);
            }
    return best;
}

void BotMinimax()
{
    int best=-1000,bx=0,by=0;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            if(board[i][j]==' ')
            {
                board[i][j]=BOT;
                int s = Minimax(false);
                board[i][j]=' ';
                if(s>best){ best=s; bx=i; by=j; }
            }
    board[bx][by]=BOT;
    PlaySound(sClick);
}

void BotMove()
{
    if(difficulty==1) BotRandom();
    else if(difficulty==2) BotRule();
    else BotMinimax();
}


int main()
{
    InitWindow(600,600,"XOX");
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    SetTargetFPS(60);
    srand((unsigned)time(0));

    sClick=LoadSound("assets/click.wav");
    sWin=LoadSound("assets/win.wav");
    sLose=LoadSound("assets/lose.wav");

    ResetBoard();

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Vector2 m = GetMousePosition();

        // -------- MENU --------
        if(gameState==0)
        {
            soundPlayed=false;
            DrawText("XOX OYUNU",190,50,40,BLACK);

            Rectangle b1={200,170,200,55};
            Rectangle b2={200,240,200,55};
            Rectangle b3={200,310,200,55};
            Rectangle b4={200,380,200,55};

            if(CheckCollisionPointRec(m,b1)) selected=1;
            if(CheckCollisionPointRec(m,b2)) selected=2;
            if(CheckCollisionPointRec(m,b3)) selected=3;
            if(CheckCollisionPointRec(m,b4)) selected=4;

            if(IsKeyPressed(KEY_ONE)) selected=1;
            if(IsKeyPressed(KEY_TWO)) selected=2;
            if(IsKeyPressed(KEY_THREE)) selected=3;
            if(IsKeyPressed(KEY_FOUR)) selected=4;

            DrawRectangleRec(b1,selected==1?ORANGE:LIGHTGRAY);
            DrawRectangleRec(b2,selected==2?ORANGE:LIGHTGRAY);
            DrawRectangleRec(b3,selected==3?ORANGE:LIGHTGRAY);
            DrawRectangleRec(b4,selected==4?ORANGE:LIGHTGRAY);

            DrawText("1 - Kolay",250,185,25,BLACK);
            DrawText("2 - Orta",250,255,25,BLACK);
            DrawText("3 - Imkansiz",235,325,25,BLACK);
            DrawText("4 - Iki Oyunculu",210,395,22,BLACK);

            if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON)||IsKeyPressed(KEY_ENTER)) && selected!=0)
            {
                difficulty=selected;
                ResetBoard();
                winner=' ';
                currentTurn = PLAYER;
                gameState=1;
            }
        }

        else if(gameState==1)
        {
            bool twoPlayer = (difficulty==4);

            if(twoPlayer)
            {
                if(currentTurn=='X') DrawText("Sira: X",10,10,25,RED);
                else DrawText("Sira: O",10,10,25,BLUE);
            }
            else
            {
                DrawText("Sen: X  Bot: O",10,10,20,GRAY);
            }

            for(int i=0;i<3;i++)
                for(int j=0;j<3;j++)
                {
                    Rectangle r={100+j*130,100+i*130,120,120};
                    DrawRectangleLinesEx(r,2,BLACK);

                    if(board[i][j]=='X') DrawText("X",r.x+40,r.y+25,80,RED);
                    if(board[i][j]=='O') DrawText("O",r.x+40,r.y+25,80,BLUE);

                    if(CheckCollisionPointRec(m,r) &&
                       IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                       board[i][j]==' ')
                    {
                        if(twoPlayer)
                        {
                            board[i][j]=currentTurn;
                            PlaySound(sClick);

                            winner=CheckWinner();
                            if(winner==' ' && !BoardFull())
                            {
                                currentTurn = (currentTurn=='X') ? 'O' : 'X';
                            }
                            else
                            {
                                gameState=2;
                            }
                        }
                        else
                        {
                            // botlu: oyuncu oynar, sonra bot oynar
                            board[i][j]=PLAYER;
                            PlaySound(sClick);

                            winner=CheckWinner();
                            if(winner==' ' && !BoardFull()) BotMove();

                            winner=CheckWinner();
                            if(winner!=' ' || BoardFull()) gameState=2;
                        }
                    }
                }

            DrawText("ESC : Menu",10,560,20,GRAY);
            if(IsKeyPressed(KEY_ESCAPE))
            {
                gameState=0;
                selected=0;
            }
        }

        else
        {
            bool twoPlayer = (difficulty==4);

            if(!soundPlayed)
            {
                if(twoPlayer)
                {
                    if(winner!=' ') PlaySound(sWin);
                }
                else
                {
                    if(winner==PLAYER) PlaySound(sWin);
                    else if(winner==BOT) PlaySound(sLose);
                }
                soundPlayed=true;
            }

            if(twoPlayer)
            {
                if(winner=='X') DrawText("X KAZANDI!",160,200,40,GREEN);
                else if(winner=='O') DrawText("O KAZANDI!",160,200,40,BLUE);
                else DrawText("BERABERE!",180,200,40,ORANGE);
            }
            else
            {
                if(winner==PLAYER) DrawText("KAZANDIN!",180,200,40,GREEN);
                else if(winner==BOT) DrawText("KAYBETTIN!",170,200,40,RED);
                else DrawText("BERABERE!",180,200,40,ORANGE);
            }

            Rectangle back={200,300,200,60};
            DrawRectangleRec(back,LIGHTGRAY);
            DrawText("MENUYE DON",215,320,25,BLACK);

            if(CheckCollisionPointRec(m,back) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                gameState=0;
                selected=0;
            }
        }

        EndDrawing();
    }

    UnloadSound(sClick);
    UnloadSound(sWin);
    UnloadSound(sLose);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

