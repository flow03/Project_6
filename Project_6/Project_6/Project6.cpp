
// ����������
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

// ���������
const int maxUnitsCount = 35;

// ���������� ����������
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

// �������
void SetupSystem()
{
	srand(time(0));

	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// �������� ���������� ������
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = 0;
	SetConsoleCursorInfo(consoleHandle, &cursorInfo);

}

void Initialize()
{
	unitsCount = 0;

	// �������� ������
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
	// ����������� ������ � (0,0)
	COORD cursorCoord;
	cursorCoord.X = 0;
	cursorCoord.Y = 0;
	SetConsoleCursorPosition(consoleHandle, cursorCoord);

	// ������ �������� ����
	setlocale(LC_ALL, "Russian");
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_Green);
	printf("\n\t������ �������!");
	setlocale(LC_ALL, "C");

	// ������ �������� �����
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_Red);
	printf("\n\n\tHP: ");
	SetConsoleTextAttribute(consoleHandle, ConsoleColor_White);
	printf("%i ", unitsData[heroIndex].health); // 12+3=15

	// ������ �������� �����
	
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
	
	// ������ ������� ������
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
	

	// ������ �������
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

	// ������ ��������������
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
	// ������������ ������ �����
	if (pointerToUnitData->health <= 0)
	{
		return;
	}

	unsigned char unitSymbol = levelData[pointerToUnitData->row][pointerToUnitData->column];
	unsigned char destinationCellSymbol = levelData[row][column];
	bool canMoveToCell = false;

	// ��� �������� ������
	switch(destinationCellSymbol)
	{ 
		// ������ ������
		case CellSymbol_Empty:
		{
			canMoveToCell = true;
			break;
		}

		// ������ ������
		case CellSymbol_Hero:
		case CellSymbol_Orc:
		case CellSymbol_Skeleton:
		{
			UnitType destinationUnitType = GetUnitTypeFromCell(destinationCellSymbol);

			// ���� �� ������, ���� �� ��������, ���� ������� ����
			for (int u = 0; u < unitsCount; u++)
			{
				// ������������ ������ ������
				if (unitsData[u].health <= 0)
					continue;
				if (unitsData[u].row == row && unitsData[u].column == column && unitsData[u].type != pointerToUnitData->type)
				{
					
					// ������������ ���� �� ������
					int damage = GetWeaponDamage(pointerToUnitData->weapon);

					// ������� ����
					unitsData[u].health = unitsData[u].health - damage;

					// ������� ����������� �������� ����������
					if (pointerToUnitData->type == UnitType_Hero)
					{
						defaultHealth = GetUnitDefaultHealth(unitsData[u].type);
						enemyWarning = true;
						enemyHealth = unitsData[u].health;
					}

					// ���� ��������� ���� �������
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

	// ������ �������� �����
	if (pointerToUnitData->type == UnitType_Hero)
	{
		switch (destinationCellSymbol)
		{
			// ������ � �������
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

			// ������ ������
			case CellSymbol_Life:
			{
				unitsData[heroIndex].health = unitsData[heroIndex].health+100;
				if (unitsData[heroIndex].health > 400)
					unitsData[heroIndex].health = 400;
				levelData[row][column] = CellSymbol_Empty;
				canMoveToCell = true;
				break;
			}
			// ������ ������
			case CellSymbol_Exit:
			{
				isGameActive = false;
				break;
			}
		}
	}

	if (canMoveToCell)
	{
		// ������� ������ ����� � ���������� �������
		levelData[pointerToUnitData->row][pointerToUnitData->column] = CellSymbol_Empty;
		// ����������� ����� ������� �����
		pointerToUnitData->row = row;
		pointerToUnitData->column = column;
		// ����������� ������ ����� �� ����� �������
		levelData[pointerToUnitData->row][pointerToUnitData->column] = unitSymbol;
	}
}

void UpdateAI()
{
	// ���������� ���� ������
	for (int u = 0; u < unitsCount; u++)
	{
		// �� ��������� �����
		if (u == heroIndex)
			continue;

		// �� ��������� ������ ������
		if (unitsData[u].health <= 0)
			continue;

		// ���������� �� �����
		int distanceToHeroR = abs(unitsData[heroIndex].row - unitsData[u].row);
		int distanceToHeroC = abs(unitsData[heroIndex].column - unitsData[u].column);

		// ���� ����� ������
		if ((distanceToHeroR + distanceToHeroC) == 1)
		{
			// ��������� �����
			MoveUnitTo(&unitsData[u], unitsData[heroIndex].row, unitsData[heroIndex].column);
		}
		else
		{
			// ��������� ���
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
		// �����
		case 'w':
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row - 1, unitsData[heroIndex].column);
			break;
		case 72:
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row - 1, unitsData[heroIndex].column);
			break;
		// ����
		case 's':
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row + 1, unitsData[heroIndex].column);
			break;
		case 80:
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row + 1, unitsData[heroIndex].column);
			break;
		// �����
		case 'a':
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row, unitsData[heroIndex].column - 1);
			break;
		case 75:
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row, unitsData[heroIndex].column - 1);
			break;
		// ������
		case 'd':
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row, unitsData[heroIndex].column + 1);
			break;
		case 77:
			MoveUnitTo(&unitsData[heroIndex], unitsData[heroIndex].row, unitsData[heroIndex].column + 1);
			break;
		// ������������� �������
		case 'r':
			Initialize();
			break;
	}

	// ��� �������(AI)
	UpdateAI();

	// ������ �����
	if (unitsData[heroIndex].health <= 0)
	{
		isGameActive = false;
	}
	else
	{
		// ����������� ��������
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

