#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

enum {
  KEY_ESC = 27,
  KEY_ENTER = 10,
  KEY_W = 119,
  KEY_S = 115,
  KEY_A = 97,
  KEY_D = 100
};

struct noparacobra {
  int Xno;
  int Yno;
  struct noparacobra *next;
};

struct ranking {
  int score;
  struct ranking *next;
};

void printembaixo(int placar, int recorde, int tempo) {
  screenSetColor(YELLOW, DARKGRAY);
  screenGotoxy(7, 13);
  printf("Placar :");
  screenGotoxy(6, 14);
  printf("       ");
  screenGotoxy(9, 14);
  printf("%d", placar);
  screenGotoxy(18, 13);
  printf(" tempo vivo :");
  screenGotoxy(17, 14);
  printf("       ");
  screenGotoxy(24, 14);
  printf("%d", tempo);
  screenGotoxy(35, 13);
  printf("Recorde :");
  screenGotoxy(34, 14);
  printf("       ");
  screenGotoxy(37, 14);
  printf("%d", recorde);
}

void addcobra(struct noparacobra **head, int x, int y) {
  if (*head == NULL) {
    *head = (struct noparacobra *)malloc(sizeof(struct noparacobra));
    (*head)->Xno = x;
    (*head)->Yno = y;
    (*head)->next = NULL;
  } 
  else {
    struct noparacobra *temp2 = *head;
    struct noparacobra *novo = (struct noparacobra *)malloc(sizeof(struct noparacobra));
    novo->Xno = x;
    novo->Yno = y;
    while (temp2->next != NULL) {
      temp2 = temp2->next;
    }
    novo->next = NULL;
    temp2->next = novo;
  }
}

void printcobra(struct noparacobra *head) {
  struct noparacobra *temp2 = head;
  while (temp2 != NULL) {
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(temp2->Xno, temp2->Yno);
    printf("I");
    temp2 = temp2->next;
  }
}

void atualizarcobra(struct noparacobra *head) {
  struct noparacobra *temp2 = head;
  while (temp2 != NULL) {
    screenGotoxy(temp2->Xno, temp2->Yno);
    printf(" ");
    temp2 = temp2->next;
  }
}

void freecobra(struct noparacobra **head) {
  struct noparacobra *temp2 = *head;
  while (temp2 != NULL) {
    struct noparacobra *temp1 = temp2;
    temp2 = temp2->next;
    free(temp1);
  }
}

void cobrandando(struct noparacobra **head, int x, int y) {
  struct noparacobra *newHead = (struct noparacobra *)malloc(sizeof(struct noparacobra));
  if (newHead == NULL) {
    exit(1);
  }

  newHead->Xno = x;
  newHead->Yno = y;
  newHead->next = *head;
  *head = newHead;

  struct noparacobra *temp1 = *head;
  while (temp1->next->next != NULL) {
    temp1 = temp1->next;
  }

  free(temp1->next);
  temp1->next = NULL;
}

void printmaca(int x, int y) {
  screenSetColor(RED, DARKGRAY);
  screenGotoxy(x, y);
  printf("O");
}

int baternocorpo(struct noparacobra *head, int x, int y) {
  struct noparacobra *temp2 = head;
  while (temp2 != NULL) {
    if (temp2->Xno == x && temp2->Yno == y) {
      return 1;
    }
    temp2 = temp2->next;
  }
  return 0;
}

void randonmaca(int *x, int *y) {
  *x = rand() % (MAXX - MINX - 1) + MINX + 1;
  *y = rand() % (MAXY - MINY - 1) + MINY + 1;
}

void rankingemordem(struct ranking **cabeca, int score) {
  if (*cabeca == NULL || score > (*cabeca)->score) {
    struct ranking *novo = (struct ranking *)malloc(sizeof(struct ranking));
    novo->score = score;
    novo->next = *cabeca;
    *cabeca = novo;
  } else {
    rankingemordem(&((*cabeca)->next), score);
  }
}

void addnoranking(struct ranking *cabeca, FILE *in) {
  struct ranking *temp = cabeca;
  while (temp != NULL) {
    int score = temp->score;
    if (fwrite(&score, sizeof(int), 1, in) != 1) {
      break;
    }
    temp = temp->next;
  }
}

void printranking(struct ranking *cabeca) {
  struct ranking *temp = cabeca;
  int cont = 0;
  while (temp != NULL && (cont < 3)) {
    printf("%d° colocado: %d pontos\n", cont + 1, temp->score);
    temp = temp->next;
    cont++;
  }
}

void freeranking(struct ranking **head) {
  struct ranking *temp2 = *head;
  while (temp2 != NULL) {
    struct ranking *temp1 = temp2;
    temp2 = temp2->next;
    free(temp1);
  }
}

int main() {
  struct noparacobra *head = NULL;
  static int ch = 0;
  int placar = 0;
  int dirX = 1, dirY = 0;
  FILE *in;
  printf("          🐍🍎 SNAKE GAME 🍎🐍\n");

  screenInit(1);
  keyboardInit();
  timerInit(80);

  addcobra(&head, 25, 7);
  srand((unsigned int)time(NULL));
  int PosMacaX = rand() % (MAXX - MINX) + MINX, PosMacaY = rand() % (MAXY - MINY) + MINY;
  printmaca(PosMacaX, PosMacaY);
  screenUpdate();

  int recorde = 0;
  in = fopen("rankfile.txt", "r");
  if (in != NULL) {
    while (fread(&placar, sizeof(int), 1, in) == 1) {
      if (placar > recorde) {
        recorde = placar;
      }
    }
    fclose(in);
  }

  time_t tempoinicial, tempovivo;
  int tempo = 0;
  tempoinicial = time(NULL);

  while (ch != KEY_ESC) {
    if (keyhit()) {
      ch = readch();
      switch (ch) {
      case KEY_W:
        if (dirY != 1) {
          dirX = 0;
          dirY = -1;
        }
        break;
      case KEY_S:
        if (dirY != -1) {
          dirX = 0;
          dirY = 1;
        }
        break;
      case KEY_A:
        if (dirX != 1) {
          dirX = -1;
          dirY = 0;
        }
        break;
      case KEY_D:
        if (dirX != -1) {
          dirX = 1;
          dirY = 0;
        }
        break;
      }
      screenUpdate();
    }
    if (timerTimeOver() == 1) {
      int newX = head->Xno + dirX;
      int newY = head->Yno + dirY;
      if (newX >= (MAXX) || newX <= MINX || newY >= MAXY || newY <= MINY) {
        break;
      }
      if (baternocorpo(head, newX, newY) == 1) {
        break;
      }
      if (newX == PosMacaX && newY == PosMacaY) {
        addcobra(&head, PosMacaX, PosMacaY);
        randonmaca(&PosMacaX, &PosMacaY);
        printmaca(PosMacaX, PosMacaY);
        placar++;
      }
      atualizarcobra(head);
      cobrandando(&head, newX, newY);
      printcobra(head);
      screenUpdate();
      printembaixo(placar, recorde, tempo);
    }
    tempovivo = time(NULL);
    tempo = difftime(tempovivo, tempoinicial);
  }
  freecobra(&head);
  keyboardDestroy();
  screenDestroy();
  in = fopen("rankfile.txt", "a");
  fwrite(&placar, sizeof(int), 1, in);
  fclose(in);
  struct ranking *lista = NULL;
  in = fopen("rankfile.txt", "r");
  while (fread(&placar, sizeof(int), 1, in) == 1) {
    rankingemordem(&lista, placar);
  }
  fclose(in);
  in = fopen("rankfile.txt", "w");
  addnoranking(lista, in);
  fclose(in);
  printranking(lista);
  freeranking(&lista);
  timerDestroy();

  return 0;
}