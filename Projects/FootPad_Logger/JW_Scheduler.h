typedef enum{
	IDLE = 0,
	FIVE_MINUTE_UNTIL_START,
	LOGGING,
} SchedulerStatus_e;

typedef enum{
	_7AM_BREAKFAST = 0,
	_12PM_LUNCH,
	_6PM_DINNER,
	_8PM_SNACK,
	_5PM_DINNER,
} ScheduleName_e;

typedef struct{
	ScheduleName_e Name;
	uint8_t Hour;//0~23
	uint8_t Minute;
	uint8_t Second;
	uint16_t duration;//0~65535[s]
} ScheduleTime_t;

typedef enum{
	SUNDAY = 0,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THRURSDAY,
	FRIDAY,
	SATURDAY,
} ScheduleDoW_e;

typedef struct{
  ScheduleTime_t * schedules[4];
  uint8_t length;
} ScheduleTemplate_t;

ScheduleTime_t __7am_breakfast =  {_7AM_BREAKFAST,6,55,00,4500};//6:55 ~ 7:10 (75 minute = 4500s.)
ScheduleTime_t __12pm_lunch =  {_12PM_LUNCH,11,55,00,3900};//11:55 ~ 13:00 (65 minute = 3900s.)
ScheduleTime_t __6pm_dinner =  {_6PM_DINNER,17,55,00,3900};//17:55 ~ 19:00 (65 minute = 3900s.)
ScheduleTime_t __8pm_snack =  {_8PM_SNACK,20,35,00,2100};// 20:35 ~ 21:10 (35 minute = 2100s.) 
ScheduleTime_t __5pm_dinner = {_5PM_DINNER,16,55,00,3900};//16:55 ~ 18:00 (65 minute = 3900s.)

ScheduleTime_t * _7am_breakfast = &__7am_breakfast;
ScheduleTime_t * _12pm_lunch = &__12pm_lunch;
ScheduleTime_t * _6pm_dinner = &__6pm_dinner;
ScheduleTime_t * _8pm_snack = &__8pm_snack;
ScheduleTime_t * _5pm_dinner = &__5pm_dinner;

const ScheduleTemplate_t TASK_LIST[] = {//TASK for 'each Day'[Monday ~ Sunday]
  
  {//SUNDAY.
    {_7am_breakfast,
    _12pm_lunch,
    _5pm_dinner},
    3
  },

  {//MONDAY.
    {_7am_breakfast,
    _12pm_lunch,
    _6pm_dinner,
    _8pm_snack},
    4
  },

  
  {//TUESDAY.
    {_7am_breakfast,
    _12pm_lunch,
    _6pm_dinner,
    _8pm_snack},
    4
  },
  {//WEDNESDAY.
    {_7am_breakfast,
    _12pm_lunch,
    _6pm_dinner,
    _8pm_snack},
    4
  },
  
  {//THURSDAY.
    {_7am_breakfast,
    _12pm_lunch,
    _6pm_dinner,
    _8pm_snack},
    4
  },
  
  {//FRIDAY.
    {_7am_breakfast,
    _12pm_lunch,
    _6pm_dinner},
    4
  },
  
  {//STAURDAY.
    {_7am_breakfast,
    _12pm_lunch,
    _5pm_dinner},
    3
  }

};

int32_t deltaT_DateTime_and_ScheduleTime(DateTime& datetime, ScheduleTime_t& scheduletime)
{
  int32_t temp = (int32_t)( (int16_t)(scheduletime.Hour - (int16_t)datetime.hour()) * 60 + (int16_t)scheduletime.Minute - (int16_t)datetime.minute() );
  int32_t later = (int32_t)((int8_t)scheduletime.Second - (int8_t)datetime.second());
  return( temp * 60 + later);
}

SchedulerStatus_e getSchedulerStatus(DateTime& nowtime,uint8_t DoW)
{
  uint32_t SmallestPositiveDeltaT = 86400;//One Day = 86400s.
  bool isSmallestPositiveDeltaT_found = false;
  ScheduleTemplate_t TodayTemplate = TASK_LIST[DoW];
  int index = 0;
  while(index < TodayTemplate.length)
  {
    int32_t deltaT_to_start = deltaT_DateTime_and_ScheduleTime(nowtime,*TodayTemplate.schedules[index]);
    int32_t deltaT_to_end = deltaT_to_start + TodayTemplate.schedules[index]->duration;
    
    if(deltaT_to_start <= 0 && deltaT_to_end >= 0)
      return LOGGING;
    
    if(deltaT_to_start > 0 && deltaT_to_start <= 300)
      return FIVE_MINUTE_UNTIL_START;

    index++;
  }
  return IDLE;
}
