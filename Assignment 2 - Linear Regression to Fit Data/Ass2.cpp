/******* File Header : Cover **********************************************************************************************************
* Student name										: John Dobie
* Student number									: 040 659 609
* Assignment number (and name if any)				: Assignment 2, Linear Regression Project in C++
* Course name and number							: CST 8233 – F18 – Numerical Computing
* Lab section number								: 304
* Professor’s name									: Andrew Tyler
* Due date of assignment							: Saturday, November 11th, 2018
* Submission date of assignment						: Wednesday, November 11th, 2018
* List of source and header files in the project	: ass2.cpp
***************************************************************************************************************************************/
/******* File Header ******************************************************************************************************************
* Filename											: ass2.cpp
* Version											: 1.0
* Purpose											: To get user input and display values linear regression of numbers based on two
*													: seperate formulas, linear and powe law.
**************************************************************************************************************************************/
#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

void menu();

/******* Function Header **************************************************************************************************************
* Function name										: main()
* Purpose											: To handle the core of our program, entry point and exit upon success.
* Function In parameters							: none
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	menu();
	return 0;
}

bool subMenu(bool function);
void runProgram(bool function);
void squares(int x_modifier, float y_modifier, double& m, double& b, vector<vector<double>> xy);
void calculateLinearRegression(vector<vector<double>> xy, bool linearFunction, double& m, double& b);
double calculateLinearRegression(vector<vector<double>> xy, int x, bool linearFunction);
vector<vector<double>> parse(string filename, int& n, bool linearFunction);

/******* Function Header **************************************************************************************************************
* Function name										: menu()
* Purpose											: To handle the menu loop of our program.
*													| 1. Evaluate Linear Fit, 2. Evaluate Power Law Fit, 3. Quit out of the program.
* Function In parameters							: none
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void menu()
{
	bool alive = true;
	int option = -1;

	cout << "\tLEAST_SQUARES LINEAR REGRESSION" << endl;
	while (alive)
	{
		cout << "MENU" << endl << " 1. Linear Fit" << endl << " 2. Power Law Fit" << endl << " 3. Quit" << endl;
		cin >> option;
		switch (option)
		{
		case 1: runProgram(true);
			break;
		case 2: runProgram(false);
			break;
		case 3: alive = !alive;
			break;
		}
	}
}

/******* Function Header **************************************************************************************************************
* Function name										: subMenu()
* Purpose											: To handle the inner menu loop of our program after initial evaluations.
*													| 1. Interpolate/Exterpolate values, 2. Quit back to the main menu.
* Function In parameters							: bool - the type of function we are evaluating
* Function Out parameters							: bool - should the loop continue?
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
bool subMenu(bool linearFunction)
{
	int option = -1;
	cout << "MENU" << endl << " 1. Interpolate / Extrapolate" << endl << " 2. Main Menu" << endl;
	cin >> option;
	switch (option)
	{
		case 1: return true;
		case 2: return false;
		default: return true;
	}
}

/******* Function Header **************************************************************************************************************
* Function name										: runProgram()
* Purpose											: To take user input and display the calculated output.
* Function In parameters							: bool - the type of function we are evaluating
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void runProgram(bool linearFunction)
{
	string filename = "";
	cout << "Please enter the name of the file to open: ";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(cin, filename);
	
	int n;
	double a, b;
	vector<vector <double>> xyValues = parse(filename, n, linearFunction);
	calculateLinearRegression(xyValues, linearFunction, a, b); //y = mx + b , y = ax^b

	cout << "There are " << n << " records." << endl;
	if (n == 0) return;

	a = round(a * 100.0) / 100.0;
	b = round(b * 100.0) / 100.0;
	linearFunction ? cout << "y = " << a << "x + " << b << endl : cout << "y = " << b << "x^" << a << endl;
	
	while (subMenu(linearFunction))	//interpolation and exterpolation values
	{
		cout << "Please enter the year to interpolate/extrapolate to: ";
		cin >> n;
		cout << "The " << (linearFunction ? "linear" : "power law") << " interpolated / extrapolated CO2 level in the year " << n << " is " << 
			(round(calculateLinearRegression(xyValues, n, linearFunction) * 100.0) / 100.0) << endl;
	}
}

/******* Function Header **************************************************************************************************************
* Function name										: parse()
* Purpose											: To parse the input file into numerical values to process.
* Function In parameters							: bool - the type of function we are evaluating
*													| int& - the number of records in the file
*													| string - the name of the file to open
* Function Out parameters							: vector<vector<double>> - a 2d vector array list that contains our values
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
vector<vector<double>> parse(string filename, int& n, bool linearFunction)
{
	vector<vector<double>> xyValues;
	n = 0;
	ifstream file;
	file.open(filename, ios::in);

	if (file.is_open())
	{
		string line, delimiter = " ";
		while (getline(file, line))
		{
			string xStr = line.substr(0, line.find(delimiter));
			string yStr = line.substr(xStr.length() + 1, line.length() - xStr.length());

			double x = stod(xStr, nullptr);
			double y = stod(yStr, nullptr);

			if (!linearFunction)
			{
				x = log(x); y = log(y);
			}

			vector<double> column;
			column.push_back(x);
			column.push_back(y);
			xyValues.push_back(column);
			n++;
		}
	}
	else
	{
		cout << "Error opening file." << endl;
		return xyValues;
	}
	return xyValues; //where [t][xy] t is the column, [0] is x, [1] is y 
}

/******* Function Header **************************************************************************************************************
* Function name										: calculateLinearRegression()
* Purpose											: To calculate the initial slope and intercept of the parsed file values.
* Function In parameters							: double& - intercept
*													| double& - slope
*													| bool - the type of function we are evaluating
*													| vector<vector<double>> - the 2d vector array of values to use
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void calculateLinearRegression(vector<vector<double>> xy, bool linearFunction, double& m, double& b)
{
	squares(0, 0, m, b, xy);
	if (!linearFunction) b = exp(b);
}

/******* Function Header **************************************************************************************************************
* Function name										: calculateLinearRegression()
* Purpose											: To calculate the result of the linear regression function based on passed in values.
* Function In parameters							: bool - the type of function we are evaluating
*													| int - the x value to run the function on
*													| vector<vector<double>> - the 2d vector array of values to use
* Function Out parameters							: double - the calculated result of the function
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
double calculateLinearRegression(vector<vector<double>> xy, int x, bool linearFunction)
{
	double m = 0;
	double b = 0;
	double result;
	linearFunction ? squares(1960, 316.5, m, b, xy) : squares(0, 0, m, b, xy);
	linearFunction ? result = ((m*x) + b) : result = (exp(b) * pow(x - 1960, m) + 316.5);
	return result;
}

/******* Function Header **************************************************************************************************************
* Function name										: squares()
* Purpose											: To loop through the input data and calculate required sigmas to perform the 
*													| linear and power fit functions.
* Function In parameters							: vector<vector<double>> - the data set to use
*													| double& - the calculated intercept
*													| double& - the calculated slope
*													| float - the y value modifier for our function
*													| float - the x value modifier for our function
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
void squares(int x_modifier, float y_modifier, double& m, double& b, vector<vector<double>> xy)
{
	double xs = 0.0;	//sigma X
	double xs2 = 0.0;	//sigma X^2
	double ys = 0.0;	//sigma Y
	double xys = 0.0;	//sigma XY
	m = 0.0;			//slope
	b = 0.0;			//intercept
	int n = xy.size();	

	for (int i = 0; i < n; ++i)
	{
		xs += xy[i][0] + x_modifier;								//calculate sigma x
		ys += xy[i][1] + y_modifier;								//calculate sigma y
		xs2 += pow(xy[i][0] + x_modifier, 2);						//calculate sigma x^2
		xys += ((xy[i][0] + x_modifier) * (xy[i][1] + y_modifier));	//calculate sigma xy
	}

	m = ((n*xys) - (xs * ys)) / ((n*xs2) - (xs*xs));
	b = ((ys*xs2) - (xs*xys)) / ((n*xs2) - (xs*xs));
}