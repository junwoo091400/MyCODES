from tkinter import *
import sys
import os
import time
import random

def getPNGList():
    listed = os.listdir('./PlayDATA/')
    retList = []
    for j in listed:
        if(j[-4:] == '.png'):
            retList.append('./PlayDATA/'+j)
    return retList

DATA = []

Export_Pipe = 2
RECORD_STAT_pipe = 1

line = input("Your ID???\n>")
tempPath = './BlackBox/'+line#+ '_STAT' + '.txt'
#if(os.path.exists
Export_Pipe = open(tempPath+'_STAT.txt','w')
RECORD_STAT_pipe = open(tempPath+'_RECO.txt','w')

print('Train Starting...')

root = Tk()
root.title("Who is Playing What??? QUIZ")

MAP_NAME_INDEX = 0
PLAY_TIME_INDEX = 1
PLAYER_NAME_INDEX = 2
FULL_NAME_INDEX = 3
PHOTO_IMAGE_INDEX = 4
#LABEL_INDEX = 5

for indi in getPNGList():
    indexes = indi.split('_')
    indexes[PLAY_TIME_INDEX] = int(indexes[PLAY_TIME_INDEX]) # make it integer.
    indexes[PLAYER_NAME_INDEX] = indexes[PLAYER_NAME_INDEX][:-4] # cut .png
    indexes.append(indi)#Full NAME
    photoimg = PhotoImage(file = indi).subsample(2,2)
    indexes.append(photoimg)
    #indexes.append(Label(root, image = indexes[PHOTO_IMAGE_INDEX]))
    DATA.append(indexes)

ROOT_X_MID = int(1400 / 2)
ROOT_Y_MID = int(800 / 2)
root.geometry('{}x{}'.format(2*ROOT_X_MID,2*ROOT_Y_MID))

DATA_len = len(DATA)

############################
globalIndex = 0
realIndex = 0
GuessedPlayer = False
GuessedMap = False
CorrectPlayer = False
CorrectMap = False
inputPlayer = ''
inputMap = ''
correctPlayer = ''
correctMap = ''
EpochCount = 0
pressGUESStoNextImg = False
playerRightCount = 0
mapRightCount = 0
bothRightCount = 0
total_Round = 0
STATISTICS = []
RECORD_STAT = [[0,0]for _ in range(DATA_len)]
Train_END = False
#Player_
############################

ImageList = []

Current_label = Label(root)
Current_label.grid(row=7,column=0)

def dispCur():
    global Current_label, GuessedMap, GuessedPlayer, inputPlayer, inputMap
    dispStr = '> '
    if(GuessedPlayer):
        dispStr += inputPlayer
    else:
        dispStr += '_'
    dispStr += ','
    if(GuessedMap):
        dispStr += inputMap
    else:
        dispStr += '_'
    Current_label.config(text=dispStr)
    print(dispStr)

def genImageList():
    global ImageList, globalIndex
    ImageList = []
    randomList = random.sample(range(DATA_len),DATA_len)
    for j in randomList:
        newList = [j]
        newList.append(DATA[j][PHOTO_IMAGE_INDEX])
        ImageList.append(newList)
    globalIndex = 0#Reset
    #ShowImage()#Show 1st 'new' list image.


def playerGuess_juho():
    global GuessedPlayer, inputPlayer, CorrectPlayer
    GuessedPlayer = True
    if('Kim' == correctPlayer):
        CorrectPlayer = True
    else:
        CorrectPlayer = False
    inputPlayer = 'Kim'
    dispCur()
    
def playerGuess_daun():
    global GuessedPlayer, inputPlayer, CorrectPlayer
    GuessedPlayer = True
    if('Jeong' == correctPlayer):
        CorrectPlayer = True
    else:
        CorrectPlayer = False
    inputPlayer = 'Jeong'
    dispCur()

def mapGuess_K():
    global GuessedMap, inputMap, CorrectMap
    GuessedMap = True
    if('K' == correctMap):
        CorrectMap = True
    else:
        CorrectMap = False
    inputMap = 'K'
    dispCur()

def mapGuess_S():
    global GuessedMap, inputMap , CorrectMap
    GuessedMap = True
    if('S' == correctMap):
        CorrectMap = True
    else:
        CorrectMap = False
    inputMap = 'S'
    dispCur()

def ShowImage():
    global ImageList, LABEL_INDEX, globalIndex, DATA_len, realIndex #Setts real one too!
    global bigPicLabel
    global correctPlayer, correctMap
    
    realIndex = ImageList[globalIndex][0]
    
    correctPlayer = DATA[realIndex][PLAYER_NAME_INDEX]
    correctMap = DATA[realIndex][MAP_NAME_INDEX]

    bigPicLabel.config(image = ImageList[globalIndex][1])
    globIdxLabel.config(text=(str(globalIndex+1)+'/{}'.format(len(ImageList))))

def displayEPOCH():
    global EpochCount
    topleft.config(text='Epoch: {}'.format(EpochCount))

def printSTATISTICS():
    global STATISTICS
    global ImageList
    global Export_Pipe
    global Train_END

    len_per_epoch = len(ImageList)
    print('printing STATISTICS....')
    temp = STATISTICS[-1][:]#Latest one, copy.
    Export_Pipe.write('Player / Map / BOTH\n')
    Export_Pipe.write('{},{},{}\n'.format(temp[0],temp[1],temp[2]))
    print('{},{},{}\n'.format(temp[0],temp[1],temp[2]))
    for j in range(3):
        temp[j] = round(temp[j]/len_per_epoch * 100,2)
    Export_Pipe.write('{}%,{}%,{}%\n'.format(temp[0],temp[1],temp[2]))
    if(temp[2] == 100):
        Train_END = True

def printRECORD_STAT():
    global RECORD_STAT
    global RECORD_STAT_pipe
    global DATA
    global EpochCount
    RECORD_STAT_pipe.write('>>\nEpoch:{}\n'.format(EpochCount))
    for i in range(len(RECORD_STAT)):
        RECORD_STAT_pipe.write((DATA[i][FULL_NAME_INDEX])[:-4])
        RECORD_STAT_pipe.write(':(')
        RECORD_STAT_pipe.write(str(RECORD_STAT[i][0]))#Player
        RECORD_STAT_pipe.write(',')
        RECORD_STAT_pipe.write(str(RECORD_STAT[i][1]))#MAP
        RECORD_STAT_pipe.write(')\n')
        print(RECORD_STAT[i])### YEAH PRINT!

def FILE_CLOSE():
    root.destroy()
    RECORD_STAT_pipe.close()
    Export_Pipe.close()

def gotoNextImage():
    global bigPicLabel, inputMap, inputPlayer, correctPlayer, correctMap, CorrectPlayer, CorrectMap
    global GuessedPlayer, GuessedMap, globalIndex

    global ImageList, EpochCount, pressGUESStoNextImg

    global playerRightCount,mapRightCount,bothRightCount, total_Round

    global STATISTICS, RECORD_STAT, realIndex

    global Train_END
    
    if(pressGUESStoNextImg):#HE is determined to GO!!!!
        pressGUESStoNextImg = False
        inputMap = ''
        inputPlayer = ''
        correctMap = ''
        correctPlayer = ''
        CorrrectPlayer = False
        CorrectMap = False
        GuessedMap = False
        GuessedPlayer= False
        if(globalIndex == len(ImageList)):#Reached the End
            EpochCount += 1

            displayEPOCH()
            
            globalIndex = 0
            genImageList()#Generate new LIST!!!
            
            newList = [playerRightCount,mapRightCount,bothRightCount]
            STATISTICS.append(newList)

            playerRightCount = 0
            mapRightCount = 0
            bothRightCount = 0
            
            printSTATISTICS()

            printRECORD_STAT()

            if(Train_END):
                FILE_CLOSE()
                print('You are TRAINED!!! Congrats.')
                input('DONE')
                quit()
            
            playerRightCount = 0
            mapRightCount = 0
            bothRightCount = 0
        ShowImage()
        
        globalIndex += 1
        
    elif(GuessedPlayer and GuessedMap):
        bigPicLabel.config(image = '')
        displayStr = 'You guessed (Player,Map) : ({},{})\n'.format(inputPlayer,inputMap)
        temp = 'Answer is (Player,Map) : ({},{})\n'.format(correctPlayer,correctMap)
        displayStr += temp
        temp1 = 'X'
        temp2 = 'X'
        if CorrectPlayer:
            RECORD_STAT[realIndex][0]+=1
            playerRightCount += 1
            temp1 = 'O' 
        if CorrectMap:
            RECORD_STAT[realIndex][1]+=1
            mapRightCount += 1
            temp2 = 'O'
        if CorrectPlayer and CorrectMap:
            bothRightCount += 1
        total_Round += 1
        temp = 'Result(O/X) : (플레이어:{}, 맵:{})\n'.format(temp1,temp2)
        displayStr += temp
        displayStr += 'Press guess! AGAIN to continue...\n\n\n'
        displayStr += '<<ROUND {}>>\n'.format(total_Round)
        displayStr += 'Your Both_Correct-rate : {}/{}({}%)\n'.format(bothRightCount,len(ImageList),round(bothRightCount/len(ImageList)*100,1))
        displayStr += 'Your Player_Correct-rate : {}/{}({}%)\n'.format(playerRightCount,len(ImageList),round(playerRightCount/len(ImageList)*100,1))
        displayStr += 'Your Map_Correct-rate : {}/{}({}%)\n'.format(mapRightCount,len(ImageList),round(mapRightCount/len(ImageList)*100,1))
        bigPicLabel.config(text = displayStr)
        pressGUESStoNextImg = True
        

#############

##############################
bigPicLabel = Label(root)
bigPicLabel.grid(row=0,column=1,rowspan=12)#!! PIC !!

topleft = Label(root)
topleft.grid(row = 0, column = 0)

playerLabel = Label(root,text="<PLAYER>")
playerLabel.grid(row=1,column=0)

Juho_label = Button(root,text="(Kim)주호",command=playerGuess_juho)
Juho_label.grid(row=2,column=0)

Daun_label = Button(root,text = "(Jeong)다운",command=playerGuess_daun)
Daun_label.grid(row=3,column=0)

MapLabel = Label(root,text="<MAP>")
MapLabel.grid(row=4,column=0)

Kal_label = Button(root,text="(K)칼바람",command=mapGuess_K)
Kal_label.grid(row=5,column=0)

Sohwan_label = Button(root,text="(S)소환사의 협곡",command=mapGuess_S)
Sohwan_label.grid(row=6,column=0)


globIdxLabel = Label(root)
globIdxLabel.grid(row=8,column=0)


####################################

nextButton = Button(root,text="Guess!",command=gotoNextImage)
nextButton.grid(row=9,column=0)

genButton = Button(root,text="CLOSE",command=FILE_CLOSE)
genButton.grid(row=10,column=0)

def Cheat():
    global pressGUESStoNextImg
    global ImageList
    global globalIndex

    global STATISTICS

    global playerRightCount,mapRightCount,bothRightCount
    
    pressGUESStoNextImg = True
    globalIndex = len(ImageList)

    playerRightCount,mapRightCount,bothRightCount = 0,0,len(ImageList)

    gotoNextImage()

cheatButton = Button(root,text="CHEAT",command = Cheat)
cheatButton.grid(row=11,column=0)

############

displayEPOCH()
genImageList()# DEFAULT
ShowImage()# DEFAULT
globalIndex = 1

###################################
'''
id_tk = Tk()
id_tk.title("Your ID???")
id_tk.geometry('200x200')
here = Entry(id_tk)
here.pack()

def madafunka():
    global id_tk, here, Export_Pipe, RECORD_STAT_pipe
    line = here.get()
    tempPath = './'+line#+ '_STAT' + '.txt'
    #if(os.path.exists
    Export_Pipe = open(tempPath+'_STAT.txt','w')
    RECORD_STAT_pipe = open(tempPath+'_RECO.txt','w')
    id_tk.destroy()
ook = Button(id_tk,text="OK",command=madafunka)
ook.pack()
'''
print('Look At Me!!')

mainloop()