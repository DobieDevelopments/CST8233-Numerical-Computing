/******* File Header : Cover **********************************************************************************************************
* Student name										: John Dobie
* Student number									: 040 659 609
* Assignment number (and name if any)				: Assignment 3, Martian Lander, Differential Equation Project in C++
* Course name and number							: CST 8233 – F18 – Numerical Computing
* Lab section number								: 304
* Professor’s name									: Andrew Tyler
* Due date of assignment							: Saturday, December 2nd, 2018
* Submission date of assignment						: Wednesday, December 2nd, 2018
* List of source and header files in the project	: ass3.cpp
***************************************************************************************************************************************/
/******* File Header ******************************************************************************************************************
* Filename											: ass3.cpp
* Version											: 1.0
* Purpose											: To get user input and display values of differential equations of numbers based 
*													| on the velocity of a falling spaceship affected by different factors.
**************************************************************************************************************************************/

#include <Windows.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <conio.h>
#include <fstream>
#include <vector>

using namespace std;

void Update(HANDLE h, COORD cursor);
void Input(float& b, bool& quit, int& kbhit);
void DrawShip(HANDLE h, bool hasBoost, float b, bool invert);
string Save(double elapsed_time, int num_bounces);
void Load(HANDLE h, string yourname);
template<typename T> void Print(T params, bool endline);
template<typename T> void Print(HANDLE h, WORD wAttributes, T param, bool endline);
template <typename T> string ToString(const T& n);
void ShowGameOverText(HANDLE h, COORD cursor);

enum COLOR	//used to simplify displaying text in the console using a variety of colours.
{
	// Standard text colors
	MAGENTA_TEXT = 5, GRAY_TEXT = 8, BLUE_TEXT, GREEN_TEXT, CYAN_TEXT, RED_TEXT, PINK_TEXT, YELLOW_TEXT, WHITE_TEXT,
	// Faded text background color
	BLACK_BACKGROUND = 0 << 4
};

/******* Function Header **************************************************************************************************************
* Function name										: main()
* Purpose											: To handle the core of our program, entry point, menu and exit upon success.
* Function In parameters							: none
* Function Out parameters							: int - return code
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
int main(void)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleScreenBufferInfo(h, &consoleBufferInfo);
	SetConsoleActiveScreenBuffer(h);

	//incase we need the original window settings
	WORD defaultWindowAttributes;
	defaultWindowAttributes = consoleBufferInfo.wAttributes;

	//hide the cursor
	GetConsoleCursorInfo(h, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(h, &cursorInfo);

	bool win = false, quit = false;
	//cursor location setting
	COORD cursor;
	cursor.X = cursor.Y = 0;

	/////GAME_LOOP/////
	while (!quit)
	{
	/////SPLASH_SCREEN/////
	Print("Martian Lander Game, by: John Dobie", true);
	Print("Press any key to play.\n", true);
	Load(h, "");
		while (!quit)
		{
			if (_kbhit())
			{
				if (_getch() == 'q') 
				{
					quit = true; 
					break;
				}
				cin.clear();
				system("CLS");
				break;
			}
		}
		/////GAME/////
		if (!quit)
		{
			Update(h, cursor);
			Print(h, CYAN_TEXT, "\nPlay again? : [Q] to quit. [Any] to retry.\n", true);
			Print(h, WHITE_TEXT, "", true);
			cin.clear();
			while (!quit)
				if (_kbhit())
				{
					if (_getch() == 'q') quit = true;
					else break;
				}
		}
	}

	return 0;
}

/******* Function Header **************************************************************************************************************
* Function name										: Update()
* Purpose											: Handle the game loop, providing game logic calculations and rendering any text or 
*													| graphics needed to the user.
* Function In parameters							: HANDLE - window handle, used for cursor movement and colours
*													| COORD - used for cursor movement for repainting
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void Update(HANDLE h, COORD cursor)
{
	bool running = true, win = false;
	/////////// TIME
	double timestep = 16, max_frames_per_second = 60;
	double	previous = GetCurrentTime(),
		current = 0,
		elapsed = 0,
		delta_time = 0,
		accumulated_delta_time = 0,
		min, sec, milli;
	/////////// GAME
	float burn = 0, burn_reserve = 100, gravity = 3.7f, acceleration = 8.3f, atmospheric_drag = 0.13f;
	int number_of_bounces = 0, velocity_max = 46, kbhit = 0;
	double velocity = 0, last_velocity = 0, last_delta_velocity = 0, x = 1000;
	bool hasBoost = true;
	int game_over = 25;
	///////////
	while (running)
	{
		/////TIME/////
		current = GetCurrentTime();
		delta_time = (current - previous);
		elapsed += delta_time;
		accumulated_delta_time += delta_time;
		timestep = 1000 / max_frames_per_second;//1.0 / deltaTime; tickrate
		previous = current;
		delta_time /= CLOCKS_PER_SEC;

		min = (int)elapsed / CLOCKS_PER_SEC / 60;
		sec = (int)elapsed / CLOCKS_PER_SEC % 60;
		milli = (int)elapsed % CLOCKS_PER_SEC;

		while (accumulated_delta_time >= timestep)
		{
			/////GAME_LOGIC/////
			if (burn_reserve > 0)
			{
				if (burn > 0)
					burn_reserve -= burn * (float)(delta_time);
			}
			else
			{
				burn_reserve = 0; burn = 0;
				if (hasBoost)
					system("CLS");
				hasBoost = false;
			}
			/////CALCULATIONS/////
			//delta_time /= CLOCKS_PER_SEC;
				//initial step of dv0, the differential equation [g - c * (v + (a * (v/vmax)^3) - B]
			double delta_velocity = gravity - atmospheric_drag * (velocity + (acceleration * pow(velocity / velocity_max, 3))) - burn;
				//Use Huen's method on dv values for the first step of v0 velocity. [lv = v + (dv0 + dv)/2 * dt]
			last_velocity = velocity + delta_velocity * delta_time;
				//next step dv1 of the differential equation [g - c * (lv + (a * (v/vmax)^3) - B]
			last_delta_velocity = gravity - atmospheric_drag * (last_velocity + (acceleration * pow(last_velocity / velocity_max, 3))) - burn;
				//Use Huen's method on dv values for the next step of v1 velocity. [v = lv + (dv0 + dv1)/2 * dt] 
			velocity = last_velocity + ((last_delta_velocity + delta_velocity) / 2) * delta_time;
				/*	for solving [v = dx/dt] ---> [dx = v * dt]
				Use Huen's method using both velocity steps [x = x + (v0 + v1)/2 * dt] */
			x -= ((last_velocity + velocity) / 2) * delta_time;

			/////GAME_OVER?/////
			if (x < 1)
			{
				if (velocity < 1 && velocity > 0 || number_of_bounces > game_over)
				{
					running = false;
					win = true;
				}
				if (x <= 0)
					if (velocity > 1)
					{
						velocity = -velocity;
						number_of_bounces++;
					}
			}
			/////INPUT/////
			Input(burn, running, kbhit);
			/////TIME/////
			accumulated_delta_time -= timestep;
		}
		cursor.X = cursor.Y = 0;
		SetConsoleCursorPosition(h, cursor);	//move the cursor position if it isn't in the right place
		/////SHOW_TITLE/////
		if (kbhit == 0)
		{
			Print(h, GREEN_TEXT, "*********************************", false);
			Print(h, WHITE_TEXT, " Starting Descent ", false);
			Print(h, GREEN_TEXT, "***************************************", true);
		}
		else
			kbhit == 1 ? Print(h, WHITE_TEXT, "W was pressed - increase burn", false) : Print(h, WHITE_TEXT, "E was pressed - reduce burn", false);

		/////DISPLAY/////
			//_thrust//
		Print(h, WHITE_TEXT, ": BURN = ", false);
		if (burn == 0)
			Print(burn, false);
		else Print(h, GREEN_TEXT, burn, false);
		//_fuel//
		Print(h, WHITE_TEXT, "; BURN RESERVE = ", false);
		if (burn_reserve == 0)
			Print(h, RED_TEXT, burn_reserve, true);
		else Print(burn_reserve, true);
		//_time//
		Print(h, WHITE_TEXT, "MINUTE = " + ToString(min) + "; SECOND = " + ToString(sec) + "; MILLISECOND = " + ToString(milli) + ";", true);
		//_velocity//
		Print("SPEED OF DESCENT = ", false);
		Print(velocity, false);
		Print("m/s     ", true);
		//_position//
		Print("HEIGHT ABOVE MARTIAN SURFACE = ", false);
		Print(x, false);
		Print("metres     ", true);
		//_did_you_crash?//
		Print("NUMBER OF BOUNCES = " + ToString(number_of_bounces), true);
		/////RENDER_SHIP/////
		cursor.Y = 10;
		SetConsoleCursorPosition(h, cursor);
		DrawShip(h, hasBoost, burn, velocity < 0);
	}
	system("cls");
	if (win)
	{
		ShowGameOverText(h, cursor);	//print graphic

		Print("                                                ", true);
		Print(h, CYAN_TEXT, "Number of bounces: " + ToString(number_of_bounces) + " Game Time: ", false);
		//display the time
		(min < 10) ? Print("0", false), Print(ToString(min) + ":", false) : Print(ToString(min) + ":", false);
		(sec < 10) ? Print("0", false), Print(ToString(sec) + ":", false) : Print(ToString(sec) + ":", false);
		(milli < 10) ? Print("00", false), Print(ToString(milli), true) : (milli < 100) ? Print("0", false), Print(ToString(milli), true) : Print(ToString(milli), true);

		if(number_of_bounces == 0) { Print("You're the best, like no one ever was!", true); }
		else if(number_of_bounces == 1) { Print("You're the second best!", true); }
		else if(number_of_bounces == 2 || number_of_bounces == 3) { Print("Better luck next time!", true); }
		else if (number_of_bounces >= 4 && number_of_bounces < 8) { Print("Great Job!", true); }
		else if (number_of_bounces >= 8 && number_of_bounces < 11) { Print("Niiiiice!", true); }
		else if (number_of_bounces >= 11 && number_of_bounces < 14) { Print("Wow, not bad!", true); }
		else if (number_of_bounces == 14) { Print("Could've been better, but good effort!", true); }
		else if (number_of_bounces == 15) { Print("Try harder next time!", true); }
		else { Print("You can do better! try again :)", true); }

		Load(h, Save(elapsed, number_of_bounces));
	}
}

/******* Function Header **************************************************************************************************************
* Function name										: Save()
* Purpose											: To handle getting the user's name and saving the score to a file.
* Function In parameters							: double - total elapsed time of the game simulation
*													| int - the number of bounces for the user's 'score'
* Function Out parameters							: string - the users name, used to highlight in the highscore list
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
string Save(double elapsed_time, int num_bounces)
{
	ofstream highscores;
	highscores.open("highscores.txt", ofstream::app | ofstream::out);

	cout << "Save your score, Enter your name: ";
	string name;
	cin >> name;
	cout << name.substr(0, name.length() < 8 ? name.length() : 8) << ", your scores are displayed in green." << endl;
	if(name.length() > 8)
	name = name.substr(0, 8);
	else
		name.append(8 - name.length(), ' ');
	highscores << name << " " << num_bounces << " " << elapsed_time << endl;
	highscores.close();
	cin.clear();
	return name;
}

typedef struct //a data structure used to hold the information for a player score entry.
{
	string name;
	double elapsed_time;
	int num_bounces;
}player_entry;
void Sort_vector(vector<player_entry>& list);

/******* Function Header **************************************************************************************************************
* Function name										: Load()
* Purpose											: Used to load in the scores of the file and display them in a sorted list.
* Function In parameters							: HANDLE - the window handle
*													| string - the current player's name, used to highlight their scores from previous runs.
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void Load(HANDLE h, string yourname)
{
	ifstream highscores;
	highscores.open("highscores.txt", ifstream::in);

	vector<player_entry> list;

	if (highscores.peek() != ifstream::traits_type::eof())	//check if the file is empty
	{
		player_entry score;
		int min, sec, milli, RECORDS_TO_SHOW = 20;

		while (highscores >> score.name >> score.num_bounces >> score.elapsed_time)
			list.push_back(score);
		Sort_vector(list);	//sort the highscores

		//print 'top 20' if there are 20 or more scores
		list.size() >= (unsigned)RECORDS_TO_SHOW ? Print("------------- Scoreboard [top " + ToString(RECORDS_TO_SHOW) + "] ---------------\n", true) : Print("------------------ Scoreboard -------------------\n", true);
		for (unsigned int i = 0; i < (list.size() < (unsigned)RECORDS_TO_SHOW ? list.size() : RECORDS_TO_SHOW); ++i)
		{
			min = (int)list[i].elapsed_time / CLOCKS_PER_SEC / 60;	//parse the time again
			sec = (int)list[i].elapsed_time / CLOCKS_PER_SEC % 60;
			milli = (int)list[i].elapsed_time % CLOCKS_PER_SEC;

			//highlight the player's scores in green, if they have the same name on the scoreboard
			(strncmp(yourname.c_str(), list[i].name.c_str(), list[i].name.length()) == 0) ? Print(h, GREEN_TEXT, "Name: " + list[i].name + ((list[i].name.length() > 1) ? "\t" : "\t\t"), false) : Print(h, MAGENTA_TEXT, "Name: " + list[i].name + ((list[i].name.length() > 1) ? "\t" : "\t\t"), false);

			//display information with additional needed 0's for padding
			Print(" bounces: ", false);
			if (list[i].num_bounces < 10) Print("0", false);
			Print(ToString(list[i].num_bounces) + " \ttime:  ", false);

			(min < 10) ? Print("0", false), Print(ToString(min) + ":", false) : Print(ToString(min) + ":", false);
			(sec < 10) ? Print("0", false), Print(ToString(sec) + ":", false) : Print(ToString(sec) + ":", false);
			(milli < 10) ? Print("00", false), Print(ToString(milli), true) : (milli < 100) ? Print("0", false), Print(ToString(milli), true) : Print(ToString(milli), true);
		}
	}
}

/******* Function Header **************************************************************************************************************
* Function name										: Difference()
* Purpose											: Used to determine which score came before another using the bounces and time.
* Function In parameters							: player_entry - the first score to compare
*													| player_entry - the second score to compare.
* Function Out parameters							: int - the result used to sort the scores.
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
int Difference(player_entry score1, player_entry score2)
{
	int diff = score1.num_bounces - score2.num_bounces;
	if (diff == 0)
		return (int)(score1.elapsed_time - score2.num_bounces);
	else return diff;
}

/******* Function Header **************************************************************************************************************
* Function name										: Sort_vector()
* Purpose											: Rearranges player_entry's in a vector list depending on bounces and time.
* Function In parameters							: vector<player_entry>& - the list of scores to sort
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void Sort_vector(vector<player_entry>& list)
{
	for (unsigned int i = 0; i < list.size(); ++i)
		for (unsigned int j = 0; j < list.size() - 1; ++j)
			if (Difference(list[j], list[j + 1]) > 0)
			{
				player_entry temp = list[j];
				list[j] = list[j + 1];
				list[j + 1] = temp;
			}
}

/******* Function Header **************************************************************************************************************
* Function name										: Input()
* Purpose											: Handles player input.
* Function In parameters							: float& - the burn value to modify based on input
*													| bool& - did the player press 'q'uit?
*													| int& - used to determine if the player has pressed a key yet?
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void Input(float& b, bool& quit, int& kbhit)
{
	int key = 0;
	if (_kbhit())
	{
		key = _getch();
		if (key == 'q')
			quit = false;

		if (key == 'w')
		{
			b += 1;
			if (kbhit == 0 || kbhit == 2)
				system("CLS");
			kbhit = 1;
		}
		else if (key == 'e')
		{
			if(b > 0)
			b -= 1;
			if (kbhit == 0 || kbhit == 1)
				system("CLS");
			kbhit = 2;
		}
	}
}

/******* Function Header **************************************************************************************************************
* Function name										: DrawShip()
* Purpose											: Renders an ASCII ship drawing, which changes depending on thrust and velocity
*													| direction values.
* Function In parameters							: HANDLE - the window handle, used for colours
*													| bool - is the ship able to use it's thrusters still
*													| float - how much thrust is the ship using
*													| bool - has the ship changed direction due to a bounce, etc.
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void DrawShip(HANDLE h, bool hasBoost, float b, bool invert)
{
	if (invert)
	{
		Print(h, CYAN_TEXT,  "     .      ", true);
		Print(h, WHITE_TEXT, "    .'.     ", true);
		Print(h, WHITE_TEXT, "    |o|     ", true);
		Print(h, CYAN_TEXT,  "   .'o'.    ", true);
		Print(h, PINK_TEXT,  "   |.-.|    ", true);
		Print(h, GREEN_TEXT, "   '   '    ", true);
		
		if (hasBoost && b > 0)
		{
			Print(h, YELLOW_TEXT, "    ( )     ", true);
			Print(h, YELLOW_TEXT, "     )      ", true);
			Print(h, (b < 4) ? 0 : RED_TEXT,    "    ( )     ", true);
		}
		else
		{
			Print(h, WHITE_TEXT, "            ", true);
			Print("            ", true);
			Print("            ", true);
		}
	}
	else
	{
		if (hasBoost && b > 0)
		{
			Print(h, WHITE_TEXT,	"            ", true);
			Print(					"            ", true);
			Print(					"            ", true);
			Print(h, GREEN_TEXT,	"   '   '    ", true);
			Print(h, PINK_TEXT,		"   |.-.|    ", true);
			Print(h, CYAN_TEXT,		"   .'o'.    ", true);
			Print(					" ", false); 
			Print(h, (b < 4) ? YELLOW_TEXT : RED_TEXT,		  "/", false);
			Print(h, WHITE_TEXT,		"  |o|   ", false);
			Print(h, (b < 4) ? YELLOW_TEXT : RED_TEXT,					"\\", false);
			Print(									"  ", true);
			Print(h, WHITE_TEXT,	"    .'.     ", true);
			Print(h, CYAN_TEXT,		"     .      ", true);
		}
		else
		{
			Print(h, WHITE_TEXT,	"            ", true);
			Print(					"            ", true);
			Print(					"            ", true);
			Print(h, GREEN_TEXT,	"   '   '    ", true);
			Print(h, PINK_TEXT,		"   |.-.|    ", true);
			Print(h, CYAN_TEXT,		"   .'o'.    ", true);
			Print(h, WHITE_TEXT,	"    |o|     ", true);
			Print(h, WHITE_TEXT,	"    .'.     ", true);
			Print(h, CYAN_TEXT,		"     .      ", true);
		}
	}
}

/******* Function Header **************************************************************************************************************
* Function name										: Print()
* Purpose											: Used to print out text to the screen with any variable type, and truncates numbers 
*													| to 3 decimal places, with an option to add a '\n' character as well.
* Function In parameters							: template<typename T> - used to take in any primitive variable type.
*													| bool - do we want to insert a newline character into the output stream or not.
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
template<typename T> void Print(T param, bool endline)
{
	cout << setprecision(3) << fixed << param;
	if (endline)
		cout << endl;
}

/******* Function Header **************************************************************************************************************
* Function name										: Print()
* Purpose											: Used to print out text to the screen with any variable type, text colour, and truncates numbers
*													| to 3 decimal places, with an option to add a '\n' character as well.
* Function In parameters							: HANDLE - the window handle 
*													| WORD - used to pass colour value enums to determine the console text colour.
*													| template<typename T> - used to take in any primitive variable type.
*													| bool - do we want to insert a newline character into the output stream or not.
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
template<typename T> void Print(HANDLE h, WORD wAttributes, T param, bool endline)
{
	SetConsoleTextAttribute(h, wAttributes);
	cout << setprecision(3) << fixed << param;
	if (endline)
		cout << endl;
}

/******* Function Header **************************************************************************************************************
* Function name										: ToString()
* Purpose											: Used to convert numbers into strings for easier parsing and chaining of values 
*													| to print out.
* Function In parameters							: const template<typename T>& - used to take in any primitive variable type.
* Function Out parameters							: string - the parameter converted into a string using stringstream string.
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
template <typename T> string ToString(const T& n)
{
	std::ostringstream ss;
	ss << n;
	return ss.str();
}

/******* Function Header **************************************************************************************************************
* Function name										: ShowGameOverText()
* Purpose											: Used to display a GAME OVER graphic using ASCII text and various colours to the console.
* Function In parameters							: HANDLE - the window handle
*													| COORD - the cursor position, used to print at the top of the screen
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void ShowGameOverText(HANDLE h, COORD cursor)	//http://patorjk.com/software/taag/#p=display&f=Varsity&t=GAME%0AOVER
{
	int bigsleep = 1000, smallsleep = 150;
	system("CLS");
	Sleep(bigsleep);
	cursor.X = cursor.Y = 0;
	Print(h, WHITE_TEXT | 5 << 4, "   ______       _       ____    ____  ________  ", true);
	Sleep(smallsleep);
	Print(" .' ___  |     / \\     |_   \\  /   _||_   __  | ", true);
	Sleep(smallsleep);
	Print("/ .'   \\_|    / _ \\      |   \\/   |    | |_ \\_| ", true);
	Sleep(smallsleep);
	Print("| |   ____   / ___ \\     | |\\  /| |    |  _| _  ", true);
	Sleep(smallsleep);
	Print("\\ `.___]  |_/ /   \\ \\_  _| |_\\/_| |_  _| |__/ | ", true);
	Sleep(smallsleep);
	Print(" `._____.'|____| |____||_____||_____||________| ", true);
	Sleep(bigsleep);
	Print("   ___   ____   ____  ________  _______         ", true);
	Sleep(smallsleep);
	Print(" .'   `.|_  _| |_  _||_   __  ||_   __ \\        ", true);
	Sleep(smallsleep);
	Print("/  .-.  \\ \\ \\   / /    | |_ \\_|  | |__) |       ", true);
	Sleep(smallsleep);
	Print("| |   | |  \\ \\ / /     |  _| _   |  __ /        ", true);
	Sleep(smallsleep);
	Print("\\  `-'  /   \\ ' /     _| |__/ | _| |  \\ \\_      ", true);
	Sleep(smallsleep);
	Print(" `.___.'     \\_/     |________||____| |___|     ", true);
	Sleep(bigsleep / 2);
}