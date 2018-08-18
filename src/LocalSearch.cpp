/* 
 * File:   LocalSearch.cpp
 * Author: Kapil Thakkar
 * 
 */

#include "LocalSearch.h"
#include "Util.h"

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
}

void LocalSearch::organizePapers ( )
{
	//for one session: get papers
	unordered_map<int, int> papers_avl;
	cout << "totalpapers" << totalpapers << endl;
	for (int i = 0; i < totalpapers; i++)
	{
		papers_avl[i] = 0; //we dont care for value. key=paperID
	}
	unordered_map<int, unordered_map<int, int>> all_sessions;
	int sess_count = -1;
	
	while (papers_avl.size())
	{
		cout << papers_avl.size()<<endl;
		sess_count++;
		//first element chosen
		int firstp = papers_avl.begin()->first;
		cout << firstp << endl;
		papers_avl.erase(firstp);
		unordered_map<int, int> selected; //docID 
		selected[firstp] = 1;
		
		//make one session:
		cout << selected.size() << " " << papersInSession << endl;
		while (selected.size() < papersInSession)
		{
			//similarities of rest available
			cout << selected.size() << endl;
			double c = 0;
			unordered_map<int, double> sim; //ID to cumulative similarity
			for (auto&mapelem: papers_avl)
			{
				for (auto&elem : selected)
				{
					sim[mapelem.first] += (1.0 - distanceMatrix[elem.first][mapelem.first]);
					c += 1.0 - distanceMatrix[elem.first][mapelem.first];
				}
			}
	        //cprobs of available
			vector<pair<int, int>> cprob(papers_avl.size()); // <docID,sum>
			double sum = 0;
			int counter = 0;
			for (auto&elem : sim)
			{
				sum += elem.second / c;
				//cout << "sum:" << sum << endl;
				cprob[counter++]=make_pair(elem.first, sum);
			}
			//cout<<sum<<endl;

			//choose next randomly
			//can be made faster
			double r = ((double)rand() / (RAND_MAX));
			for (auto&elem : cprob)
			{
				if (elem.second > r)
				{
					selected[elem.first] = 1;
					papers_avl.erase(elem.first);
				}
			}
		}
		all_sessions[sess_count] = selected;
	}

	int sessCounter = 0;
	cout << "it is here:" << endl;
	system("pause");
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
	conference->printConferenceStdout();
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
		system("pause");
    }
    else
    {
        cout << "Unable to open input file";
		system("pause");
        exit ( 0 );
    }

    if ( 6 > lines.size ( ) )
    {
        cout << "Not enough information given, check format of input file";
        exit ( 0 );
    }

    processingTimeInMinutes = atof ( lines[0].c_str () );
    papersInSession = atoi ( lines[1].c_str () );
    parallelTracks = atoi ( lines[2].c_str () );
    sessionsInTrack = atoi ( lines[3].c_str () );
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
    conference->printConference ( filename);
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
                for ( int l = i + 1; l < conference->getParallelTracks ( ); l++ )
                {
                    Track* tmpTrack2 = conference->getTrack ( l );
                    Session* tmpSession2 = tmpTrack2->getSession ( j );
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
