/*******************************************************************
* Atomic Model : Connection Manager : part of a peer that manages the connections to other peers
*
*  AUTHOR: Alan
*
*
*  DATE: November 2010
*
*******************************************************************/
#ifndef __CONMANAGER_H
#define __CONMANAGER_H

/** include files **/
#include "atomic.h"  // class Atomic
#include <set>
#include "..\complexmessages.h" // functions to manage "coded" messages between components


/** forward declarations **/
class Distribution ;


/** declarations **/
class ConnectionManager: public Atomic
{
public:
	ConnectionManager( const string &name = "ConnectionManager" ) ;	 // Default constructor

	~ConnectionManager();					// Destructor

	virtual string className() const
		{return "ConnectionManager";}

protected:
	Model &initFunction();

	Model &externalFunction( const ExternalMessage & );

	Model &internalFunction( const InternalMessage & );

	Model &outputFunction( const InternalMessage & );

private:
	const Port &in_connect ;
	const Port &in_disconnect ;
	//const Port &activate ; //port that commands beginning / end of session by inputs 0 or 1
	const Port & online ; //these are inputs as well now
    const Port & offline;
    Port & out_connect;
    Port & out_disconnect;

    int whoAmI; // my identifier
    set<int> friendlist;  // my friends (never changes from the time I define the list)
    set<int> connectionlist; // the friends I'm currently connected to
    set<int> toconnect; // the ones I want to try to connect to because right now I'm not !
    //int  whoto;
    bool shutdown;

};	// class ConnectionManager


#endif    //__CONMANAGER_H
