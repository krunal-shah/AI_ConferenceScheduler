/*
 * File:   LocalSearch.cpp
 * Author: Kapil Thakkar
 *
 */

#include "LocalSearch.h"
#include "Util.h"
#include <float.h>
#include <time.h>       /* time */


LocalSearch::LocalSearch()
{
	parallelTracks = 0;
	papersInSession = 0;
	sessionsInTrack = 0;
	processingTimeInMinutes = 0;
	tradeoffCoefficient = 1.0;
}

LocalSearch::LocalSearch(string filename)
{
	readInInputFile(filename);
	conference = new Conference(parallelTracks, sessionsInTrack, papersInSession);
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

void LocalSearch::organizePapers()
{
	// initialised start state
	getStartState();
	currentScore = scoreOrganization();
	bestState = new Conference(conference);
	bestScore = scoreOrganization(bestState);
	// bestScore = currentScore;
	cout << "Start state exited with score " << currentScore << endl;
	system("pause");
	int stepsToClimb = (parallelTracks <= 8) ? pow(2 * parallelTracks, parallelTracks) : 1000000;
	int tempStepsLimit = -1;
	int stepNumber = 0;
	vector<double> stepInfo(6);
	cout << "Steps" << stepsToClimb << endl;
	while (stepNumber < stepsToClimb)
	{
		cout << "Computing step\n";
		stepNumber++;
		computeBestTransition(stepInfo);

		cout << "Next step computed:\n";
		cout << "time = " << stepInfo[4] << " change in score = " << stepInfo[5] << endl;
		cout << "sessionOne = " << stepInfo[0] << " paperOne = " << stepInfo[1] << endl;
		cout << "sessionTwo = " << stepInfo[2] << " paperTwo = " << stepInfo[3] << endl;

		decideStep(stepInfo, tempStepsLimit);

		if (tempStepsLimit > 0)
		{
			tempStepsLimit--;
			if (tempStepsLimit == 0)
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

	if (bestStepScore < 0)
	{
		tempStepsLimit = 10;

		Track *curTrack = conference->getTrack(trackIndex);
		Session *curSessionOne = curTrack->getSession(sessionOne);
		Session *curSessionTwo = curTrack->getSession(sessionTwo);
		int tempPaperId = curSessionOne->getPaper(paperOne);
		curSessionOne->setPaper(paperOne, curSessionTwo->getPaper(paperTwo));
		curSessionTwo->setPaper(paperTwo, tempPaperId);
	}
	else if (bestStepScore > 0)
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

	if (currentScore > bestScore)
	{
		cout << "Updating bestState\n";
		updateState();
	}
}

void LocalSearch::computeBestTransition(vector<double> &best_results)
{
	//considering costs for swapping two members in the same row, for every row, to constitute transition space
	double bestcost = DBL_MIN;
	best_results[0] = 0;
	best_results[1] = 0;
	best_results[2] = 0;
	best_results[3] = 0;
	best_results[4] = 0;
	best_results[5] = DBL_MIN;

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

void LocalSearch::AssignSchedule(unordered_map<int, vector<int>> &all_sessions)
{
	int sessCounter = 0;
	for (int i = 0; i < conference->getParallelTracks(); i++)
	{
		for (int j = 0; j < conference->getSessionsInTrack(); j++)
		{
			//for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
			int counter = 0;
			for (auto& elem : all_sessions[sessCounter])
			{
				conference->setPaper(i, j, counter++, elem);
			}
			sessCounter++;
		}
	}
	assert(sessCounter == all_sessions.size());
}


void LocalSearch::getStartState_weighted()
{
	//this is partially probabilistic greedy initialisation
	unordered_set<int> papers_avl = this->all_papers;
	unordered_map<int, vector<int>> all_sessions;
	int sess_count = 0;
	while (papers_avl.size())
	{
		//for every "row" (time-slot)
		int random = rand() % papers_avl.size();
		unordered_set<int>::const_iterator iter = papers_avl.begin();
		advance(iter, random);
		int first_ID = *iter;
		papers_avl.erase(first_ID);
		unordered_map <int, double> global_similarity;

		//select first session considering similarity [greedy] 
		vector<int> selected = { first_ID };
		int firstID_next = -1; //firstID for second session
		while (selected.size() < papersInSession)
		{
			double max_sim = DBL_MIN;
			double min_sim = DBL_MAX;
			int bestID = -1;

			for (auto& availID : papers_avl)
			{
				global_similarity[availID] += (1.0 - distanceMatrix[selected.back()][availID]);
				if (global_similarity[availID] > max_sim)
				{
					max_sim = global_similarity[availID];
					bestID = availID;
				}
				if (global_similarity[availID] < min_sim)
				{
					min_sim = global_similarity[availID];
					firstID_next = availID;
				}
			}
			selected.push_back(bestID);
			papers_avl.erase(bestID);
			global_similarity.erase(bestID);
		}
		all_sessions[sess_count++] = selected;

		//all following sess. in this row will weigh intra-cluster sim. with dissim. of all same-row previous clusters using "C" [greedy]	
		while (sess_count%conference->getSessionsInTrack()) //assuming we interchanged getSessionsInTrack and getParallelTracks 
		{
			selected = { firstID_next };
			papers_avl.erase(*selected.begin());
			global_similarity.erase(firstID_next);
			unordered_map <int, double> local_similarity;
			double best_score;
			int bestID;
			double temp_score;
			while (selected.size() < papersInSession)
			{
				best_score = DBL_MIN;
				bestID = -1;
				temp_score = 0;
				for (auto& availID : papers_avl)
				{
					local_similarity[availID] += (1.0 - distanceMatrix[selected.back()][availID]);
					temp_score = local_similarity[availID] - tradeoffCoefficient * global_similarity[availID];
					if (temp_score > best_score)
					{
						best_score = temp_score;
						bestID = availID;
					}
				}
				//next best ID has been determined.
				selected.push_back(bestID);
				papers_avl.erase(bestID);
				global_similarity.erase(bestID);
				local_similarity.erase(bestID);
			}
			all_sessions[sess_count++] = selected;
			//now, update global_similarity and find most dissimilar paperID (wrt to ALL previous clusters) [will be used as first_ID for following session]
			double min_sim = DBL_MAX;
			assert(local_similarity.size() == global_similarity.size());
			for (auto & elem : global_similarity)
			{
				elem.second += local_similarity[elem.first];
				if (elem.second < min_sim)
				{
					min_sim = elem.second;
					firstID_next = elem.first;
				}
			}
		}
	}
	//assign determined schedule to conference -
	AssignSchedule(all_sessions);  //**ROW-WISE** assignment
	cout << "Printing starting conference\n";
	conference->printConferenceStdout();
}



void LocalSearch::getStartState()
{
	unordered_set<int> papers_avl = this->all_papers;
	unordered_map<int, vector<int>> all_sessions;
	int sess_count = 0;
	srand(time(NULL));
	//select first paper of every conference randomly:
	int random = rand() % papers_avl.size();
	unordered_set<int>::const_iterator iter = papers_avl.begin();
	advance(iter, random);
	int first_ID = *iter;
	while (papers_avl.size())
	{
		vector<int> selected = { first_ID }; //docID of first randomly-selected paper 
		papers_avl.erase(*selected.begin());
		unordered_map<int, double> similarity; //ID to (cumulative) similarity for one session
		while (selected.size() < papersInSession) //make one session:
		{
			//similarities of rest available
			double c = 0;
			for (auto& availID : papers_avl)
			{
				similarity[availID] += (1.0 - distanceMatrix[selected.back()][availID]);
				c += similarity[availID];
			}
			if (c == 0) //edge-case; resolve deterministically
			{
				selected.push_back(*papers_avl.begin());
			}
			else
			{
				vector<pair<int, double>> cprob(papers_avl.size()); // <docID,sum>
				double sum = 0;
				int counter = 0;
				for (auto& elem : similarity)
				{
					sum += elem.second / c;
					cprob[counter++] = make_pair(elem.first, sum);
				}
				assert(sum == 1.0);
				//choose next randomly
				//can be made faster -- how?
				double r = ((double)rand() / (double)RAND_MAX);
				cout << "Chose r = " << r << " " << endl;
				for (auto& elem : cprob)
				{
					if (elem.second > r)
					{
						selected.push_back(elem.first);
						break;
					}
				}
			}
			papers_avl.erase(selected.back());
			similarity.erase(selected.back());
		}
		all_sessions[sess_count++] = selected;
		//now, find the most dissimilar paperID from remaining wrt latest cluster [will be used as first_ID for following session]
		double min_sim = DBL_MAX;
		for (auto & elem : similarity)
		{
			if (elem.second < min_sim)
			{
				min_sim = elem.second;
				first_ID = elem.first;
			}
		}
	}

	int sessCounter = 0;
	for (int i = 0; i < conference->getSessionsInTrack(); i++)
	{
		for (int j = 0; j < conference->getParallelTracks(); j++)
		{
			//for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
			int counter = 0;
			for (auto& elem : all_sessions[sessCounter])
			{
				conference->setPaper(j, i, counter++, elem);
			}
			sessCounter++;
		}
	}
	assert(sessCounter == all_sessions.size());
	cout << "Printing starting conference\n";
	conference->printConferenceStdout();
	// system("pause");
}

void LocalSearch::readInInputFile(string filename)
{
	vector<string> lines;
	string line;
	ifstream myfile(filename.c_str());
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			//cout<<"Line read:"<<line<<endl;
			lines.push_back(line);
		}
		myfile.close();
	}
	else
	{
		cout << "Unable to open input file";
		exit(0);
	}

	if (6 > lines.size())
	{
		cout << "Not enough information given, check format of input file";
		exit(0);
	}

	processingTimeInMinutes = atof(lines[0].c_str());
	papersInSession = atoi(lines[1].c_str());
	sessionsInTrack = atoi(lines[2].c_str());
	parallelTracks = atoi(lines[3].c_str());
	tradeoffCoefficient = atof(lines[4].c_str());

	int n = lines.size() - 5;
	totalpapers = n;
	for (int i = 0; i < n; i++)
	{
		all_papers.insert(i);
	}
	double ** tempDistanceMatrix = new double*[n];
	for (int i = 0; i < n; ++i)
	{
		tempDistanceMatrix[i] = new double[n];
	}

	for (int i = 0; i < n; i++)
	{
		string tempLine = lines[i + 5];
		vector<string> elements(n);
		splitString(tempLine, " ", elements, n);

		for (int j = 0; j < n; j++)
		{
			tempDistanceMatrix[i][j] = atof(elements[j].c_str());
		}
	}
	distanceMatrix = tempDistanceMatrix;

	int slots = parallelTracks * papersInSession*sessionsInTrack;
	if (slots != totalpapers)
	{
		cout << "More papers than slots available! slots:" << slots << " num papers:" << totalpapers << endl;
		exit(0);
	}
}

double** LocalSearch::getDistanceMatrix()
{
	return distanceMatrix;
}

void LocalSearch::printSessionOrganiser(char * filename)
{
	bestState->printConference(filename);
}

double LocalSearch::scoreSwitch(int track, int sessionOne, int paperOne, int sessionTwo, int paperTwo)
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
	Track* tmpTrack = conference->getTrack(track);
	Session* tmpSessionOne = tmpTrack->getSession(sessionOne);
	Session* tmpSessionTwo = tmpTrack->getSession(sessionTwo);
	paperOne = tmpSessionOne->getPaper(paperOne);
	paperTwo = tmpSessionTwo->getPaper(paperTwo);

	for (int k = 0; k < tmpSessionOne->getNumberOfPapers(); k++)
	{
		int index = tmpSessionOne->getPaper(k);
		if (index != paperOne)
		{
			similarityOne += (1 - distanceMatrix[index][paperOne]);
			similarityTwoNew += (1 - distanceMatrix[index][paperTwo]);
			distanceOneNew += distanceMatrix[index][paperOne];
		}
		distanceTwo += distanceMatrix[index][paperTwo];
	}

	for (int k = 0; k < tmpSessionTwo->getNumberOfPapers(); k++)
	{
		int index = tmpSessionTwo->getPaper(k);
		if (index != paperTwo)
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

double LocalSearch::scoreOrganization()
{
	// Sum of pairwise similarities per session.
	double score1 = 0.0;
	for (int i = 0; i < conference->getParallelTracks(); i++)
	{
		Track* tmpTrack = conference->getTrack(i);
		for (int j = 0; j < tmpTrack->getNumberOfSessions(); j++)
		{
			Session* tmpSession = tmpTrack->getSession(j);
			for (int k = 0; k < tmpSession->getNumberOfPapers(); k++)
			{
				int index1 = tmpSession->getPaper(k);
				for (int l = k + 1; l < tmpSession->getNumberOfPapers(); l++)
				{
					int index2 = tmpSession->getPaper(l);
					score1 += 1 - distanceMatrix[index1][index2];
				}
			}
		}
	}

	// Sum of distances for competing papers.
	double score2 = 0.0;
	for (int i = 0; i < conference->getParallelTracks(); i++)
	{
		Track* tmpTrack1 = conference->getTrack(i);
		for (int j = 0; j < tmpTrack1->getNumberOfSessions(); j++)
		{
			Session* tmpSession1 = tmpTrack1->getSession(j);
			for (int k = 0; k < tmpSession1->getNumberOfPapers(); k++)
			{
				int index1 = tmpSession1->getPaper(k);

				// Get competing papers.
				for (int l = j + 1; l < tmpTrack1->getNumberOfSessions(); l++)
				{
					Session* tmpSession2 = tmpTrack1->getSession(l);
					for (int m = 0; m < tmpSession2->getNumberOfPapers(); m++)
					{
						int index2 = tmpSession2->getPaper(m);
						score2 += distanceMatrix[index1][index2];
					}
				}
			}
		}
	}
	double score = score1 + tradeoffCoefficient*score2;
	return score;
}

double LocalSearch::scoreOrganization(Conference *conferenceTmp)
{
	// Sum of pairwise similarities per session.
	double score1 = 0.0;
	for (int i = 0; i < conferenceTmp->getParallelTracks(); i++)
	{
		Track* tmpTrack = conferenceTmp->getTrack(i);
		for (int j = 0; j < tmpTrack->getNumberOfSessions(); j++)
		{

			Session* tmpSession = tmpTrack->getSession(j);
			for (int k = 0; k < tmpSession->getNumberOfPapers(); k++)
			{
				int index1 = tmpSession->getPaper(k);
				for (int l = k + 1; l < tmpSession->getNumberOfPapers(); l++)
				{
					int index2 = tmpSession->getPaper(l);
					score1 += 1 - distanceMatrix[index1][index2];
				}
			}
		}
	}

	// Sum of distances for competing papers.
	double score2 = 0.0;
	for (int i = 0; i < conferenceTmp->getParallelTracks(); i++)
	{
		Track* tmpTrack1 = conferenceTmp->getTrack(i);
		for (int j = 0; j < tmpTrack1->getNumberOfSessions(); j++)
		{
			Session* tmpSession1 = tmpTrack1->getSession(j);
			for (int k = 0; k < tmpSession1->getNumberOfPapers(); k++)
			{
				int index1 = tmpSession1->getPaper(k);

				// Get competing papers.
				for (int l = j + 1; l < tmpTrack1->getNumberOfSessions(); l++)
				{
					Session* tmpSession2 = tmpTrack1->getSession(l);
					for (int m = 0; m < tmpSession2->getNumberOfPapers(); m++)
					{
						int index2 = tmpSession2->getPaper(m);
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
