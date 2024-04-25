#ifndef INC_5POINTS__TYPES_H_
#define INC_5POINTS__TYPES_H_

#endif //INC_5POINTS__TYPES_H_


struct Game {
    int player1;
    int player2;
    int choice1;
    int choice2;
    int winner; // -1 game is not take place yet, 0 - draw, >0 - id player who wins
    int loser; // -1 game is not take place yet, 0 - draw, >0 - id player who loses
} typedef Game;

struct Player {
    int id;
    int score;
} typedef Player;