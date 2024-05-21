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

struct Jogador {
  char nome[11];
  int score;
};

struct ranking {
  struct Jogador jogador;
  struct ranking *next;
};

void printembaixo(int placar, int recorde, int  temp1o);
void addcobra(struct noparacobra **head, int x, int y);
void printcobra(struct noparacobra *head);
void atualizarcobra(struct noparacobra *head);
void freecobra(struct noparacobra **head);
void cobrandando(struct noparacobra **head, int x, int y);
void printfruta(int x, int y);
int baternocorpo(struct noparacobra *head, int x, int y);
void randonmaca(int *x, int *y);
void rankingemordem(struct ranking **cabeca, struct Jogador nick);
void addnoranking(struct ranking *cabeca, FILE *in);
void printranking(struct ranking *cabeca);
void freeranking(struct ranking **head);

int incX = 1, incY = 1;

int main() {
  struct noparacobra *head = NULL;
  static int ch = 0;
  int placar = 0;
  int dirX = 1, dirY = 0; // Inicialmente movendo para a direita
  FILE *in;
  struct Jogador player;
  printf("🐍🍎 SNAKE GAME 🍎🐍\n");
  printf("Digite seu nick para ser salvo no ranking: ");
  scanf("%s", player.nome);

  screenInit(1);
  keyboardInit();
  timerInit(50);

  addcobra(&head, 34, 12);
  srand((unsigned int)time(NULL));
  int PosMacaX = rand() % 68 + 8, PosMacaY = rand() % 16 + 4;
  printfruta(PosMacaX, PosMacaY);
  screenUpdate();

  int recorde = INT_MIN; // definido no <limits.h>
  in = fopen("placar.txt", "r");
  if (in != NULL) {
    while (fread(&player, sizeof(struct Jogador), 1, in) == 1) {
      if (player.score > recorde) {
        recorde = player.score;
      }
    }
    fclose(in);
  }

  time_t start_time, current_time;
  int  temp1o = 0;
  start_time = time(NULL);

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

      if (baternocorpo(head, newX, newY) == 1) {
        break; // Colisão com o próprio corpo
      }

      if (newX == PosMacaX && newY == PosMacaY) {
        addcobra(&head, PosMacaX, PosMacaY);
        randonmaca(&PosMacaX, &PosMacaY);
        printfruta(PosMacaX, PosMacaY);
        placar++;
      }
      // Mover a cobra
      atualizarcobra(head); // Limpa a posição anterior
      cobrandando(&head, newX, newY);
      printcobra(head); // Desenha a cobra na nova posição
      screenUpdate();
      printembaixo(placar, recorde,  temp1o);
    }

    current_time = time(NULL);
     temp1o = difftime(current_time, start_time);
  }

  freecobra(&head);
  keyboardDestroy();
  screenDestroy();
  player.score = placar;
  in = fopen("placar.txt", "a"); // começo placar
  fwrite(&player, sizeof(struct Jogador), 1, in);
  fclose(in);
  struct ranking *lista = NULL;
  in = fopen("placar.txt", "r");
  while (fread(&player, sizeof(struct Jogador), 1, in) == 1) {
    rankingemordem(&lista, player);
  }
  fclose(in);
  in = fopen("placar.txt", "w");
  addnoranking(lista, in);
  fclose(in);
  printranking(lista);
  freeranking(&lista); // fim do placar
  timerDestroy();

  return 0;
}

void printembaixo(int placar, int recorde, int  temp1o) {
  screenSetColor(YELLOW, DARKGRAY);
  screenGotoxy(12, 22);
  printf("Placar :");
  screenGotoxy(11, 23);
  printf("       ");
  screenGotoxy(16, 23);
  printf("%d", placar);

  screenGotoxy(32, 22);
  printf(" tempo vivo :");
  screenGotoxy(31, 23);
  printf("       ");
  screenGotoxy(36, 23);
  printf("%d",  temp1o);

  screenGotoxy(57, 22);
  printf("Recorde :");
  screenGotoxy(56, 23);
  printf("       ");
  screenGotoxy(61, 23);
  printf("%d", recorde);
}
//começa: 2   acaba: 78
void addcobra(struct noparacobra **head, int x, int y) {
  if (*head == NULL) {
    *head = (struct noparacobra *)malloc(sizeof(struct noparacobra));
    (*head)->Xno = x;
    (*head)->Yno = y;
    (*head)->next = NULL;
  } 
  else {
    struct noparacobra *n = *head;
    struct noparacobra *novo =
        (struct noparacobra *)malloc(sizeof(struct noparacobra));
    novo->Xno = x;
    novo->Yno = y;
    while (n->next != NULL) {
      n = n->next;
    }
    novo->next = NULL;
    n->next = novo;
  }
}

void printcobra(struct noparacobra *head) {
  struct noparacobra *n = head;
  while (n != NULL) {
    screenSetColor(GREEN, DARKGRAY);
    screenGotoxy(n->Xno, n->Yno);
    printf("I");
    n = n->next;
  }
}

void atualizarcobra(struct noparacobra *head) {
  struct noparacobra *n = head;
  while (n != NULL) {
    screenGotoxy(n->Xno, n->Yno);
    printf(" ");
    n = n->next;
  }
}

void freecobra(struct noparacobra **head) {
  struct noparacobra *n = *head;
  while (n != NULL) {
    struct noparacobra * temp1 = n;
    n = n->next;
    free( temp1);
  }
}

void cobrandando(struct noparacobra **head, int x, int y) {
  struct noparacobra *newHead =
      (struct noparacobra *)malloc(sizeof(struct noparacobra));
  if (newHead == NULL) {
    // Tratamento de erro, se a alocação de memória falhar
    exit(1);
  }

  newHead->Xno = x;
  newHead->Yno = y;
  newHead->next = *head;
  *head = newHead;

  // Remover a última parte da cauda
  struct noparacobra * temp1 = *head;
  while ( temp1->next->next != NULL) {
     temp1 =  temp1->next;
  }

  free( temp1->next);
   temp1->next = NULL;
}

void printfruta(int x, int y) {
  screenSetColor(RED, DARKGRAY);
  screenGotoxy(x, y);
  printf("O");
}

int baternocorpo(struct noparacobra *head, int x, int y) {
  struct noparacobra *n = head;
  while (n != NULL) {
    if (n->Xno == x && n->Yno == y) {
      return 1; // Colisão com o corpo
    }
    n = n->next;
  }
  return 0; // Sem colisão
}

void randonmaca(int *x, int *y) {
  *x = rand() % 68 + 8;
  *y = rand() % 16 + 4;
}

void rankingemordem(struct ranking **cabeca, struct Jogador nick) {
  if (*cabeca == NULL) {
    *cabeca = (struct ranking *)malloc(sizeof(struct ranking));
    (*cabeca)->jogador = nick;
    (*cabeca)->next = NULL;
  } 
  else {
    struct ranking *n = *cabeca;
    struct ranking *novo = (struct ranking *)malloc(sizeof(struct ranking));
    novo->jogador = nick;
    while (n->next != NULL && nick.score < n->next->jogador.score) {
      n = n->next;
    }
    if (nick.score > (*cabeca)->jogador.score) {
      novo->next = *cabeca;
      *cabeca = novo;
    } 
    else if (n->next == NULL) {
      novo->next = NULL;
      n->next = novo;
    } 
    else {
      novo->next = n->next;
      n->next = novo;
    }
  }
}

void addnoranking(struct ranking *cabeca, FILE *in) {
  struct ranking *temp = cabeca;
  struct Jogador Ojogador;
  while (temp != NULL) {
    Ojogador = temp->jogador;
    if (fwrite(&Ojogador, sizeof(struct Jogador), 1, in) != 1) {
      break;
    }
    temp = temp->next;
  }
}

void printranking(struct ranking *cabeca) {
  struct ranking *temp = cabeca;
  struct Jogador Ojogador;
  int cont = 0;
  while (temp != NULL && (cont < 3)) {
    printf("%d° colocado: %d pontos -> %s\n", cont+1, temp->jogador.score, temp->jogador.nome);
    temp = temp->next;
    cont++;
  }
}

void freeranking(struct ranking **head) {
  struct ranking *temp = *head;
  while (temp != NULL) {
    struct ranking * temp1 = temp;
    temp = temp->next;
    free( temp1);
  }
}
