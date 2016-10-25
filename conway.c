/*
 * Conway's Game of Life implemented in C.
 * Allows for starvation/reproduction to wrap around the edges of the board.
 *
 * Usage:
 *  ./conway [-f stateFile]
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termcap.h>
#include <unistd.h>

void printUsage();
char *parseArgs(char *argv[]);
void buf_push(char **buf);
void printGame(char **buf);
int neighbors(int x, int y, char **buf);

typedef struct {
  char **buf1;
  char **buf2;
  int x, y;
  int width, height;
} field;

field game;

int main(int argc, char *argv[])
{
  if (argc != 3)
    printUsage();

  char c;
  int i, j, n_neighbors;
  char buf[1024], *clear_scr, **writebuf, **readbuf;
  int coin = 0;

  char *filename = parseArgs(argv);
  FILE *stateFile = fopen(filename, "r");
  if (!stateFile) {
    printf("Error opening stateFile: %s\n", strerror(errno));
    exit(1);
  }

  memset(&game, 0, sizeof(game));

  // scan statefile for width/height
  printf("Reading statefile...\n");
  while ((c = fgetc(stateFile)) != EOF) {
    if (c == '\n') {
      break;
    } else {
      game.width++;
    }
  }
  fseek(stateFile, 0L, SEEK_END);
  game.height = ftell(stateFile) / game.width;

  // allocate game buffers
  game.buf1 = malloc(sizeof(char**) * game.height);
  game.buf2 = malloc(sizeof(char**) * game.height);
  if (game.buf1 == NULL || game.buf2 == NULL) {
    printf("Error allocating game buffer, whoops!\n");
    exit(1);
  }
  for (i = 0; i < game.height; i++) {
    game.buf1[i] = malloc(sizeof(char*) * game.width);
    game.buf2[i] = malloc(sizeof(char*) * game.width);
    if (!game.buf1[i] || !game.buf2[i]) {
      printf("Error allocating game buffer, whoops!\n");
      exit(1);
    }
  }

  // read in statefile to field buffer
  if (fseek(stateFile, 0, SEEK_SET) == -1) {
    printf("Error rewinding file pointer, whoops!\n");
    exit(1);
  }
  while ((c = fgetc(stateFile)) != EOF) {
    if (c == '\n') {
      game.y++;
      game.x = 0;
    } else {
      game.buf1[game.y][game.x] = c;
      game.buf2[game.y][game.x] = c;
      game.x++;
    }
  }

  // retrieve terminal info
  tgetent(buf, getenv("TERM"));
  clear_scr = tgetstr("cl", NULL);

  while (1) {
    // looks at each cell in readbuf, output its fate in writebuf
    for (i = 0; i < game.height; i++) {
      for (j = 0; j < game.width; j++) {
        writebuf[i][j] = readbuf[i][j];
        n_neighbors = (neighbors(j, i, readbuf));
        if (readbuf[i][j] == '.') {
          if (n_neighbors == 3) {
            writebuf[i][j] = 'x';
          }
        } else {
          if (n_neighbors < 2 || n_neighbors > 3) {
            writebuf[i][j] = '.';
          }
        }
      }
    }

    // alternate reading/writing between two buffers
    coin = 1 - coin;
    writebuf = (coin ? game.buf1 : game.buf2);
    readbuf = (coin ? game.buf2 : game.buf1);

    fputs(clear_scr, stdout);
    printGame(readbuf);

    usleep(100*1000);
  }
}

void printUsage()
{
  printf("Usage: ./conway -f stateFile");
  exit(1);
}

/*
 * Enforce that we receive exactly 2 arguments, of the form '-f fileName.txt'.
 */
char *parseArgs(char *argv[])
{
  if (strncmp(argv[1],"-f",2) == 0) {
    return argv[2];
  } else {
    printUsage();
    return NULL;
  }
}

/*
 * Print game buffer to terminal.
 */
void printGame(char **buf)
{
  int i, j;
  for (i = 0; i < game.height; i++) {
    for(j = 0; j < game.width; j++) {
      printf("%c", buf[i][j]);
    }
    printf("\n");
  }
}

/*
 * Return number of neighbors around the cell in the yth row and xth column.
 */
int neighbors(int x, int y, char **buf)
{
  // compute indices of neighbors
  int u = (y+1) % game.height;
  int d = (y-1) % game.height;
  int r = (x+1) % game.width;
  int l = (x-1) % game.width;
  // make negative indices wrap around
  u = (u < 0 ? u + game.height : u);
  d = (d < 0 ? d + game.height : d);
  r = (r < 0 ? r + game.width : r);
  l = (l < 0 ? l + game.width : l);
  // count up neighbors using indices
  return (
    (buf[y][l] == 'x' ? 1 : 0) +
    (buf[y][r] == 'x' ? 1 : 0) +
    (buf[u][x] == 'x' ? 1 : 0) +
    (buf[d][x] == 'x' ? 1 : 0) +
    (buf[u][l] == 'x' ? 1 : 0) +
    (buf[u][r] == 'x' ? 1 : 0) +
    (buf[d][l] == 'x' ? 1 : 0) +
    (buf[d][r] == 'x' ? 1 : 0)
  );
}
