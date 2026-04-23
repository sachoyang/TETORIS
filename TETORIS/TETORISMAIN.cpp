// 리소스를 사용하기 위해서 resource.h 파일을 include해 주어야 한다.
#include "resource.h"
// API 함수들을 사용하려면 windows.h 파일을 include해 주어야 한다.
#include <windows.h>
// C 언어에서 자주 사용하였던 표준 입출력 헤더 추가
#include <stdio.h>
// fmod sound 헤더 추가
#include "FmodSound.h"
// c++ 용 헤더 추가
#include <iostream>
using namespace std;

// 각종 전처리기 매크로 선언
// 새로운 블록을 출력
/*
	윈도우 시스템은, 메시지구동 시스템이다.
	모든 이벤트는 메세지의 발생에 의해서 처리된다.

	리스트박스 윈도우, 콤보박스윈도우등 모든 이런 각각의 윈도우들은 내부적
	메시지처리에의해 원하는 동작을 하게 되는데, 이런 메시지외에 사용자
	정의 메시지가 필요할때가 많다. 예를들어, 콤보에 값이 변경될때, 부모에게 뿐
	아니라, 다른곳에도 알려줄 필요가 있거나 할때...

	그러면, 다음과 같이 메시지의 값을 아무렇게나 정의해서 사용할순 없다.

	#define WM_NewBlock 100
	처럼, 이런식으로 선언한다면, 이미 100이라는 값은 시스템이 내부적으로
	예약해서 사용하는 메시지 상수일 수가 있다.

	그래서, 사용자가 사용해도 되는 구역을 만들어뒀는데 그게
	0x400 값부터 , 0x7FFF 까지 이다. 여기서 0x400 이 WM_USER 로 선언되있음

	참고로, ---------------------------------------------
	0 - 0x400 : 시스템 예약
	0x401 - 0x7FFF : WM_USER메세지용으로 사용하게 만든 구역
	0x8000 - 0xBFFF : 애플리케이션 전체 메세지 호환 구역
	기타...
	-----------------------------------------------------

	우선 다른 구역까지 생각하면 머리아프니까 우선은 이렇다는것만 알아두자.

	그리고, WM_USER+1에서 1을 더하는것은 WM_USER가 0x400이니까 이 값까지가
	시스템이 사용하니까, 1을 더해줘서 0x401 - 0x7FFF까지가 맘대로 사용해도 되는
	거다.

	(정리) 프로그래머의 필요에 따라 스스로 정의해서 사용할 수 있는 메시지이다.
	WM_USER 부터는 윈도우에서 사용하는 구간이 아니므로 필요한 갯수만큼 +n 해가면서
	여러개의 메시지를 정의해서 쓸 수 있다. 물론 윈도우가 정의한 메시지가 아니고,
	프로그래머가 정의한 메시지이므로, 그 메시지를 발생시키는것도 그 메시지를 처리하는것도
	모두 프로그래머의 책임.
*/
#define WM_NewBlock WM_USER + 1

#define YES 1
#define NO 0
#define SUCCESS 1
#define FAIL 0
#define ON 1
#define OFF 0
#define ALIVE 1
#define DEAD 0
#define BLOCKSIZE 20

#define HEX2RGB(c) RGB((c>>16)&0xFF, (c>>8)&0xFF, c&0xFF)
// 블록 색깔
COLORREF BlockColors[7] = {
	HEX2RGB(0x8B0000), // 0: 어두운 빨강 (Dark Red)
	HEX2RGB(0xB8860B), // 1: 어두운 황금색 (Dark Goldenrod)
	HEX2RGB(0x9B870C), // 2: 어두운 노랑 (Dark Yellow)
	HEX2RGB(0x006400), // 3: 어두운 초록 (Dark Green)
	HEX2RGB(0x008B8B), // 4: 어두운 청록 (Dark Cyan)
	HEX2RGB(0x00008B), // 5: 어두운 파랑 (Dark Blue)
	HEX2RGB(0x4B0082)  // 6: 인디고 (Indigo)
};

COLORREF PenColor = 0xF8F8CB;

// 테트릭스 블록을 위한 4차원 배열
// 이 배열에는 모든 블록의 4방향 모양이 저장되어 있다.
/*
	블록의 모든 회전 모양을 출력하기 위해서 4차원 배열을 사용하였다. 여기에서는 첫 번재 7는 블록의 종류,
	두 번째 4는 블록의 회전을 나타내는 것이라고 생각하자.
	모든 블록 모양의 4가지(방향) 모양을 출력하기 위해서 배열에 값을 더 추가하였다. 아래의 배열의 값이 혼라스러우면
	아래와 같이 각각 4줄씩 끊어서 보면 블록의 모양을 알 수 있다.
*/

// 전체 블록 모양
int Block[7][4][4][4] = { 0, 1, 0, 0,
						 0, 1, 0, 0,
						 0, 1, 0, 0,
						 0, 1, 0, 0,

						 0, 0, 0, 0,
						 1, 1, 1, 1,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 0, 1, 0, 0,
						 0, 1, 0, 0,
						 0, 1, 0, 0,

						 0, 0, 0, 0,
						 1, 1, 1, 1,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 0, 1, 0,
						 0, 0, 1, 0,
						 0, 1, 1, 0,
						 0, 0, 0, 0,

						 1, 1, 1, 0,
						 0, 0, 1, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 1, 1, 0, 0,
						 1, 0, 0, 0,
						 1, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 0, 0, 0,
						 1, 0, 0, 0,
						 1, 1, 1, 0,
						 0, 0, 0, 0,

						 1, 0, 0, 0,
						 1, 0, 0, 0,
						 1, 1, 0, 0,
						 0, 0, 0, 0,

						 0, 0, 0, 0,
						 0, 0, 1, 0,
						 1, 1, 1, 0,
						 0, 0, 0, 0,

						 0, 1, 1, 0,
						 0, 0, 1, 0,
						 0, 0, 1, 0,
						 0, 0, 0, 0,

						 1, 1, 1, 0,
						 1, 0, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 0, 0, 0,
						 0, 1, 0, 0,
						 1, 1, 1, 0,
						 0, 0, 0, 0,

						 0, 0, 1, 0,
						 0, 1, 1, 0,
						 0, 0, 1, 0,
						 0, 0, 0, 0,

						 1, 1, 1, 0,
						 0, 1, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 1, 0, 0, 0,
						 1, 1, 0, 0,
						 1, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 0, 1, 1, 0,
						 0, 0, 1, 0,
						 0, 0, 0, 0,

						 0, 1, 1, 0,
						 1, 1, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 0, 1, 1, 0,
						 0, 0, 1, 0,
						 0, 0, 0, 0,

						 0, 1, 1, 0,
						 1, 1, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 1, 1, 0, 0,
						 1, 0, 0, 0,
						 0, 0, 0, 0,

						 1, 1, 0, 0,
						 0, 1, 1, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 1, 1, 0, 0,
						 1, 0, 0, 0,
						 0, 0, 0, 0,

						 1, 1, 0, 0,
						 0, 1, 1, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 1, 1, 0, 0,
						 1, 1, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 1, 1, 0, 0,
						 1, 1, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 1, 1, 0, 0,
						 1, 1, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,

						 1, 1, 0, 0,
						 1, 1, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0 };

// 배경의 배열
int BackGround[21][12];

int Level;
int second = 0;
int saveBlockNum;

// 타임 OFF
BOOL bTime = OFF;
//저장되어있음?
BOOL isSave = FALSE;
BOOL isUsed = FALSE;
// 블록의 수(종류)
int BlockNum;
// 블록의 회전
int RotateNum;
// 현재의 블록 좌표
int NowX, NowY;
// 다음 블록의 수(종류)
int NextBlockNum;
// 블록의 한 줄이 다 채워졌을 경우 (점수)
int FullLineNum;
// 게임을 진행할지 결정
int PlayerState;
// 장애물 개수
int obstacleNum = 0;
// 윈도우의 핸들이 저장될 변수를 만든다. (사용자 정의 함수를 위함)
HWND g_hWnd;

// ==========리소스=============
HBITMAP hBmpTetoris, hBmpDanceTeto, hBmpGameOver;

int TETORISBGSound = AddSoundFile("./resourse/Sound/테토리스-카사네 테토.mp3", true);
int EffectSoundPlus = AddSoundFile("./resourse/Sound/쪼아요쪼아요.mp3");
int EffectSoundDie = AddSoundFile("./resourse/Sound/으아앙스피키.mp3");

// =============================

// 배경의 배열 초기화 하는 함수
void InitBackGround(void);

// 배경을 그리는 함수
void DrawBackGround(void);

// 블록을 그리는 함수
void DrawBlock(void);

// 블록을 지우는 함수
void EraseBlock(void);

// 다음 블록을 그리는 함수
void DrawNextBlock(void);

// 블록이 움직일 수 있는지를 체크하는 함수
BOOL BlockCanMove(int x, int y);

// 블록을 왼쪽으로 움직이는 함수
void LeftMove(void);

// 블록을 오른쪽으로 움직이는 함수
void RightMove(void);

// 블록을 회전하는 함수
void Rotate(void);

// 블록을 아래로 움직이는 함수
BOOL DownMove(void);

// 배경의 배열을 업데이트하는 함수
void UpdateBackGround(void);

// 블록의 한 줄이 꽉 찼는지를 체크
void CheckFullLine(void);

// 블록의 한 줄이 꽉 차면 지우는 함수
void EraseFullLine(int);

// 첫시작 시 그림 보여주기
void ShowTeto(HDC hDC);

// 4면 테토 1초마다 돌아가기
void DrawDancingTeto(int frame);

void UpObstacle();

// 블록 홀드하는 기능
void SaveBlock();

void DrawSaveBlock();

void UseBlock();

void PrintInfo(HDC hDC);

void DrawGameOver(HDC hDC);

void DrawGhostBlock();

void EraseGhostBlock();

/*
(1) C declaration(__cdecl) : __cdecl 로 함수가 규약이 되어 있으면,
	함수를 호출한 쪽에서 parameter 값들을 함수가 return 되고 난후
	stack 에서 제거함.따라서 호출한 함수가 자신의 parameter 를 알고 있기 때문에
	printf 와 같이 parameter 가 가변적인 함수에서 사용가능.

	__cdecl은 호출자에 의해 가변인자로 받은 스텍 메모리를 해제하는 식이며,
	현재 함수호출규약의 기본형(디폴트)로 돼 있다. 물론, 이 기본형을 [프로젝트] -> [속성] -> [C/C++] -> [고급] -> [호출 규칙] 에서
	수정할 수는 있다. 여기서! 호출자(Caller)라는 꽤나 말이 어색할 수 있는데 호출자는 함수를 부르는 함수이다.
	예를 들면, main()함수에서 A()라는 함수를 실행했다면, A()의 호출자는 main()함수가 되는 것이다.
	더불어 이 상황에서 A()의 함수는 main()으로부터 호출 수신자(Callee)가 되는 것이다.
	스택 메모리를 호출자가 직접 해제를 하다보니 실행파일이 커진다는 설명이 있는데,
	이는 해제하는 코드를 가변 인자 코드를 가진 함수 하나하나 마다 메모리를 해제하는 코드를 넣어야하기 때문이라 설명하고 있다.


(2) standard call(__stdcall) : 이 규약은 함수를 호출 받은 쪽(즉 함수의 내부)에서
	parameter 를 제거하도록 되어 있다.따라서 parameter 의 갯수가 명확한 곳에서
	사용하게 되어 있다.윈도우즈의 API 와 같은 경우 함수에 따른 인자들이 명확히
	정의 되어 있기 때문에 바로 __stdcall 규약을 사용하고 있다.

	WINAPI로 define까지 해놓은 코드이며, 그 특징은 호출자에서 인자 스텍을 넣는 행위만 할뿐 삭제해주지 않는다는 것이다.
	따라서 해당 스텍은 함수 내부에서 직접 삭제를 해야하며, 만약 가변인자로 인자가 들어갈 경우 정확한 인자의 크기를 함수 내에서 알 수 없어
	함수 실행 전으로 회귀하지 못한다. 즉, __stdcall은 가변인자를 사용할 수 없다.

	(참고)stdcall 호출 방식이 cdecl 호출 방식보다 빠르고, 프로그램 용량도 적어짐.
		  stdcall 호출 형식은 호출한 함수 내부에서 파라미터 크기만큼 pop을 해 주기 때문에 가변인자를 사용하는 것이 불가능

*/

// 메시지의 처리를 위해 콜 백 함수를 선언한다.
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 프로그램의 시작점을 지정한다.
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	// 메시지 구조체를 선언한다.
	MSG mSg;

	// 윈도우의 캡션을 지정한다.
	WCHAR szTitle[] = L"테트리스";
	// 윈도우의 클래스를 지정한다.
	WCHAR szClass[] = L"Class";


	// 추가적인 아이콘 셋팅 법/////////////////////////////////////
	HICON hIcon1;   // icon handle 
	HICON hIcon2;   // icon handle 

	// 기본 아이콘 생성
	hIcon1 = LoadIcon(NULL, IDI_QUESTION);

	// 사용자 정의 아이콘 생성 
	hIcon2 = LoadIcon(hInstance, MAKEINTRESOURCE(101));
	///////////////////////////////////////////////////////////////

	// WNDCLASSEX 구조체를 선언한다. 참고로 창은 하나의 윈도우 클래스를 기반으로 생성된다.
	WNDCLASSEX WndEx;

	// WNDCLASSEX 구조체의 크기를 지정한다.
	WndEx.cbSize = sizeof(WndEx);

	// 윈도우의 스타일을 지정한다.
	WndEx.style = NULL;

	// 메시지를 처리할 함수를 지정한다. 즉 메시지가 발생하면 그 메시지를 WndProc 함수에서 처리한다.
	WndEx.lpfnWndProc = WndProc;

	// 추가 클래스를 지정한다. 추가 클래스가 없기 때문에 NULL 값으로 지정한다.
	WndEx.cbClsExtra = 0;

	// 추가 윈도우를 지정한다. 추가 윈도우가 없기 때문에 NULL 값으로 지정한다.
	WndEx.cbWndExtra = 0;

	// 인스턴스 핸들을 지정한다.
	WndEx.hInstance = hInstance;

	// 실행 파일에 쓰일 아이콘을 지정한다. LoadIcon() 함수는 리소스에서 아이콘을 불러와 반환한다.
	WndEx.hIcon = LoadIcon(NULL, L"IDI_ICON");

	// 윈도우에서 쓰일 커서를 지정한다. LoadCursor() 함수는 미리 예약된 커서 값을 반환한다.
	WndEx.hCursor = LoadCursor(NULL, IDC_ARROW);

	// 윈도우의 배경색을 지정한다. GetStockObject() 함수는 검정색을 반환한다.
	WndEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	// 윈도우에 사용될 메뉴를 지정한다. 메뉴가 없기 때문에 NULL 값으로 지정하였다.
	WndEx.lpszMenuName = /*L"IDR_MENU"*/MAKEINTRESOURCE(IDR_MENU);

	// 클래스를 지정한다. 참고로 이 클래스는 CreateWindowEx() 함수에서 지정될 클래스와 동일해야 한다.
	WndEx.lpszClassName = szClass;

	// 윈도우의 캡션이 쓰일 아이콘을 지정한다. LoadIcon() 함수는 리소스에서 아이콘을 불러와 반환한다.
	WndEx.hIconSm = LoadIcon(hInstance, /*L"IDI_ICON"*/MAKEINTRESOURCE(IDI_ICON));
	//WndEx.hIconSm = hIcon2;

	/*
		리소스란 -> 리소스란 그래픽 요소를 활용하는데 초점이 맞춰진 자원이다.
		리소스 등록 -> 프로젝트- 리소스 파일 마우스 오른쪽 클릭- 추가- 리소스
		리소스 ID란 -> 리소스에는 각각의 ID가 할당되며, 이것을 식별자로 이용한다. 리소스 뷰로 리소스를 한 눈에 볼 수 있다.
		리소스 ID 확인하기 -> 리소스 뷰에서 한눈에 보고 관리할 수 있다.

		아이콘, 커서 제작
		리소스 등록을 통해 아이콘, 커서를 제작하면 직접 그림을 그려서 제작할 수도 있고, 가져오기를 통해 기존 이미지를 불러올 수도 있다.
		커서는 특별히 핫스폿 설정을 통해, 입력받을 커서의 정확한 위치를 설정할 수도 있다. 그리기 도구에 있습니다.


		cf)	리소스 자원 활용을 위한 세팅
			resource 헤더 파일을 인클루드 합니다.
			#include "Resource.h“



		MAKEINTRESOURCE 는 매크로 함수로서 정수를 문자열 변수로 만들어주는 기능을 한다.
		윈도우즈 API 의 리소스 관리 함수들은 그 전달 인자로 문자열을 받는다.
		그러나 리소스 번호는 WORD 즉 정수형이므로 이것을 변환한다.
		아이디는 resource.h 파일에 각각 정수형으로 define 되어 있다.
		소스 파일에서 리소스 아이디를 인식하지 못할 때는 헤더파일에 올바르게 define되어 있는지 확인합니다.

		WndEx.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(커서 아이디));
		WndEx.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(아이콘 아이디));
	*/

	// WNDCLASSEX에 지정된 값들을 레지스터에 등록한다.
	RegisterClassEx(&WndEx);

	hBmpTetoris = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_TETORIS));
	hBmpDanceTeto = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_DANCE));
	hBmpGameOver = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_GAMEOVER));

	// 윈도우를 생성한다.
	g_hWnd = CreateWindowEx(NULL,				// 윈도우의 스타일을 지정한다.
		szClass,					// 윈도우의 클래스를 지정한다.
		szTitle,					// 윈도우의 캡션을 지정한다.
		WS_OVERLAPPEDWINDOW,		// 윈도우의 스타일을 지정한다.
		0,							// 윈도우의 x좌표 위치를 지정한다.
		0,							// 윈도우의 y좌표 위치를 지정한다.
		675,						// 윈도우의 넓이를 320으로 지정하였다.
		750,						// 윈도우의 높이를 300으로 지정하였다.
		NULL,						// 윈도우를 포함하고 있는 다른 윈도우가 없으므로 NULL 값으로 지정하였다.
		NULL,						// 메뉴가 없으므로 NULL 값으로 지정하였다.
		hInstance,					// 인스턴스 핸들을 지정한다.
		NULL);						// 잘 쓰이지 않는다.

	// 윈도우가 실제로 보여질 형태를 지정한다.
	ShowWindow(g_hWnd, nShowCmd);
	// 윈도우를 갱신하고 WM_PAINT 메시지를 호출한다.
	UpdateWindow(g_hWnd);

	// 메시지 처리를 위한 반복문 시작
	while (TRUE)
	{
		// 메시지 큐에 들어온 메시지가 있는지 확인한다.
		if (PeekMessage(&mSg, NULL, 0, 0, PM_NOREMOVE))
		{
			// 만약 메시지가 있으면 MSG 구조체에서 메시지를 읽어온다.
			if (!GetMessage(&mSg, NULL, 0, 0))
				break;	// 메시지 중에 WM_QUIT메시지가 있으면 빠져나간다.
			// 가상키가 발생하면 그 메시지를 받아와서 WM_CHAR에 해당하는 코드를 응용 프로그램의 메시지 큐에 넣어 둔다.
			TranslateMessage(&mSg);
			// 그 메시지들을 처리하기 위해 WndProc() 함수로 보낸다.
			DispatchMessage(&mSg);
		}
	}

	// 기타 부수적인 메시지를 반환한다.
	return mSg.wParam;
}

// 메시지를 처리하기 위해서 메시지 처리 함수를 정의한다.
LRESULT CALLBACK WndProc(HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	static bool isStarted = FALSE;
	static int frameIndex = 0; // 0:정면, 1:우측, 2:뒤, 3:좌측
	// uMsg 변수에는 DispatchMessage() 함수에서 보낸 메시지들이 저장되어 있다.
	switch (uMsg)
	{
	case WM_PAINT:
	{
		// 처음 화면 출력
		PAINTSTRUCT pS;
		HDC hDC = BeginPaint(hWnd, &pS);

		if (isStarted == FALSE)
		{
			ShowTeto(hDC);
		}
		else if ((isStarted)&&(PlayerState==DEAD))
		{
			BGStop();
			DrawGameOver(hDC);
			EffectPlay(EffectSoundDie);
		}
		else
		{
			DrawDancingTeto(frameIndex);
			PrintInfo(hDC);
		}

		EndPaint(hWnd, &pS);
		return FALSE;
	}
		// 윈도우의 메뉴가 클릭 되었을때 전달되는 메시지.
	case WM_COMMAND:
		// 전달된 데이타를 하위 워드로 잘라서 ...
		switch (LOWORD(wParam))
		{
			// 메뉴의 [File] -> [Start]가 클릭되면...(단축키 alt + F + S 순서대로...)
		case FILE_START:
			if (isStarted==FALSE)
			{
				isStarted = TRUE;
				InvalidateRect(g_hWnd, NULL, TRUE);
			}
			BGPlay(TETORISBGSound);
			if (PlayerState == DEAD)
			{
				PlayerState = ALIVE;
				// 배경 블록의 모양을 초기화한다.
				InitBackGround();
				// 이 함수를 이용하여 1이 들어간 곳에 사각형을 출력한다. 즉 경계 줄을 그린다.
				DrawBackGround();
				// 플레이어 상태를 초기화 한다.(ALIVE)
				// 다음 블록의 값을 셋팅한다.
				NextBlockNum = rand() % 7;
				// 블록의 한 줄이 다 채워졌을 경우 (점수)
				FullLineNum = 0;
				RECT scoreRect = { 500, 0, 700, 30 };
				InvalidateRect(hWnd, &scoreRect, TRUE); // TRUE로 해야 잔상 없이 깨끗하게 지워짐

				// 장애물 초기화
				obstacleNum = 0;
				isSave = FALSE;
				isUsed = FALSE;
				Level = 0;
				second = 0;
				
				// 메시지를 발생 시킨다.
				SendMessage(hWnd, WM_NewBlock, 0, 0);
			}

			// 타임이 ON 이면
			if (bTime == ON)
			{
				// 해당 아이디에 타이머를 제거(정지)한다.
				KillTimer(hWnd, 3);
				bTime = OFF;
			}
			else
			{
				// 타이머를 설치...즉 해당 아이디로 타이머를 출력한다.(생성한다.)
				// 타이머의 아이디는 wParam에 저장된다.(메시지 발생시...)
				SetTimer(hWnd, 3, 1000, NULL); // 1초마다 WM_TIMER 메시지 발생.(?)
				// 타임을 ON으로 셋팅
				bTime = ON;
			}
			break;
			// 메뉴의 [File] -> [EXIT]이 클릭되면...(단축키 alt + F + E 순서대로...)
		case FILE_EXIT:
			// 윈도우를 종료(파괴)한다.
			DestroyWindow(hWnd);
			break;
		}
		return FALSE;

		/* 이 메시지는 새로운 블록이 들어올 때 발생한다.
		   여기서는 새로운 블록을 입구에 그려주고
		   다음 블록 또한 윈도우의 오른쪽에 그려준다.*/
	case WM_NewBlock:
		// 블럭의 현재 x좌표
		NowX = 3;
		// 블럭의 현재 y좌표
		NowY = 0;
		// 블록의 회전 종류 (기본값 시작)
		RotateNum = 0;
		// 이번에 나올 블록
		BlockNum = NextBlockNum;
		// 다음에 나올 블록
		NextBlockNum = rand() % 7;

		// 새 블록을 입구에 그린다.
		DrawGhostBlock();
		DrawBlock();
		// 다음에 나올 블록을 그린다.
		DrawNextBlock();
		DrawSaveBlock();

		/*
			새 블록이 나올 수 없으면
			즉! 블록이 입구까지 가득 차 있으면...
		*/
		if (!BlockCanMove(NowX, NowY))
		{
			// 게임을 종료한다.
			PlayerState = DEAD;
			InvalidateRect(hWnd, NULL, TRUE);
		}
			

		return FALSE;

		// 키보드의 키가 클릭되면
	case WM_KEYDOWN:
		if ((isStarted == FALSE))
		{
			return FALSE;
		}
		switch (LOWORD(wParam))
		{
			// 방향키 LEFT
		case VK_LEFT:
			// 블록 좌측 이동
			LeftMove();
			break;
			// 방향키 RIGHT
		case VK_RIGHT:
			// 블록 우측 이동
			RightMove();
			break;
			// 방향키 UP
		case VK_UP:
			// 블록 회전
			Rotate();
			break;
			// 방향키 DOWN
		case VK_DOWN:
			// 블록 내리기
			DownMove();
			break;
			// SPACE키
		case VK_SPACE:
			// 블록 한번에 내리기
			while (DownMove());
			break;
		case VK_RETURN:
		{
			if (PlayerState == DEAD)
			{
				break;
			}
			if (isSave && !isUsed)
			{
				isUsed = TRUE;
				EraseBlock();
				EraseGhostBlock();
				UseBlock();
				SendMessage(hWnd, WM_NewBlock, 0, 0);
				DrawSaveBlock();
				break;
			}
			else if(!isSave)
			{
				EraseBlock();
				EraseGhostBlock();
				SaveBlock();
				SendMessage(hWnd, WM_NewBlock, 0, 0);
				break;
			}
			else
			{
				DrawSaveBlock();
				break;
			}
		}
			
		}

		return FALSE;

		// WM_TIMER 메시지가 발생하면 (일정 시간 간격으로 메시지가 전달...(1000은 1초다))
	case WM_TIMER:
	{
		// 플레이어가 살아있을때 
		if (PlayerState == ALIVE)
		{
			// 블록을 아래도 계속 내린다.
			DownMove();
			second++;
			if (second % (20-Level) == 0)
			{
				second = 0;
				UpObstacle();
			}

			frameIndex = (frameIndex + 1) % 4;
			RECT charRect = { 120, 500, 120 + 225, 500 + 225 };
			InvalidateRect(hWnd, &charRect, TRUE);

			RECT uiRect = { 350, 380, 450, 480 };
			InvalidateRect(hWnd, &uiRect, TRUE);
		}	
		else
		{
			// 타임 변수가 ON 일때
			if (bTime == ON)
			{
				// 다음 시작을 위해 리셋
				bTime = OFF;
				// 해당 아이디에 타이머를 제거(정지)한다.
				KillTimer(hWnd, 3);
			}
		}
		
		return FALSE;
	}
		// uMsg 변수에 WM_DESTROY메시지가 있으면
	case WM_DESTROY:
		// 종료 전 타임 ON 일때
		if (bTime == ON)
			// 해당 아이디에 타이머를 제거한다.
			KillTimer(hWnd, 3);

		// 윈도우를 종료한다.
		PostQuitMessage(0);

		return FALSE;
	}
	// 미처 처리하지 못한 메시지들을 WndProc함수로 보내어 확실히 처리되어지도록 한다.
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 아래의 함수는 사용자 정의 함수로서 배경 블록의 모양을 초기화한다.
// 이 함수는 함수의 블록이 움직이는 공간을 만들어 주는 것으로 매우 중요하다.
void InitBackGround()
{
	// 재 실행을 위한 초기화를 한다./////////////////////////////////
	InvalidateRect(g_hWnd, NULL, TRUE);
	// UpdateWindow 안하면 배경이 안그려짐 테스트 해보자~이유 알기~
	UpdateWindow(g_hWnd);

	// 배경 0으로 초기화
	for (int row = 0; row < 21; row++)
		for (int col = 0; col < 12; col++)
		{
			BackGround[row][col] = 0;
		}
	///////////////////////////////////////////////////////////////

	// 아래 두 반복문은 배경 블록의 값을 셋팅한다.
	for (int row = 0; row < 21; row++)
		for (int col = 0; col < 12; col++)
		{
			/*
				row값이 20이면 BackGround[row][col] 값에 1을 넣는다. 즉 배경의 밑줄을 그려준다.
				그리고 또한 블록을 밑줄 밖으로 벗어나지 못하도록 해야 한다.
			*/
			if (row == 20)
				BackGround[row][col] = 1;
			/*
				col 값이 0이면 BackGround[row][col] 값에 1을 넣는다. 즉 배경의 왼쪽 줄을 그려준다.
				그리고 또한 블록을 왼쪽 줄밖으로 벗어나지 못하도록 해야 한다.
			*/
			else if (col == 0)
				BackGround[row][col] = 1;
			/*
				col 값이 11이면 BackGround[row][col] 값에 1을 넣는다. 즉 배경의 오른쪽 줄을 그려준다.
				그리고 또한 블록을 오른쪽 줄밖으로 벗어나지 못하도록 해야 한다.
			*/
			else if (col == 11)
				BackGround[row][col] = 1;
			/*
				위의 if문에 해당하지 않으면 모두 0 값을 넣는다. 0 값이 들어간 곳은 블록이 움직일 공간이다.
				그리고 각각의 줄은 블록이 그 0값인 곳에서만 움직일수 있도록 경계 줄 역할을 한다.
			*/
			else
				BackGround[row][col] = 0;
		}
}

// 아래의 함수는 배경 블록의 모양을 그린다. 
void DrawBackGround()
{
	// 디바이스 컨텍스트를 선언 및 
	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환해서 대입..
	HDC hDC = GetDC(g_hWnd);
	// 펜은 사각형을 그릴때 사용된다.
	HPEN hPen, hOldPen;
	// 브러쉬는 사각형을 채울때 사용된다.
	HBRUSH hBrush, hOldBrush;
	int x, y;

	// 초록색 펜
	hPen = CreatePen(PS_SOLID, 1, PenColor);
	// 초록색 브러쉬
	hBrush = CreateSolidBrush(HEX2RGB(0xF08080));

	/*
		SelectObject 함수로 교체 가능한 GDI Object는 Bitmap, Pen,
		Brush, Font, Region 이다. 그런데 Bitmap의 경우에는
		DC가 메모리(Memory) DC의 경우에는 Bitmap 객체가 출력장치와
		직접 관련이 되어 있어서 Bitmap 객체를 교체할 수 없습니다.
		그리고 이 함수는 GDI Object 교체 작업이 성공되면 DC가 이전에
		사용하고 있던 GDI Object의 핸들 값을 반환하고 실패하면 NULL
		값을 반환합니다.
		예를 들어, SelectObject 함수를 사용해서 Brush Object를 교체하는
		경우, 새로 만든 Brush 객체의 핸들 값이 0x10000012이고 기존에 DC에
		저장되어 있던 Brush 객체의 핸들 값이 0x30000412인 경우에 SelectObject
		함수를 사용하고 나면 DC의 Brush Object 핸들 값은 0x10000012으로 변경
		된다는 뜻입니다. 그리고 0x30000412 값은 SelectObject 함수의 반환 값이 됨!!!!~~~~
		그런데 SelectObject 함수를 보면 '두 번째 매개 변수'와 '반환' 자료형이 HGDIOBJ이다.
		실제로 사용 되는 자료형은 HBITMAP, HPEN, HBRUSH 그리고 HFONT인데 말이다. 이것은
		각 GDI Object 별로 교체 함수를 만들면 함수도 많아지고 GDI Object 가 새롭게 추가되는
		경우 새로운 함수를 만들어야 하는 불편함도 생기기 때문에 이런 핸들 자료형을 통일할 수 있는
		HGDIOBJ를 추가로 만들어서 사용하는 것입니다. 결국 HGDIOBJ 자료형이 HBITMAP, HPEN, HBRUSH
		그리고 HFONT의 상위 자료형처럼 처리되는 것입니다.(상속)
		그래서 SelectObject함수의 두 번째 매개 변수가 HGDIOBJ임에도 인자 전달시 특별한 형 변환(type casting)
		없이도 그대로 사용할 수 있는 것입니다.

		ex)
			// 파란색 Brush 객체 생성
			HBRUSH h_brush = CreateSolidBrush(RGB(0, 0, 255));
			// HBRUSH 형식을 HGDIOBJ에 대입 가능
			HGDIOBJ h_gdiobj = h_brush;

			하지만 아래와 같이 HGDIOBJ 값을 HBITMAP, HPEN, HBRUSH 또는 HFONT에 대입하는 경우 형 변환을
			사용하지 않으면 오류가 발생한다.
			// 파란색 Brush 객체 생성
			HBRUSH h_brush = CreateSolidBrush(RGB(0, 0, 255));
			// HBRUSH 형식을 HGDIOBJ에 대입 가능
			HGDIOBJ h_gdiobj = h_brush;
			// 오류 발생
			HBRUSH check = h_gdiobj;
			errir C2440: '초기화 중': 'HGDIOBJ'에서 'HBRUSH'(으)로 변환할 수 없다.

			이 문제를 해결하려면 아래와 같이 정확한 핸들 자료형으로 형 변환을 해야 한다.

			// 파란색 Brush 객체 생성
			HBRUSH h_brush = CreateSolidBrush(RGB(0, 0, 255));
			// HBRUSH 형식을 HGDIOBJ에 대입 가능
			HGDIOBJ h_gdiobj = h_brush;
			// 형변환을 사용하면 오류 해결~!
			HBRUSH check = (HBRUSH)h_gdiobj;

			(만약) SelectObject 함수의 반환 값을 HGDIOBJ 자료형으로 받았는데 SelectObject
			함수에 의해서 반환되는 값이 Brush 객체의 핸들 값이 분명할 경우... 때문에 HBRUSH
			변수로 저장하고 싶어서 아래와 같이 사용하면 C2440 오류가 발생~.

			HDC h_dc = ::GetDC(m_hWnd);
			HBURSH h_brush = CreateSolidBrush(RGB(0, 0, 255));
			// 오류!
			HBRUSH h_old_brush = selectObject(h_dc, h_brush);

			이 문제도 아래와 같이 형 변환을 사용해서 해결하면 됨.
			HDC h_dc = ::GetDC(m_hWnd);
			HBURSH h_brush = CreateSolidBrush(RGB(0, 0, 255));
			// 정상 동작!
			HBRUSH h_old_brush = (HBRUSH)selectObject(h_dc, h_brush);

	*/

	// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 아래 두 반복문은 배경 블록의 값을 검사한다.
	for (int row = 0; row < 21; row++) // 행
		for (int col = 0; col < 12; col++) // 열	
			// 만약 배열의 값이 1이면...
			if (BackGround[row][col] == 1)
			{
				// 배경 블록의 위치를 셋팅 (사각블록 하나의 크기가 10이니 *10 해준다)
				x = 100 + col * BLOCKSIZE;		// 배경을 x 축으로 100 만큼 띄워서 생성한다.
				y = row * BLOCKSIZE;
				// 해당 위치에 해당 크기로(10) 사각형을 출력한다.
				Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
			}

	// 펜을 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldPen);
	// 브러쉬를 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldBrush);

	// 펜, 브러쉬 객체를 삭제한다.
	DeleteObject(hPen);
	DeleteObject(hBrush);

	// 디바이스 컨텍스트를 반납한다.
	ReleaseDC(g_hWnd, hDC);
}

// 아래의 함수는 사용자 정의 함수로서 블록의 모양을 그린다. 
void DrawBlock()
{
	// 디바이스 컨텍스트를 선언 및 
	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환해서 대입..
	HDC hDC = GetDC(g_hWnd);
	// 펜은 사각형을 그릴때 사용된다.
	HPEN hPen, hOldPen;
	// 브러쉬는 사각형을 채울때 사용된다.
	HBRUSH hBrush, hOldBrush;
	int x, y;

	// 초록색 펜
	hPen = CreatePen(PS_SOLID, 1, PenColor);
	// 흰색 브러쉬
	hBrush = CreateSolidBrush(BlockColors[BlockNum]);

	// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 아래 두 반복문은 블록의 값을 검사한다.
	for (int row = 0; row < 4; row++) // 행
		for (int col = 0; col < 4; col++) // 열
			// 만약 배열의 값이 1이면...
			if (Block[BlockNum][RotateNum][row][col] == 1) // BlockNum는 블록의 종류, RotateNum는 블록의 회전 종류
			{
				// 블록의 위치를 셋팅 (사각블록 하나의 크기가 10이니 *10 해준다)
				x = 100 + BLOCKSIZE + NowX * BLOCKSIZE + col * BLOCKSIZE; // 110(기본) + NowX * 10 으로 출력하는 블록의 넓이를 설정하여 안 겹치게함~.
				y = NowY * BLOCKSIZE + row * BLOCKSIZE;       // NowY * 10 으로 출력하는 블록의 높이를 설정하여 안 겹치게함~.
				// 해당 위치에 해당 크기로(10) 사각형을 출력한다.
				Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
			}

	/*
		즉 간단하게 생각하면 된다. 블록의 모양을 만들기 위해서 배열의 값이 0이면
		아무 것도 출력하지 않고 1이면 사각형을 출력하여 출력결과 같은 블록을 만드는 것이다.
	*/

	// 펜을 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldPen);
	// 브러쉬를 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldBrush);

	// 펜, 브러쉬 객체를 삭제한다.
	DeleteObject(hPen);
	DeleteObject(hBrush);

	// 디바이스 컨텍스트를 반납한다.
	ReleaseDC(g_hWnd, hDC);
}

// 블록을 지움
void EraseBlock()
{
	// 디바이스 컨텍스트를 선언 및 
	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환해서 대입..
	HDC hDC = GetDC(g_hWnd);
	// 펜은 사각형을 그릴때 사용된다.
	HPEN hPen, hOldPen;
	// 브러쉬는 사각형을 채울때 사용된다.
	HBRUSH hBrush, hOldBrush;
	int x, y;

	// 검정색 펜 (배경 색상으로 지움)
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	// 검정색 브러쉬 (배경 색상으로 지움)
	hBrush = CreateSolidBrush(RGB(0, 0, 0));

	// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 아래 두 반복문은 블록의 값을 검사한다.
	for (int row = 0; row < 4; row++) // 행
		for (int col = 0; col < 4; col++) // 열
			// 만약 배열의 값이 1이면...
			if (Block[BlockNum][RotateNum][row][col]) // BlockNum는 블록의 종류, RotateNum는 블록의 회전 종류 (==1 없어도 같은 의미이다. 즉 true는 1)
			{
				// 블록의 위치를 셋팅 (사각블록 하나의 크기가 10이니 *10 해준다)
				x = 100 + BLOCKSIZE + NowX * BLOCKSIZE + col * BLOCKSIZE; // 110(기본) + NowX * 10 으로 출력하는 블록의 넓이를 설정하여 안 겹치게함~.
				y = NowY * BLOCKSIZE + row * BLOCKSIZE;       // NowY * 10 으로 출력하는 블록의 높이를 설정하여 안 겹치게함~.
				// 해당 위치에 해당 크기로(10) 사각형을 출력한다.
				Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
			}

	// 펜을 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldPen);
	// 브러쉬를 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldBrush);

	// 펜, 브러쉬 객체를 삭제한다.
	DeleteObject(hPen);
	DeleteObject(hBrush);

	// 디바이스 컨텍스트를 반납한다.
	ReleaseDC(g_hWnd, hDC);
}

// 다음 블록을 그린다.
void DrawNextBlock()
{
	// 다음 블록의 배경을 그린다.

	// 디바이스 컨텍스트를 정의한다.
	HDC hDC;
	// 펜은 사각형을 그릴때 사용된다.
	HPEN hPen, hOldPen;
	// 브러쉬는 사각형을 채울때 사용된다.
	HBRUSH hBrush, hOldBrush;

	int x, y;

	// 초록색 펜
	hPen = CreatePen(PS_SOLID, 1, PenColor);
	// 검정색 브러쉬 (배경 색상으로 셋팅)
	hBrush = CreateSolidBrush(RGB(0, 0, 0));

	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환한다.
	hDC = GetDC(g_hWnd);

	// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 해당 위치에 해당 크기로 사각형을 출력한다.
	Rectangle(hDC, 500 - BLOCKSIZE, 100 - BLOCKSIZE, 500 + BLOCKSIZE * 4, 100 + BLOCKSIZE * 5);

	// 펜을 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldPen);
	// 브러쉬를 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldBrush);

	// 펜, 브러쉬 객체를 삭제한다.
	DeleteObject(hPen);
	DeleteObject(hBrush);

	// 디바이스 컨텍스트를 반납한다.
	ReleaseDC(g_hWnd, hDC);

	// 다음 블록을 그린다.
	// 초록색 펜
	hPen = CreatePen(PS_SOLID, 1, PenColor);
	// 흰색 브러쉬
	hBrush = CreateSolidBrush(BlockColors[NextBlockNum]);

	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환한다.
	hDC = GetDC(g_hWnd);

	// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 아래 두 반복문은 블록의 값을 검사한다.
	for (int row = 0; row < 4; row++) // 행
		for (int col = 0; col < 4; col++) // 열
		{
			// 만약 배열의 값이 1이면...
			if (Block[NextBlockNum][RotateNum][row][col]) // BlockNum는 블록의 종류, RotateNum는 블록의 회전 종류 (==1 없어도 같은 의미이다. 즉 true는 1)
			{
				// 블록의 위치를 셋팅 (사각블록 하나의 크기가 10이니 *10 해준다)
				x = 500 + col * BLOCKSIZE;		 // 윈도우 x 축 방향으로 250(기본) 위치로 출력
				y = 100 + row * BLOCKSIZE;       // 윈도우 y 축 방향으로  50(기본) 위치로 출력
				// 해당 위치에 해당 크기로(10) 사각형을 출력한다.
				Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
			}
		}

	// 펜을 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldPen);
	// 브러쉬를 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldBrush);

	// 펜, 브러쉬 객체를 삭제한다.
	DeleteObject(hPen);
	DeleteObject(hBrush);

	// 디바이스 컨텍스트를 반납한다.
	ReleaseDC(g_hWnd, hDC);
}

// 블록이 이동 가능한지 여부 체크
BOOL BlockCanMove(int x, int y)
{
	// 체크 변수
	int check = 0;

	// 열, 행
	int row, col;

	// 아래 두 반복문은 블록의 값을 검사한다.
	for (row = 0; row < 4; row++) // 행
		for (col = 0; col < 4; col++) // 열
			if (Block[BlockNum][RotateNum][row][col]) // BlockNum는 블록의 종류, RotateNum는 블록의 회전 종류 (==1 없어도 같은 의미이다. 즉 true는 1)
				check += BackGround[y + row][x + col + 1]; // 블록이 배경 경계에 걸릴때...체크 변수 1 추가(옆으로는 시작점이 벽 때문에 + 1 해줘야함.)

	// 이동 가능
	if (check == 0)
		return YES;
	// 이동 불가능
	else
		return NO;
}

// 블록 좌측 이동.
void LeftMove()
{
	// 플레이어가 죽었을때
	if (PlayerState == DEAD)
		return;

	// 그렇지 않다면 블록 좌측 이동
	if (BlockCanMove(NowX - 1, NowY))
	{
		EraseGhostBlock();

		// 블록을 지우고
		EraseBlock();
		// 블록 위치 이동 시키고
		NowX--;
		// 블록을 다시 그린다.
		DrawGhostBlock();
		DrawBlock();
	}
}

// 블록 우측 이동.
void RightMove()
{
	// 플레이어가 죽었을때
	if (PlayerState == DEAD)
		return;

	// 그렇지 않다면 블록 우측 이동
	if (BlockCanMove(NowX + 1, NowY))
	{
		EraseGhostBlock();

		// 블록을 지우고
		EraseBlock();
		// 블록 위치 이동 시키고
		NowX++;
		// 블록을 다시 그린다.
		DrawGhostBlock();
		DrawBlock();
	}
}

// 블록 회전 함수
void Rotate()
{
	// 플레이어가 죽었을때
	if (PlayerState == DEAD)
		return;
	EraseGhostBlock();

	// 현재 값 임시 변수에 넣자
	int temp = RotateNum;
	int tempX = NowX;
	// RotateNum 값 1 증가
	RotateNum++;
	// 변수의 한계 값을 3까지만 갖기 위함(if 안써도 됨 스킬이니 기억해두길)
	RotateNum %= 4;

	// 회전이 가능 하면
	if (BlockCanMove(NowX, NowY))
	{
		// 복구
		RotateNum = temp;
		// 블록을 지우고
		EraseBlock();
		// RotateNum 값 1 증가
		RotateNum++;
		// 변수의 한계 값을 3까지만 갖기 위함(if 안써도 됨 스킬이니 기억해두길)
		RotateNum %= 4;
		// 회전한 블록 그리기
		DrawGhostBlock();
		DrawBlock();
	}
	// 회전이 불가능 하면 복구
	else
	{
		bool isRotate = FALSE;
		int finalX;
		NowX++;
		if (BlockCanMove(NowX, NowY))
		{
			isRotate = TRUE;
			finalX = NowX;
		}
		else
		{
			NowX++;
			if (BlockCanMove(NowX, NowY))
			{
				isRotate = TRUE;
				finalX = NowX;
			}
			else//안돼? 반대편으로 가
			{
				NowX = tempX - 1;
				if (BlockCanMove(NowX, NowY))
				{
					isRotate = TRUE;
					finalX = NowX;
				}
				else
				{
					NowX--;
					if (BlockCanMove(NowX, NowY))
					{
						isRotate = TRUE;
						finalX = NowX;
					}
					else
					{
						//답없음.
						isRotate = FALSE;
						finalX = tempX;
					}
				}
			}
		}

		if (isRotate)
		{
			// 복구
			RotateNum = temp;
			NowX = tempX;
			// 블록을 지우고
			EraseBlock();
			// RotateNum 값 1 증가
			RotateNum++;
			// 변수의 한계 값을 3까지만 갖기 위함(if 안써도 됨 스킬이니 기억해두길)
			RotateNum %= 4;
			NowX = finalX;
			// 회전한 블록 그리기
			DrawGhostBlock();
			DrawBlock();
		}

		//RotateNum = temp;
	}
}

// 블록 다운 함수
BOOL DownMove()
{
	// 플레이어가 죽었을때
	if (PlayerState == DEAD) return FAIL;

	// 블록이 아래로 내려 갈수 있다면(미리 체크)
	if (BlockCanMove(NowX, NowY + 1))
	{
		// 블록을 지우고
		EraseBlock();
		// 블록 위치 이동 시키고
		NowY++;
		// 블록을 다시 그린다.
		DrawGhostBlock();
		DrawBlock();
		return SUCCESS;
	}
	// 블록이 아래로 내려 갈 수 없다면
	else
	{
		// 움직일수 없는 블록을 배경으로 업데이트.
		UpdateBackGround();
		// 라인이 꽉 차있는지 검사.(미션 성공인지 검사)
		CheckFullLine();

		// 메시지를 발생 시킨다.(다음 블록 진행)
		SendMessage(g_hWnd, WM_NewBlock, 0, 0);
		return FAIL;
	}
}

// 움직일수 없는 블록을 배경으로 업데이트.
void UpdateBackGround()
{
	// 요소 변수
	int element;
	// 아래 두 반복문은 블록의 값을 검사한다.
	for (int row = 0; row < 4; row++) // 행
		for (int col = 0; col < 4; col++) // 열
		{
			// 현재 블록의 요소 값을 저장한다.
			element = Block[BlockNum][RotateNum][row][col];
			// 만약 현재 블록 배열의 값이 1 이면, 
			// 블록 배열의 값을 블록 현재 위치의 배경으로 업뎃~!.
			if (element)
				BackGround[NowY + row][NowX + col + 1] = element;
		}
}

// 한줄 또는 그 이상 미션이 성공했는지 체크  
void CheckFullLine()
{
	// 행, 열, 성공 라인
	int row, col, line;
	// 미션 체크 변수
	int elementNum;

	// 바텀부터 검색...
	for (row = 19; row >= 0; row--)
	{
		// 미션 체크 변수 초기화
		elementNum = 0;
		// 해당 열, 행의 배경 값으로 elementNum 변수 증가
		for (col = 1; col <= 10; col++)
		{
			if (BackGround[row][col] > 0)
			{
				elementNum++;
			}
		}

		// 블록이 가득 체워져 있는지 검사한다.
		if (elementNum == 10)
		{
			// 삭제 할 라인 바로 윗 라인부터 백업 해두고~
			for (line = row; line > 0; line--)
				for (col = 1; col <= 10; col++)
					BackGround[line][col] = BackGround[line - 1][col];

			// 한칸 내려갈거니깐 맨 윗줄 삭제(?)
			for (col = 1; col <= 10; col++)
				BackGround[0][col] = 0;
			
			// 블록이 가득찬 줄을 삭제한다.
			EraseFullLine(row);
			// 점수가 될수도 있다~(미처리)(?)(미션)
			EffectPlay(EffectSoundPlus);
			FullLineNum++;
			if (FullLineNum % 3 == 0)
			{
				if (Level<10)
				{
					Level++;
				}
			}
			RECT scoreRect = { 400, 0, 700, 30 }; // 텍스트가 있는 충분한 범위
			InvalidateRect(g_hWnd, &scoreRect, TRUE);
			// 2줄 이상이 있을 경우를 대비
			row++;
		}
	}
}

// 블록이 가득찬 줄을 삭제하는 함수.(라인을 지우는 함수)
void EraseFullLine(int row)
{
	// 디바이스 컨텍스트를 정의한다.
	HDC hDC, hMemDC;
	// 비트맵의 핸들을 지정하기 위함.
	HBITMAP hBmp;

	// 배경 제외 블록 시작점 110
	int Xlen = 10 * BLOCKSIZE + BLOCKSIZE;

	/* 복사할 영역의 Ylen은 제일 위에서 가득찬 라인 바로 위까지
	의 길이이다.
	row는 0부터 시작하므로 row*10은 full line의 바로 위까지의 길이
	이다.
	즉! full line이 10번째 줄이라면 이 라인의 y 영역은
	90~99이다. 이때 row=9이고 복사할 부분의 y영역은 0~89까지 이다.*/

	// 한마디로 첫 라인부터 삭제 라인 전까지 라인이다. (ZeroBase)(윗 말이 힘들면 그냥 이것만 기억)
	int Ylen = row * BLOCKSIZE;

	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환한다.
	hDC = GetDC(g_hWnd);
	// 주어진 디바이스와 호환되는 메모리 디바이스 컨텍스트를 생성한다.
	hMemDC = CreateCompatibleDC(hDC);
	// 비트맵을 생성한다. 
	hBmp = CreateCompatibleBitmap(hDC, 720, 720); // 충분한 크기로 잡는다.
	// 비트맵을 메모리 디바이스 컨텍스트에 넣는다.
	SelectObject(hMemDC, hBmp);

	/*
	 함수 원형
	BOOL BitBlt(
	HDC hdcDest,
	int nXDest,
	int nYDest,
	int nWidth,
	int nHeight,
	HDC hdcSrc,
	int nXSrc,
	int nYSrc,
	DWORD dwRop);

	인수
	hdcDest
	이미지를 출력할 위치의 핸들.

	nXDest, nYDest
	이미지를 출력할 위치인 x, y 좌표.

	nWidth, nHeight
	원본 이미지의 너비, 높이 이다.
	이 크기만큼 원본 이미지에서 잘라와 그린다.

	hdcSrc
	이미지의 핸들.

	nXSrc, nYSrc
	가져올 이미지의 시작지점인 x, y좌표이다.
	이 위치부터 nWidth, nHeight만큼 이미지를 잘라온다.

	dwRop
	이미지의 출력 방법

	*/

	// 화면의 일부를 메모리로 복사한다.
	BitBlt(hMemDC, 0, 0, Xlen, Ylen, hDC, 100 + BLOCKSIZE, 0, SRCCOPY);

	// 메모리에서 화면의 다른 부분으로 복사한다.
	// 즉! 두번째 줄부터 복사한다.
	BitBlt(hDC, 100 + BLOCKSIZE, BLOCKSIZE, Xlen, Ylen, hMemDC, 0, 0, SRCCOPY);

	// (메모리)디바이스 컨텍스트(DC)를 없앤다
	DeleteDC(hMemDC);
	// 디바이스 컨텍스트를 반납한다.
	ReleaseDC(g_hWnd, hDC);
	// 객체를 삭제한다.
	DeleteObject(hBmp);
}

void ShowTeto(HDC hDC)
{
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBmpTetoris);
	StretchBlt(
		hDC,      // 목적지 DC
		0, 0,     // 시작 좌표
		675, 675, // 출력하고 싶은 크기 (창 크기에 맞춰서!)
		hMemDC,   // 원본 DC
		0, 0,     // 원본에서 가져올 시작 좌표
		225, 225, // 원본 사진의 실제 크기
		SRCCOPY   // 복사 방식
	);	
	DeleteDC(hMemDC);
}

void DrawDancingTeto(int frame)
{
	HDC hDC = GetDC(g_hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBmpDanceTeto);

	int srcX = (frame % 2) * 380;
	int srcY = (frame / 2) * 380;

	StretchBlt(hDC, 100, 430, 225, 225,
		hMemDC, srcX, srcY, 380, 380, SRCCOPY);

	DeleteDC(hMemDC);
	ReleaseDC(g_hWnd, hDC);
}

void UpObstacle()
{
	obstacleNum++;
	int row, col;

	// ====================================================
	// 1. [데이터] 배열(BackGround)을 한 칸씩 위로 당기기
	// ====================================================
	for (row = 0; row < 19; row++)
	{
		for (col = 1; col <= 10; col++)
			BackGround[row][col] = BackGround[row + 1][col];
	}

	// 현재 조작 중인 블록도 같이 위로 올려야 파묻히지 않음
	if (NowY > 0) NowY--;

	// ====================================================
	// 2. [이미지] 화면(DC)을 복사해서 위로 밀어올리기 (BitBlt)
	// ====================================================
	HDC hDC = GetDC(g_hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);

	// 전체 화면을 담을 넉넉한 비트맵 생성
	HBITMAP hBmp = CreateCompatibleBitmap(hDC, 1280, 720);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp);

	int StartX = 100 + BLOCKSIZE; // 게임판 시작 X (120)
	int Width = 10 * BLOCKSIZE;  // 게임판 너비 (200)
	int CopyHeight = 19 * BLOCKSIZE; // 1~19행까지의 높이 (맨 윗줄은 덮어씌워짐)

	// 화면의 '1번째 줄'부터 '바닥'까지를 -> 메모리의 (0,0)으로 복사
	BitBlt(hMemDC, 0, 0, Width, CopyHeight, hDC, StartX, BLOCKSIZE, SRCCOPY);

	// 메모리 이미지를 -> 화면의 '0번째 줄(맨 위)'부터 붙여넣기 (위로 이동 효과)
	BitBlt(hDC, StartX, 0, Width, CopyHeight, hMemDC, 0, 0, SRCCOPY);

	// ====================================================
	// 3. [그리기] 맨 아랫줄(19행)에 회색 장애물 그리기
	// ====================================================

	// 회색 브러쉬 준비 (RGB: 128, 128, 128)
	HBRUSH hGrayBrush = CreateSolidBrush(RGB(128, 128, 128));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hGrayBrush);

	// 테두리 펜 설정 (기존 PenColor 사용)
	HPEN hPen = CreatePen(PS_SOLID, 1, PenColor);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	int hole = (rand() % 10) + 1; // 구멍 위치

	for (col = 1; col <= 10; col++)
	{
		int x = 100 + col * BLOCKSIZE;
		int y = 19 * BLOCKSIZE; // 맨 아랫줄 Y좌표

		if (col == hole)
		{
			BackGround[19][col] = 0; // 구멍 데이터

			// 구멍 부분은 검은색으로 덮어줘야 깨끗함 (배경색)
			HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			SelectObject(hDC, hBlackBrush);
			Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
			SelectObject(hDC, hGrayBrush); // 다시 회색 잡기
		}
		else
		{
			BackGround[19][col] = 9; // 장애물 데이터 (9번)

			// 회색 사각형 그리기
			Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
		}
	}

	// ====================================================
	// 4. [정리] 리소스 해제
	// ====================================================
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	DeleteObject(hGrayBrush);
	DeleteObject(hPen);

	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBmp);
	DeleteDC(hMemDC);
	ReleaseDC(g_hWnd, hDC);
}

void SaveBlock()
{
	isSave = TRUE;
	saveBlockNum = BlockNum;
	DrawSaveBlock();
}

void UseBlock()
{
	NextBlockNum = saveBlockNum;
}

void PrintInfo(HDC hDC)
{
	SetTextColor(hDC, RGB(255, 255, 255));

	SetBkMode(hDC, TRANSPARENT);
	WCHAR szScore[64];
	wsprintf(szScore, L"SCORE : %d", FullLineNum * 100);
	WCHAR szLevel[64];
	wsprintf(szLevel, L"LEVEL : %d", Level);
	TextOut(hDC, 500, 0, szScore, lstrlen(szScore));
	TextOut(hDC, 400, 0, szLevel, lstrlen(szLevel));

	int remainTime = (20-Level) - second; // 남은 시간 계산
	WCHAR szTime[64];
	wsprintf(szTime, L"WALL UP : %d", remainTime);

	// 3초 이하로 남으면 '빨간색'으로 경고
	if (remainTime <= 3)
	{
		SetTextColor(hDC, RGB(255, 50, 50)); // 빨강
	}
	else
	{
		SetTextColor(hDC, RGB(0, 255, 0));   // 평소엔 초록
	}
	TextOut(hDC, 350, 380, szTime, lstrlen(szTime));

	SetTextColor(hDC, RGB(255, 255, 255));

	// 안내 문구
	TextOut(hDC, 480, 60, L"NEXT BLOCK", 10);
	TextOut(hDC, 480, 260, L"HOLD BLOCK", 10);

	TextOut(hDC, 480, 500, L"SPACE: DROP", 11);
	TextOut(hDC, 480, 520, L"ENTER: HOLD", 11);
	TextOut(hDC, 480, 540, L"UPKEY: ROTATE BLOCK", 19);

	TextOut(hDC, 350, 600, L"LvUP: 300점마다 WALL UP 1초씩 감소", 27);
}

void DrawGhostBlock()
{
	int ghostY = NowY;
	while (BlockCanMove(NowX, ghostY + 1))
	{
		ghostY++;
	}

	// 디바이스 컨텍스트를 선언 및 
	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환해서 대입..
	HDC hDC = GetDC(g_hWnd);
	// 펜은 사각형을 그릴때 사용된다.
	HPEN hPen, hOldPen;
	// 브러쉬는 사각형을 채울때 사용된다.
	HBRUSH hBrush, hOldBrush;
	int x, y;

	// 초록색 펜
	hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	// 흰색 브러쉬
	hBrush = CreateSolidBrush(RGB(50, 50, 50));

	// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 아래 두 반복문은 블록의 값을 검사한다.
	for (int row = 0; row < 4; row++) // 행
		for (int col = 0; col < 4; col++) // 열
			// 만약 배열의 값이 1이면...
			if (Block[BlockNum][RotateNum][row][col] == 1) // BlockNum는 블록의 종류, RotateNum는 블록의 회전 종류
			{
				// 블록의 위치를 셋팅 (사각블록 하나의 크기가 10이니 *10 해준다)
				x = 100 + BLOCKSIZE + NowX * BLOCKSIZE + col * BLOCKSIZE; // 110(기본) + NowX * 10 으로 출력하는 블록의 넓이를 설정하여 안 겹치게함~.
				y = ghostY * BLOCKSIZE + row * BLOCKSIZE;       // NowY * 10 으로 출력하는 블록의 높이를 설정하여 안 겹치게함~.
				// 해당 위치에 해당 크기로(10) 사각형을 출력한다.
				Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
			}

	/*
		즉 간단하게 생각하면 된다. 블록의 모양을 만들기 위해서 배열의 값이 0이면
		아무 것도 출력하지 않고 1이면 사각형을 출력하여 출력결과 같은 블록을 만드는 것이다.
	*/

	// 펜을 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldPen);
	// 브러쉬를 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldBrush);

	// 펜, 브러쉬 객체를 삭제한다.
	DeleteObject(hPen);
	DeleteObject(hBrush);

	// 디바이스 컨텍스트를 반납한다.
	ReleaseDC(g_hWnd, hDC);
}

void EraseGhostBlock()
{
	int ghostY = NowY;
	while (BlockCanMove(NowX, ghostY + 1))
	{
		ghostY++;
	}

	// 디바이스 컨텍스트를 선언 및 
	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환해서 대입..
	HDC hDC = GetDC(g_hWnd);
	// 펜은 사각형을 그릴때 사용된다.
	HPEN hPen, hOldPen;
	// 브러쉬는 사각형을 채울때 사용된다.
	HBRUSH hBrush, hOldBrush;
	int x, y;

	// 초록색 펜
	hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	// 흰색 브러쉬
	hBrush = CreateSolidBrush(RGB(0, 0, 0));

	// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 아래 두 반복문은 블록의 값을 검사한다.
	for (int row = 0; row < 4; row++) // 행
		for (int col = 0; col < 4; col++) // 열
			// 만약 배열의 값이 1이면...
			if (Block[BlockNum][RotateNum][row][col] == 1) // BlockNum는 블록의 종류, RotateNum는 블록의 회전 종류
			{
				// 블록의 위치를 셋팅 (사각블록 하나의 크기가 10이니 *10 해준다)
				x = 100 + BLOCKSIZE + NowX * BLOCKSIZE + col * BLOCKSIZE; // 110(기본) + NowX * 10 으로 출력하는 블록의 넓이를 설정하여 안 겹치게함~.
				y = ghostY * BLOCKSIZE + row * BLOCKSIZE;       // NowY * 10 으로 출력하는 블록의 높이를 설정하여 안 겹치게함~.
				// 해당 위치에 해당 크기로(10) 사각형을 출력한다.
				Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
			}

	/*
		즉 간단하게 생각하면 된다. 블록의 모양을 만들기 위해서 배열의 값이 0이면
		아무 것도 출력하지 않고 1이면 사각형을 출력하여 출력결과 같은 블록을 만드는 것이다.
	*/

	// 펜을 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldPen);
	// 브러쉬를 디바이스 컨텍스트에 넣는다.
	SelectObject(hDC, hOldBrush);

	// 펜, 브러쉬 객체를 삭제한다.
	DeleteObject(hPen);
	DeleteObject(hBrush);

	// 디바이스 컨텍스트를 반납한다.
	ReleaseDC(g_hWnd, hDC);
}

void DrawSaveBlock()
{
	// 다음 블록의 배경을 그린다.

	// 디바이스 컨텍스트를 정의한다.
	HDC hDC;
	// 펜은 사각형을 그릴때 사용된다.
	HPEN hPen, hOldPen;
	// 브러쉬는 사각형을 채울때 사용된다.
	HBRUSH hBrush, hOldBrush;

	int x, y;

	// 초록색 펜
	hPen = CreatePen(PS_SOLID, 1, PenColor);
	// 검정색 브러쉬 (배경 색상으로 셋팅)
	hBrush = CreateSolidBrush(RGB(0, 0, 0));

	// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환한다.
	hDC = GetDC(g_hWnd);

	// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	// 해당 위치에 해당 크기로 사각형을 출력한다.
	Rectangle(hDC, 500 - BLOCKSIZE, 300 - BLOCKSIZE, 500 + BLOCKSIZE * 4, 300 + BLOCKSIZE * 5);

	if (!isSave)
	{
		DeleteObject(hPen);
		DeleteObject(hBrush);

		// 디바이스 컨텍스트를 반납한다.
		ReleaseDC(g_hWnd, hDC);
		return;
	}
	
	if (isUsed)
	{
		int left = 500 - BLOCKSIZE;
		int top = 300 - BLOCKSIZE;
		int right = 500 + BLOCKSIZE * 4;
		int bottom = 300 + BLOCKSIZE * 5;

		// 1. 사각형 테두리 그리기
		Rectangle(hDC, left, top, right, bottom);

		// 2. 대각선 줄 긋기 (X 모양)
		// 왼쪽 위 -> 오른쪽 아래
		MoveToEx(hDC, left, top, NULL);
		LineTo(hDC, right, bottom);

		// 오른쪽 위 -> 왼쪽 아래
		MoveToEx(hDC, right, top, NULL);
		LineTo(hDC, left, bottom);
		// 펜, 브러쉬 객체를 삭제한다.
		DeleteObject(hPen);
		DeleteObject(hBrush);

		// 디바이스 컨텍스트를 반납한다.
		ReleaseDC(g_hWnd, hDC);
		return;
	}
	else
	{
		// 펜을 디바이스 컨텍스트에 넣는다.
		SelectObject(hDC, hOldPen);
		// 브러쉬를 디바이스 컨텍스트에 넣는다.
		SelectObject(hDC, hOldBrush);

		// 펜, 브러쉬 객체를 삭제한다.
		DeleteObject(hPen);
		DeleteObject(hBrush);

		// 디바이스 컨텍스트를 반납한다.
		ReleaseDC(g_hWnd, hDC);

		// 다음 블록을 그린다.
		// 초록색 펜
		hPen = CreatePen(PS_SOLID, 1, PenColor);
		// 흰색 브러쉬
		hBrush = CreateSolidBrush(BlockColors[saveBlockNum]);

		// 윈도우 클라이언트 영역의 디스플레이 디바이스 컨텍스트(DC) 핸들을 구해서 반환한다.
		hDC = GetDC(g_hWnd);

		// 펜을 디바이스 컨텍스트에 넣고 반환된 전에 펜 핸들을 대입.
		hOldPen = (HPEN)SelectObject(hDC, hPen);
		// 브러쉬를 디바이스 컨텍스트에 넣고 반환된 전에 브러쉬 핸들을 대입.
		hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

		// 아래 두 반복문은 블록의 값을 검사한다.
		for (int row = 0; row < 4; row++) // 행
			for (int col = 0; col < 4; col++) // 열
			{
				// 만약 배열의 값이 1이면...
				if (Block[saveBlockNum][RotateNum][row][col]) // BlockNum는 블록의 종류, RotateNum는 블록의 회전 종류 (==1 없어도 같은 의미이다. 즉 true는 1)
				{
					// 블록의 위치를 셋팅 (사각블록 하나의 크기가 10이니 *10 해준다)
					x = 500 + col * BLOCKSIZE;		 // 윈도우 x 축 방향으로 250(기본) 위치로 출력
					y = 300 + row * BLOCKSIZE;       // 윈도우 y 축 방향으로  50(기본) 위치로 출력
					// 해당 위치에 해당 크기로(10) 사각형을 출력한다.
					Rectangle(hDC, x, y, x + BLOCKSIZE, y + BLOCKSIZE);
				}
			}

		// 펜을 디바이스 컨텍스트에 넣는다.
		SelectObject(hDC, hOldPen);
		// 브러쉬를 디바이스 컨텍스트에 넣는다.
		SelectObject(hDC, hOldBrush);

		// 펜, 브러쉬 객체를 삭제한다.
		DeleteObject(hPen);
		DeleteObject(hBrush);

		// 디바이스 컨텍스트를 반납한다.
		ReleaseDC(g_hWnd, hDC);
	}
}

void DrawGameOver(HDC hDC)
{
	// 1. [배경 이미지] 그리기
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmpGameOver);

	// 창 크기가 675x750이니까 그 크기에 맞춤
	StretchBlt(hDC, 0, 0, 675, 750, hMemDC, 0, 0, 1024, 1024, SRCCOPY);

	HFONT hFont, hOldFont;
	hFont = CreateFont(50, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, L"Arial");
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(255, 255, 255));

	WCHAR szScore[64];
	wsprintf(szScore, L"FINAL SCORE: %d", FullLineNum * 100);

	TextOut(hDC, 180, 130, szScore, lstrlen(szScore));

	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);
}