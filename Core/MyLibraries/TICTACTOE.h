/*
 * tic_tac_toe.h
 *
 *  Created on: May 19, 2024
 *      Author: indie
 */

#ifndef MYLIBRARIES_TICTACTOE_H_
#define MYLIBRARIES_TICTACTOE_H_

#include <stdbool.h>

struct Shape{
	int s;
	int x;
	int y;
};

void TicInterruptHandler();
void TicMain();

#endif /* MYLIBRARIES_TICTACTOE_H_ */
