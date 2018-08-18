/* 
 * File:   SessionOrganizer.h
 * Author: Kapil Thakkar
 *
 */

#ifndef LOCALSEARC_H
#define LOCALSEARC_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>


#include "Conference.h"
#include "Track.h"
#include "Session.h"

using namespace std;


/**
 * SessionOrganizer reads in a similarity matrix of papers, and organizes them
 * into sessions and tracks.
 * 
 * @author Kapil Thakkar
 *
 */
class LocalSearch {
public:
    double ** distanceMatrix;

    int parallelTracks ;
    int papersInSession ;
    int sessionsInTrack ;
	int totalpapers;
	
    double currentScore;
    double bestScore;
    Conference *conference;
    Conference *bestState;

    double processingTimeInMinutes ;
    double tradeoffCoefficient ; // the tradeoff coefficient


public:
    LocalSearch();
    LocalSearch(string filename);
    
    
    /**
     * Read in the number of parallel tracks, papers in session, sessions
     * in a track, and the similarity matrix from the specified filename.
     * @param filename is the name of the file containing the matrix.
     * @return the similarity matrix.
     */
    void readInInputFile(string filename);
    
    
    
    /**
     * Organize the papers according to some algorithm.
     */
	void organizePapers();


    
    /**
     * Get the distance matrix.
     * @return the distance matrix.
     */
    double** getDistanceMatrix();
    
    
    /**
     * Score the organization.
     * @return the score.
     */
    double scoreOrganization();
    double scoreOrganization ( Conference *conference );

    double scoreSwitch (int track, int sessionOne, int paperOne, int sessionTwo, int paperTwo);


	void getStartState ( );

	void computeBestTransition(vector<double>&);

    void LocalSearch::decideStep(vector<double>, int &);

    void updateState();

	double LocalSearch::scoreBestOrganization();
    
    void printSessionOrganiser(char *);
};

#endif  /* LOCALSEARC_H */

