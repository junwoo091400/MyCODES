import csv
import datetime

class DateHandler:
	BREAKFAST = 0
	LUNCH = 1
	DINNER = 2
	SNACK = 3
	NOT_VALID = 4
	WeekDayStr_l = ['Monday','Tuesday','Wednesday','Thursday','Friday','Saturday','Sunday']
	StateStr_l = ['Breakfast','Lunch','Dinner','Snack','NOT_VALID']
	@staticmethod
	def Weekday2Str(weekday):
		return DateHandler.WeekDayStr_l[weekday]
	@staticmethod
	def State2Str(state):
		return DateHandler.StateStr_l[state]

	@staticmethod
	def getDateTime(date_str):
		ymd, hms, _ = date_str.split(' ')
		year,month,day = (int(x) for x in ymd.split('-'))
		hour,minute,seconds = (int(x) for x in  hms.split(':'))
		cur_Date = datetime.datetime(year,month,day,hour,minute,seconds)
		cur_Date += datetime.timedelta(hours = 9)#Because, raw data is in UTC.
		return cur_Date

	@staticmethod
	def getScheduleIdx(dateTime_obj):
		WeekDay = dateTime_obj.weekday()
		hour,minute,seconds = (int(x) for x in dateTime_obj.timetuple()[3:6])

		if(WeekDay <= 3):#Monday ~ Thursday. (NORMAL)
			if(hour>=6 and hour <= 8):
				return DateHandler.BREAKFAST
			elif(hour >= 11 and hour <= 13):
				return DateHandler.LUNCH
			elif(hour >= 17 and hour <= 19):
				return DateHandler.DINNER
			elif(hour >= 20 and hour <= 21):
				return DateHandler.SNACK
			else:
				return DateHandler.NOT_VALID
		elif(WeekDay == 4):#Friday! (NO SNACK)
			if(hour>=6 and hour <= 8):
				return DateHandler.BREAKFAST
			elif(hour >= 11 and hour <= 13):
				return DateHandler.LUNCH
			elif(hour >= 17 and hour <= 19):
				return DateHandler.DINNER
			else:
				return DateHandler.NOT_VALID
		elif(WeekDay <= 6):#Sat,Sunday. (NO SNACK + EARLY-DINNER)
			if(hour>=6 and hour <= 8):
				return DateHandler.BREAKFAST
			elif(hour >= 11 and hour <= 13):
				return DateHandler.LUNCH
			elif(hour >= 16 and hour <= 18):
				return DateHandler.DINNER
			else:
				return DateHandler.NOT_VALID
		else:
			return DateHandler.NOT_VALID#Not even possible :)

class FootLog(DateHandler):
	BASE_ORD = ord('A')#For Decoding.

	def __init__(self,Date_str):
		cur_Date = self.getDateTime(Date_str)#DateTime OBJECT!
		self.Date = str(cur_Date.date())#No 'hms', just date!
		self.Weekday = cur_Date.weekday()
		self.State = self.getScheduleIdx(cur_Date)
		if(self.State == self.NOT_VALID):
			print("Warning! Scheudle_State NOT_VALID for : ",cur_Date,"derived from",Date_str)
		self.Data = []

	def __str__(self):
		return ('Date: '+ self.Date +', Weekday: '+DateHandler.Weekday2Str(self.Weekday)+', State: '+DateHandler.State2Str(self.State))
	
	def append_data(self,raw_list):
		timestamp = self.getDateTime(raw_list[0])
		Time_obj = timestamp.time()
		Time_str = str(Time_obj)#Would be like '12:05:07'
		apdList = [Time_str,int(raw_list[1])]#Idx is thingspeak kinda thing.
		decodeList = []
		for eachField in raw_list[2:7]:
			smallList = []
			for ch in eachField:
				smallList.append(ord(ch)-FootLog.BASE_ORD)
			decodeList += smallList
		apdList.append(decodeList)
		apdList.append(int(raw_list[7]))#8th is 'field 6'.

		self.Data.append(apdList)


def get_Logs(target_dir):
	file = open(target_dir,'r')
	data = csv.reader(file)
	retList = []
	idx= 0
	lastIdx = 0
	FootObj = False
	for row in data:#Each List!
		if(row[0][-3:] != 'UTC'):#Kinda my marker.
			print("Invalid list:",row)
			continue
		elif(FootObj == False):#First Valid.
			FootObj = FootLog(row[0])
			curIdx = DateHandler.getScheduleIdx(DateHandler.getDateTime(row[0]))
			lastIdx = curIdx
			#print("First Schedule!",curIdx)
		curIdx = DateHandler.getScheduleIdx(DateHandler.getDateTime(row[0]))
		#print(curIdx)
		if(curIdx != lastIdx):
			print("Append: ",FootObj)
			retList.append(FootObj)
			FootObj = FootLog(row[0])
		lastIdx = curIdx
		FootObj.append_data(row)
		idx += 1
	#Finally.
	print("Append: ",FootObj)
	retList.append(FootObj)
	file.close()
	return retList

if(__name__ == '__main__'):
	file_dir = input("What file(.csv)?")
	get_Logs(file_dir)

	input("Waiting...")