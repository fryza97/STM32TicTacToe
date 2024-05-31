/*
 * tic_tac_toe.c
 *
 *  Created on: May 19, 2024
 *      Author: indie
 */
#include "main.h"
#include "TICTACTOE.h"
#include "LCD.h"
#include "ADC.h"

#include "TICTACTOE_O.c"
#include "TICTACTOE_X.c"

#define ADC_DELAY 100
#define ADC_BOTTOM_LIMITER 500
#define ADC_UPPER_LIMITER 3500

struct Pole{
	int a;
	int b;
};

/*struct Shape{
	int s;
	int x;
	int y;
}*/

const struct Pole player_x 	= {0, 159};
const struct Pole player_y 	= {0, 127};

const struct Pole menu1_x 	= {0, 159};
const struct Pole menu1_y 	= {0, 61};

const struct Pole menu2_x 	= {0, 159};
const struct Pole menu2_y 	= {66, 127};

const struct Pole x_1 	= {3, 51};
const struct Pole x_2 	= {55, 106};
const struct Pole x_3 	= {110, 156};

const struct Pole y_1 	= {3, 40};
const struct Pole y_2 	= {44, 85};
const struct Pole y_3 	= {89, 124};

static uint16_t adc_read[] = {2000, 2000};
static struct Pole pos = {1, 1};
static int player = 0;

int end_game = 0;
int i = 0;
int shape_size = 0;
volatile static bool button_clicked = false;

static void TicSetContour(struct Pole pole1, struct Pole pole2, uint16_t color){
	/*for (int y = 0; y < LCD_HEIGHT; y++) {
		      for (int x = 0; x < LCD_WIDTH; x++) {
		    	  if(((x >= pole1.a && x <= pole1.b) && (y >= pole2.a && y <= pole2.b)) &&
		    			  !((x >= pole1.a+3 && x <= pole1.b-3) && (y >= pole2.a+3 && y <= pole2.b-3)))
		    		  LCDPutPixel(x, y, color);
		      }
	}*/

	LCDPutRect(pole1.a, pole2.a, pole1.b-pole1.a+1, 3, color);
	LCDPutRect(pole1.a, pole2.b-2, pole1.b-pole1.a+1, 3, color);
	LCDPutRect(pole1.a, pole2.a, 3, pole2.b-pole2.a+1, color);
	LCDPutRect(pole1.b-2, pole2.a, 3, pole2.b-pole2.a+1, color);
}

static void TicSetShape(int a, int b, uint8_t* tic_shape, uint16_t shape_color, uint16_t background_color){
		int j = 0;
		int x_0, y_0;
		uint16_t healper;

		switch(a){
			case 0:	x_0 = 7;
					break;
			case 1:	x_0 = 59;
					break;
			case 2: x_0 = 113;
					break;
			default:x_0 = 7;
					break;
		}

		switch(b){
			case 0:	y_0 = 7;
					break;
			case 1:	y_0 = 51;
					break;
			case 2:	y_0 = 92;
					break;
			default:y_0 = 7;
					break;
		}

		for (int y = 0; y < 32; y++) {
		      for (int x = 0; x < 43; x++) {
		    	  	  healper = (tic_shape[j+1] << 8) + tic_shape[j];

		    	  	  if(healper != 0xFFFF)
		    	  		  healper = shape_color;
		    	  	  else
		    	  		  healper = background_color;

		    	  	  LCDPutPixel(x+x_0, y+y_0, healper);
		    	  	j += 2;
		      }
		 }
}

static void TicSetO(int a, int b){
	TicSetShape(a, b, (uint8_t*)tic_o, RED, WHITE);
}

static void TicSetX(int a, int b){
	TicSetShape(a, b, (uint8_t*)tic_x, BLUE, WHITE);
}

static void TicSetOG(int a, int b){
	TicSetShape(a, b, (uint8_t*)tic_o, GRAY, WHITE);
}

static void TicSetXG(int a, int b){
	TicSetShape(a, b, (uint8_t*)tic_x, BLACK, WHITE);
}

static void TicSetBlanc(int a, int b){
	TicSetShape(a, b, (uint8_t*)tic_x, WHITE, WHITE);
}

static void TicChoosePlayer(int a){
	if(a == 0)
		TicSetContour(player_x, player_y, RED);
	else
		TicSetContour(player_x, player_y, BLUE);
}

static void TicChoosePole(int a, int b){
	struct Pole x1;
	struct Pole x2;

	switch(a){
		case 0:	x1 = x_1;
				break;
		case 1:	x1 = x_2;
				break;
		case 2: x1 = x_3;
				break;
		default:x1 = x_2;
				break;
	}

	switch(b){
			case 0:	x2 = y_1;
					break;
			case 1:	x2 = y_2;
					break;
			case 2:	x2 = y_3;
					break;
			default:x2 = y_2;
					break;
	}

	TicSetContour(x1, x2, MAGENTA);
}

static void TicSetBoard(void){
	for (int y = 0; y < LCD_HEIGHT; y++) {
	      for (int x = 0; x < LCD_WIDTH; x++) {
	    	  if((x > 51 && x < 55) || (x > 106 && x < 110) || (y > 40 && y < 44) || (y > 85 && y < 89))
	    		  LCDPutPixel(x, y, BLACK);
	    	  else
	    		  LCDPutPixel(x, y, WHITE);
	      }
	 }
}

static bool TicWinIf(bool board[3][3]){
	if( (board[0][0] && board[0][1] && board[0][2]) ||
		(board[1][0] && board[1][1] && board[1][2]) ||
		(board[2][0] && board[2][1] && board[2][2]) ||
		(board[0][0] && board[1][0] && board[2][0]) ||
		(board[0][1] && board[1][1] && board[2][1]) ||
		(board[0][2] && board[1][2] && board[2][2]) ||
		(board[0][0] && board[1][1] && board[2][2]) ||
		(board[0][2] && board[1][1] && board[2][0]) ){
				return true;
	}
	else{
		return false;
	}
}

static void TicPrintTrophy(int a_0, int b_0){
	LCDPutRect(a_0, b_0, 46, 60, YELLOW);
	LCDPutRect(a_0-15, b_0+10, 15, 15, YELLOW);
	LCDPutRect(a_0+46, b_0+10, 15, 15, YELLOW);
	LCDPutRect(a_0+16, b_0+60, 14, 30, YELLOW);
	LCDPutRect(a_0+6, b_0+90, 34, 10, YELLOW);
	LCDPutRect(a_0, b_0+100, 46, 10, YELLOW);
}

static void TicReward(bool player){
	uint16_t color;

	if(player)
		color = RED;
	else
		color = BLUE;

	for (int x = 0; x < LCD_WIDTH; x++) {
	  		for (int y = 0; y < LCD_HEIGHT; y++) {
	  			LCDPutPixel(x, y, color);
	  		}
	}

	TicPrintTrophy(57, 10);
}

static void TicReadPosition(){
	ADCReadData(adc_read);

	if(adc_read[0] > ADC_UPPER_LIMITER && pos.a < 2){
		pos.a++;
	}
	else if(adc_read[0] < ADC_BOTTOM_LIMITER  && pos.a > 0){
		pos.a--;
	}

	if(adc_read[1] > ADC_UPPER_LIMITER && pos.b > 0){
		pos.b--;
	}
	else if(adc_read[1] < ADC_BOTTOM_LIMITER  && pos.b < 2){
		pos.b++;
	}

	LL_mDelay(ADC_DELAY);
}

static bool TicCheckFreePole6(struct Shape* figures){
	for(int j = 0; j < shape_size; j++){
		if(figures[j].x == pos.a && figures[j].y == pos.b && figures[j].s != 4){
			button_clicked = false;
			return false;
		}
	}
	return true;
}

static void TicAddPoint6(struct Shape* figures){
	if(button_clicked){
		if(TicCheckFreePole6(figures)){
			if(player == 0){
				figures[i%6].s = 0;
				player++;
			}
			else{
				figures[i%6].s = 1;
				player--;
			}
			figures[i%6].x = pos.a;
			figures[i%6].y = pos.b;

			if(i >= 4){
				if(figures[(i-4)%6].s == 0)
					figures[(i-4)%6].s = 2;
				else if(figures[(i-4)%6].s == 1)
					figures[(i-4)%6].s = 3;

				if(i > 4){
					figures[(i-5)%6].s = 4;
				}
			}

			i++;
			button_clicked = false;
		}
	}
}

static int TicWinCheck6(struct Shape *figures){
	bool board_o[3][3] = {false};

	bool board_x[3][3] = {false};

	int g;

	if(i > 5)
		g = shape_size;
	else
		g = i;

	for(int j = 0; j < g; j++){
		if(figures[j].s != 4){
			if(figures[j].s == 1 || figures[j].s == 3){
				board_x[figures[j].x][figures[j].y] = true;
			}
			else if(figures[j].s == 0 || figures[j].s == 2){
				board_o[figures[j].x][figures[j].y] = true;
			}
		}
	}

	if(TicWinIf(board_o)){
		return 3;
	}
	if(TicWinIf(board_x)){
		return 4;
	}
	else{
		return 1;
	}

}

static void TicRefreshPoints6(struct Shape* figures){
	for(int g = 0; g < shape_size; g++){
		switch(figures[g].s){
			case 0:
				TicSetO(figures[g].x, figures[g].y);
				break;
			case 1:
				TicSetX(figures[g].x, figures[g].y);
				break;
			case 2:
				TicSetOG(figures[g].x, figures[g].y);
				break;
			case 3:
				TicSetXG(figures[g].x, figures[g].y);
				break;
			case 4:
				TicSetBlanc(figures[g].x, figures[g].y);
			default:
				break;
		}
	}
}

static void TicRefreshBoard6(struct Shape* figures){
	TicSetBoard();
	TicRefreshPoints6(figures);
	TicChoosePlayer(player);
	TicChoosePole(pos.a, pos.b);
}

static bool TicCheckFreePole9(struct Shape* figures){
	for(int j = 0; j < i; j++){
		if(figures[j].x == pos.a && figures[j].y == pos.b){
			button_clicked = false;
			return false;
		}
	}
	return true;
}

static void TicAddPoint9(struct Shape* figures){
	if(button_clicked){
		if(TicCheckFreePole9(figures) && i < 9){
			if(player == 0){
				figures[i].s = 0;
				player++;
			}
			else{
				figures[i].s = 1;
				player--;
			}
			figures[i].x = pos.a;
			figures[i].y = pos.b;

			i++;
			button_clicked = false;
		}
		else if(i == 9){
			end_game = 0;
			button_clicked = false;
		}
	}
}

static int TicWinCheck9(struct Shape *figures){
	bool board_o[3][3] = {false};

	bool board_x[3][3] = {false};

	for(int j = 0; j < i; j++){
		if(figures[j].s == 1){
			board_x[figures[j].x][figures[j].y] = true;
		}
		else if(figures[j].s == 0){
			board_o[figures[j].x][figures[j].y] = true;
		}
	}

	if(TicWinIf(board_o)){
		return 3;
	}
	if(TicWinIf(board_x)){
		return 4;
	}
	else{
		return 2;
	}

}

static void TicRefreshPoints9(struct Shape* figures){
	for(int j = 0; j < i; j++){
		if(figures[j].s == 0){
			TicSetO(figures[j].x, figures[j].y);
		}
		else if(figures[j].s == 1){
			TicSetX(figures[j].x, figures[j].y);
		}
	}
}

static void TicRefreshBoard9(struct Shape* figures){
	TicSetBoard();
	TicRefreshPoints9(figures);
	TicChoosePlayer(player);
	TicChoosePole(pos.a, pos.b);
}

static void TicRefreshMenu(){
	pos.b = 0;
	LCDPutColor(WHITE);
	LCDPutRect(0, 62, 160, 4, BLACK);
	TicSetX(0, 0);
	TicSetOG(1, 0);
	TicSetXG(2, 0);
	TicSetO(0, 2);
	TicSetX(1, 2);
	TicSetO(2, 2);
	TicSetContour(menu1_x, menu1_y, MAGENTA);
	LCDCopy();
	LL_mDelay(100);

	while(end_game == 0){
		if(button_clicked && pos.b == 0)
			end_game = 1;
		else if(button_clicked && pos.b == 1)
			end_game = 2;

		ADCReadData(adc_read);

		if(adc_read[1] > ADC_UPPER_LIMITER && pos.b == 1){
			pos.b--;
		}
		else if(adc_read[1] < ADC_BOTTOM_LIMITER  && pos.b == 0){
			pos.b++;
		}

		if(pos.b == 0){
			LCDPutColor(WHITE);
			LCDPutRect(0, 62, 160, 4, BLACK);
			TicSetX(0, 0);
			TicSetOG(1, 0);
			TicSetXG(2, 0);
			TicSetO(0, 2);
			TicSetX(1, 2);
			TicSetO(2, 2);
			TicSetContour(menu1_x, menu1_y, MAGENTA);
		}
		else if(pos.b == 1){
			LCDPutColor(WHITE);
			LCDPutRect(0, 62, 160, 4, BLACK);
			TicSetO(0, 0);
			TicSetXG(1, 0);
			TicSetOG(2, 0);
			TicSetX(0, 2);
			TicSetO(1, 2);
			TicSetX(2, 2);
			TicSetContour(menu2_x, menu2_y, MAGENTA);
		}
		LCDCopy();
		LL_mDelay(ADC_DELAY);
	}

	button_clicked = false;
	pos.a = 1;
	pos.b = 1;
}

void TicInterruptHandler(){
	button_clicked = true;
	while(LL_GPIO_IsInputPinSet(BUTTON_GPIO_Port, BUTTON_Pin) == 0);
	LL_mDelay(25);
	while(LL_GPIO_IsInputPinSet(BUTTON_GPIO_Port, BUTTON_Pin) == 0);
	LL_mDelay(25);
}

void TicMain(){
	if(end_game == 0){
		TicRefreshMenu();
	}

	if(end_game == 1){
		shape_size = 6;
		struct Shape figures[shape_size];
		for(int j = 0; j < shape_size; j++){
			figures[j].x = 8 + j;
			figures[j].y = 3 + j;
			figures[j].s = 5;
		}

		while(end_game == 1){
			if(i >= 4)
				end_game = TicWinCheck6(figures);

			TicReadPosition();
			TicAddPoint6(figures);
			TicRefreshBoard6(figures);
			LCDCopy();
		}
	}
	else if(end_game == 2){
		shape_size = 9;
		struct Shape figures[shape_size];

		while(end_game == 2){
			if(i >= 5)
				end_game = TicWinCheck9(figures);

			TicReadPosition();
			TicAddPoint9(figures);
			TicRefreshBoard9(figures);
			LCDCopy();
		}
	}

	if(end_game == 3){
		TicReward(true);
		LCDCopy();
		while(!button_clicked);
		end_game = 0;
		button_clicked = false;
	}
	else if(end_game == 4){
		TicReward(false);
		LCDCopy();
		while(!button_clicked);
		end_game = 0;
		button_clicked = false;
	}

	i = 0;
	end_game = 0;
	button_clicked = false;
}





