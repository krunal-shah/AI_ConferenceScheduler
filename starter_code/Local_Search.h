/* 
 * File:   SessionOrganizer.h
 * Author: Kapil Thakkar
 *
 */

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
private:
    double ** distanceMatrix;

    int parallelTracks ;
    int papersInSession ;
    int sessionsInTrack ;

    Conference *conference;

    double processingTimeInMinutes ;
    double tradeoffCoefficient ; // the tradeoff coefficient


public:
    LocalSearch();
    LocalSearch(string filename);
    
    
    /**
     * Organize the papers according to some algorithm.
     */
    void initializeStartState();

    void printLocalSearch(char *);
};

#endif  /* SESSIONORGANIZER_H */

