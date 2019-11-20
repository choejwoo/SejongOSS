#include "inGame.h"
#include "screen.h"

struct point {
	int x;
	int y;
}; // x,y 좌표 저장

struct window {
	int width;
	int height;
}; // 콘솔창 크기 저장

struct gameInfo {
	int snakeLength;
	int direction;
	int score;
	int speed;
}; // 게임 정보 저장

int getGameSpeed(void)
{
	int speed;
	clrscr();

	do
	{
		gotoxy(10, 5);
		printf("Select The game speed between 1 and 9.");
		speed = waitForAnyKey() - '0'; // ascii코드 사용 입력 숫자로 리턴 
	} while (speed < 1 || speed > 9); // 정상적으로 1~9가 입력이 될때 까지
	return(speed);
} // 게임 스피드를 받는 함수

int checkKeysPressed(int direction)
{
	int pressed;

	if (_kbhit()) // kbhit에서 _kbhit으로 대체됨
	{
		pressed = _getch(); // getch에서 _getch로 대체됨
		if (direction != pressed)
		{
			if (pressed == DOWN_ARROW && direction != UP_ARROW)
				direction = pressed;
			else if (pressed == UP_ARROW && direction != DOWN_ARROW)
				direction = pressed;
			else if (pressed == LEFT_ARROW && direction != RIGHT_ARROW)
				direction = pressed;
			else if (pressed == RIGHT_ARROW && direction != LEFT_ARROW)
				direction = pressed;
			else if (pressed == EXIT_BUTTON || pressed == PAUSE_BUTTON)
				pauseMenu(); // 일시정지
		} // 입력한 키가 유효한지 확인하는 조건문
	}
	return(direction);
} // 입력한 방향키를 확인하는 함수


int collisionSnake(struct point PT, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect)
{
	int i;
	for (i = detect; i < snakeLength; i++)
	{
		if (PT.x == snakeXY[0][i] && PT.y == snakeXY[1][i])
			return(1);
	} // 입력받은 detect인자부터 뱀의 길이만큼의 좌표와 입력한 PT 좌표를 비교
	return(0);
} // 뱀머리PT와 몸이 부딪혔는지, 또는 Food생성시 Food와 접촉했는지 확인하는 함수


int generateFood(int foodXY[], struct window WD, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
	struct point PT;
	do
	{
		srand((unsigned int)(time(NULL)));
		foodXY[0] = rand() % (WD.width - 2) + 2;
		srand((unsigned int)(time(NULL)));
		foodXY[1] = rand() % (WD.height - 6) + 2;
		PT.x = foodXY[0];
		PT.y = foodXY[1];
	} while (collisionSnake(PT, snakeXY, snakeLength, 0)); // 콘솔 내 좌표 랜덤값과 뱀의 위에 Food가 생성되지 않도록 방지

	gotoxy(foodXY[0], foodXY[1]);
	printf("%c", FOOD); // Food 출력

	return(0);
} // Food 생성 함수


void moveSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	int i;
	for (i = snakeLength - 1; i >= 1; i--)
	{
		snakeXY[0][i] = snakeXY[0][i - 1];
		snakeXY[1][i] = snakeXY[1][i - 1];
	} // 뱀배열의 값을 한칸씩 이동시켜주는 반복문

	switch (direction)
	{
	case DOWN_ARROW:
		snakeXY[1][0]++;
		break;
	case RIGHT_ARROW:
		snakeXY[0][0]++;
		break;
	case UP_ARROW:
		snakeXY[1][0]--;
		break;
	case LEFT_ARROW:
		snakeXY[0][0]--;
		break;
	} // 현재 이동방향에 따라 뱀배열 Head좌표 수정

	return;
} // 뱀배열 좌표를 이동시키는 함수

void move(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	int x;
	int y;

	x = snakeXY[0][snakeLength - 1];
	y = snakeXY[1][snakeLength - 1];

	gotoxy(x, y); // 뱀배열 제일 끝 좌표로 이동
	printf("%c", BLANK); // 콘솔창 지우기

	gotoxy(snakeXY[0][0], snakeXY[1][0]); // 뱀배열 Head좌표로 이동
	printf("%c", SNAKE_BODY); // 현재 머리부분에 몸통 출력

	moveSnakeArray(snakeXY, snakeLength, direction); // 뱀배열 값 수정

	gotoxy(snakeXY[0][0], snakeXY[1][0]); // 새로운 뱀 배열 Head좌표로 이동
	printf("%c", SNAKE_HEAD); // Head출력

	gotoxy(1, 1); // 입력창 지우기

	return;
} // 뱀을 이동시키는 함수(새로 출력하는 함수)

int eatFood(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[])
{
	if (snakeXY[0][0] == foodXY[0] && snakeXY[1][0] == foodXY[1])
	{
		foodXY[0] = 0;
		foodXY[1] = 0;

		printf("\7"); // 알림음
		return(1);
	}

	return(0);
} // 뱀머리좌표와 음식의 좌표를 확인하는 함수

int collisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], struct window WD, int snakeLength) //Need to Clean this up a bit
{
	struct point PT;
	PT.x = snakeXY[0][0];
	PT.y = snakeXY[1][0];
	int colision = 0;
	if ((snakeXY[0][0] == 1) || (snakeXY[1][0] == 1) || (snakeXY[0][0] == WD.width) || (snakeXY[1][0] == WD.height - 4))
		colision = 1; // 벽에 부딪혔는지 조건 검사
	else
		if (collisionSnake(PT, snakeXY, snakeLength, 1))
			colision = 1; // 뱀 스스로 부딪혔는지 검사

	return(colision);
} // 게임종료 조건의 충돌을 검사하는 함수

void startGame(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], struct window WD, struct gameInfo GI)
{
	int gameOver = 0;
	clock_t endWait;

	int waitMili = CLOCKS_PER_SEC - (GI.speed) * (CLOCKS_PER_SEC / 10);// 선택한 속도에 따른 진행속도 설정
	int tempScore = 10 * GI.speed; // 난이도 조절용 점수
	int oldDirection = 0; // 기존 방향
	int canChangeDirection = 1; // 방향 바꿀 수 있는지 검사

	endWait = clock() + waitMili; // 현재시각 + 기다릴시간 계산

	do
	{
		if (canChangeDirection)
		{
			oldDirection = GI.direction;
			GI.direction = checkKeysPressed(GI.direction);
		}

		if (clock() >= endWait) // 지정해 놓은 시간을 넘어갔을때
		{
			move(snakeXY, GI.snakeLength, GI.direction); // move함수 호출
			canChangeDirection = 1;

			if (eatFood(snakeXY, foodXY)) // 음식을 먹었을 때
			{
				generateFood(foodXY, WD , snakeXY, GI.snakeLength); // 음식을 새로 하나 생성
				GI.snakeLength++; // 뱀 길이 증가
				GI.score += GI.speed; // 난이도에 따른 점수 증가

				if (GI.score >= 10 * GI.speed + tempScore) // 점수가 일정이상 증가하면
				{
					GI.speed++; // 속도증가
					tempScore = GI.score; // 점수 임시 저장

					if (GI.speed <= 9) // 9레벨 이하일때 증가폭
						waitMili = waitMili - (CLOCKS_PER_SEC / 10);
					else
					{
						if (waitMili >= 40) // 너무 높으면 조정(MAX speed 제한)
							waitMili = waitMili - (CLOCKS_PER_SEC / 200);

					}
				}

				refreshInfoBar(GI.score, GI.speed); // 정보 최신화
			}

			endWait = clock() + waitMili; // 새로운 시간 설정
		}

		gameOver = collisionDetection(snakeXY, WD , GI.snakeLength); // 게임 종료 검사

		if (GI.snakeLength >= SNAKE_ARRAY_SIZE - 5) // 게임 승리 조건
		{
			gameOver = 2; // 승리 저장
			GI.score += 1500; // 승리시 추가 포인트
		}

	} while (!gameOver); // 조건에 검사되기 전까지 반복

	switch (gameOver) // 종료 case에 따라 결과 표시
	{
	case 1:
		printf("\7"); // 알람
		printf("\7"); // 알람

		gameOverScreen(); // 패배화면

		break;
	case 2:
		youWinScreen(); // 승리화면
		break;
	}

	if (GI.score >= getLowestScore() && GI.score != 0)
	{
		inputScore(GI.score);
		displayHighScores();
	} // 스코어보드에 올라갈 점수라면 갱신

	return;
} // 게임함수



void loadSnake(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
	int i;
	for (i = 0; i < snakeLength; i++)
	{
		gotoxy(snakeXY[0][i], snakeXY[1][i]);
		printf("%c", SNAKE_BODY); // 몸통 출력
	}

	return;
} // 가장 처음 뱀 그리는 함수


void prepairSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
	int i;
	int snakeX = snakeXY[0][0];
	int snakeY = snakeXY[1][0]; // 첫 좌표 저장

	for (i = 1; i <= snakeLength; i++)
	{
		snakeXY[0][i] = snakeX + i;
		snakeXY[1][i] = snakeY;
	} // 배열에 좌표 저장

	return;
} // 가장 처음 뱀 배열에 좌표 저장하는 함수


void loadGame(void)
{
	struct gameInfo GI;
	int snakeXY[2][SNAKE_ARRAY_SIZE]; // 뱀 배열

	GI.snakeLength= 4; // 시작 길이

	GI.direction = LEFT_ARROW; // 처음설정 방향

	int foodXY[2];// 음식 위치 저장할 배열

	GI.score= 0; // 시작 점수

	struct window WD; // 창 크기 저장
	WD.width = 80;
	WD.height = 25;
	GI.speed = getGameSpeed(); // speed 입력받는 함수
	snakeXY[0][0] = 40; // 시작위치
	snakeXY[1][0] = 10;

	loadEnvironment(WD); // 배경설정 불러오기
	prepairSnakeArray(snakeXY, GI.snakeLength); // 뱀배열에 값넣기
	loadSnake(snakeXY, GI.snakeLength); // 뱀배열 그리기
	generateFood(foodXY, WD , snakeXY, GI.snakeLength); // 음식생성
	refreshInfoBar(GI.score, GI.speed); // 하단에 정보 초기화
	startGame(snakeXY, foodXY, WD, GI); // 게임 시작함수 호출

	return;
}


void loadEnvironment(struct window WD)
{
	int x = 1, y = 1;
	int rectangleHeight = WD.height - 4;

	clrscr(); // 화면 초기화

	gotoxy(x, y); // 화면 좌측 상단으로 이동

	for (; y < rectangleHeight; y++)
	{
		gotoxy(x, y); // 좌측 벽
		printf("%c", WALL);

		gotoxy(WD.width, y); // 우측 벽

		printf("%c", WALL);
	}

	y = 1;

	for (; x < WD.width + 1; x++)
	{
		gotoxy(x, y); // 상단 벽
		printf("%c", WALL);

		gotoxy(x, rectangleHeight); // 하단 벽
		printf("%c", WALL);
	}
	return;
} // 배경 불러오는 함수

