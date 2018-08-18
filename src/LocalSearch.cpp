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
	vector<int> papers_avl(totalpapers);
	for (int i = 0; i < totalpapers; i++)
	{
		papers_avl[i] = i;
	}
	while (papers_avl.size())
	{
		//first element chosen
		int firstp = papers_avl[0];
		papers_avl.erase(papers_avl.begin());
		unordered_map<int, int> selected;
		selected[firstp] = 1;
		
		//make one session:
		while (selected.size() < papersInSession)
		{
			//similarities of rest available
			double c = 0;
			unordered_map<int, double> sim; //ID to cumulative similarity
			for (int i = 1; i < papers_avl.size(); i++)
			{
				for (auto&elem : selected)
				{
					sim[papers_avl[i]] += (1.0 - distanceMatrix[elem.first][i]);
					c += 1.0 - distanceMatrix[elem.first][i];
				}
			}
	        //cprobs of available
			vector<pair<int, int>> cprob(papers_avl.size()); //cprob to docID
			double sum = 0;
			int counter = 0;
			for (auto&elem : sim)
			{
				sum += elem.second / c;
				cprob[counter++]=make_pair(elem.first, sum);
			}
			cout<<sum;

			//choose next randomly
			// can be made faster
			double r = ((double)rand() / (RAND_MAX));
			for (auto&elem : cprob)
			{
				if (elem.second > r)
				{
					selected[elem.first] = 1;
					papers_avl.erase(papers_avl.begin()) + j);
				}
			}
		}
	}

	int paperCounter = 0;
    for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ )
    {
        for ( int j = 0; j < conference->getParallelTracks ( ); j++ )
        {
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                conference->setPaper ( j, i, k, paperCounter );
                paperCounter++;
            }
        }
    }
    system("pause");
    Conference *newConference = new Conference(this->conference);
    cout << "this conference is \n";
    system("pause");
    this->conference->printConferenceStdout();
    
    cout << "\ncopied conference is \n";
    newConference->printConferenceStdout();
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
