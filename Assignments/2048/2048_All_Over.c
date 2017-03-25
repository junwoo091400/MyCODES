/*
<Developer's Note>

The 2048 GAME by Junwoo HWANG

1. First created at November/25/2016

2. This is re-creation of my Old- Lagging 2048 game(Paragliding)

3. Intended to acheive high efficiency & detailed Code Explanation
*/

//------------------------------------------------------------------------------------------------------

//////////////////////////////////////////
//All Necessary Headers
/////////////////////////////////////////
#include<stdio.h>//standard input/output
#include<stdlib.h>//rand() function
#include<conio.h>//getch() function
#include<time.h>//srand Seed -> time(NULL)
#include<windows.h>//system("cls")
#pragma warning(disable:4996)//scanf enable!

//------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////
//Global Variables/////////////////
////////////////////////////////////////////////////
int main_board[4][4];//Main Game-Processing Board
int chance_count = 0, stage = 0, score = 0;
char AI_mode = 0;//if 1, the computer automatically plays the game
char Exit_Game_Flag = 0, New_Game_Flag = 0;//Contains data whether user wants to end/start a new game
char User_Input;//Saves 'user's' input data
int Biggest_Score = 0;
char wrongKey = 0;//In case 'user-input' is invalid, turns it into 1

//Under Part, contains User's DATA(SigNum & RANK & Score & Stage)
int SigNum_Score_Stage[101][2] = {};//Maximum 1~100 Sig.Number, Score/Stage for each 2-int storage.
char SigNum_ID[101][21] = {};//Maximum 1~100 Sig.Number, 20 Alphabets for each String MAX.
int Rank_SigNum[101];//Contains RANKs by Sig.number down to 100th MAX.
int Now_SigNum=0, ID_count = 0;//Current (while playing) User's SigNum, Total ID in Data(Start from 1~)

//Additional Storage Space for Undo/Redo Function
int Stage_array[5001][17] = {};//Upto 5000th Stage (start from 1) holding 4x4 array + Score memory.
int ConsecutiveUndos = 0;

//Variables for the A.I movement!
int AI_Simulated_Score[4] = {};//Contains simulated score for 'w', 'a', 's', 'd' directions.
int AI_Repetition = 50;//Hardcoded, AI will perform 'AI_Repetition' simulations for each direction.
int AI_count = 0;//Count of 'HOW MANY' A.I was needed
//------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////
//Just FOR FUN Functions, I don't want to use 'string.h' library etc.
///////////////////////////////////////////////////////

void StringCopy(char*dest, char*source,int max_charac) {//Copies the String from source to Destinatoin
	int Length_source = 0;
	while (source[Length_source++] != 0);//Now, array[length-1] = 0 <NULL>
	if (Length_source > max_charac)
		Length_source = max_charac;//Limit Max. Amount of characters getting Copied
	for (int i = 0; i < Length_source - 1; i++)//Except for the NULL.
		dest[i] = source[i];//Copying
	dest[Length_source - 1] = 0;//NULL insert
}

int StringCompare(char*dest, char*source) {//Comparing two string, if Same, return 0, else, return 1
	int Length_dest = 0, Length_source = 0;
	while (dest[Length_dest++] != 0);
	while (source[Length_source++] != 0);//Total Length of each string calculated
	
	if (Length_dest == Length_source) {//If Same Length, Same Position of NULL
		int count = 0;
		while (dest[count] == source[count] && count < Length_dest - 1)
			count++;
		if (count == Length_dest - 1)//Last Character(length-2) verified and moved onto (length-1) => Same String!
			return 0;
		else
			return 1;//NOT SAME
	}
	else
		return 1;//Length different, no way those two are same string.
}

void ArrayCopy(int(*dest)[4], int(*source)[4]) {//Copying 4x4 array from source to destination
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dest[i][j] = source[i][j];
}

void AddOn_Data(char *dest, int *Data) {//Probably Save Stage - Score, each count(2,4,8,16,32,64,128,256,512,1024,2048,4096)//2+12
	FILE * fp = fopen(dest, "at");
	char buffer[20];
	/*while (1) {
		fgets(buffer, sizeof(buffer), fp);
		fputs(buffer, fp);
		if (feof != 0)//End
			break;
	}
	*/
	fprintf(fp,"%d", Data[0]);
	for (int i = 1; i < 14; i++) {
		fprintf(fp, ",%d", Data[i]);//CSV format
	}
	fprintf(fp,"\n");
	//Additional Data
	fclose(fp);
}

//------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////
//Functions(General)//////////////////////////
//////////////////////////////////////////////////
void New_Game_Initiate() {//New Game setting
	  
	////////Reset main_board to 0////////////////
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			main_board[i][j] = 0;

	//////////Reset Score, Stage, Chance_count///////
	chance_count = 0;
	stage = 1;//Start from Stage 1
	score = 0;
	AI_mode = 0;//No AI for 'default'
	New_Game_Flag = 0;
	int insertcount = 0;
	while (insertcount<2) {//until two blanks are filled

		int v = rand() % 4;
		int h = rand() % 4;
		
		if (main_board[v][h] == 0) {//If Blank found
			int insertingNumber = rand() % 5;//1:4 probability
			
			if (insertingNumber == 0)//20% probabiligy
				insertingNumber = 4;
			else
				insertingNumber = 2;//80% probability

			main_board[v][h] = insertingNumber;
			insertcount++;
		}
	}//End of 2 initial values insertion.

}//End of New_Game_Initiate() function

void New_Game_getID() {//Get User's ID for NEW GAME
	char s[21]="";//Max. 20 Alphabets
	//printf("SPACE Key in your ID doesn't matter\n");
	
	system("cls");//Erase console window's writing

	printf("WELCOME ABOARD NEW-GAME!\n");//Nice Greeting
	
	printf("ENTER YOUR ID(Max. 20 chracters)\n");
	
	
	scanf("%s", s);
	
	
	if (ID_count > 0) {//if this is after 1st game
		
		for (int i = 1; i <= ID_count; i++) {
			if (!StringCompare(s, SigNum_ID[i])) {//comparing s with ID of SigNum 'i'. If SAME! it returns 0
				Now_SigNum = i;//Already Existing ID. Now is that ID.
				return;
			}//End of 'if Strings are Same'
		}//End of Comparing i= 1 ~ ID_count 'for' statement

		//Couldn't find Original ID
		ID_count++;//New ID!
		Now_SigNum = ID_count;
		StringCopy(SigNum_ID[ID_count], s, 21);//Save ID into SigNum_ID array!

	}//End of ID_count>0 verification (if-statement)
	else if(ID_count==0) {//ID_count is 0.
		ID_count = 1;//First ID!
		Now_SigNum = 1;
		StringCopy(SigNum_ID[1], s, 21);//Copy the NEW ID to the SigNum_ID[1] string
	}
}//End of New_Game_getID() function.

//BIGGEST FUNCTION//////////////////////////////////////////////////////////
void Process_movement(int (*main_board)[4],char c,int *score,int *stage) {//Typical process for combining etc.

	int CompareBoard[4][4] = {};//To see if the board moved.

	ArrayCopy(CompareBoard, main_board);//Save the original

	int horizontal = 0, vertical = 0;//To combine all complicated lines of code into one, use coefficient
	int startpoint = 0;//Provides the first point to start looking for 0-blank

	switch (c) {//Setting coefficients for simple function
	case 'w': horizontal = 0; vertical = -1; startpoint = 0; break;
	case 'a': horizontal = -1; vertical = 0; startpoint = 0; break;
	case 's': horizontal = 0; vertical = 1; startpoint = 3; break;
	case 'd': horizontal = 1; vertical = 0; startpoint = 3; break;
	default: return;//Just in case something bad comes in 
	}

	////////////////////////////////////////////////////
	//1. Eliminating Zeros on the way(TESTED: Fast and Finally working, 161125)
	////////////////////////////////////////////////////

	//Horizontal Zero-Blank Elimination////////////////////
	if (vertical == 0) {//Going horizontal, 'a' or 'd'
		for (int i = 3; i >= 1; i--) {//Repeated 3 Times!(Short Version : 3,2,1 : 6 times)

			for (int vertic = 0; vertic < 4; vertic++) {//Horizontal operation goes through vertical axis evenly

				for (int hori = 0; hori < i; hori++)//3->2->1 comparison
					if (main_board[vertic][startpoint - hori*horizontal] == 0 && main_board[vertic][startpoint - hori*horizontal - horizontal] != 0) {//Change Needed
						main_board[vertic][startpoint - hori*horizontal] = main_board[vertic][startpoint - hori*horizontal - horizontal];
						main_board[vertic][startpoint - hori*horizontal - horizontal] = 0;//You Move out!
					}//End of statement if(0-blank detected)
			}//End of going through vertical axis 0 ~> 4
		}//End of Repetition
	}//End of Horizontal Manuver

	//Vertical Zero-Blank Elimination///////////////
	else {
		for (int i = 3; i >= 1; i--) {//Repeated 3 Times!(Short Version : 3,2,1 : 6 times)

			for (int hori = 0; hori < 4; hori++) {//Vertical operation goes through horizontal axis evenly

				for (int vertic = 0; vertic < i; vertic++)//3->2->1 comparison
					if (main_board[startpoint - vertic*vertical][hori] == 0 && main_board[startpoint - vertic*vertical - vertical][hori] != 0) {//Change Needed
						main_board[startpoint - vertic*vertical][hori] = main_board[startpoint - vertic*vertical - vertical][hori];
						main_board[startpoint - vertic*vertical - vertical][hori] = 0;//You Move out!
					}//End of statement if(0-blank detected)
			}//End of going through horizontal axis 0 ~> 4
		}//End of Repetition
	}//End of Vertical Manuver
	/////////////Elimination of "0 Blank" completed!

	///////////////////////////////////////////////////////////////////////
	//2. Pushing numbers & Combining Them (Tested, Also Finally working and Fast, however 
	//in 'TWO SUM MODE', this will also process 0-0-0-0 or 0-0-2-2 kind of array
	//I found this quite annoying and if it affects the program too much, whihc probably wouldn't happen,
	//I would add two (!=0) statements to the 'if' where it checks if it is 'TWO SUM-situation',161126)
	//////////////////////////////////////////////////////////////////////
	if (vertical == 0) {//Horizontal Movement Needed : 'a' or 'd'

		for (int vertic = 0; vertic < 4; vertic++) {//Horizontal operation goes through vertical axis evenly
			if (main_board[vertic][0] == main_board[vertic][1] && main_board[vertic][2] == main_board[vertic][3]) {//Horizontal Two Sum CASE
				main_board[vertic][startpoint] *= 2;
				main_board[vertic][startpoint - horizontal] = main_board[vertic][startpoint-2*horizontal]*2;
				main_board[vertic][startpoint - 2 * horizontal] = 0;
				main_board[vertic][3 - startpoint] = 0;
				*score += (main_board[vertic][startpoint] + main_board[vertic][startpoint - horizontal]);//MODIFY SCORE FOR: 'TWO SUM CASE'
			}//TWO SUM CASE 'if' End.
			else {//NOT a 'TWO SUM case'.
				for (int hori = startpoint; hori!=(3-startpoint); hori -= horizontal) {//Startpoint ~ Just 'before' End
					if (main_board[vertic][hori] == main_board[vertic][hori - horizontal] && main_board[vertic][hori] != 0) {//If SAME(Can combine), No '0' sum!

						main_board[vertic][hori] *= 2;
						main_board[vertic][hori - horizontal] = 0;
						*score += main_board[vertic][hori];//MODIFY SCORE FOR: 'ONE SUM CASE'

						//////////////////////////////////////////////////////////
						/////Because [vertic][hori-horizontal] is now '0', we need to push some numbers
						int temp = hori - horizontal;//start

						while (temp > 0 && temp < 3) {//until '1' or '2'. '0' and '3' cases is Treated Below V
							main_board[vertic][temp] = main_board[vertic][temp - horizontal];
							temp -= horizontal;
						}
						main_board[vertic][3 - startpoint] = 0;//Opposite End set to '0' because it was pushed
						///////////////////////////////////////////////

					}//end of 'if' Sum-able!
				}//END of 'for' statement, of horizontal variation, searching for 'Sum-able' groups.
			}//End of 'else', which means it is not a 'TWO-SUM' case.
		}//End of going through vertical axis 0 ~> 4
	}//End of 'if' Vertical=0. Horizontally Pushing Numbers!

	else {//Vertical ==1. Now 'Vertically' pushing numbers!

		for (int hori = 0; hori < 4; hori++) {//Vertical operation goes through horizontal axis evenly

			if (main_board[0][hori] == main_board[1][hori] && main_board[2][hori] == main_board[3][hori]) {//Vertical Two Sum CASE
				main_board[startpoint][hori] *= 2;
				main_board[startpoint - vertical][hori] = main_board[startpoint - 2 *vertical][hori]*2;
				main_board[startpoint - 2 * vertical][hori] = 0;
				main_board[3-startpoint][hori] = 0;
				*score += (main_board[startpoint][hori] + main_board[startpoint - vertical][hori]);//MODIFY SCORE FOR: 'TWO SUM CASE'
			}//TWO SUM CASE 'if' End.
			else {//ONE SUM CASE
				for (int vertic = startpoint; vertic!=(3-startpoint); vertic -= vertical) {//Startpoint ~ Just 'before' End
					if (main_board[vertic][hori] == main_board[vertic - vertical][hori] && main_board[vertic][hori] != 0) {//If SAME(Can combine), No '0' sum!

						main_board[vertic][hori] *= 2;
						main_board[vertic - vertical][hori] = 0;
						*score += main_board[vertic][hori];//MODIFY SCORE FOR: 'ONE SUM CASE'

						//////////////////////////////////////////////////////////
						/////Because [vertic][hori-horizontal] is now '0', we need to push some numbers
						int temp = vertic - vertical;//start

						while (temp > 0 && temp < 3) {//until '1' or '2'. '0' and '3' cases is Treated Below V
							main_board[temp][hori] = main_board[temp - vertical][hori];
							temp -= vertical;
						}
						main_board[3 - startpoint][hori] = 0;//Opposite End set to '0' because it was pushed
						///////////////////////////////////////////////

					}//end of 'if' Sum-able!
				}//END of 'for' statement for vertic variation, finding sum-able groups
			}//END of 'else' of TWO-SUM CASE.
		}//End of going through vertical axis 0 ~> 4
	}//End of 'else' Vertical==0. Which means, Vertically Pushing Numbers!

	//So, the Pushing stuffs are Over. Now, we see if it really changed.
	int un_changed = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (CompareBoard[i][j] == main_board[i][j])//If same as the old one,
				un_changed++;

	if (un_changed == 16);//If everyghing Unchanged, do Nothing
	else {//If something Changed, that means new '0' is available after pushing/combining.
		
		(*stage)++;//Stage Update

		//And... Insert Numbers!
		int newNumber = rand() % 5;//1:4 Probability
		
		if (newNumber == 0)
			newNumber = 4;//Insert 4
		else
			newNumber = 2;//Insert 2
	//Got the New Number of 2 or 4

		int v, h;
		while (1) {
			v = rand() % 4;
			h = rand() % 4;
			if (main_board[v][h] == 0) {
				main_board[v][h] = newNumber;
				break;//Because newNumber is inserted, get out of this loop.
			}
		}
	}
}//End of Process_Movement() Function!
/////////////////////////////////////////////////////////////////////////////////


void Refresh_and_Print() {//Refreshes the Screen & Prints the 'Main-board'
	system("cls");//Empties console window

	printf("Up(w), Down(s), Left(a), Right(d)\n");//Instruction of Keys
	printf("NewGame(n), Exit(x), Chance(c), Undo(u), Redo(u), AutoPlay(p)\n");
	if (AI_mode)//Special command to indicate that 'AI mode' is ON
		printf("\n<<A.I MODE (Computer is in command)>>\n");
	printf("\nScore: %d Stage: %d Biggest Score: %d\n", score, stage,Biggest_Score);//Print Saved-Variables

	printf(" %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n ", 1, 6, 6, 6, 6, 22, 6, 6, 6, 6, 22, 6, 6, 6, 6, 22, 6, 6, 6, 6, 2);//가장 윗 선 출력
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%c", 5);
			if (main_board[i][j] == 0)
				printf("   0");
			else
				printf("%4d", main_board[i][j]);

		}printf("%c\n ", 5);//가장 오른쪽 칸막이
		if (i != 3)
			printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n ", 25, 6, 6, 6, 6, 16, 6, 6, 6, 6, 16, 6, 6, 6, 6, 16, 6, 6, 6, 6, 23);//중간 경계선 출력
	}
	printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 3, 6, 6, 6, 6, 21, 6, 6, 6, 6, 21, 6, 6, 6, 6, 21, 6, 6, 6, 6, 4);//가장 아래선 출력

	printf("Chances Left: %d\n", 3 - chance_count);
	printf(">>");//Indication for user to Type something
}

void Chance_Initiate(int *chance_count,int(*main_board)[4]) {//Initiates 'Chance-sequence'
	/////TESTED: works! (161126)
	int temp[16] = {};
	if ((*chance_count) < 3) {//Just in case we are processing this function where user already used 3 chanes
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				temp[4 * (i % 4) + j] = main_board[i][j];//Saves the data to re-organize in Snail-Configuration
	////Dirty Style of Snail-method//////////////
		int Snail[16][2] = { 0,0,0,1,0,2,0,3,
		1,3,2,3,3,3,
		3,2,3,1,3,0,
		2,0,1,0,///////////////!!!!!!!! Yeah, I know, it is dirty, I will change it(161126).
		1,1,1,2,
		2,2,2,1 };
	///////////////////////////////////////////////
		for (int i = 0; i < 16; i++)
			main_board[Snail[i][0]][Snail[i][1]] = temp[i];
		(*chance_count)++;//Add 1 to chance_count
	}//End of 'if(chance_count<3)'
}

void EndofGame_SaveData_Rank_Modify() {//Saves the Last game's data and Organizes the Rank system.
	
	if (Now_SigNum == ID_count) {//This was a NEW-ID Game
		SigNum_Score_Stage[Now_SigNum][0] = score;
		SigNum_Score_Stage[Now_SigNum][1] = stage;
		Rank_SigNum[Now_SigNum] = Now_SigNum;//Save at the 'loser' part.
	}
	else {//This Game was performed with OLD-ID(reused).
		if (SigNum_Score_Stage[Now_SigNum][0] < score) {//If Bigger-than-before Score was set,
			SigNum_Score_Stage[Now_SigNum][0] = score;//Over-write the DATA
			SigNum_Score_Stage[Now_SigNum][1] = stage;
		}
	}//End of 'OLD-ID' processing

	//Since we now have organized & updated Data, we can Rank them. Of course, only one pass-through is needed.
	//Because the Rank would be organized already, except for the possibility that it could've changed from
	//'Now_SigNum' part ~ 1st Rank.
	
	int Start_Rank = 0;//This 'last game' User's rank.

	for (int i = 1; i <= ID_count; i++) {//FINDING the current RANK of this Particular SigNum(ID)
		if (Rank_SigNum[i] == Now_SigNum)//if(Now_SigNum was detected at 'i'th Rank
			Start_Rank = i;//If, New ID case, this would be 'ID_count'. The last one. 
	}//FOUND the Rank!

	if (Start_Rank != 0) {//Just in Case, something went wrong and Rank couldn't be detected.
		for (int i = Start_Rank; i >= 2; i--) {//Starting from Start until 2, finally comparing with Rank No.1
			int LowRank_SigNum = Rank_SigNum[i];
			int HighRank_SigNum = Rank_SigNum[i - 1];
			if (SigNum_Score_Stage[LowRank_SigNum][0] > SigNum_Score_Stage[HighRank_SigNum][0]) {//if lower rank has bigger score
				Rank_SigNum[i - 1] = LowRank_SigNum;//Moving Lower Ranked SigNum to the Higher level!
				Rank_SigNum[i] = HighRank_SigNum;//Moving Higher Ranked(original) SigNum ot the Lower level!
			}//End of Switching two Rank's SigNums
		}//End of going through Low Rank of 'start ~ 2'
	}//End of 'if(current SigNum's Rank DETECTED)'

	else {//Start_Rank == 0, this is wrong!
		printf("ERROR: Rank of your ID can't be detected\n");
		printf("YOUR Now_SigNum : %d\n ID : %s\n", Now_SigNum, SigNum_ID[Now_SigNum]);
		return;
	}
}

void EndofGame_Print() {//Print the TOTAL Result after GAME OVER
	system("cls");
	Biggest_Score = SigNum_Score_Stage[Rank_SigNum[1]][0];
	printf("<< GAME OVER! >>\n");
	printf("\nYOUR GAME Score: %d Stage: %d Biggest Score:%d\n", score, stage, Biggest_Score);//Print Saved-Variables
	printf("\n<<RANK LIST>>\n");
	printf("RANK . ID . SCORE . STAGE\n");
	for (int i = 1; i <= 5; i++) {
		int temp = Rank_SigNum[i];
		printf("No.%d . %s . %d . %d\n", i, SigNum_ID[temp], SigNum_Score_Stage[temp][0], SigNum_Score_Stage[temp][1]);
	}
	printf("-------------------------\n");
	for (int i = 6; i <= ID_count; i++) {
		int temp = Rank_SigNum[i];
		printf("No.%d . %s . %d . %d\n", i, SigNum_ID[temp], SigNum_Score_Stage[temp][0], SigNum_Score_Stage[temp][1]);
	}
	Sleep(500);
	printf("\nPRESS ANY KEY TO PLAY ANOTHER GAME!\n");
	char c = getch();//Until user types something
}

int Check_GameOver(int(*main_board)[4]) {//Checks wheter game is over or not
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (main_board[i][j] == 0 || (main_board[i][j] == main_board[i + 1][j]) || (main_board[i][j] == main_board[i][j + 1]))
				return 0;//If any 0, any cross 'Same' match appears, return.
	for (int i = 0; i < 3; i++) {
		if (main_board[3][i] == 0 || (main_board[3][i] == main_board[3][i + 1]))
			return 0;
		if (main_board[i][3] == 0 || (main_board[i][3] == main_board[i + 1][3]))
			return 0;
	}
	//No 0 and No Same Pairs. Only way to survive is using: 'Chances'.
	return 1;
}

void Undo() {//Goes Back, no limitation
	if (stage > 1) {//At least from stage 2 man!
		stage--;//Stage Decrease
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				main_board[i][j] = Stage_array[stage][i*4+j];//To the Past!(0~15)
		score = Stage_array[stage][16];//Score Update of the Past.
		ConsecutiveUndos++;//ConsecutiveUndos +1, can go to 'stage-1' to reach stage 1.
	}
}//End of 'undo' function.

void Redo() {//Going back to the 'future'
	if (ConsecutiveUndos > 0) {//Redo only possible if consecutiveundos is more than 0.
		stage++;//Going to the future
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				main_board[i][j] = Stage_array[stage][i*4+j];//To the future!
		score = Stage_array[stage][16];
		ConsecutiveUndos--;//might become 0.
	}
}//End of 'redo' function.

void Save_StageHistory() {//Saving Data for each stage for Undo & Redo function.
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Stage_array[stage][i * 4 + j]=main_board[i][j];//4x4 save
	Stage_array[stage][16] = score;//score save
}

//------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////
//Functions(A.I)//////////////////////////
//////////////////////////////////////////////////

//Most Important function in A.I processing. This function tests the result of 
//rand() party. After 'one' push for desired 'direction'. This will return the Additional Score
//It might get. From the given 'chance_count' and 'Better_Mode' constant.
//Also, if the movement is not even possible, it will return -1
//Because when the movement is possible but no merging happens, the result will be same as
//the case of Non-moving disaster. So this will surely prevent Misconception later on.
int Score_Until_Over(int(*board)[4],char direction,int chance_count,int Better_Mode) {
	char array[4] = { 'w','a','s','d' };

	//This function processes so quickly. So if we give srand() here,
	//It will cause give 'return values' for consecutive function-calls
	//Which means that this function doesn't take more than 1 second to process.
	//And, to simulate 'chance' in AI_pre scoring, if Better_Mode==1, chance count is evaluated
	//in AI's run.

	int AI_chance_count = chance_count;
	int AI_score = 0;//Score getting earned from the array given state.
	int AI_stage = 0;//Does nothing, just to prevent interference with 'main-stage' while Processing
	int Compare_Array[4][4];
	
	char Same = 0;//Is this movement even possible at first?
	
	ArrayCopy(Compare_Array,board);//Copying, don't touch the 'board' array, it will cause problems!

	Process_movement(Compare_Array, direction, &AI_score,&AI_stage);//Don't change the original 'board' array.

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (Compare_Array[i][j] == board[i][j])
				Same++;

	if (Same == 16)//Nothing Chaned, Unmovable to this direction
		return -1;//Can't Even Move!
	
	else {//Something changed, movable!
		while (1) {//Loop!
		
			int Game_Over_check = Check_GameOver(Compare_Array);
			if (Game_Over_check == 1) {//if Game Over!
				
				if (Better_Mode == 1&&AI_chance_count<3) {//Account for chances.
					Chance_Initiate(&AI_chance_count, Compare_Array);//Simulate 'chance'
				}
				else {//Not able to survive, just dead(either Better Mode disabled / chance count expired!)
					break;//Get Out of the 'while(1)' statement
				}
			}
			//Get A new Key!
			char Key = rand() % 4;
			
			Process_movement(Compare_Array, array[Key], &AI_score,&AI_stage);//Random direction, give Score's pointer to modify it.
		}
	}
	//Game Over, Ended!
	return AI_score;//all that it got!

}//End of Score_Until_Over function

void AI_Give_Name() {//This function gives 'Name' to A.I
	AI_count++;//AI count increase by 1

	if (Now_SigNum == ID_count);//This game was by NEW ID
	else {//If playing with OLD ID!
		ID_count++;
		}
	char NewString[21] = "1516JunwoHWNAG_AI_";//18 Characters
	int Tenth = AI_count / 10;
	int Oneth = AI_count % 10;
	Tenth += 48;//ASCII code '0' = 48
	Oneth += 48;
	NewString[18] = Tenth;//Over-write
	NewString[19] = Oneth;
	NewString[20] = 0;//NULL at the END!
	StringCopy(SigNum_ID[ID_count], NewString, 21);
}

//Puts 'AI_Simulated_Score' array with simulated 'mean' value for each direction
void AI_Simulation_Score_Four_Directions(int *AI_Simulated_Score,int (*main_board)[4],int chance_count,int Better_Mode) {
	
	char array[4] = { 'w','a','s','d' };

	unsigned long AI_Final_Score = 0;

	for (int i = 0; i < 4; i++) {//Four directions

		AI_Final_Score = 0;//Reset the Sum thing.

		for (int j = 0; j < AI_Repetition; j++) {//At least '50' times
		
			int Score_at_End = Score_Until_Over(main_board, array[i], chance_count, Better_Mode);//Save score returned
		
			if (Score_at_End == -1)//Not even moved!
				AI_Final_Score += 0;//Add nothing
			else
				AI_Final_Score += (Score_at_End + 1); //Add score+1
		}
		AI_Simulated_Score[i] = AI_Final_Score / AI_Repetition-1;//Get the 'mean' value for Simulated Score
		
		//AI_Simulated_Score array is set to be 'int', so it would be able to handle '-1' value for sure
	}//End of 'for' for All 4 directions

}//End of 'AI_Simulation_Score_Flur_Directions' function

////////////////////////////////////////////
//Functions Under is for 'Evaluation Tests'

int Scan(int(*board)[4], int number) {//See how many components match with 'number'
	int numberCount = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (board[i][j] == number)
				numberCount++;
	return numberCount;
}

void AI_evaluation() {//Evaluate A.I's performance
	/*
	
	*/
	int TotalScoreArray[100] = {};
	char array[4] = { 'w','a','s','d' };

	for (int testNumber = 0; testNumber < 100; testNumber++) {//100 Tests
		//Test # Start//
		
		//Initialization for every test.
		int test_Board[4][4] = {};
		int test_Score = 0;
		int test_Stage = 0;
		int test_Chance_count = 0;
		unsigned long test_Final_Score = 0;
		int test_Simulated_Score[4] = {};
		int Better_Mode = 0;
		int Inserted = 0;
		char Stream_Array[37] = "C:\\JunwooHWANG\\2048_AI\\Test00.txt";
		//Saving Address Initialization
		Stream_Array[27] = testNumber / 10 + 48;
		Stream_Array[28] = testNumber % 10 + 48;
		//
		while (Inserted < 2) {
			int v = rand() % 4;
			int h = rand() % 4;
			if (test_Board[v][h] == 0)
				test_Board[v][h] = (rand() % 5 == 0 ? 4 : 2); Inserted++;
		}//Insert Complete(Two numbers)

		while (1) {//MainTestLoop

			//GAME OVER CHECK///
			if (Check_GameOver(test_Board)) {//Game Over Check. We don't want to get Refreshed! Even if Game Over determined, user need to input one more key.
				if (test_Chance_count < 3) {
					Chance_Initiate(&test_Chance_count, test_Board);
				}
				else {//Game Over, but no chance Left
					TotalScoreArray[testNumber] = test_Score;
					printf("%d. %d\n", testNumber, test_Score);
					break;//Out of Main Test# loop
				}
		}//if (Check_GameOver)
			
			///////////////////////////////////////////////////
			//Main AI Processing//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			AI_Simulation_Score_Four_Directions(test_Simulated_Score, test_Board, test_Chance_count, Better_Mode);

			int test_biggestScore_Movement = 0;

			//Determining which movement gave the highest Score
			for (int i = 1; i < 4; i++)
				if (test_Simulated_Score[i]>test_Simulated_Score[test_biggestScore_Movement])//If bigger score is found
					test_biggestScore_Movement = i;//Biggest Score got.

			//////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			///////////////////////////////////////////////////
			int Last_Stage = test_Stage;
				Process_movement(test_Board, array[test_biggestScore_Movement], &test_Score, &test_Stage);//Really MOVE
				
				///TXT file SAVING......
				if (Last_Stage != test_Stage) {//changed Stage

					int Data[14] = { test_Stage,test_Score };//0~1
					for (int i = 2; i <= 13; i++)
						Data[i] = Scan(test_Board, 1 << (i - 1));//2~13

					AddOn_Data(Stream_Array, Data);
				}
				//////SAVING END...

		}//End of Main Test# loop

		//Test # End//
	}

	FILE * fp = fopen("C:\\JunwooHWANG\\2048_AI\\TestEND.txt", "wt");
	for (int i = 0; i < 100; i++)
		fprintf(fp,"%d,%d\n", i, TotalScoreArray[i]);
	fclose(fp);
}//End of 'AI_evaluation' function

//------------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////
//////////Main Function//////////////////////////////
/////////////////////////////////////////////////////
int main() {
	srand(time(NULL));
	/*
	AI_evaluation();
	while (1);
	*/
while (1) {//This is the part where everything related to the game happens, out of this = PROGRAM END!

		srand(time(NULL));//Give New Seed for srand every 'New' Game. Only srand in this program!

		New_Game_Initiate();//Reset values
		New_Game_getID();//Gets the ID, examines it etc.

		while (1) {//Main Game LOOP

			///////////////////////////////////
			//Main-0. Doin' Ground_Leveling_Work!
			/////////////////////////////////////
			
			Refresh_and_Print();//Clear Screen and Print

			if (Check_GameOver(main_board)) {//Game Over Check. We don't want to get Refreshed! Even if Game Over determined, user need to input one more key.
				if (chance_count < 3) {
					printf("Well.. You can't move now but you have 3 Chances Left!\n");
					if (AI_mode == 1) {//Computer has no way but pressing this button
						printf("c"); Chance_Initiate(&chance_count,main_board);
					}//END of if(AI mode)
				}
				else {//Game Over!!

					/*///Test
					if (AI_mode == 1) {
						char c = getch();
						c = getch();//To see the final score
					}
					///////////
					*/

					printf("I don't care whether you can do redo/undo or sth like that. You are out!\n");
					New_Game_Flag = 1;//Indicate New Game!
				}
			}//End of Checking GameOver
			
			Save_StageHistory();//This will be repeated at every loop. Just to insure.

			///////////////////////////////////
			//Main-1. Ordinary User_Input Sequence
			/////////////////////////////////////
			
			if (AI_mode) {//If AI mode enabled, run AI, instead of user 
				
				char array[4] = { 'w','a','s','d' };
				
				int Better_Mode = 0;//Let's use better mode for now.

				AI_Simulation_Score_Four_Directions(AI_Simulated_Score, main_board, chance_count, Better_Mode);

					/////////////TEST()161128
					for (int i = 0; i < 4; i++)
						printf("%d\n", AI_Simulated_Score[i]);
					//char c = getch();
					////////////////////////////////////

				int AI_biggestScore_Movement = 0;

				//Determining which movement gave the highest Score
				for (int i = 1; i < 4; i++)
					if (AI_Simulated_Score[i]>AI_Simulated_Score[AI_biggestScore_Movement])//If bigger score is found
						AI_biggestScore_Movement = i;//Biggest Score got.

				printf("%c", array[AI_biggestScore_Movement]);//Print to show what AI selected

				//Processing the board with AI_movement
				Process_movement(main_board, array[AI_biggestScore_Movement], &score,&stage);//Really MOVE
				
				Sleep(1);//Prevent 'CPU' crushing/ overloading, give Sleep(millis)	
			
			}//End of 'AI-Mode'
			
			else {//AI mode disabled, get user input

				User_Input = getch();//Get User Input
				switch (User_Input) {
					//Normal Keys
				case 'w':
				case 'a':
				case 's':
				case 'd': Process_movement(main_board, User_Input,&score,&stage); break;

					//Special Keys
				case 'x': Exit_Game_Flag = 1; break;//User wants to Exit the program
				case 'n': New_Game_Flag = 1; break;//User wants to start a New Game
				case 'c': Chance_Initiate(&chance_count,main_board); break;//Initiate Chance !
				case 'r': Redo(); break;
				case 'u': Undo(); break;
				case 'p': AI_mode = 1; AI_Give_Name(); break;//Auto Play!
				default: wrongKey = 1;
				}
			}//End of 'AI mode' disabled 'else'

			if (!(User_Input == 'r' || User_Input == 'u') && wrongKey == 0) {//If non-redo/undo & Intentional Key is typed
				ConsecutiveUndos = 0;//Future is Gone!
			}
			wrongKey = 0;//Initialize

			///////////////////////////////////
			//Main-2. New Game or Exit Game CHECK
			/////////////////////////////////
			if (New_Game_Flag == 1 || Exit_Game_Flag == 1)
				break;//Go to the Last part. Showing Rank. And Automatically RESTART

		}//End of Main Game Loop

		EndofGame_SaveData_Rank_Modify();//Save the Last game's data, Rank them.
		EndofGame_Print();//Print Last game result, Ranks etc.

		if (Exit_Game_Flag == 1)//User wants EXIT?
			break;//GET OUT OF THE BIGGEST WHILE LOOP -> Program END

	}//End of the Biggest While(1) loop

	return 0;
}
/////////////End of the Program///////////////////
