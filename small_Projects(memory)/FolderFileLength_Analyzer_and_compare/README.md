Made on 2018 / 12 / 07 (Friday) by Junwoo HWANG

What Each Program Does:

1) CompleteFileNameComparison.py

You can COMPARE two (supposidly the same, but is different) Folders, and find which files are missing from the Target Folder.

For example :

You can check if your Backup-Drive is up-to-date with your current-Data.

So in that case, FROM directory will be your 'Current-Data'(because it is the Origin of the copy)

and TO directory will be your 'Backup-Drive'(because it is the result of the copy).

Then, you can see basically (FROM - TO). In other words, WHICH files are Missing from 'TO(Backup-Drive)' folder!

2) filenamelength_dic.py

You can see how the 'fileName Length' of every single file in your designated folder is distributed. This can be used for finding some FILES with 'overly large name length'.

For example :

You can use this to find every Windows-OS-Rejected files, because Windows doesn't like long file-names, it is handy to find exactly where they are located, and manually rename them. Because you don't want those rejections to be excluded from being copied to the BackUp-Drive :)

3) compare.py

Just a utility to compare two files exported from filenamelength_dic PROGRAM. Useful when comparing if two Folder contains Different Spectrum of File-Name-Lengths.

****************************************

I coded this just to find out that the files with excessive name length(including directory name) were just few garbage-like Android Studio image files. And one actual Video that I wanted to find. So anyways, I am leaving this for the future use. Maybe.