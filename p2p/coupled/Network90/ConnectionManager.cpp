/*******************************************************************
*
*  DESCRIPTION: Atomic Model ConnectionManager
*
*  AUTHOR: Alan
*
*  EMAIL: mailto://hsaadawi@scs.carleton.ca
*
*
*  DATE: 18/10/2002
*
*******************************************************************/

/** include files **/
#include <math.h>            // fabs( ... )
#include <stdlib.h>
#include "randlib.h"         // Random numbers library
#include "ConnectionManager.h"  // base header
#include "message.h"       // InternalMessage ....
#include "distri.h"        // class Distribution
#include "mainsimu.h"      // class MainSimulator
#include "strutil.h"       // str2float( ... )
#include <fstream>


/*******************************************************************
* Function Name: ConnectionManager
* Description: constructor
********************************************************************/
ConnectionManager::ConnectionManager( const string &name )
: Atomic( name )
, in_connect( addInputPort( "in_connect" ) )
, in_disconnect( addInputPort( "in_disconnect" ) )
//, activate( addInputPort( "addfriend" ) ) // we may decide to add friends at some point for some reason (queryhits?)
, online( addInputPort( "online" ) )
, offline( addInputPort( "offline" ) )
, out_connect( addOutputPort( "out_connect") )
, out_disconnect( addOutputPort( "out_disconnect") )
{
	//get the identifier of this peer from MA file
	string Id_str = MainSimulator::Instance().getParameter( description(), "whoAmI" );
	whoAmI = int(floor(str2float(Id_str))); // clumsy way of turning a string into an integer

	shutdown = false;
	// read the friendlist from a file :
	//1 - get file :
	string datafile = MainSimulator::Instance().getParameter( description(), "friendlist" );

	ifstream fis;

	fis.open(datafile.c_str()); // open stream to file

	int i, n;
	/*2- read number of friends */
	fis >> n;

	//3- read list of friends
	for (i = 0; i < n; i++)
	{
		int w;
		fis >> w;
		friendlist.insert(w);
	}

	//4 - close file
	fis.close();

}

Model &ConnectionManager::initFunction()
{
	passivate(); //we start off passive, waiting to the session manager module to wake us
    return *this;
}

/*******************************************************************
* Function Name: externalFunction
* Description: the Balanceverifier receives the Amount
********************************************************************/
Model &ConnectionManager::externalFunction( const ExternalMessage &msg )
{
	int mval = msg.value();
	if ( this->state() == passive)
	{
		//mval should be my Id on port online
		if ((msg.port() == online) && (mval==whoAmI)) {
			set<int>::iterator it;
			for (it = friendlist.begin(); it != friendlist.end(); it++ ) {
				if(connectionlist.find(*it)==connectionlist.end()) // if we don't already have an active connection to this guy
					toconnect.insert(*it); //add to "toconnect" set
			}

			holdIn( active, 0.0f ) ; //instantly change state
		}
		else{ //a message we don't expect when we're passive
			cout<<"ConnectionManager :: error : I'm passive so I'm ignoring this message";
			passivate();
		}
	}
	else { //state is active
		//we are actively maintaining connections

		if ((msg.port() == offline) && (mval==whoAmI)){
			shutdown = true;
			holdIn(active, Time(0.0f)); //prepare for an instant change to passive state; but we need to go through an internal change to be able to output the correct "offline" message
		}
		else if ((msg.port() == in_connect) && (getTTL(mval)==1)){ // this because messages on connect port have both connection and disconnection events, encoded differently (differntiated through TTL)

			//we just got a message to the effect that the connection with another peer "mval" is successfully established
			//we assume that this peer is a friend and we accept the connection //TODO maybe another behavior is possible
			int whichfriend = getPeerId(mval); // using encoding from complexMessages.h
			if (whichfriend==whoAmI) whichfriend=getMessageId(mval); // then it's the other one
			connectionlist.insert(whichfriend);
			toconnect.erase(whichfriend); // if this friend was in our prospective connections, he no longer needs to be there
			//holdIn(active, 0.0f); // just go back to what we were doing immediately
			// i want to ignore the change : see how that works
			// inspired by Queue example
		}
		else if((msg.port() == in_connect)&& (getTTL(mval)==0)){// using encoding from complexMessages.h
			//we just got a message to the effect that the another peer has disconnected from us
			int whichfriend = getPeerId(mval);
			if (whichfriend==whoAmI) whichfriend=getMessageId(mval);
			connectionlist.erase(whichfriend); // record disconnection
			//holdIn(active, 0.0f); // just go back to what we were doing immediately
		}
		//else nothing !

	}
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &ConnectionManager::internalFunction( const InternalMessage & )
{
	if (shutdown==true){
		connectionlist.clear(); // clear all active and prospective connections
		toconnect.clear();
		passivate();
	}
	else {
		if (toconnect.empty()) {//if we've finished iterating through the list of friends to connect to
			//rebuild toconnect list

			set<int>::iterator it;
			for (it = friendlist.begin(); it != friendlist.end(); it++ ) {
				if(connectionlist.find(*it)==connectionlist.end()) // if we don't already have an active connection to this guy
					toconnect.insert(*it); //add to "toconnect" set
			}
			//ready to start again
			//cout<<"waiting 2 min! \n";
			holdIn(active, Time(0,2,0,0)); // wait 2 min before starting again
		} else { // we currently have a list of peers that we're busy connecting to
			holdIn(active, Time(0,0,1,0)); // we'll just wait one second before outputting a connect message
		}
	}

	return *this;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &ConnectionManager::outputFunction( const InternalMessage &msg )
{
      if ( !shutdown ) {
    	  // if shutdown is true the offline is already taken care of.
    	  if (!toconnect.empty()) { // if we have nobody to connect to then we just stay quiet
    		  set<int>::iterator it;
    		  it = toconnect.begin(); // get a first element from this list and output a connection message for this friend
    		  int tosend = *it;
    		  tosend = buildMessage(tosend, whoAmI); // encode who we want to connect from + who we are
    		  toconnect.erase(it); //remove the friend from the list of prospective contacts
    	      sendOutput( msg.time(), out_connect, tosend );
    	  }
      }

	return *this;
}

ConnectionManager::~ConnectionManager()
{
	//nothing to be done
}
