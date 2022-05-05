/*******************************************************************
* Atomic Model : Connection Manager : part of a peer that manages the connections to other peers
*
*  AUTHOR: Alan
*
*
*  DATE: November 2010
*
*******************************************************************/
#ifndef __SESSMANAGER_H
#define __SESSMANAGER_H
#define PVERBOSE true

/** include files **/
#include "atomic.h"  // class Atomic
#include <set>
#include "..\complexmessages.h" // functions to manage "coded" messages between components

/** forward declarations **/
class Distribution ;

/** declarations **/
class SessionManager: public Atomic
{
public:
	SessionManager( const string &name = "SessionManager" ) ;	 // Default constructor

	~SessionManager();					// Destructor

	virtual string className() const
		{return "SessionManager";}

protected:
	Model &initFunction();

	Model &externalFunction( const ExternalMessage & );

	Model &internalFunction( const InternalMessage & );

	Model &outputFunction( const InternalMessage & );

private:
	const Port &queryhit ;
	Port & online ;
    Port & offline;
    Port & query;
    Port & publish;
    Port & remove;

    int whoAmI;
    bool isonline;
    bool finished;
    set<int> querylist;
    set<int> doclist;
    set<int> alldocs; //docs already published
    Distribution *dist ;

	Distribution &distribution()
			{return *dist;}
};	// class SessionManager


#endif    //__SESSMANAGER_H
