/* 
 * File:   Util.cpp
 * Author: Suyash Agrawal
 */

#include <string>
#include <iostream>
#include "Util.h"

using namespace std;

void splitString(std::string message, std::string delimiter, vector<string> &result, int n) {
    int i = 0, pos = 0, length = 0, temp;
    temp = message.find ( delimiter.c_str ( ), pos );
    
    while ( temp != -1 )
    {
        length = temp - pos;
        result[i] = message.substr ( pos, length );
        pos = temp + delimiter.size ( );
        temp = message.find ( delimiter.c_str ( ), pos );
        i++;
    }
    result[i] = message.substr ( pos );
    i++;
    if ( i != n )
    {
        std::cout << "The similarity matrix does not have the correct format.";
        exit ( 0 );
    }
}