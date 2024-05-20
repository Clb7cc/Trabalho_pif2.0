/**
 * main.h
 * Created on Aug, 23th 2023
 * Author: Tiago Barros
 * Based on "From C to C++ course - 2002"
*/

#include <string.h>
#include <stdio.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


enum{
    KEY_ESC = 27,
    ENTER = 10,
    KEY_W = 119,
    KEY_S = 115,
    KEY_A = 97,
    KEY_D = 100
};

struct cobrinha{
    int noX;
    int noY;
    struct cobrinha *next;
};

struct ranking{
    char nome[21];
    int pontuacao;
};

int x = 34, y = 12;
int incX = 1, incY = 1;

void printHello(int nextX, int nextY)
{
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(x, y);
    printf(" ");
    x = nextX;
    y = nextY;
    screenGotoxy(x, y);
    printf("S");
}

void printKey(int ch)
{
    screenSetColor(YELLOW, DARKGRAY);
    screenGotoxy(35, 22);
    printf("Pontuação :");

    screenGotoxy(34, 23);
    printf(" ");
    
    if (ch == 27) screenGotoxy(36, 23);
    else screenGotoxy(39, 23);

    printf("%d ", ch);
    while (keyhit())
    {
        printf("%d ", readch());
    }
}

void printmaca(int x, int y)
{
    screenSetColor(RED, DARKGRAY);
    screenGotoxy(x, y);
    printf("O");
}


int main() 
{
    static int ch = 0;

    screenInit(1);
    keyboardInit();
    timerInit(50);

    srand((unsigned int)time(NULL));
    int macax = rand() % 76, macay = rand() % 20;
    printmaca(macax, macay);
    screenUpdate();

    while (ch != 10) //enter
    {
        // Handle user input
        if (keyhit()) 
        {
            ch = readch();
            printKey(ch);
            screenUpdate();
        }

        // Update game state (move elements, verify collision, etc)
        if (timerTimeOver() == 1)
        {
            int newX = x + incX;
            if (newX >= (MAXX -strlen("Hello World") -1) || newX <= MINX+1) incX = -incX;
            int newY = y + incY;
            if (newY >= MAXY-1 || newY <= MINY+1) incY = -incY;

            printKey(ch);
            printHello(newX, newY);

            screenUpdate();
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}
