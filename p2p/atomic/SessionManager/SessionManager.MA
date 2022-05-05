[top]
components : session@SessionManager 
out : online
out : offline
out : query
out : publish
out : remove
in : queryhit 
Link : online@session online
Link : offline@session offline
Link : query@session query
Link : publish@session publish
Link : remove@session remove
Link : queryhit queryhit@session

[session]
distribution : exponential
mean : 90
querylist : qs.dat
doclist : docs.dat
whoAmI : 31