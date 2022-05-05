/*******************************************************************
*
*  DESCRIPTION: Simulator::registerNewAtomics()
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#include "modeladm.h"
#include "mainsimu.h"
#include "..\..\atomic\SessionManager\SessionManager.h"        // class SessionManager
#include "..\..\atomic\connection_generator\ConnectionManager.h"

void MainSimulator::registerNewAtomics()
{
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<SessionManager>() , "SessionManager" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<ConnectionManager>() , "ConnectionManager" ) ;
	}
