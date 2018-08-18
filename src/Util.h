/* 
 * File:   Util.h
 * Author: Kapil Thakkar
 */

#ifndef UTIL_H
#define	UTIL_H

#include<vector>

/**
 * Utility Function to split string message, using give delimiter. The result is stored in result[] array.
 * 
 * @param message string to be splitted
 * @param delimter delimiter used to split string
 * @param result result will be stored in this array
 * @param expcted length of the result
 */
void splitString(std::string message, std::string delimiter, std::vector<std::string> result, int n);

#endif	/* UTIL_H */