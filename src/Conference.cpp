/* 
 * File:   Conference.cpp
 * Author: Kapil Thakkar
 * 
 */

#include "Conference.h"

Conference::Conference ( )
{
    this->parallelTracks = 0;
    this->sessionsInTrack = 0;
    this->papersInSession = 0;
}

Conference::Conference ( int parallelTracks, int sessionsInTrack, int papersInSession )
{
    this->parallelTracks = parallelTracks;
    this->sessionsInTrack = sessionsInTrack;
    this->papersInSession = papersInSession;
    initTracks ( parallelTracks, sessionsInTrack, papersInSession );
}

Conference::Conference ( Conference *baseConference )
{
    this->parallelTracks = baseConference->getParallelTracks();
    this->sessionsInTrack = baseConference->getSessionsInTrack();
    this->papersInSession = baseConference->getPapersInSession();
    initTracks ( parallelTracks, sessionsInTrack, papersInSession );
    
    for ( int i = 0; i < this->getParallelTracks ( ); i++ )
    {
        Track *baseTrack = baseConference->getTrack(i);
        for ( int j = 0; j < this->getSessionsInTrack ( ); j++ )
        {
            Session *baseSession = baseTrack->getSession(j);
            for ( int k = 0; k < this->getPapersInSession ( ); k++ )
            {
                int basePaper = baseSession->getPaper(k);
                this->setPaper ( i, j, k, basePaper );
            }
        }
    }
}

void Conference::initTracks ( int parallelTracks, int sessionsInTrack, int papersInSession )
{
    tracks = ( Track * ) malloc ( sizeof (Track ) * parallelTracks );
    for ( int i = 0; i < parallelTracks; i++ )
    {
        Track tempTrack ( sessionsInTrack );
        for ( int j = 0; j < sessionsInTrack; j++ )
        {
            Session tempSession ( papersInSession );
            tempTrack.setSession ( j, tempSession );
        }
        tracks[i] = tempTrack;
    }
}

int Conference::getParallelTracks ( )
{
    return parallelTracks;
}

int Conference::getSessionsInTrack ( )
{
    return sessionsInTrack;
}

int Conference::getPapersInSession ( )
{
    return papersInSession;
}

Track* Conference::getTrack ( int index )
{
    if ( index < parallelTracks )
    {
        return &(tracks[index]);
    }
    else
    {
        cout << "Index out of bound - Conference::getTrack" << endl;
        exit ( 0 );
    }
}

void Conference::setPaper ( int trackIndex, int sessionIndex, int paperIndex, int paperId )
{
    if ( this->parallelTracks > trackIndex )
    {
        Track curTrack = tracks[trackIndex];
		curTrack.setPaper ( sessionIndex, paperIndex, paperId );
    }
    else
    {
        cout << "Index out of bound - Conference::setPaper" << endl;
		system("pause");
        exit ( 0 );
    }
}

// void Conference::getPaper ( int trackIndex, int sessionIndex, int paperIndex)
// {
//     if ( this->parallelTracks > trackIndex )
//     {
//         Track *curTrack = &(tracks[trackIndex]);
// 		Session *curSession = curTrack->getSession(sessionIndex);
//         return curSession->getPaper(paperIndex);
//     }
//     else
//     {
//         cout << "Index out of bound - Conference::setPaper" << endl;
// 		system("pause");
//         exit ( 0 );
//     }
// }

void Conference::printConference (char * filename )
{
    ofstream ofile(filename);

    for ( int i = 0; i < parallelTracks; i++ )
    {
        for ( int j = 0; j < sessionsInTrack; j++ )
        {
            for ( int k = 0; k < papersInSession; k++ )
            {
                ofile<< tracks[i].getSession ( j )->getPaper ( k ) << " ";
            }
            if ( j != sessionsInTrack - 1 )
            {
                ofile<<"| ";
            }
        }
        ofile<<"\n";
    }
    ofile.close();
    cout<<"Organization written to ";
    printf("%s :)\n",filename);

}

void Conference::printConferenceStdout ()
{
    for ( int i = 0; i < sessionsInTrack; i++ )
    {
        for ( int j = 0; j < parallelTracks; j++ )
        {
            for ( int k = 0; k < papersInSession; k++ )
            {
                cout << tracks[j].getSession ( i )->getPaper ( k ) << " ";
            }
            if ( j != parallelTracks - 1 )
            {
                cout <<"| ";
            }
        }
        cout<<"\n";
    }
    cout<<"\n\n";
	system("pause");
}