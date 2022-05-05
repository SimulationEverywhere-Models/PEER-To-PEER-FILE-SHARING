#edit here the name of the input log file

lfname = raw_input("please enter the name of the log file to process")
if (lfname == "") : lfname ="P2P90LOG.log"
logfile = open(lfname,"r")
processedfile = open("ProcessedLog.txt", "w")
count = 0
for line in logfile:
    words = line.split()
    #msg type is words[1], time is words[3], whofrom is words[5],
    #port is words[7] value is words[9] and whoto is words[11]
    count = count +1

    if (words[1]=='Y'): #possibly interesting
        if(words[5].startswith('session')):
            if (words[7].startswith('online') | words[7].startswith('offline') |words[7].startswith('query')|words[7].startswith('publish')):
                peernumber = int(words[9][0:words[9].index('.')])#left of the period
                timebits = words[3].split(':')
                timemillisec = int(timebits[0]) * 3600000 + int(timebits[1])*60000 + int(timebits[2])*1000 + int(timebits[3])
                if (words[7].startswith('online')|words[7].startswith('offline')):
                    processedfile.write(str(timemillisec)+ ':'+words[7]+':'+str(peernumber)+ "\n")
                    #print timemillisec, words[7] ,peernumber  #, 'goes ', words[7]
#                if (words[7].startswith('offline')):
 #                   print timemillisec, 'remove',peernumber  #, 'goes ', words[7]
                if (words[7].startswith('query')|words[7].startswith('publish')):
                    queryterm = peernumber %1000
                    querypeer = (peernumber -queryterm)/1000
                    processedfile.write(str(timemillisec)+':'+ words[7]+ ':'+ str(querypeer)+ ':'+ str(queryterm) + '\n')
                    #print timemillisec, words[7], querypeer, queryterm  #, 'goes ', words[7]

        if (words[5].startswith('netwk') & words[7].endswith('connect')):
            mvalue=int(words[9][0:words[9].index('.')])#left of the period
            
            peer1 = (mvalue -1000000)%1000
            peer2 = (mvalue -(1000000+peer1))/1000
            if (words[7].endswith('_connect')):
                #print words[3], ': peers ',mvalue, ' disconnect.'
            #else:
                timebits = words[3].split(':')
                timemillisec = int(timebits[0]) * 3600000 + int(timebits[1])*60000 + int(timebits[2])*1000 + int(timebits[3])
                #print mvalue
                processedfile.write(str(timemillisec)+ ':connect:'+str(peer1)+':'+str(peer2)+"\n")
#                print timemillisec, 'connect',peer2, peer1
                #print words[3], ': peers ',mvalue, ' connect.'
    elif ((words[1]=='X') & (words[5].startswith('peer')) & (words[7].startswith('queryhit'))):
        encodedmsg = int(words[9][0:words[9].index('.')])#left of the period
        docnumber = int(encodedmsg/1000)%1000 #middle 3 digits
        peernumber = int(encodedmsg/1000000)#left 3 digits
        receivingpeer = int(words[5][4:words[5].index('(')])#get the peer number from a string like 'peer43(122)'
        #print 'encoded:', encodedmsg, 'peer:', peernumber, "doc", docnumber
        timebits = words[3].split(':')
        timemillisec = int(timebits[0]) * 3600000 + int(timebits[1])*60000 + int(timebits[2])*1000 + int(timebits[3])
        if (peernumber != receivingpeer): processedfile.write(str(timemillisec)+':'+ words[7]+ ':'+ str(peernumber)+ ':'+ str(docnumber) + '\n')
        
    if (count%100000==0): print count, "lines..."

print count,"lines read."
print "The processed log has been output to file ProcessedLog.txt."
processedfile.close()
logfile.close()
