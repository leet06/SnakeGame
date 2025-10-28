#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <windows.h>
#include <random>
#include <conio.h>

constexpr int WIDTH = 20;
constexpr int HEIGHT = 10;
constexpr int DELAY = 200;
constexpr char EMPTY = ' ';
constexpr char WALL = '#';
constexpr char APPLE = '@';
constexpr char HEAD = 'O';
constexpr char TAIL = 'o';
const std::string MESSAGE_GAME_OVER = "GAME OVER";
const std::string MESSAGE_GAME_WIN = "YOU WIN";

struct Point
{
	int x;
	int y;
};

enum Direction
{
	UP, LEFT, DOWN, RIGHT
};

struct GameState
{
	std::vector<std::vector<char>> Field;
	std::deque<Point> Snake;
	Point Apple;
	Direction Direction;
};

HANDLE init_console()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo = { 0 };
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);
	return hConsole;
}

int GetRandomValue(int minValue, int maxValue)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(minValue, maxValue);
	return dist6(rng);
}

std::deque<Point> GetSnakeRandomPoint(std::deque<Point>& Snake)
{
	int headX = GetRandomValue(2, WIDTH - 1);
	int headY = GetRandomValue(2, HEIGHT - 1);
	Snake.push_back({ headX, headY });
	Snake.push_back({ headX, headY + 1 });
	return Snake;
}

Direction GetSnakeRandomDirection(Direction& Direction)
{
	int randomDirection = GetRandomValue(0, 3);
	switch (randomDirection)
	{
		case 0:
			Direction = UP;
			break;
		case 1:
			Direction = LEFT;
			break;
		case 2:
			Direction = DOWN;
			break;
		case 3:
			Direction = RIGHT;
			break;
	}
	return Direction;
}

Point GetNewApple(std::deque<Point>& Snake)
{
	int appleX;
	int appleY;
	bool isCorrectApple = false;
	while (!isCorrectApple)
	{
		appleX = GetRandomValue(1, WIDTH);
		appleY = GetRandomValue(1, HEIGHT);
		isCorrectApple = true;
		for (int i = 0; i < Snake.size(); i++)
		{
			if (appleX == Snake[i].x && appleY == Snake[i].y)
			{
				isCorrectApple = false;
				break;
			}
		}
	}
	Point Apple = { appleX, appleY };
	return Apple;
}

void InitGameState(GameState& Game)
{
	GetSnakeRandomPoint(Game.Snake);
	GetSnakeRandomDirection(Game.Direction);
	Game.Apple = GetNewApple(Game.Snake);
}

Direction GetDirection(Direction& Direction)
{
	if (_kbhit())
	{
		switch (_getch())
		{
			case 'w':
				if (Direction != DOWN)
					Direction = UP;
				break;
			case 'a':
				if (Direction != RIGHT)
					Direction = LEFT;
				break;
			case 's':
				if (Direction != UP)
					Direction = DOWN;
				break;
			case 'd':
				if (Direction != LEFT)
					Direction = RIGHT;
				break;
		}
	}
	return Direction;
}

GameState GetNewHead(GameState& Game)
{
	Point newHead;
	switch (Game.Direction)
	{
		case UP:
			newHead = { Game.Snake[0].x, Game.Snake[0].y - 1 };
			break;
		case LEFT:
			newHead = { Game.Snake[0].x - 1, Game.Snake[0].y };
			break;
		case DOWN:
			newHead = { Game.Snake[0].x, Game.Snake[0].y + 1 };
			break;
		case RIGHT:
			newHead = { Game.Snake[0].x + 1, Game.Snake[0].y };
			break;
	}
	Game.Snake.push_front(newHead);
	return Game;
}

int ReverseSnakeEdgeCoordinate(int& coord, int maxcoord)
{
	if (coord == 0)
		coord = maxcoord;
	else if (coord == maxcoord + 1)
		coord = 1;
	return coord;
}

std::deque<Point> GetSnakeIfWallCollision(std::deque<Point>& Snake)
{
	ReverseSnakeEdgeCoordinate(Snake[0].x, WIDTH);
	ReverseSnakeEdgeCoordinate(Snake[0].y, HEIGHT);
	return Snake;
}

bool IsSnakeEatApple(GameState& Game)
{
	return Game.Snake[0].x == Game.Apple.x && Game.Snake[0].y == Game.Apple.y;
}

bool IsWin(std::deque<Point>& Snake)
{
	return Snake.size() == WIDTH * HEIGHT;
}

GameState GetNewTail(GameState& Game)
{
	if (IsSnakeEatApple(Game))
	{
		if (!IsWin(Game.Snake))
			Game.Apple = GetNewApple(Game.Snake);
	}
	else
	{
		Game.Snake.pop_back();
	}
	return Game;
}

std::deque<Point> MoveSnake(GameState& Game)
{
	GetNewHead(Game);
	GetSnakeIfWallCollision(Game.Snake);
	GetNewTail(Game);
	return Game.Snake;
}

void CreateNewField(std::vector<std::vector<char>>& Field)
{
	Field = std::vector<std::vector<char>>(HEIGHT + 2, std::vector<char>(WIDTH + 2, EMPTY));
	for (int i = 0; i < WIDTH + 2; i++)
	{
		Field[0][i] = WALL;
		Field[HEIGHT + 1][i] = WALL;
	}
	for (int i = 0; i < HEIGHT + 2; i++)
	{
		Field[i][0] = WALL;
		Field[i][WIDTH + 1] = WALL;
	}
}

std::vector<std::vector<char>> PushObjectsToField(GameState& Game)
{
	Game.Field[Game.Apple.y][Game.Apple.x] = APPLE;
	Game.Field[Game.Snake[0].y][Game.Snake[0].x] = HEAD;
	for (int i = 1; i < Game.Snake.size(); i++)
	{
		Game.Field[Game.Snake[i].y][Game.Snake[i].x] = TAIL;
	}
	return Game.Field;
}

void CreateFrame(GameState& Game)
{
	GetDirection(Game.Direction);
	MoveSnake(Game);
	CreateNewField(Game.Field);
	PushObjectsToField(Game);
}

bool IsCollisionSelf(std::deque<Point>& Snake)
{
	for (int i = 1; i < Snake.size(); i++)
	{
		if (Snake[0].x == Snake[i].x && Snake[0].y == Snake[i].y)
			return true;
	}
	return false;
}

void PrintFrame(std::vector<std::vector<char>>& Field)
{
	for (int y = 0; y < HEIGHT + 2; y++)
	{
		for (int x = 0; x < WIDTH + 2; x++)
		{
			std::cout << Field[y][x];
		}
		std::cout << '\n';
	}
}

void GameCycle(GameState& Game, HANDLE& hConsole)
{
	InitGameState(Game);
	while (1)
	{
		CreateFrame(Game);
		if (IsCollisionSelf(Game.Snake))
		{
			std::cout << MESSAGE_GAME_OVER;
			break;
		}
		SetConsoleCursorPosition(hConsole, { 0, 0 });
		PrintFrame(Game.Field);
		if (IsWin(Game.Snake))
		{
			std::cout << MESSAGE_GAME_WIN;
			break;
		}
		Sleep(DELAY);
	}
	char key = _getch();
}

int main()
{
	HANDLE hConsole = init_console();
	GameState Game = {};
	GameCycle(Game, hConsole);
	return 0;
}