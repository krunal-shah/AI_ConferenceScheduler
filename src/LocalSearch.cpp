/* 
 * File:   LocalSearch.cpp
 * Author: Kapil Thakkar
 * 
 */

#include "LocalSearch.h"
#include "Util.h"
#include <time.h>       /* time */


LocalSearch::LocalSearch ( )
{
    parallelTracks = 0;
    papersInSession = 0;
    sessionsInTrack = 0;
    processingTimeInMinutes = 0;
    tradeoffCoefficient = 1.0;
}

LocalSearch::LocalSearch ( string filename )
{
    readInInputFile ( filename );
    conference = new Conference ( parallelTracks, sessionsInTrack, papersInSession );
    currentScore = 0;
    bestScore = 0;
}

void printMap(vector<pair<int, double>> tmap)
{
	for (auto&elem : tmap)
	{
		cout << elem.first << " " << elem.second << endl;
	}
	cout << "end of map printing\n";
}

void LocalSearch::organizePapers ( )
{
	// initialised start state
    getStartState();
    currentScore = scoreOrganization();
    bestState = new Conference(conference);
    bestScore = scoreOrganization(bestState);
    // bestScore = currentScore;
    cout << "Start state exited with score " << currentScore << endl;
    system("pause");
    int stepsToClimb = (parallelTracks <= 8)?pow(2*parallelTracks, parallelTracks):1000000;
    int tempStepsLimit = -1;
    int stepNumber = 0;
    vector<double> stepInfo(6);
    cout << "Steps" << stepsToClimb << endl;
    while(stepNumber < stepsToClimb)
    {
        cout << "Computing step\n";
        stepNumber++;
        computeBestTransition(stepInfo);
		
        cout << "Next step computed:\n";
        cout << "time = " << stepInfo[4] << " change in score = " << stepInfo[5] << endl;
        cout << "sessionOne = " << stepInfo[0] << " paperOne = " << stepInfo[1] << endl;
        cout << "sessionTwo = " << stepInfo[2] << " paperTwo = " << stepInfo[3] << endl;  
        
        decideStep(stepInfo, tempStepsLimit);
        
        if(tempStepsLimit > 0)
        {
            tempStepsLimit--;
            if(tempStepsLimit == 0)
            break;
        }
    }
    bestScore = scoreOrganization(bestState);
    // int paperCounter = 0;
    // for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ )
    // {
    //     for ( int j = 0; j < conference->getParallelTracks ( ); j++ )
    //     {
    //         for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
    //         {
    //             conference->setPaper ( j, i, k, paperCounter );
    //             paperCounter++;
    //         }
    //     }
    // }
}

void LocalSearch::updateState()
{
    delete bestState;
    bestState = new Conference(conference);
    bestScore = scoreOrganization(bestState);
}

void LocalSearch::decideStep(vector<double> stepInfo, int &tempStepsLimit)
{
    double bestStepScore = stepInfo[5];
    int sessionOne = stepInfo[0];
    int paperOne = stepInfo[1];
    int sessionTwo = stepInfo[2];
    int paperTwo = stepInfo[3];
    int trackIndex = stepInfo[4];
    
    if(bestStepScore < 0)
    {
        tempStepsLimit = 10;
        
        Track *curTrack = conference->getTrack(trackIndex);
		Session *curSessionOne = curTrack->getSession(sessionOne);
        Session *curSessionTwo = curTrack->getSession(sessionTwo);
        int tempPaperId = curSessionOne->getPaper(paperOne);
        curSessionOne->setPaper(paperOne, curSessionTwo->getPaper(paperTwo));
        curSessionTwo->setPaper(paperTwo, tempPaperId);
    }
    else if(bestStepScore > 0)
    {
        tempStepsLimit = -1;

        Track *curTrack = conference->getTrack(trackIndex);
		Session *curSessionOne = curTrack->getSession(sessionOne);
        Session *curSessionTwo = curTrack->getSession(sessionTwo);
        int tempPaperId = curSessionOne->getPaper(paperOne);
        curSessionOne->setPaper(paperOne, curSessionTwo->getPaper(paperTwo));
        curSessionTwo->setPaper(paperTwo, tempPaperId);
    }
    else
    {
        tempStepsLimit = 20;

        Track *curTrack = conference->getTrack(trackIndex);
		Session *curSessionOne = curTrack->getSession(sessionOne);
        Session *curSessionTwo = curTrack->getSession(sessionTwo);
        int tempPaperId = curSessionOne->getPaper(paperOne);
        curSessionOne->setPaper(paperOne, curSessionTwo->getPaper(paperTwo));
        curSessionTwo->setPaper(paperTwo, tempPaperId);
    }
    currentScore += bestStepScore;

    if(currentScore > bestScore)
    {
        cout << "Updating bestState\n";
        updateState();
    }
}

void LocalSearch::computeBestTransition(vector<double> &best_results)
{
	//considering costs for swapping two members in the same row, for every row, to constitute transition space
	double bestcost = -1000000000;
	best_results[0] = 0;
    best_results[1] = 0;
    best_results[2] = 0;
    best_results[3] = 0;
    best_results[4] = 0;
    best_results[5] = -1000000000;

	for (int i = 0; i < conference->getParallelTracks(); i++)
	{
		//for every "row"
		Track *curtrack = this->conference->getTrack(i);
		for (int j = 0; j < curtrack->getNumberOfSessions() - 1; j++)
		{//first session 
			for (int k = j + 1; k < curtrack->getNumberOfSessions(); k++)
			{//second session
				for (int l = 0; l < papersInSession; l++)
				{//compare cost of every paper of first session with second session 
					for (int m = 0; m < papersInSession; m++)
					{
						double tempcost = scoreSwitch(i, j, l, k, m);
						if (tempcost > bestcost)
						{
							bestcost = tempcost;
                            best_results[0] = j;
							best_results[1] = l;
							best_results[2] = k;
							best_results[3] = m;
                            best_results[4] = i;
                            best_results[5] = bestcost;
						}
					}
				}
			}
		}
	}
}

void LocalSearch::getStartState ( )
{
	//for one session: get papers
	unordered_map<int, int> papers_avl;
	for (int i = 0; i < totalpapers; i++)
	{
		papers_avl[i] = 0; //we dont care for value. key=paperID
	}
	unordered_map<int, unordered_map<int, int>> all_sessions;
	int sess_count = -1;
	srand(time(NULL));
	while (papers_avl.size())
	{
		sess_count++;
		//first element chosen
		int firstp = papers_avl.begin()->first;
		papers_avl.erase(firstp);
		unordered_map<int, int> selected; //docID 
		// cout << "Starting a cluster\n";
		// cout << "Added " << firstp << " to cluster\n";
		selected[firstp] = 1;
		
		//make one session:
		while (selected.size() < papersInSession)
		{
			//similarities of rest available
			double c = 0;
			unordered_map<int, double> similarity; //ID to cumulative similarity
			for (auto&availID: papers_avl)
			{
				for (auto&selID : selected)
				{
					similarity[availID.first] += (1.0 - distanceMatrix[selID.first][availID.first]);
					c += 1.0 - distanceMatrix[selID.first][availID.first];
				}
			}
	        vector<pair<int, double>> cprob(papers_avl.size()); // <docID,sum>
			double sum = 0;
			int counter = 0;
			for (auto&elem : similarity)
			{

				sum += elem.second / c;
				cprob[counter++]=make_pair(elem.first, sum);
			}

			//cout << "Printing cprob\n";
			//printMap(cprob);
			
			//choose next randomly
			//can be made faster
			double r = ((double)rand());
			r = r / (double)(RAND_MAX);
			// cout << "Chose r = " << r << " " << RAND_MAX << endl;
			// system("pause");

			for (auto&elem : cprob)
			{
				if (elem.second > r)
				{
					selected[elem.first] = 1;
					papers_avl.erase(elem.first);
					// cout << "Added " << elem.first << " to cluster\n";
					break;
				}
			}
		}
		all_sessions[sess_count] = selected;
	}
    // system("pause");
	
	int sessCounter = 0;
    for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ )
    {
        for ( int j = 0; j < conference->getParallelTracks ( ); j++ )
        {
			unordered_map<int, int> sel=all_sessions[sessCounter];
            //for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
			int counter = 0;
			for (auto&elem: sel)
			{
				conference->setPaper ( j, i, counter++, elem.first);
            }
			sessCounter++;
        }
    }
    cout << "Printing starting conference\n";
	conference->printConferenceStdout();
	// system("pause");
}

void LocalSearch::readInInputFile ( string filename )
{
    vector<string> lines;
    string line;
    ifstream myfile ( filename.c_str () );
    if ( myfile.is_open ( ) )
    {
        while ( getline ( myfile, line ) )
        {
            //cout<<"Line read:"<<line<<endl;
            lines.push_back ( line );
        }
        myfile.close ( );
    }
    else
    {
        cout << "Unable to open input file";
		exit ( 0 );
    }

    if ( 6 > lines.size ( ) )
    {
        cout << "Not enough information given, check format of input file";
        exit ( 0 );
    }

    processingTimeInMinutes = atof ( lines[0].c_str () );
    papersInSession = atoi ( lines[1].c_str () );
    sessionsInTrack = atoi ( lines[2].c_str () );
    parallelTracks = atoi ( lines[3].c_str () );
    tradeoffCoefficient = atof ( lines[4].c_str () );

    int n = lines.size ( ) - 5;
	totalpapers = n;

    double ** tempDistanceMatrix = new double*[n];
    for ( int i = 0; i < n; ++i )
    {
        tempDistanceMatrix[i] = new double[n];
    }

    for ( int i = 0; i < n; i++ )
    {
        string tempLine = lines[ i + 5 ];
        vector<string> elements(n);
        splitString ( tempLine, " ", elements, n );

        for ( int j = 0; j < n; j++ )
        {
            tempDistanceMatrix[i][j] = atof ( elements[j].c_str () );
        }
    }
    distanceMatrix = tempDistanceMatrix;

    int numberOfPapers = n;
    int slots = parallelTracks * papersInSession*sessionsInTrack;
    if ( slots != numberOfPapers )
    {
        cout << "More papers than slots available! slots:" << slots << " num papers:" << numberOfPapers << endl;
        exit ( 0 );
    }
}

double** LocalSearch::getDistanceMatrix ( )
{
    return distanceMatrix;
}

void LocalSearch::printSessionOrganiser ( char * filename)
{
    bestState->printConference ( filename);
}

double LocalSearch::scoreSwitch (int track, int sessionOne, int paperOne, int sessionTwo, int paperTwo)
{
    // Sum of pairwise similarities per session.
    double similarityOne = 0.0;
    double similarityTwo = 0.0;
    double distanceOne = 0.0;
    double distanceTwo = 0.0;
    double similarityOneNew = 0.0;
    double similarityTwoNew = 0.0;
    double distanceOneNew = 0.0;
    double distanceTwoNew = 0.0;
    Track* tmpTrack = conference->getTrack ( track );
    Session* tmpSessionOne = tmpTrack->getSession ( sessionOne );
    Session* tmpSessionTwo = tmpTrack->getSession ( sessionTwo );
    paperOne = tmpSessionOne->getPaper( paperOne );
    paperTwo = tmpSessionTwo->getPaper( paperTwo );

    for ( int k = 0; k < tmpSessionOne->getNumberOfPapers ( ); k++ )
    {
        int index = tmpSessionOne->getPaper ( k );
        if(index != paperOne)
        {
            similarityOne += (1 - distanceMatrix[index][paperOne]);
            similarityTwoNew += (1 - distanceMatrix[index][paperTwo]);
            distanceOneNew += distanceMatrix[index][paperOne];
        }
        distanceTwo += distanceMatrix[index][paperTwo];
    }

    for ( int k = 0; k < tmpSessionTwo->getNumberOfPapers ( ); k++ )
    {
        int index = tmpSessionTwo->getPaper ( k );
        if(index != paperTwo)
        {
            similarityTwo += (1 - distanceMatrix[index][paperTwo]);
            similarityOneNew += (1 - distanceMatrix[index][paperOne]);
            distanceTwoNew += distanceMatrix[index][paperTwo];
        }
        distanceOne += distanceMatrix[index][paperOne];
    }
    distanceOneNew += distanceMatrix[paperOne][paperTwo];
    distanceTwoNew += distanceMatrix[paperOne][paperTwo];

    double score = tradeoffCoefficient*(distanceOneNew + distanceTwoNew + similarityOneNew + similarityTwoNew) - (similarityOne + similarityTwo + distanceOne + distanceTwo);
    return score;
}

double LocalSearch::scoreOrganization ( )
{
    // Sum of pairwise similarities per session.
    double score1 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track* tmpTrack = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack->getNumberOfSessions ( ); j++ )
        {
            Session* tmpSession = tmpTrack->getSession ( j );
            for ( int k = 0; k < tmpSession->getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession->getPaper ( k );
                for ( int l = k + 1; l < tmpSession->getNumberOfPapers ( ); l++ )
                {
                    int index2 = tmpSession->getPaper ( l );
                    score1 += 1 - distanceMatrix[index1][index2];
                }
            }
        }
    }
    
    // Sum of distances for competing papers.
    double score2 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track* tmpTrack1 = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack1->getNumberOfSessions ( ); j++ )
        {
            Session* tmpSession1 = tmpTrack1->getSession ( j );
            for ( int k = 0; k < tmpSession1->getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession1->getPaper ( k );

                // Get competing papers.
                for ( int l = j + 1; l < tmpTrack1->getNumberOfSessions ( ); l++ )
                {
                    Session* tmpSession2 = tmpTrack1->getSession ( l );
                    for ( int m = 0; m < tmpSession2->getNumberOfPapers ( ); m++ )
                    {
                        int index2 = tmpSession2->getPaper ( m );
                        score2 += distanceMatrix[index1][index2];
                    }
                }
            }
        }
    }
    double score = score1 + tradeoffCoefficient*score2;
    return score;
}

double LocalSearch::scoreOrganization ( Conference *conferenceTmp )
{
    // Sum of pairwise similarities per session.
    double score1 = 0.0;
    for ( int i = 0; i < conferenceTmp->getParallelTracks ( ); i++ )
    {
        Track* tmpTrack = conferenceTmp->getTrack ( i );
        for ( int j = 0; j < tmpTrack->getNumberOfSessions ( ); j++ )
        {

            Session* tmpSession = tmpTrack->getSession ( j );
            for ( int k = 0; k < tmpSession->getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession->getPaper ( k );
                for ( int l = k + 1; l < tmpSession->getNumberOfPapers ( ); l++ )
                {
                    int index2 = tmpSession->getPaper ( l );
                    score1 += 1 - distanceMatrix[index1][index2];
                }
            }
        }
    }

    // Sum of distances for competing papers.
    double score2 = 0.0;
    for ( int i = 0; i < conferenceTmp->getParallelTracks ( ); i++ )
    {
        Track* tmpTrack1 = conferenceTmp->getTrack ( i );
        for ( int j = 0; j < tmpTrack1->getNumberOfSessions ( ); j++ )
        {
            Session* tmpSession1 = tmpTrack1->getSession ( j );
            for ( int k = 0; k < tmpSession1->getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession1->getPaper ( k );

                // Get competing papers.
                for ( int l = j + 1; l < tmpTrack1->getNumberOfSessions ( ); l++ )
                {
                    Session* tmpSession2 = tmpTrack1->getSession ( l );
                    for ( int m = 0; m < tmpSession2->getNumberOfPapers ( ); m++ )
                    {
                        int index2 = tmpSession2->getPaper ( m );
                        score2 += distanceMatrix[index1][index2];
                    }
                }
            }
        }
    }
    double score = score1 + tradeoffCoefficient*score2;
    return score;
}

double LocalSearch::scoreBestOrganization()
{
    return scoreOrganization(bestState);
}
