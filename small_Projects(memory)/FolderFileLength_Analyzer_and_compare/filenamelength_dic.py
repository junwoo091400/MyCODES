'''
<181207(Friday) Written by Junwoo HWANG>

TITLE:
A program that shows you the Spectrum of how your file Name - LENGTH (s) are Distributed...

WHY:
I personally needed this, because I couldn't copy some files from my Hard-Drive to another Hard-Drive.
I needed to find if there are some files with 'EXCESSIVE' lengths.

YOU CAN DO:
Modify 'Threshold' value at around line #33, as the Length Up until which, is considered 'Normal' length.
ex) Threshold = 143 => Will only show(on CMD console) fileNames with Length Over 143.

EPILOG:
The Windows Message said "File Name Too Long". And after a quick search, I found out that file names
(actually, including the directory TOO!!!) with length over 255 are NOT_RECOMMENDED. And actually, Windows
just rejects dealing with them.

'''

root = input('Base Dir?')

if(root[0]=="\""):
	root = root[1:-1]#Cut em off

exportName = 'result.txt'

import os


MAX_POSSIBLE_FILE_LEN = 200

Threshold = 143#Till THIS! It is considered Normal.


FilesList = [[] for _ in range(MAX_POSSIBLE_FILE_LEN)]

for root, dirs, files in os.walk(root, topdown=True):
    for f in files:#Check
        tmp = len(f)
        if(tmp > MAX_POSSIBLE_FILE_LEN):
            print('ERROR! Exceeded MAX POsss...',root,f)
        FilesList[tmp].append(os.path.join(root,f))

for i in range(len(FilesList)):
    tmp = len(FilesList[i])#TOTAL Number of Files.
    if(i > Threshold and tmp > 0):#Valid Number + OVER threshold.
        print('\nFiles with Name Length',i,':', tmp)
        for f in FilesList[i]:
            print(f)

export = open(exportName, 'wt')
for i in range(len(FilesList)):
    tmp = len(FilesList[i])
    if(tmp > 0):
        export.write('{}:{}\n'.format(i, tmp))
export.close()

input('Waiting...')

