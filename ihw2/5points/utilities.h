#ifndef INC_5POINTS__UTILITIES_H_
#define INC_5POINTS__UTILITIES_H_

#endif //INC_5POINTS__UTILITIES_H_

const char *convert(int choice) {
  if (choice == 1) {
    return "rock";
  }
  if (choice == 2) {
    return "paper";
  }
  if (choice == 3) {
    return "scissors";
  }
}

void wait(sem_t *sem, int value) { // Waits until semaphore is 0
  int tmp;
  while (1) {
    sem_getvalue(sem, &tmp);
    if (tmp == value) break;
  }
}

int get_winner(int player1, int player2, int choice1, int choice2) {
  if (choice1 == choice2) {
    return 0; // draw
  }
  // 1 - rock, 2 - paper, 3 - scissors
  if (choice1 == 1) {
    if (choice2 == 3) {
      return player1;
    } else {
      return player2;
    }
  }
  if (choice1 == 2) {
    if (choice2 == 1) {
      return player1;
    } else {
      return player2;
    }
  }
  if (choice1 == 3) {
    if (choice2 == 2) {
      return player1;
    } else {
      return player2;
    }
  }
}