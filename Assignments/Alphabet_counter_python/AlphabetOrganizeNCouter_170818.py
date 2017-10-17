import string

def process(w):
    output=""
    for ch in w:
        if(ch.isalpha()):
            output+=ch
    return output.lower()

words=set()
fname=input("Funk")
file=open(fname,"r")
Export = open("Exported"+fname,"w")
for line in file:
    wordlist=line.split()
    for w in wordlist:
        words.add(process(w))

words.remove('')#This is a bug.

Export.write("Total Len :"+str(len(words))+'\n\n')

Dictionary=[0]*26
LastRecord=0
totalRecord=0
totalLength=len(words)

for yalfa in sorted(words):
    #print(yalfa[0])# DEBUGGING... showing every first alphabet
    idx = string.ascii_lowercase.index(yalfa[0])
    if(Dictionary[idx]==0):
        Dictionary[idx]=-1#Read Flag
        
        if(idx>=1):#END of each ALphabet Group!
            Export.write("\nTotal Words count : "+str(LastRecord)+'\n\n')
            totalRecord+=LastRecord#totalRecord UPDATE
            print(str(idx)+'th'+'...'+str(totalRecord/totalLength*100)+'%'+'..\n')    
            LastRecord=0
            
        Export.write('='*12+yalfa[0].upper()+'='*12+'\n')#NEW LOL
    LastRecord+=1
    Export.write(yalfa+'\n')

Export.write("\nTotal Words count : "+str(LastRecord)+'\n')#For the Last word

Export.close()
