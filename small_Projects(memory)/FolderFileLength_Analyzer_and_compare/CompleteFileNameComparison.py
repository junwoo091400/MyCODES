def getFileNameSet(root):
    
    if(root[0]=="\""):
        root = root[1:-1]#Cut em off

    if(root[-1] == '\\'):#Make sure...
        root = root[:-1]#Strip that '\\'

    root_len = len(root)

    import os

    MAX_POSSIBLE_FILE_LEN = 200

    FilesList = [set() for _ in range(MAX_POSSIBLE_FILE_LEN)]

    for root_, dirs, files in os.walk(root, topdown=True):

        root_ = root_[root_len:]#Yes. Get Only folder, etc.
        
        for f in files:#Check
            tmp = len(f)
            if(tmp > MAX_POSSIBLE_FILE_LEN):
                print('ERROR! Exceeded MAX POsss...',root,f)
            FilesList[tmp].add(os.path.join(root_,f))

    return FilesList

root1 = input('FROM(copy origin)?')#'G:\\Junwoo_HWANG_dontTOUCH'
root2 = input('TO?')#'E:\\Junwoo_HWANG_dontTOUCH'

print('From',root1,'to',root2,'comparing...')

fl1 = getFileNameSet(root1)
fl2 = getFileNameSet(root2)

exportName = 'compare_result.txt'

export = open(exportName, 'wt')

export.write('From ' + root1 + ' to ' + root2 + ' comparing...\n')
export.write('Basically the result of TO - FROM. Showing all the missing Files in TO_ folder!\n')
export.write('Note that FROM should be the bigger folder, it is assumed to be the Origin\n')

export.write('#########################\n')

for i in range(len(fl1)):
    if(len(fl1[i]) != len(fl2[i])):
        print('Found Discrepency at index:', i)
        subset = fl1[i] - fl2[i]#PRETTY MUCH.
        print('Dir1 has some more of...')
        #export.write('#########################\n')
        for s in subset:
            print(s)
            export.write(s + '\n')#NL hahaha.
        print('ok.')#LF!

export.close()

input('Waiting...')

