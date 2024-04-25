#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include "types.h"
#include "utilities.h"

int pid;
void sig_term_handler(int sig) {
  if (pid == 0) {
    exit(0);
  }
  if (sig == SIGINT) {
    printf("The tournament is over ahead of schedule\n");
    exit(0);
  }
}

void close_and_unlink(sem_t *sem, const char *name) {
  if (sem_close(sem) == -1) {
    perror("sem_close");
    exit(1);
  }
  if (sem_unlink(name) == -1) {
    perror("sem_unlink");
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  if (n <= 0) {
    printf("Invalid input\n");
    exit(1);
  }

  if (n == 1) {
    printf("The permanent champion!!!\n");
    exit(0);
  }

  signal(SIGINT, sig_term_handler);
  const char *shm_name = "shm";
  int shm_id;
  if ((shm_id = shm_open(shm_name, O_CREAT | O_RDWR, 0666)) == -1) {
    perror("shmget()");
    exit(1);
  }

  if (ftruncate(shm_id, sizeof(Game)) == -1) {
    perror("ftruncate");
    exit(1);
  }

  Game *game = (Game *) mmap(NULL, sizeof(Game), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

  const char *sem_name_player = "/player-semaphore";
  const char *sem_name_judge = "/judge-semaphore";
  const char *sem_name_match = "/match-semaphore";
  sem_t *sem_player = sem_open(sem_name_player, O_CREAT, 0666, 0);
  sem_t *sem_judge = sem_open(sem_name_judge, O_CREAT, 0666, 0);
  sem_t *sem_match = sem_open(sem_name_match, O_CREAT, 0666, 0);
  sem_init(sem_player, 1, n);
  sem_init(sem_judge, 1, 1);
  sem_init(sem_match, 1, 0);

  int player_id = 0;
  for (int i = 0; i < n; ++i) {
    pid = fork();
    if (pid == 0) {
      printf("Player with id %d joined the game\n", i + 1);
      player_id = i + 1;
      break;
    }
  }

  if (pid == 0) sem_wait(sem_player);

  wait(sem_player, 0);
  Game games[n * (n - 1) / 2];
  Player players[n];

  if (pid != 0) {
    printf("All players ready, the tournament is starting...\n");
    printf("\n");

    int t = 0;
    for (int i = 0; i < n; ++i) {
      players[i] = (Player) {i + 1, 0};
      for (int j = i + 1; j < n; ++j) {
        games[t++] = (Game) {i + 1, j + 1, -1, -1};
      }
    }

    printf("Tournament grid: \n");
    for (int i = 0; i < n * (n - 1) / 2; ++i) {
      printf("%d vs %d\n", games[i].player1, games[i].player2);
    }
    printf("\n");
    sem_wait(sem_judge);
  }

  wait(sem_judge, 0);
  srand(time(0));
  if (pid == 0) {
    int num_games = 0;
    while (1) {
      int tmp;
      sem_getvalue(sem_match, &tmp);
      if (tmp == 1 && (player_id == game->player1 || player_id == game->player2)) {
        int choice = rand() % 3 + 1;
        if (game->player1 == player_id) {
          game->choice1 = choice;
        } else {
          game->choice2 = choice;
        }
        sleep(1);
        printf("Player %d throws %s\n", player_id, convert(choice));
        ++num_games;
        sem_post(sem_player);
        sem_getvalue(sem_player, &tmp);
        if (tmp == 2) sem_wait(sem_match);
        if (num_games == n - 1) {
          printf("  Player %d finishes the tournament\n", player_id);
          exit(0);
        }
      }
    }
  } else {
    for (int i = 0; i < n * (n - 1) / 2; ++i) {
      wait(sem_match, 0);
      printf("[Match] %d vs %d\n", games[i].player1, games[i].player2);
      game->player1 = games[i].player1;
      game->player2 = games[i].player2;
      game->winner = -1;
      sem_post(sem_match);
      wait(sem_player, 2);
      sem_wait(sem_player);
      sem_wait(sem_player);
      game->winner = get_winner(game->player1, game->player2, game->choice1, game->choice2);
      if (game->winner == game->player1) {
        game->loser = game->player2;
      } else {
        game->loser = game->player1;
      }
      if (game->winner == 0) {
        printf("[Draw] (%s vs %s) -> player %d vs player %d\n", convert(game->choice1), convert(game->choice2), game->player1, game->player2);
      } else {
        if (game->winner == game->player1) {
          printf("[Win] (%s vs %s) -> player %d vs player %d\n", convert(game->choice1), convert(game->choice2), game->winner, game->loser);
        } else {
          printf("[Win] (%s vs %s) -> player %d vs player %d\n", convert(game->choice2), convert(game->choice1), game->winner, game->loser);
        }
      }

      if (game->winner == 0) {
        players[game->player1 - 1].score += 1;
        players[game->player2 - 1].score += 1;
      } else {
        players[game->winner - 1].score += 2;
        players[game->loser - 1].score += 0;
      }
    }

    if (shm_unlink(shm_name) == -1) {
      perror("shm_unlink");
      exit(1);
    }

    close_and_unlink(sem_player, sem_name_player);
    close_and_unlink(sem_judge, sem_name_judge);
    close_and_unlink(sem_match, sem_name_match);

    printf("Tournament is over\n");
    printf("Results: \n");

    for (int i = 0; i < n; ++i) {
      printf("Player %d earned %d points\n", players[i].id, players[i].score);
    }
  }

  return 0;
}

