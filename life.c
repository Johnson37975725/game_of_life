#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>
#include <sys/ioctl.h>

#define MAX_R      100
#define MAX_C      100

#define SEC    1000000
#define MAX_S 10000000
#define MIN_S    10000

typedef unsigned char u_char;

static int ROW;
static int COL;

void set_window_size(int r, int c){
  struct winsize ws;

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);

  if(r > 9){
    if((ROW = r) > MAX_R){
      ROW = MAX_R;
    }
  }else{
    if((ROW = ws.ws_row - 3) > MAX_R){
      ROW = MAX_C;
    }
  }

  if(c > 9){
    if((COL = c) > MAX_C){
      COL = MAX_C;
    }
  }else{
    if((COL = (ws.ws_col - 2) / 2) > MAX_C){
      COL = MAX_C;
    }
  }
}

void display_table(u_char table[MAX_R][MAX_C], int steps){
  int n_pop, i, j;

  init_pair(1, COLOR_RED, COLOR_GREEN);

  clear();
  mvprintw(0, 0, "+");
  for(i = 1; i < (COL*2+1); i++){
    mvprintw(0, i, "-");
  }
  mvprintw(0, i, "+");

  n_pop = 0;
  for(i = 0; i < ROW; i++){
    mvprintw((i+1), 0, "|");
    attrset(COLOR_PAIR(1));
    for(j = 0; j < COL; j++){
      if(table[i][j]){
        mvprintw((i+1), (j*2+1), "  ");
        n_pop++;
      }
    }
    attroff(COLOR_PAIR(1));
    mvprintw((i+1), (j*2+1), "|");
  }

  mvprintw((ROW+1), 0, "+");
  for(i = 1; i < (COL*2+1); i++){
    mvprintw((ROW+1), i, "-");
  }
  mvprintw((ROW+1),  i, "+");
  mvprintw((ROW+2),  1, "POPULATION : %3d   STEPS : %3d", n_pop, steps);
  mvprintw((ROW+2), 35, "FIELD-SIZE : WIDTH=%2d, HEIGHT=%2d", COL, ROW);

  refresh();
}

int moore_neighbour(u_char table[MAX_R][MAX_C], int r, int c){
  int r_index[8] = { -1, -1,  0,  1,  1,  1,  0, -1 };
  int c_index[8] = {  0,  1,  1,  1,  0, -1, -1, -1 };
  int count, i;

  count = 0;
  for(i = 0; i < 8; i++){
    if(table[(r+r_index[i]+ROW)%ROW][(c+c_index[i]+COL)%COL]){
      count++;
    }
  }

  return count;
}

int next_step(u_char table[MAX_R][MAX_C]){
  u_char next[MAX_R][MAX_C] = {};
  int pop, i, j;

  for(i = 0; i < ROW; i++){
    for(j = 0; j < COL; j++){
      switch(moore_neighbour(table, i, j)){
      case 3:
        next[i][j] = 1;
        break;

      case 2:
        next[i][j] = table[i][j];
        break;

      default:
        next[i][j] = 0;
        break;
      }
    }
  }

  pop = 0;
  for(i = 0; i < ROW; i++){
    for(j = 0; j < COL; j++){
      if((table[i][j] = next[i][j])){
	pop++;
      }
    }
  }

  return pop;
}

int initialise_table(u_char table[MAX_R][MAX_C]){
  int pop, i, j;

  pop = 0;
  for(i = 0; i < ROW; i++){
    for(j = 0; j < COL; j++){
      if((table[i][j] = (rand() % 2))){
	pop++;
      }
    }
  }

  return pop;
}

int copy_table(u_char src[MAX_R][MAX_C], u_char dst[MAX_R][MAX_C]){
  int d, i, j;

  d = 0;
  for(i = 0; i < ROW; i++){
    for(j = 0; j < COL; j++){
      if(src[i][j] != dst[i][j]){
	dst[i][j] = src[i][j];
	d++;
      }
    }
  }

  return d; // return hamming distance
}

int main(int argc, char **argv){
  u_char table[MAX_R][MAX_C] = {};
  u_char  prev[MAX_R][MAX_C] = {};
  long t_sleep;
  int flag, opt, i;

  t_sleep = 1000000;
  flag    = 0;
  set_window_size(0, 0);

  while((opt = getopt(argc, argv, "hs:r:c:")) != (-1)){
    switch(opt){
    case 'h':
      fprintf(stderr, 
	      "usage: %s [-h][-s time][-r row][-c column]\n"
	      " -h : help\n"
	      " -s : sleeping time at each step\n"
	      " -r : row size of field\n"
	      " -c : column size of field\n", argv[0]);
      exit(EXIT_FAILURE);

    case 's':
      t_sleep = (long)(atof(optarg) * SEC);
      if(t_sleep < MIN_S || t_sleep > MAX_S){
	t_sleep = 1000000;
      }
      break;

    case 'r':
      flag |= 1;
      set_window_size(atoi(optarg), COL);
      break;

    case 'c':
      flag |= 2;
      set_window_size(ROW, atoi(optarg));
      break;
    }
  }

  initscr();

  if(!has_colors()){
    endwin();
    fprintf(stderr, "Error - no color support on this terminal\n");
    exit(EXIT_SUCCESS);
  }

  if(start_color() != OK){
    endwin();
    fprintf(stderr, "Error - could not initialise colours\n");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  while(1){
    set_window_size((ROW*(flag&1)), (COL*(flag>>1)));
    
    initialise_table(table);
    
    copy_table(table, prev);
    
    i = 0;
    while(1){
      display_table(table, (i+1));

      next_step(table);

      usleep(t_sleep);

      if((i++%2) && (copy_table(table, prev) < 1)){
	break;
      }
    }
  }

  endwin();

  exit(EXIT_SUCCESS);
}

