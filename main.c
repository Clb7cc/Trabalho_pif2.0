#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

enum {
  KEY_ESC = 27,
  KEY_ENTER = 10,
  KEY_W = 119,
  KEY_S = 115,
  KEY_A = 97,
  KEY_D = 100
};

struct Snakenode {
  int Xno;
  int Yno;
  struct Snakenode *next;
};

struct Jogador {
  char Nome[21];
  int score;
};

struct Highscore {
  struct Jogador jogador;
  struct Highscore *next;
};

void printPlacar(int placar);
void printGameOver();
void AdicionarSnake(struct Snakenode **head, int x, int y);
void printSnake(struct Snakenode *head);
void LimparSnake(struct Snakenode *head);
void FreeSnake(struct Snakenode **head);
void MoveSnake(struct Snakenode **head, int x, int y);
void PrintMaca(int x, int y);
int ColisaoCorpo(struct Snakenode *head, int x, int y);
void ProxMaca(int *x, int *y);
void OrdenarLista(struct Highscore **cabeca, struct Jogador nick);
void EscreverLista(struct Highscore *cabeca, FILE *in);
void PrintarLista(struct Highscore *cabeca);
void FreeLista(struct Highscore **head);

int incX = 1, incY = 1;

int main() {
  struct Snakenode *head = NULL;
  static int ch = 0;
  int placar = 0;
  int dirX = 1, dirY = 0; // Inicialmente movendo para a direita
  FILE *in;
  struct Jogador player;
  printf("Digite seu nick para ser salvo no rank: ");
  scanf("%s", player.Nome);

  screenInit(1);
  keyboardInit();
  timerInit(50);

  AdicionarSnake(&head, 34, 12);
  srand((unsigned int)time(NULL));
  int PosMacaX = rand() % 68 + 8, PosMacaY = rand() % 16 + 4;
  PrintMaca(PosMacaX, PosMacaY);
  screenUpdate();

  while (ch != KEY_ESC) {
    if (keyhit()) {
      ch = readch();
      // Atualizar direção com base na tecla de seta
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

      // Verifica colisão
      if (newX >= (MAXX) || newX <= MINX || newY >= MAXY || newY <= MINY) {
        break; // Colisão com a parede
      }

      if (ColisaoCorpo(head, newX, newY) == 1) {
        break; // Colisão com o próprio corpo
      }

      if (newX == PosMacaX && newY == PosMacaY) {
        AdicionarSnake(&head, PosMacaX, PosMacaY);
        ProxMaca(&PosMacaX, &PosMacaY);
        PrintMaca(PosMacaX, PosMacaY);
        placar++;
      }
      printPlacar(placar);
      // Mover a cobra
      LimparSnake(head); // Limpa a posição anterior
      MoveSnake(&head, newX, newY);
      printSnake(head); // Desenha a cobra na nova posição
      screenUpdate();
    }
  }

  FreeSnake(&head);
  keyboardDestroy();
  screenDestroy();
  player.score = placar;
  in = (fopen("placar.txt", "a")); // começo placar
  fwrite(&player, sizeof(struct Jogador), 1, in);
  fclose(in);
  struct Highscore *lista = NULL;
  in = (fopen("placar.txt", "r"));
  while (fread(&player, sizeof(struct Jogador), 1, in) == 1) {
    OrdenarLista(&lista, player);
  }
  fclose(in);
  in = (fopen("placar.txt", "w"));
  EscreverLista(lista, in);
  fclose(in);
  PrintarLista(lista);
  FreeLista(&lista); // fim do placar
  printGameOver();
  timerDestroy();

  return 0;
}

void printGameOver() {
  screenSetColor(YELLOW, DARKGRAY);
  screenGotoxy(30, 10);
  printf("Game over!");
}

void printPlacar(int placar) {
  screenSetColor(YELLOW, DARKGRAY);
  screenGotoxy(35, 22);
  printf("Placar :");

  screenGotoxy(34, 23);
  printf("       ");
  screenGotoxy(39, 23);
  printf("%d", placar);
}

void AdicionarSnake(struct Snakenode **head, int x, int y) {
  if (*head == NULL) {
    *head = (struct Snakenode *)malloc(sizeof(struct Snakenode));
    (*head)->Xno = x;
    (*head)->Yno = y;
    (*head)->next = NULL;
  } else {
    struct Snakenode *n = *head;
    struct Snakenode *novo =
        (struct Snakenode *)malloc(sizeof(struct Snakenode));
    novo->Xno = x;
    novo->Yno = y;
    while (n->next != NULL) {
      n = n->next;
    }
    novo->next = NULL;
    n->next = novo;
  }
}

void printSnake(struct Snakenode *head) {
  struct Snakenode *n = head;
  while (n != NULL) {
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(n->Xno, n->Yno);
    printf("I");
    n = n->next;
  }
}

void LimparSnake(struct Snakenode *head) {
  struct Snakenode *n = head;
  while (n != NULL) {
    screenGotoxy(n->Xno, n->Yno);
    printf(" ");
    n = n->next;
  }
}

void FreeSnake(struct Snakenode **head) {
  struct Snakenode *n = *head;
  while (n != NULL) {
    struct Snakenode *temp = n;
    n = n->next;
    free(temp);
  }
}

void MoveSnake(struct Snakenode **head, int x, int y) {
  struct Snakenode *newHead =
      (struct Snakenode *)malloc(sizeof(struct Snakenode));
  if (newHead == NULL) {
    // Tratamento de erro, se a alocação de memória falhar
    exit(1);
  }

  newHead->Xno = x;
  newHead->Yno = y;
  newHead->next = *head;
  *head = newHead;

  // Remover a última parte da cauda
  struct Snakenode *temp = *head;
  while (temp->next->next != NULL) {
    temp = temp->next;
  }

  free(temp->next);
  temp->next = NULL;
}

void PrintMaca(int x, int y) {
  screenSetColor(RED, DARKGRAY);
  screenGotoxy(x, y);
  printf("O");
}

int ColisaoCorpo(struct Snakenode *head, int x, int y) {
  struct Snakenode *n = head;
  while (n != NULL) {
    if (n->Xno == x && n->Yno == y) {
      return 1; // Colisão com o corpo
    }
    n = n->next;
  }
  return 0; // Sem colisão
}

void ProxMaca(int *x, int *y) {
  *x = rand() % 68 + 8;
  *y = rand() % 16 + 4;
}

void OrdenarLista(struct Highscore **cabeca, struct Jogador nick) {
  if (*cabeca == NULL) {
    *cabeca = (struct Highscore *)malloc(sizeof(struct Highscore));
    (*cabeca)->jogador = nick;
    (*cabeca)->next = NULL;
  } else {
    struct Highscore *n = *cabeca;
    struct Highscore *novo = (struct Highscore *)malloc(sizeof(struct Highscore));
    novo->jogador = nick;
    while (n->next != NULL && nick.score
 < n->next->jogador.score
) {
      n = n->next;
    }
    if (nick.score
 > (*cabeca)->jogador.score
) {
      novo->next = *cabeca;
      *cabeca = novo;
    } else if (n->next == NULL) {
      novo->next = NULL;
      n->next = novo;
    } else {
      novo->next = n->next;
      n->next = novo;
    }
  }
}

void EscreverLista(struct Highscore *cabeca, FILE *in) {
  struct Highscore *n = cabeca;
  struct Jogador Ojogador;
  while (n != NULL) {
    Ojogador = n->jogador;
    if (fwrite(&Ojogador, sizeof(struct Jogador), 1, in) != 1) {
      break;
    }
    n = n->next;
  }
}

void PrintarLista(struct Highscore *cabeca) {
  struct Highscore *n = cabeca;
  struct Jogador Ojogador;
  int i = 1;
  while (n != NULL && (i < 4)) {
    printf("%d colocado!:\n", i);
    printf("Nome: %s\n", n->jogador.Nome);
    printf("Pontuação: %d\n", n->jogador.score
);
    n = n->next;
    i++;
  }
}

void FreeLista(struct Highscore **head) {
  struct Highscore *n = *head;
  while (n != NULL) {
    struct Highscore *temp = n;
    n = n->next;
    free(temp);
  }
}