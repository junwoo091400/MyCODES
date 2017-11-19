#pragma once
enum DAY {
	MONDAY = 1,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY
};
enum PERIOD {//enumeration for the 'period'
	PERIOD_1 = 1,
	PERIOD_2,
	PERIOD_3,
	PERIOD_4,
	PERIOD_5,
	PERIOD_6,
	PERIOD_7,
	PERIOD_8,
	PERIOD_9,
};
enum DURATION {//how long the class is.
	ONE_HOUR = 1,
	TWO_HOUR,
	THREE_HOUR
};
typedef struct {
	char *ClassName;
	char *ClassTeacher;
	char *Location;
	DAY Start_Day;//what day the class is in(1~5)
	PERIOD Start_Period;//when the class starts(1~9)
	DURATION duration;//how long the class is.
}One_Class;//One Class,containing 3 variables

typedef struct {
	/*
	One_Class period_1;//1st period (8:30 ~ 9:20)
	One_Class period_2;//2nd period (9:30 ~ 10:20)
	One_Class period_3;//3rd period (10:30 ~ 11:20)
	One_Class period_4;//4th period (11:30 ~ 12: 20)
	One_Class period_5;//5th period (13:20 ~ 14:10)
	One_Class period_6;//6th period (14:20 ~ 15:10)
	One_Class period_7;//7th period (15:20 ~ 16:10)
	One_Class period_8;//8th period (16:20 ~ 17:10)
	One_Class period_9;//9th period (17:20 ~ 18:10) ??
	*/
	One_Class PeriodofDay[9];
}One_Day;//One day,with 9 period each.

typedef struct {
	char* Name;
	char ClassNum;
	char Number;
	One_Day DayofWeek[5];//0~4.
}Schedule;//1 person's Total Schedule.

//0) Special Pointers!
const char* GONG_GANG_pointer = "공강";
const char* SEONTAEK_1_pointer = "선택1";
const char* SEONTAEK_2_pointer = "선택2";
const char* SEONTAEK_3_pointer = "선택3";

//1) 2학년 선택과목
One_Class SEONTAEK_CLASSES[]{
	{"일반물리학실험Ⅰ-가","최열기","물리실험2실",FRIDAY,PERIOD_6,TWO_HOUR},
	{ "일반물리학실험Ⅰ-나","최열기","물리실험2실",TUESDAY,PERIOD_1,TWO_HOUR },
	{ "일반물리학실험Ⅰ-다","박한얼","물리실험2실",WEDNESDAY,PERIOD_3,TWO_HOUR },

	{ "일반화학실험Ⅰ-가","정은식","화학실험2실",MONDAY,PERIOD_3,TWO_HOUR },
	{ "일반화학실험Ⅰ-나","정은식","화학실험2실",TUESDAY,PERIOD_1,TWO_HOUR },

	{ "자료구조-가","방주희","멀티1실",FRIDAY,PERIOD_6,TWO_HOUR },
	{ "자료구조-가","방주희","멀티1실",MONDAY,PERIOD_4,ONE_HOUR },
	{ "자료구조-나","방주희","멀티1실",TUESDAY,PERIOD_1,TWO_HOUR },
	{ "자료구조-나","방주희","멀티1실",THURSDAY,PERIOD_3,ONE_HOUR },

	{ "창의적문제해결기법-나","홍갑룡,정창훈","멀티2실",THURSDAY,PERIOD_3,TWO_HOUR },
	{ "창의적문제해결기법-다","홍갑룡,정창훈","멀티2실",FRIDAY,PERIOD_1,TWO_HOUR },

	{ "일반물리학Ⅰ-가","권문호","물리2실",MONDAY,PERIOD_3,TWO_HOUR },
	{ "일반물리학I-가","김경융","물리2실",FRIDAY,PERIOD_6,TWO_HOUR },
	{ "일반물리학I-나","권문호","물리2실",TUESDAY,PERIOD_1,TWO_HOUR },
	{ "일반물리학I-나","김경융","물리2실",THURSDAY,PERIOD_3,TWO_HOUR },
	{ "일반물리학I-다","권문호","물리2실",WEDNESDAY,PERIOD_3,TWO_HOUR },
	{ "일반물리학I-다","김경융","물리2실",FRIDAY,PERIOD_1,TWO_HOUR },

	{ "일반생명과학I-나","김준현","생물1실",TUESDAY,PERIOD_1,TWO_HOUR },
	{ "일반생명과학I-나","김윤희","생물1실",THURSDAY,PERIOD_3,ONE_HOUR },
	{ "일반생명과학I-다","김준현","생물1실",WEDNESDAY,PERIOD_3,TWO_HOUR },
	{ "일반생명과학I-다","김윤희","생물1실",FRIDAY,PERIOD_1,ONE_HOUR },

	{ "일반생명과학실험I-가","이문열","생물실험1실",MONDAY,PERIOD_3,TWO_HOUR },
	{ "일반생명과학실험I-나","이문열","생물실험1실",TUESDAY,PERIOD_1,TWO_HOUR },
	{ "일반생명과학실험I-다","이문열","생물실험1실",WEDNESDAY,PERIOD_3,TWO_HOUR },
};

//2) 2학년 반별 기본 과목
Schedule BigBoy[6];//all 6 classes.