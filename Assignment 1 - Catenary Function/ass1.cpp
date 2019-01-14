/******* File Header : Cover **********************************************************************************************************
* Student name										: John Dobie
* Student number									: 040 659 609
* Assignment number (and name if any)				: Assignment 1, Catenary Series Project in C++
* Course name and number							: CST 8233 – F18 – Numerical Computing
* Lab section number								: 304
* Professor’s name									: Andrew Tyler
* Due date of assignment							: Saturday, October 14th, 2018
* Submission date of assignment						: Wednesday, October 12th, 2018
* List of source and header files in the project	: ass1.cpp
***************************************************************************************************************************************/
/******* File Header ******************************************************************************************************************
* Filename											: ass1.cpp
* Version											: 1.0
* Purpose											: To get user input and display values for a catenary function approximation for
*													: different values plugged into a Maclaurin series formula.
**************************************************************************************************************************************/

#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;

#define ERR_MSG "Invalid input, try again."

/******* Function Header **************************************************************************************************************
* Function name										: main()
* Purpose											: To handle the core of our program, entry point and exit upon success.
*													| 1. Evaluate the catenary function, 2. Quit out of the program.
* Function In parameters							: none
* Function Out parameters							: none
* Version											: 1.0
* Author											: John Dobie
**************************************************************************************************************************************/
int main()
{
	int input = 0;

	while (input != 2)
	{

		cout << "**********************************" << endl;
		cout << "        Catenary Series" << endl;
		cout << "1. Evaluate the function." << endl;
		cout << "2. Quit." << endl;
		cout << "**********************************" << endl;

		while (!(cin >> input) || (input != 1 && input != 2))
		{
			cout << ERR_MSG << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			//http://www.dietmar-kuehl.de/mirror/c++-faq.old/input-output.html 
			//https://en.cppreference.com/w/cpp/io/basic_istream/ignore
		}

		if (input == 2)
		{
			cout << "Quitting... press any key to terminate" << endl;
		}
		else
		{
			cout << "       EVALUATING THE CATENARY SERIES APPROXIMATION" << endl;
			cout << endl;
			cout << "Please enter the highest power of x in the catenary series(0, 2, 4, 6, 8, or 10) : ";
			
			//handleInputI(input, 1);
			while (!(cin >> input) || input < 0 || input > 10 || input % 2 != 0)
			{
				cout << ERR_MSG << endl;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			float low = 1.0f, high = -1.0f, scale = 0.0f;
			cout << endl << "       CHOOSE THE RANGE OF EVALUATION - low x to high x" << endl;
			cout << "Please enter low x - in the range - 10.0 to 0.0: ";
			
			//handleInputf(low, -10.0f, 0.0f);
			while (!(cin >> low) || low < -10.0f || low > 0.0f)
			{
				cout << ERR_MSG << endl;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			cout << "Please enter high x - in the range 0.0 to + 10.0 : ";
			
			//handleInputf(high, 0.0f, 10.0f);
			while (!(cin >> high) || high < 0.0f || high > 10.0f)
			{
				cout << ERR_MSG << endl;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			cout << endl << "Please enter the scale factor in the range 0.0 to + 10.0: ";
			
			//handleInputf(scale, 0.0f, 10.0f);
			while (!(cin >> scale) || scale < 0.0f || scale > 10.0f)
			{
				cout << ERR_MSG << endl;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}

			double epsilon = (high - low) / 10.0f;	//increment
			double x = 0.0f;						//current value we are evaluating
			double result = 0.0f;					//our series approximation result
			double exact = 0.0f;					//the exact series result provided by the math library
			double exactErr = 0.0f;					//how far away we are with our approximation
			double truncErr = 0.0f;					//how far off our truncated term is from the series
			double truncatedTerm = 0.0f;			//what our first truncated term is

			cout << fixed;							//set up our math notation display
			cout.precision(6);
			cout << endl << "CATENARY SERIES TO x^" << input << " from x = " << low << " to x = " << high << endl;
			cout << " x\t\tSeries\t\tExact\t\tExact % Error\tTrunc. % Error" << endl;

			cout << scientific;						//modify our math notation display

			for (int i = 0; i <= 10; ++i)
			{
				result = 0.0f;
				truncatedTerm = 0.0f;
				x = (low + (epsilon * i));

				//get our first truncated term, breaking out from the switch once we calculate the value.
				switch (input + 2)
				{
				case 12:
					truncatedTerm += scale * ((x*x*x*x*x*x*x*x*x*x*x*x) / ((scale*scale*scale*scale*scale*scale*scale*scale*scale*scale*scale*scale) * (1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10 * 11 * 12)));
					break;
				case 10:
					truncatedTerm += scale * ((x*x*x*x*x*x*x*x*x*x) / ((scale*scale*scale*scale*scale*scale*scale*scale*scale*scale) * (1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10)));
					break;
				case 8:
					truncatedTerm += scale * ((x*x*x*x*x*x*x*x) / ((scale*scale*scale*scale*scale*scale*scale*scale) * (1 * 2 * 3 * 4 * 5 * 6 * 7 * 8)));
					break;
				case 6:
					truncatedTerm += scale * ((x*x*x*x*x*x) / ((scale*scale*scale*scale*scale*scale) * (1 * 2 * 3 * 4 * 5 * 6)));
					break;
				case 4:
					truncatedTerm += scale * ((x*x*x*x) / ((scale*scale*scale*scale) * (1 * 2 * 3 * 4)));
					break;
				case 2:
					truncatedTerm += scale * ((x*x) / ((scale*scale) * (1 * 2)));
					break;
				default:
					cout << "error in truncated term calculation";
					break;
				}

				//start with our highest power and add each term going down towards zero(0). break upon last.
				switch (input)
				{
				case 10:	//bring the fraction of e^x/a down to the denominator and multiply it by the factorial instead		
					result += scale * ((x*x*x*x*x*x*x*x*x*x) / ((scale*scale*scale*scale*scale*scale*scale*scale*scale*scale) * (1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10)));
				case 8:
					result += scale * ((x*x*x*x*x*x*x*x) / ((scale*scale*scale*scale*scale*scale*scale*scale) * (1 * 2 * 3 * 4 * 5 * 6 * 7 * 8)));
				case 6:
					result += scale * ((x*x*x*x*x*x) / ((scale*scale*scale*scale*scale*scale) * (1 * 2 * 3 * 4 * 5 * 6)));
				case 4:
					result += scale * ((x*x*x*x) / ((scale*scale*scale*scale) * (1 * 2 * 3 * 4)));
				case 2:
					result += scale * ((x*x) / ((scale*scale) * (1 * 2)));
				case 0:
					result += scale;
					break;
				default:
					cout << "error in series calculation";
					break;
				}

				exact = scale * (exp(x / scale) + exp(-x / scale)) / 2;
				exactErr = 100 * (exact - result) / exact;	//exact error formula: 100*(exact value – series value)/exact value
				truncErr = 100 * truncatedTerm / result;	//truncated error formula: 100 * first truncated term / series value.

				cout << setprecision(3) << showpos << x << "\t"
					<< noshowpos << setprecision(5) << result << "\t"
					<< exact << "\t" << exactErr << "\t" << truncErr << endl;
			}
			cout << endl;
			input = 0;
		}
	}
	
	return 0;
}

#if input functions
/*void handleInputI(int & input, int flag)
{
	if (flag == 0)
		while (!(cin >> input) || (input != 1 && input != 2))
		{
			cout << "Invalid input, try again." << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			//http://www.dietmar-kuehl.de/mirror/c++-faq.old/input-output.html 
			//https://en.cppreference.com/w/cpp/io/basic_istream/ignore
		}
	else if (flag == 1)
		while (!(cin >> input) || input < 0 || input > 10 || input % 2 != 0)
		{
			cout << "Invalid input, try again." << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
}

void handleInputf(float & input, float min, float max)
{
	while (!(cin >> input) || input < min || input > max)
	{
		cout << "Invalid input, try again." << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
}*/
#endif