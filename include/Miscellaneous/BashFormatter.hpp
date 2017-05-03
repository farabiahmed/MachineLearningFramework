/*
 * BashFormatter.hpp
 *
 *  Created on: Apr 13, 2017
 *      Author: farabiahmed
 */

#ifndef MISCELLANEOUS_BASHFORMATTER_HPP_

#include <string>
// Various Examples about Bash Colors and Formatting
// http://misc.flogisoft.com/bash/tip_colors_and_formatting

	/*
	Foreground colors
	30	Black
	31	Red
	32	Green
	33	Yellow
	34	Blue
	35	Magenta
	36	Cyan
	37	White

	Background colors
	40	Black
	41	Red
	42	Green
	43	Yellow
	44	Blue
	45	Magenta
	46	Cyan
	47	White
	*/

using namespace std;

class Formatter {

public:
    static const string HEADER;
    static const string OKBLUE;
    static const string OKGREEN;
    static const string WARNING;
    static const string FAIL;
    static const string RESET;
    static const string BOLD;
    static const string UNDERLINE;
    static const string BLINK;

    static const string BLACK;
    static const string RED;
    static const string GREEN;
    static const string YELLOW;
    static const string BLUE;
    static const string MAGENTA;
    static const string CYAN;
	static const string WHITE;

	static const string LIGHTGREEN;

	static const string BG_GREEN;
};

// Most Used Commands
const string Formatter::HEADER 		= "\033[95m";
const string Formatter::OKBLUE 		= "\033[94m";
const string Formatter::OKGREEN 	= "\033[92m";
const string Formatter::WARNING 	= "\033[93m";
const string Formatter::FAIL 		= "\033[91m";
const string Formatter::RESET 		= "\033[0m";
const string Formatter::BOLD 		= "\033[1m";
const string Formatter::UNDERLINE 	= "\033[4m";
const string Formatter::BLINK 		= "\033[5m";

// Colors
const string Formatter::BLACK 		= "\033[30m";      /* Black */
const string Formatter::RED  		= "\033[31m";      /* Red */
const string Formatter::GREEN 		= "\033[32m";      /* Green */
const string Formatter::YELLOW 		= "\033[33m";      /* Yellow */
const string Formatter::BLUE 		= "\033[34m";      /* Blue */
const string Formatter::MAGENTA 	= "\033[35m";      /* Magenta */
const string Formatter::CYAN 		= "\033[36m";      /* Cyan */
const string Formatter::WHITE 		= "\033[37m";      /* White */

// Light Colors
const string Formatter::LIGHTGREEN	= "\033[92m";

// Background Colors
const string Formatter::BG_GREEN 	= "\033[42m";


#endif /* MISCELLANEOUS_BASHFORMATTER_HPP_ */
