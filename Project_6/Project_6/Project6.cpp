
// Библиотеки
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <conio.h>
#include <ctype.h>
#include <locale.h>

#include "consoleColor.h"
#include "level.h"
#include "weaponType.h"
#include "unitType.h"
#include "unitData.h"

// Константы
const int maxUnitsCount = 35;

// Логические переменные
HANDLE consoleHandle = 0;
bool isGameActive = true;

unsigned char levelData[rowsCount][columnsCount];

UnitData unitsData[maxUnitsCount];
int unitsCount = 0;
int heroIndex = 0;

int enemyHealth = 0;
int defaultHealth = 0;
bool enemyWarning = false;
bool killWarning = false;
const char* killerName;
const char* corpseName;

// Функции
void SetupSystem()
{
	srand(time(0));

	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Спрятать консольный курсор
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = 0;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);

}

void Initialize()
{
	unitsCount = 0;

	// Загрузка уровня
	for (int r = 0; r < rowsCount; r++)
	{
		for (int c = 0; c < columnsCount; c++)
		{
			unsigned char cellSymbol = levelData0[r][c];

			levelData[r][c] = cellSymbol;

			switch (cellSymbol)
			{
				case CellSymbol_Hero:
					heroIndex = unitsCount;
				case CellSymbol_Orc:
				case CellSymbol_Skeleton:
				{
					UnitType unitType = GetUnitTypeFromCell(cellSymbol);
					unitsData[unitsCount].type = unitType;
					unitsData[unitsCount].row = r;
					unitsData[unitsCount].column = c;
					unitsData[unitsCount].weapon = GetUnitDefaultWeapon(unitType);
					unitsData[unitsCount].health = GetUnitDefaultHealth(unitType);
					unitsCount++;

					break;
				}
			}
		}
	}
}

void Render()
{
	// Переместить курсор в (0,0)
	COORD cursorCoord;
	cursorCoord.X = 0;
	cursorCoord.Y = 0;
	SetConsoleCursorPosition(consoleHandle, cursorCoord);

	// Рисуем название игры
	setlocale(LC_ALL, "Russian");
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_Green);
	printf("\n\tВИТАЛЯ ВЫНИМАЙ!");
	setlocale(LC_ALL, "C");

	// Рисуем здоровье героя
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_Red);
	printf("\n\n\tHP: ");
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_White);
	printf("%i ", unitsData[heroIndex].health); // 12+3=15

	// Рисуем здоровье врага
	
	if ((enemyHealth > 0) && (enemyWarning == true))
	{
		SetConsoleTextAttribute(consoleHandle, ConsoleColor_Red);
		printf(" EHP: ");
		SetConsoleTextAttribute(consoleHandle, ConsoleColor_White);
		printf("%i/%i", enemyHealth, defaultHealth); // 6+5=11
		//SetConsoleTextAttribute(consoleHandle, ConsoleColor_Gray);
		enemyWarning = false;
	}
	else printf("\t   ");
	
	// Рисует текущее оружие
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_Cyan);
	printf(" Weapon: ");
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_White);
	const char* currentWeapon = GetWeaponName(unitsData[heroIndex].weapon);
	printf("%s", currentWeapon);
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_Gray);
	int currentHeroDamage = GetWeaponDamage(unitsData[heroIndex].weapon);
	printf(" (Dmg: "); 
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_White);
	printf("%i", currentHeroDamage);
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_Gray);
	printf(")       ");
	

	// Рисуем уровень
	printf("\n\n\t");
	for (int r = 0; r < rowsCount; r++)
	{
		for (int c = 0; c < columnsCount; c++)
		{
			unsigned char cellSymbol = levelData[r][c];

			unsigned char renderCellSymbol = GetRenderCellSymbol(cellSymbol);
			ConsoleColor cellColor = GetRenderCellSymbolColor(cellSymbol);

			SetConsoleTextAttribute(consoleHandle, cellColor);
				
			printf("%c", renderCellSymbol);
		}
		printf("\n\t");
	}

	// Рисуем предупреждения
	if (killWarning == true)
	{
		SetConsoleTextAttribute(consoleHandle, ConsoleColor_Yellow);
		printf("\n\t%s just killed %s", killerName, corpseName); 
		SetConsoleTextAttribute(consoleHandle, ConsoleColor_Gray);
		killWarning = false;
	}
	else
		printf("\n\t\t\t\t\t\t");
}

void MoveUnitTo(UnitData* pointerToUnitData, int row, int column)
{
	// Игнорировать мёртвые юниты
	if (pointerToUnitData->health <= 0)
	{
		return;
	}

	unsigned char unitSymbol = levelData[pointerToUnitData->row][pointerToUnitData->column];
	unsigned char destinationCellSymbol = levelData[row][column];
	bool canMoveToCell = false;

	// Все действия юнитов
	switch(destinationCellSymbol)
	{ 
		// Пустая клетка
		case CellSymbol_Empty:
		{
			canMoveToCell = true;
			break;
		}

		// Клетки юнитов
		case CellSymbol_Hero:
		case CellSymbol_Orc:
		case CellSymbol_Skeleton:
		{
			UnitType destinationUnitType = GetUnitTypeFromCell(destinationCellSymbol);

			// Если на клетке, куда мы движемся, юнит другого типа
			for (int u = 0; u < unitsCount; u++)
			{
				// Игнорировать мёртвых юнитов
				if (unitsData[u].health <= 0)
					continue;
				if (unitsData[u].row == row && unitsData[u].column == column && unitsData[u].type != pointerToUnitData->type)
				{
					
					// Рассчитываем урон от оружия
					int damage = GetWeaponDamage(pointerToUnitData->weapon);

					// Наносим урон
					unitsData[u].health = unitsData[u].health - damage;

					// Передаём отображение здоровья противника
					if (pointerToUnitData->type == UnitType_Hero)
					{
						defaultHealth = GetUnitDefaultHealth(unitsData[u].type);
						enemyWarning = true;
						enemyHealth = unitsData[u].health;
					}

					// Если вражеский юнит умирает
					if (unitsData[u].health <= 0.0f)
					{
						killerName = GetUnitName(pointerToUnitData->type);
						corpseName = GetUnitName(unitsData[u].type);
						levelData[row][column] = CellSymbol_Empty;
						killWarning = true;
					}

					break;
				}
			}
		}

		break;
	}

	// Только действия героя
	if (pointerToUnitData->type == UnitType_Hero)
	{
		switch (destinationCellSymbol)
		{
			// Клетка с оружием
			case CellSymbol_Stick:
			case CellSymbol_Club:
			case CellSymbol_Spear:
			case CellSymbol_Saber:
			{
				canMoveToCell = true;

				WeaponType weaponType = GetWeaponTypeFromCell(destinationCellSymbol);
				if (unitsData[heroIndex].weapon < weaponType)
				{
					unitsData[heroIndex].weapon = weaponType;
				}

				break;
			}

			// Клетка жизней
			case CellSymbol_Life:
			{
				unitsData[heroIndex].health = unitsData[heroIndex].health+100;
				if (unitsData[heroIndex].health > 400)
					unitsData[heroIndex].health = 400;
				levelData[row][column] = CellSymbol_Empty;
				canMoveToCell = true;
				break;
			}
			// Клетка выхода
			case CellSymbol_Exit:
			{
				isGameActive = false;
				break;
			}
		}
	}

	if (canMoveToCell)
	{
		// Удалить символ юнита с предыдущей позиции
		levelData[pointerToUnitData->row][pointerToUnitData->column] = CellSymbol_Empty;
		// Постановить новую позицию юнита
		pointerToUnitData->row = row;
		pointerToUnitData->column = column;
		// Постановить символ юнита на новую позицию
		levelData[pointerToUnitData->row][pointerToUnitData->column] = unitSymbol;
	}
}

void UpdateAI()
{
	// Перебираем всех юнитов
	for (int u = 0; u < unitsCount; u++)
	{
		// Не учитывать героя
		if (u == heroIndex)
			continue;

		// Не учитывать мёртвых юнитов
		if (unitsData[u].health <= 0)
			continue;

		// Расстояние до героя
		int distanceToHeroR = abs(unitsData[heroIndex].row - unitsData[u].row);
		int distanceToHeroC = abs(unitsData[heroIndex].column - unitsData[u].column);

		// Если герой близко
		if ((distanceToHeroR + distanceToHeroC) == 1)
		{
			// Атаковать героя
			MoveUnitTo(&unitsData[u], unitsData[heroIndex].row, unitsData[heroIndex].column);
		}
		else
		{
			// Рандомный шаг
			switch (rand() % 4)
			{
			case 0:
				MoveUnitTo(&unitsData[u], unitsData[u].row - 1, unitsData[u].column);
				break;
			case 1:
				MoveUnitTo(&unitsData[u], unitsData[u].row + 1, unitsData[u].column);
				break;
			case 2:
				MoveUnitTo(&unitsData[u], unitsData[u].row, unitsData[u].column - 1);
				break;
			case 3:
				MoveUnitTo(&unitsData[u], unitsData[u].row, unitsData[u].column + 1);
				break;
			}
		}
	}
}

void Update()
{
	unsigned char inputChar = _getch();
	//inputChar = tolower(inputChar);

	switch (inputChar)
	{
		// Вверх
		case 'w':
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row - 1, unitsData[heroIndex].column);
			break;
		case 72:
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row - 1, unitsData[heroIndex].column);
			break;
		// Вниз
		case 's':
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row + 1, unitsData[heroIndex].column);
			break;
		case 80:
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row + 1, unitsData[heroIndex].column);
			break;
		// Влево
		case 'a':
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row, unitsData[heroIndex].column - 1);
			break;
		case 75:
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row, unitsData[heroIndex].column - 1);
			break;
		// Вправо
		case 'd':
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row, unitsData[heroIndex].column + 1);
			break;
		case 77:
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row, unitsData[heroIndex].column + 1);
			break;
		// Перезапустить уровень
		case 'r':
			Initialize();
			break;
	}

	// Ход существ(AI)
	UpdateAI();

	// Смерть героя
	if (unitsData[heroIndex].health <= 0)
	{
		isGameActive = false;
	}
	else
	{
		// Регенерация здоровья
		if (unitsData[heroIndex].health < GetUnitDefaultHealth(UnitType_Hero))
		{
			unitsData[heroIndex].health++;
		}
	}
}

void Shutdown()
{
	system("cls");
	printf("\n\tGame over...");
	_getch();
}

int main()
{
	SetupSystem();
	Initialize();

	do
	{
		Render();
		Update();
	} 
	while (isGameActive);

	Shutdown();

    return 0;
}

