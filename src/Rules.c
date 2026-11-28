
int playerOneScore = 0;
int playerTwoScore = 0;

 
void outOfBounds(int flag) { //flag of 1 means player 1 scores, 2 means player 2 scores.
  if (flag == 1) {
    playerOneScore++;
  } else {
    playerTwoScore++;
  }
  if (playerOneScore == 5) {
    winnerScreen(1);
  }
  else if (playerTwoScore == 5) {
    winnerScreen(2);
  }
}
//score is drawn using tallies.

//winner.
void winnerScreen(int flag) { //flag of 1 means player 1 wins, 2 means player 2 wins.
    if (flag == 1) {
    drawWin(5,5,RED);
  } else {
    drawWin(17,5,BLUE);
  }
}
