Alan Davoust
Term project sysc5104, fall 2010.

This directory contains :
=========================
- this file
- READM_ME_user_guide.pdf: a user guide to generate new models using python scripts, run simulations, view simulation results... 
- adavoust-simul-project.pdf : my final report.

- the p2p directory : all the code is in there. That directory can be placed in the eclipse workspace.

The directory p2p contains :
============================
- a file : distri.cpp : this is a new version (only a very minor change) of the c++ class to generate random numbers within cd++.
	   it can be copied to the internal directory of the cd++ builder plugin. It fixes a problem with generator seeds.

- directory lib : some c++ libraries 
- directory atomic : contains all the atomic models, each in their own sub-directory.
- directory coupled: contains the coupled models, and all the tools to generate them, and analyse simulation results. See below.

p2p/coupled contains :
======================
- python scripts to generate new models, and all other necessary files (see user guide).
- RunnableP2PViewer.jar : java applet for viewing simulation results. Also see user guide for details.
- directories peer, network, Network90: coupled models
- directory network_files : files to be copied when generating new models.


	 