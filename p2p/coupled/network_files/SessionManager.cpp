/*******************************************************************
 *
 *  DESCRIPTION: Atomic Model SessionManager : simulate the user's session in the p2p network.
 *  The user goes online, is active or passive until she goes offline. If active, the user outputs a random number of queries at a random time interval, then goes offline.
 *
 *
 *  AUTHOR: Alan
 *
 *
 *  DATE: november 2010
 *
 *******************************************************************/

/** include files **/
#include <math.h>            // fabs( ... )
#include <stdlib.h>
#include "randlib.h"         // Random numbers library
#include "SessionManager.h"  // base header
#include "message.h"       // InternalMessage ....
#include "distri.h"        // class Distribution
#include "mainsimu.h"      // class MainSimulator
#include "strutil.h"       // str2float( ... )
#include <fstream> // i o streaming from files


/*******************************************************************
 * Function Name: SessionManager
 * Description: constructor
 ********************************************************************/
SessionManager::SessionManager( const string &name )
: Atomic( name )
, queryhit( addInputPort( "queryhit" ) )
, query( addInputPort( "query" ) )
, online( addOutputPort( "online" ) )
, offline( addOutputPort( "offline" ) )
, publish( addOutputPort( "publish") )
, remove( addOutputPort( "remove") )
{
	//some variables to initialize
	isonline = false;
	finished = false;

	//get the parameter that specifies the id of this peer
	string Id_str = MainSimulator::Instance().getParameter( description(), "whoAmI" );
	whoAmI = int(floor(str2float(Id_str))); // clumsy way of turning a string into an integer

	//in this version we load the querylist from a file -------------------------------
	string datafile = MainSimulator::Instance().getParameter( description(), "querylist" );

	ifstream fis;
	fis.open(datafile.c_str()); // open stream to file

	int i, n;
	/*2- read number of queries */
	fis >> n;

	//3- read list of queries
	for (i = 0; i < n; i++)
	{
		int w;
		fis >> w;
		querylist.insert(w);
		//if(PVERBOSE) cout<<"Peer"<<whoAmI<<" adding query : "<<w<<endl;
	}

	//4 - close file
	fis.close();
 ///----------------------------------------------------
	//in this version we load the list of published documents also from a file
	datafile = MainSimulator::Instance().getParameter( description(), "doclist" );

	ifstream fis2;
	fis2.open(datafile.c_str()); // open stream to file

	/*2- read number of queries */
	fis2 >> n;

	//3- read list of queries
	for (i = 0; i < n; i++)
	{
		int w;
		fis2 >> w;
		doclist.insert(w);
		//if(PVERBOSE) cout<<"Peer"<<whoAmI<<" adding doc : "<<w<<endl;
	}

	//4 - close file
	fis2.close();


	try
	{

		//temporarily use a single distribution
		dist = Distribution::create( "exponential" );
		dist->setVar( 0, str2float(MainSimulator::Instance().getParameter( description(), "mean" ) )) ;

		/* /at some point need to get several distributions and deal with their different parameters
		    dist = Distribution::create( MainSimulator::Instance().getParameter( description(), "distribution" ) );
		    for ( register int i = 0 ; i < dist->varCount() ; i++ )
		    		{
		    			string parameter( MainSimulator::Instance().getParameter( description(), dist->getVar(i) ) ) ;
		    			dist->setVar( i, str2float( parameter ) ) ;
		    		}
		 */

		MASSERT( dist );


	} catch( InvalidDistribution &e )
	{
		e.addText( "The model " + description() + " has distribution problems!" ) ;
		e.print(cerr);
		MTHROW( e ) ;
	} catch( MException &e )
	{
		MTHROW( e ) ;
	}
}

Model &SessionManager::initFunction()
{
	//first decide if the session will be active or passive
	//double randnumber = (double)rand()/(double)RAND_MAX;
	/*if ( randnumber <= 0.8) {
		activesession = false;
	}
	else {
		activesession = true;
	}*/

	//We have a list of queries already, that will be output at internal transitions

	//stay in the active state for some random time, before going online. average 2 min (may be 4 because of apparent bug in Time(float) constructor
	float t = static_cast<float>( distribution().get() );

	if(PVERBOSE) cout<<"Peer"<<whoAmI<<" about to start with waiting time: "<<t<<endl;;
	holdIn(active, Time( t ));
	return *this;
}

/*******************************************************************
 * Function Name: externalFunction
 * Description: only queryhits come in here.
 * At this point we don't change the model state for them, we just trace them
 ********************************************************************/
Model &SessionManager::externalFunction( const ExternalMessage &msg )
{


	if (this->state() != passive) //if I'm not offline I'll do something about the queryhit
	{

		if(PVERBOSE) cout<<msg.time()<<"Peer "<<whoAmI<<" got queryhit:"<<msg.value()<<endl;

		//publish the document if I didn't have it
		int thedoc = getPeerId(msg.value());

		if(alldocs.find(thedoc)!=alldocs.end() && !finished ) { // we haven't got this one yet AND we're not about to go offline !
			doclist.insert(thedoc); // list it to be published
		}


		//won't hold in any new state... just stick with what I was doing.

	}

	return *this ;
}

/*******************************************************************
 * Function Name: internalFunction
 ********************************************************************/
Model &SessionManager::internalFunction( const InternalMessage & )
{
	if(!doclist.empty()){
		holdIn(active, Time(0,0,0,200 )); //wait 200 ms between publishing docs
	} else if (!querylist.empty()) {
		//just wait a random time for the next query.
		holdIn(active, Time( static_cast<float>( distribution().get()  ) ));
	} else {

		if (!finished){ // in this case we're done querying but we still have to output the "offline" message
			finished = true;
			holdIn(active, Time( static_cast<float>( distribution().get() ) ));
		}
		else
			passivate(); // we're really done
	}
	return *this;
}

/*******************************************************************
 * Function Name: outputFunction
 ********************************************************************/
Model &SessionManager::outputFunction( const InternalMessage &msg )
{
	 if ( !isonline ) {
		sendOutput( msg.time(), online, whoAmI ); // output own Id as msg value
		isonline = true;
	}
	else if (!doclist.empty()) //we're publishing stuff
		{
		set<int>::iterator it;
		it = doclist.begin();
		int tosend = buildMessage(*it, whoAmI); // encode query, who am I
		sendOutput( msg.time(), publish , tosend ); // we output the value in the query set on the query output port
		alldocs.insert(*it); // save it in the collection of already published documents
		doclist.erase(it);
		}
	else if (!querylist.empty()) //we're querying
	{
		set<int>::iterator it;
		it = querylist.begin();
		if(PVERBOSE) cout<<msg.time()<<" : Peer "<<whoAmI<< " queries:"<<*it<<endl;
		int tosend = buildMessage(*it, whoAmI); // encode query, who am I
		sendOutput( msg.time(), query , tosend ); // we output the value in the query set on the query output port
		querylist.erase(it); // remove the query from list.


	}
	else { //we're done querying. next thing we go offline

		sendOutput( msg.time(), offline, whoAmI );
	}
	return *this;
}

SessionManager::~SessionManager()
{
	delete dist;
}
