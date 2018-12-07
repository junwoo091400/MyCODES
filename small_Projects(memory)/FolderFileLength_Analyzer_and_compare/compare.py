#Just a simple program to Analyze two FileLengthCount result Files(txt). HaHA.

fileOne = '기존하드디스크.txt'
fileTwo = '새하드디스크.txt'

db1 = [0 for _ in range(200)]
db2 = [0 for _ in range(200)]

f1 = open(fileOne, 'r')
data1 = [f.rstrip().split(':') for f in f1.readlines()]#Strip '\n'
for d in data1:
    fileLen = int(d[0])
    count = int(d[1])
    db1[fileLen] = count
f1.close()

f1 = open(fileTwo, 'r')
data1 = [f.rstrip().split(':') for f in f1.readlines()]#Strip '\n'

for d in data1:
    fileLen = int(d[0])
    count = int(d[1])
    db2[fileLen] = count
f1.close()

for i in range(len(db1)):
    if(db1[i] != db2[i]):
        print('\nMismatch at index', i, '!')
        print(db1[i],':',db2[i], 'Right - Left =', db2[i] - db1[i])

input('Waiting...')
