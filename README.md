![image](https://github.com/fryza97/STM32TicTacToe/assets/157904512/92f0eb89-a96a-4ac8-ad3e-fa8ff4abd71e)
Image 1: connection scheme of the project

The project uses a NUCLEO-L476RG. The MCU reads the state of the joystick (two potentiometers) via a 12-bit ADC. The LCD is an ST7735S and communication takes place through SPI in DMA mode. There are two types of gameplay which the player can choose from the menu after starting the program:
- Classic Tic Tac Toe, in which you play until you win or the board is full [Video 1].
- Tic Tac Toe Bolt, in which the player can place six figures. After that, the figures become gray in the turn before they disappear, and players continue playing until someone wins [Video 2].

| ![TicTacToeClassic](https://github.com/fryza97/STM32TicTacToe/assets/157904512/6e6e083f-ee41-4fa1-8bf4-cc9a48ef0102) | ![TicTacToeBolt](https://github.com/fryza97/STM32TicTacToe/assets/157904512/5a49383e-7056-4b6c-95a5-88c94cd45479) |
|------------|------------|
| Video 1: Tic tac toe classic | Video 2: Tic tac toe bolt |

