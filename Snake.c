#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

const int N = 20;

void swap(char *a, char *b)
{
	*a = *a + *b;
	*b = *a - *b;
	*a = *a - *b;
}

void swapi(int *a, int *b)
{
	*a = *a + *b;
	*b = *a - *b;
	*a = *a - *b;
}

struct pos
{
	int i;
	int j;
};

struct snake
{
	int isAlive;
	int len;
	struct pos *c_pos;

	char head;
	char body;
	char end;
};

void checkGameOver(char M[][N], struct snake *S)
{
	if (M[S->c_pos[0].i][S->c_pos[0].j] == '-' ||
		M[S->c_pos[0].i][S->c_pos[0].j] == '|' ||
		M[S->c_pos[0].i][S->c_pos[0].j] == S->body ||
		M[S->c_pos[0].i][S->c_pos[0].j] == S->end
		) S->isAlive = 0;
}

void genFood(char M[][N], struct snake *S)
{
	int i = 1 + rand() % (N - 2);
	int j = 1 + rand() % (N - 2);

	char food = '1' + rand() % 3;

	while (M[i][j] == S->body || M[i][j] == S->head || M[i][j] == S->end)
	{
		i = 1 + rand() % (N - 2);
		j = 1 + rand() % (N - 2);
	}

	M[i][j] = food;
}

void draw(char M[][N])
{
    clear();

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
            printw("%c ", M[i][j]);
		}

		printw("\n");
	}
}

void init(char M[][N], struct snake *S)
{
	S->isAlive = 1;
	S->len = 3;

	S->head = '@';
	S->body = '#';
	S->end = '*';

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
		{
			M[i][j] = ' ';

			if (j == N - 1 || j == 0) M[i][j] = '|';
			if (i == N - 1 || i == 0) M[i][j] = '-';
		}

	S->c_pos[0].i = N / 2;
	S->c_pos[0].j = S->c_pos[0].i;

	M[S->c_pos[0].i][S->c_pos[0].j] = S->head;

	for (int k = 1; k < S->len; ++k)
	{
		S->c_pos[k].i = S->c_pos[k - 1].i + 1;
		S->c_pos[k].j = S->c_pos[0].j;

		M[S->c_pos[k].i][S->c_pos[k].j] = S->body;
	}

	S->c_pos[S->len - 1].i = S->c_pos[S->len - 2].i + 1;
	S->c_pos[S->len - 1].j = S->c_pos[S->len - 2].j;

	M[S->c_pos[S->len - 1].i][S->c_pos[S->len - 1].j] = S->end;
}

int main(void)
{
	srand(time(0));

	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);

	int di = -1, dj = 0;
	int i, j;
	int cost = 0;

	struct snake S;
	struct pos cp[(N - 2) * (N - 2)];
	S.c_pos = &cp[0];

	char M[N][N];
	char input = 'W';
	char lInput = input;

	init(M, &S);
	genFood(M, &S);

	while (S.isAlive)
	{
		draw(M);

        input = getch();
        input = toupper(input);

        switch (input)
        {
            case 'W': if (lInput != 'S') { di = -1; dj = 0; lInput = input; } break;
            case 'A': if (lInput != 'D') { di = 0; dj = -1; lInput = input; } break;
            case 'S': if (lInput != 'W') { di = 1; dj = 0; lInput = input; } break;
            case 'D': if (lInput != 'A') { di = 0; dj = 1; lInput = input; } break;
        }

		i = S.c_pos[0].i;
		j = S.c_pos[0].j;

		S.c_pos[0].i += di;
		S.c_pos[0].j += dj;

		checkGameOver(M, &S);

		if (M[S.c_pos[0].i][S.c_pos[0].j] > '0' && M[S.c_pos[0].i][S.c_pos[0].j] < '4')
		{
			cost = (int)M[S.c_pos[0].i][S.c_pos[0].j] - 48;

			M[S.c_pos[0].i][S.c_pos[0].j] = ' ';

			genFood(M, &S);
		}

		swap(&M[S.c_pos[0].i][S.c_pos[0].j], &M[i][j]);

		if (cost)
		{
			for (int k = 1; k < S.len - 1; k++)
			{
				swap(&M[S.c_pos[k].i][S.c_pos[k].j], &M[i][j]);

				swapi(&S.c_pos[k].i, &i);
				swapi(&S.c_pos[k].j, &j);
			}

			S.len++;

			S.c_pos[S.len - 1].i = S.c_pos[S.len - 2].i;
			S.c_pos[S.len - 1].j = S.c_pos[S.len - 2].j;

			S.c_pos[S.len - 2].i = i;
			S.c_pos[S.len - 2].j = j;

			M[S.c_pos[S.len - 2].i][S.c_pos[S.len - 2].j] = S.body;

			cost--;
		}
		else
		{
			for (int k = 1; k < S.len; k++)
			{
				swap(&M[S.c_pos[k].i][S.c_pos[k].j], &M[i][j]);

				swapi(&S.c_pos[k].i, &i);
				swapi(&S.c_pos[k].j, &j);
			}
		}

		usleep(350000);
	}

	printw("\nLOOSEER!\n");

    nodelay(stdscr, FALSE);
	getch();
    endwin();

	return 0;
}
