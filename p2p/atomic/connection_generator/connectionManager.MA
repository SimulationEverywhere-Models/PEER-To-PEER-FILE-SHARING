[top]
components : connector@ConnectionManager 
in : online
in : offline
in : in_connect
in : in_disconnect 
out : out_connect
out : out_disconnect 
Link : out_connect@connector out_connect
Link : out_disconnect@connector out_disconnect
Link : online online@connector 
Link : offline offline@connector
Link : in_connect in_connect@connector
Link : in_disconnect in_disconnect@connector


[connector]
friendlist : friends.dat
whoAmI : 31 

 
