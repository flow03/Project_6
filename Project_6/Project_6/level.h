#pragma once


#include "consoleColor.h"

//Константы
const int rowsCount = 15;
const int columnsCount = 35;

const unsigned char CellSymbol_Empty	= ' ';
const unsigned char CellSymbol_Wall		= '#';
const unsigned char CellSymbol_Hero		= 'h';
const unsigned char CellSymbol_Exit		= 'e';
const unsigned char CellSymbol_Orc		= 'o';
const unsigned char CellSymbol_Skeleton	= 's';
const unsigned char CellSymbol_Stick	= '1';
const unsigned char CellSymbol_Club		= '2';
const unsigned char CellSymbol_Spear	= '3';
const unsigned char CellSymbol_Saber	= '4';
const unsigned char CellSymbol_Life     = 'c';

const unsigned char levelData0[rowsCount][columnsCount + 1] = {

	"###################################",
	"#   #c##      o           s       #",
	"# # o  #o############## ######### #",
	"# ###### #   #c#3  s  # ##s##   # #",
	"#  s       # # ###### # #s4   #s# #",
	"###### # ### #      # # ##s##   # #",
	"##  o# # # # #### # #   ##### # # #",
	"#c #   # # o    # ##c######c#   # #",
	"######## # ####         #  s# # #s#",
	"#  2#    #      ####### # # #   # #",
	"# ### #### ######   o   # #   #   #",
	"#  o       #      ####### #########",
	"############ ######2341 # # s #   #",
	"#h              1 #    1#   #s  # #",
	"#################################e#",


};

unsigned char GetRenderCellSymbol(unsigned char cellSymbol)
{
	switch (cellSymbol)
	{
		case CellSymbol_Empty:		return ' ';
		case CellSymbol_Wall:		return 219;
		case CellSymbol_Hero:		return 2;
		case CellSymbol_Orc:		return 2;
		case CellSymbol_Skeleton:	return 2;
		case CellSymbol_Stick:		return 47;
		case CellSymbol_Club:		return 33;
		case CellSymbol_Spear:		return 24;
		case CellSymbol_Saber:		return 108;
		case CellSymbol_Exit:		return 178;
		case CellSymbol_Life:		return 3;
	}

	return '?';
}

ConsoleColor GetRenderCellSymbolColor(unsigned char cellSymbol)
{
	switch (cellSymbol)
	{
		case CellSymbol_Empty:		return ConsoleColor_Black;
		case CellSymbol_Wall:		return ConsoleColor_Gray;
		case CellSymbol_Hero:		return ConsoleColor_Yellow;
		case CellSymbol_Orc:		return ConsoleColor_Green;
		case CellSymbol_Skeleton:	return ConsoleColor_White;
		case CellSymbol_Stick:		return ConsoleColor_DarkYellow;
		case CellSymbol_Club:		return ConsoleColor_DarkRed;
		case CellSymbol_Spear:		return ConsoleColor_DarkCyan;
		case CellSymbol_Saber:		return ConsoleColor_Cyan;
		case CellSymbol_Exit:		return ConsoleColor_DarkRed;
		case CellSymbol_Life:		return ConsoleColor_Red;
	}

	return ConsoleColor_Gray;
}

