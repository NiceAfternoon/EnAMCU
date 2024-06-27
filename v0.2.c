#include <STC15F2K60S2.H>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int

uchar State = 4;
uchar TimePage = 0;
uchar SettingMode = 0;
uchar SettingLocation = 0;
uchar Page = 0;
uint temperature = 0;
uchar turetemp = 0;

void Time_DisPlay();
void SetTime_DisPlay();
void Start_DisPlay(uchar Start);
void Info_DisPlay();
void Clock();
void Key_Detect();
void Delay(uchar dtime);
void Buzzer();
void DS18B20_Init();
void Temp_DisPlay();
uint Temp_DS18B20();
void Temp_FeedBack();

//==============================时钟================================
//------------------------------变量--------------------------------
//常规变量
static long count = 0;
static long presscount = 0;
static long alarmcount = 0;
static long tempcount = 0;
static long sound = 0;
uchar pro = 0;
uchar timer = 0;
uchar alarm = 0;
uchar enter = 0;
uchar sustain = 0;
uchar none = 0;
uchar blink = 0;
uchar sec = 14;
uchar min = 45;
uchar hour = 11;
uchar setsec = 0;
uchar setmin = 0;
uchar sethour = 0;
uchar alarmsec = 0;
uchar alarmmin = 0;
uchar alarmhour = 0;
int timersec = 0;
int timermin = 5;
int timerhour = 0;
long timercount = 0;

//逻辑变量
uchar TimerSet = 0;

//----------------------------时间赋值------------------------------
void Set_Time(uchar Type) {
	switch(Type) {
		
		//将设置的时间赋给显示时钟
		case 0:
			sec = setsec;
			min = setmin;
			hour = sethour;
		break;
		
		
		//将设置的时间赋给闹钟
		case 1:
			alarmsec = setsec;
			alarmmin = setmin;
			alarmhour = sethour;
		break;
		
		
		//将设置的时间赋给定时器
		case 2:
			timersec = setsec;
			timermin = setmin;
			timerhour = sethour;
		break;
		
		
		//将当前时间赋给设置时间（用于设置时间时的显示
		case 3:
			setsec = sec;
			setmin = min;
			sethour = hour;
		break;
		
		
		//设置时间
		case 4:
			switch(SettingLocation) {
				case 0: sethour += 10; break;
				case 1: sethour++; 		 break;
				case 2: setmin += 10;  break;
				case 3: setmin++; 		 break;
				case 4: setmin += 10;  break;
				case 5: setmin++;      break;
				case 6: setsec += 10;  break;
				case 7: setsec++;      break;
			}
		break;
			
			
		//设置时间回零
		case 5:
			sethour = 0;
			setmin = 0;
			setsec = 0;
		break;
		
		
		//把定时器时间赋给设置值，用于显示
		case 6:
			sethour = timerhour;
			setmin = timermin;
			setsec = timersec;
		break;
		
		
	}
}


//----------------------------时钟中断------------------------------
void Clock() interrupt 1 {
//	TL0 = 0x20;				//设置定时初始值
//	TH0 = 0xD1;				//设置定时初始值

	count++;
	presscount++;
	tempcount++;
	//正常显示时的中断
	if(count >= 1000) {
		sec++;
		count = 0;
	}
	
	if(sec == 60) {
		min++;
		sec = 0;
	}
	
	if(min == 60) {
		hour++;
		min = 0;
	}
	
	if(hour == 24) {
		hour = 0;
	}
	
	
	//定时器显示和终止的中断
	if(TimerSet == 1) {
		timercount++;
		if(timercount >= 1000){
			timersec--;
			timercount = 0;		
		}
	
	
	if(timersec < 0) {
		timermin--;
		timersec = 59;
	}
	
	if(timermin < 0) {
		timerhour--;
		timermin = 59;
	}
	
	if(timerhour < 0) {
		TimerSet = 0;
		timerhour = 0;
		timermin = 0;
		timersec = 0;
		State = 2;
	}
}	
	
	//设置时间时的中断
	 if(setsec > 59) {
		 setsec = 0;
		 setmin++;
	 }
	 
	 if(setmin > 59) {
		 setmin = 0;
		 sethour++;
	 }
	 
	 if(Page == 2) {
		 if(sethour > 99) {
		   sethour = 0;
		 }
	 } else {
		 if(sethour > 23) {
			 sethour = 0;
		 }	 
	 }

	 
	 //闹钟中断
	 if(sec == alarmsec && min == alarmmin && hour == alarmhour) {
		 State = 1;
	 }
	 
	 
	 //载入动画的中断
	 if(sustain == 1) {
		 if(count >= 900) {
			 enter++;
			 if(enter >= 90) {
				 sustain = 0;
				 enter = 0;
			 }
		 }
	 }

	 
	 //闪烁中断
	 if(count % 250 == 0) {
		 blink++;
		 if(blink > 4) {
		   blink = 0;
		 }
	 }
	 
	 //页面的中断
	 if(Page > 4) {
		 Page = 0;
	 }
	 
	 //按键防抖
	 if(presscount == 251) {
		 presscount = 0;
	 }
	 
	 //按键中断
		 Key_Detect();
	 
	 //闹钟响起的显示中断
	 if(State == 1) {
		 if(alarm >= 200) {
			 State = 4;
			 alarm = 0;
		 }
	 }

	 //定时器终止的中断
	 	if(State == 2) {
		 if(timer >= 200) {
			 State = 4;
			 timer = 0;
		 }
	 }
	 
	 if(Page == 3 && none >= 1) {
	   Temp_FeedBack();	 	 
	 }

	 //防误触中断
	 if(pro > 3) {
		 pro = 0;
		 State = 4;
	 }

}
//---------------------------时钟初始化-----------------------------
void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}


//--------------时钟的设置模式or正常显示模式的逻辑判断--------------
void TimeSetOrNormal_Logic() {
	if(SettingMode == 0) {
		Time_DisPlay();
	} else if(SettingMode == 1) {
		SetTime_DisPlay();
	}	
}
//--------------时钟的设置模式or正常显示模式的逻辑判断--------------

//==============================时钟================================



//===============================显示===============================

//常规变量


//逻辑变量



//------------------------数码管显示消影延时-------------------------

void Delay1ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 12;
	j = 169;
	do
	{
		while (--j);
	} while (--i);
}

void Delay(uchar dtime) {
	int i;
	for(i = 0; i <= dtime; i++) {
		Delay1ms();
	}
}


//----------------------------数码管显示-----------------------------
//数码管显示
uchar Digitron[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};			//数码管显示1-9（不带点）
uchar DigitronDp[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};		//数码管显示1-9（带点）

//数码管位选+段选 
sbit Smg1 = P0^0;
sbit Smg2 = P0^1;
sbit Smg3 = P0^2;
sbit Smg4 = P0^3;
#define duan P2

//数码管显示函数
void Smg(uchar wei, uchar number) { 
	switch(wei) {
		case 4: Smg1 = 1; Smg2 = 1; Smg3 = 1; Smg4 = 0; break;
		case 3: Smg1 = 1; Smg2 = 1; Smg3 = 0; Smg4 = 1; break;
		case 2: Smg1 = 1; Smg2 = 0; Smg3 = 1; Smg4 = 1; break;
		case 1: Smg1 = 0; Smg2 = 1; Smg3 = 1; Smg4 = 1; break;
	}
	
	duan = number;
	//消除残影
	Delay(1);
	duan = 0xFF;
}


//-----------------------------页面显示------------------------------
void Page_DisPlay() {
	switch(Page) {	
		//时钟页面
		case 0:
			if(none == 0) {
				Start_DisPlay(2);
				none++;
			}
			TimeSetOrNormal_Logic();
		break;
			
			
		//闹钟页面					
		case 1:
			if(none == 0) {
				Start_DisPlay(3);		
				none++;
			}
			TimeSetOrNormal_Logic();
		break;
				
				
		//定时器页面	
		case 2:
			if(none == 0) {
				Start_DisPlay(4);	
				none++;
			}
			TimeSetOrNormal_Logic();
		break;
			
		
		//温度页面
		case 3:
			if(none == 0) {
				Start_DisPlay(5);	
				none++;
			}
			Temp_DisPlay();
			break;
		

		//关于页面
		case 4:
			if(none == 0) {
				Start_DisPlay(0);		
				none++;
			}
			Info_DisPlay();
		break;
	}
}


//-----------------------------时间显示-----------------------------
void Time_DisPlay() {
	switch(Page) {
		
		//正常时钟的显示
		case 0:
			if(TimePage == 0) {													//第一页
				Smg(1, Digitron[hour / 10]);							//时的十位
				
				if(blink < 2) {														//时的个位，冒号闪烁显示
					Smg(2, DigitronDp[hour % 10]);
				} else if(blink >= 2) {
					Smg(2, Digitron[hour % 10]);
				}
				
				Smg(3, Digitron[min / 10]);								//分的十位
				
				Smg(4, Digitron[min % 10]);								//分的个位
				
			} else if(TimePage == 1) {									//第二页
				Smg(1, Digitron[min / 10]);								//分的十位
				
				if(blink < 2) {														//分的个位，冒号闪烁显示
					Smg(2, DigitronDp[min % 10]);
				} else if(blink >= 2) {
					Smg(2, Digitron[min % 10]);
				}
				
				Smg(3, Digitron[sec / 10]);								//秒的十位
				
				Smg(4, Digitron[sec % 10]);								//秒的个位
			}
		break;


		//所设定闹钟的显示
		case 1:
			Smg(1, Digitron[alarmhour / 10]);						//时的十位
			Smg(2, DigitronDp[alarmhour % 10]);					//时的个位，显示冒号
		  Smg(3, Digitron[alarmmin / 10]);						//分的十位
			Smg(4, Digitron[alarmmin % 10]);						//分的个位
		break;
		
		
		//定时器的显示
		case 2:
			if(TimePage == 0) {													//第一页
				Smg(1, Digitron[timerhour / 10]);							//时的十位
				
				if(blink < 2) {														//时的个位，冒号闪烁显示
					Smg(2, DigitronDp[timerhour % 10]);
				} else if(blink >= 2) {
					Smg(2, Digitron[timerhour % 10]);
				}
				
				Smg(3, Digitron[timermin / 10]);								//分的十位
				
				Smg(4, Digitron[timermin % 10]);								//分的个位
				
			} else if(TimePage == 1) {									//第二页
				Smg(1, Digitron[timermin / 10]);								//分的十位
				
				if(blink < 2) {														//分的个位，冒号闪烁显示
					Smg(2, DigitronDp[timermin % 10]);
				} else if(blink >= 2) {
					Smg(2, Digitron[timermin % 10]);
				}
				
				Smg(3, Digitron[timersec / 10]);								//秒的十位
				
				Smg(4, Digitron[timersec % 10]);								//秒的个位
			}
		break;			
	}
}


//---------------------------时间设置显示---------------------------
void SetTime_DisPlay() {
	if(TimePage == 0) {										//第一页
		//设置时的十位
		if(SettingLocation == 0) {
			if(blink < 2) {
				Smg(1, Digitron[sethour / 10]);
			} else if(blink >= 2) {
				Smg(1, 0xFF);
			}
		}else {
			Smg(1, Digitron[sethour / 10]);
		}
		
		//设置时的个位
		if(SettingLocation == 1) {
			if(blink < 2) {
				Smg(2, DigitronDp[sethour % 10]);
			} else if(blink >= 2) {
				Smg(2, 0x7F);
			}
		}else {
			Smg(2, DigitronDp[sethour % 10]);
		}
		
		//设置分的十位
		if(SettingLocation == 2) {
			if(blink < 2) {
				Smg(3, Digitron[setmin / 10]);
			} else if(blink >= 2) {
				Smg(3, 0xFF);
			}
		} else {
			Smg(3, Digitron[setmin / 10]);
		}
		
		//设置分的个位
		if(SettingLocation == 3) {
			if(blink < 2) {
				Smg(4, Digitron[setmin % 10]);
			} else if(blink >= 2) {
				Smg(4, 0xFF);
			}
		} else {
			Smg(4, Digitron[setmin % 10]);
		}
	} else if (TimePage == 1) {						//第二页
		//设置分的十位
		if(SettingLocation == 4) {
			if(blink < 2) {
				Smg(1, Digitron[setmin / 10]);
			}else if(blink >= 2) {
				Smg(1, 0xFF);
			}
		} else {
			Smg(1, Digitron[setmin / 10]);
		}
		
		//设置分的个位
		if(SettingLocation == 5) {
			if(blink < 2) {
				Smg(2, DigitronDp[setmin % 10]);
			} else if(blink >= 2) {
				Smg(2, 0x7F);
			}
		} else {
			Smg(2, DigitronDp[setmin % 10]);
		}
		
		//设置秒的十位
		if(SettingLocation == 6) {
			if(blink < 2) {
				Smg(3, Digitron[setsec / 10]);
			} else if(blink >= 2) {
				Smg(3, 0xFF);
			}
		} else {
			Smg(3, Digitron[setsec / 10]);
		}
		
		//设置秒的个位
		if(SettingLocation == 7) {
			if(blink < 2) {
				Smg(4, Digitron[setsec % 10]);
			} else if(blink >= 2) {
				Smg(4, 0xFF);
			}
		} else {
			Smg(4, Digitron[setsec % 10]);
		}
	}
}


//-----------------------------载入显示-----------------------------
void Start_DisPlay(uchar Start) {

	switch(Start) {
		//关于显示
		case 0:
			do {
				sustain = 1;
				Smg(1, 0xFA);
				Smg(2, 0xAB);
				Smg(3, 0x8E);
				Smg(4, 0xA3);		
			} while(sustain == 1);
			enter = 0;
			sustain = 0;
		break;
			
			
		//开机显示
		case 1:
			do {
				sustain = 1;
				Smg(1, 0x86);
				Smg(2, 0xAB);
				Smg(3, 0x88);				
			} while(sustain == 1);
			enter = 0;
			sustain = 0;
		break;
		
		
		//载入时间显示
		case 2:
			do {
				sustain = 1;
				Smg(1, 0xC6);
				Smg(2, 0xC7);
				Smg(3, 0xA3);
				Smg(4, 0xC6);
			} while(sustain == 1);
			enter = 0;
			sustain = 0;
		break;
			
		
		//载入闹钟显示
		case 3:
			do {
				sustain = 1;
				Smg(1, 0x88);
				Smg(2, 0xC7);
				Smg(3, 0X88);
				Smg(4, 0XAF);
			}while(sustain == 1);
			enter = 0;
			sustain = 0;
		break;
			
		
		//载入计时器显示
		case 4:
			do {
				sustain = 1;
				Smg(1, 0xCE);
				Smg(2, 0xFA);
				Smg(3, 0xC8);
				Smg(4, 0x86);
			} while(sustain == 1);
			enter = 0;
			sustain = 0;
		break;
			
			
		//载入温度显示
		case 5:
			do {
				sustain = 1;
				Smg(1, 0xCE);
				Smg(2, 0x86);
				Smg(3, 0xC8);
				Smg(4, 0x8C);
			} while(sustain == 1);
			enter = 0;
			sustain = 0;
		break;
			
	
	}
}



//-----------------------------关于显示-----------------------------
void Info_DisPlay() {
//	switch(pro) {
//		case 0:
//			Smg(1, 0xAB);
//			Smg(2, 0xA3);
//			Smg(3, 0xAB);
//			Smg(4, 0x86);
//			State = 4;
//		break;
//		
//		case 1:
//			Smg(1, 0xAB);
//			Smg(2, 0xA3);
//			Smg(3, 0xAB);
//			Smg(4, 0x06);
//		break;
//		
//		case 2:
//			Smg(1, 0xAB);
//			Smg(2, 0xA3);
//			Smg(3, 0x2B);
//			Smg(4, 0x06);
//		break;
//		
//		case 3:
//			Smg(1, 0x2B);
//			Smg(2, 0xA3);
//			Smg(3, 0x2B);
//			Smg(4, 0x06);
//		  State = 3;
//		break;
//	}

	switch(pro) {
		case 0:
			Smg(2, 0x86);
			Smg(3, 0xAB);
			Smg(4, 0x88);
			State = 4;
		break;
		
		case 1:
			Smg(2, 0x86);
			Smg(3, 0xAB);
			Smg(4, 0x08);
		break;
		
		case 2:
			Smg(2, 0x86);
			Smg(3, 0x2B);
			Smg(4, 0x08);
		break;
		
		case 3:
			Smg(1, 0x7F);
			Smg(2, 0x86);
			Smg(3, 0x2B);
			Smg(4, 0x08);
			State = 3;
		break;
	
	}
}


//-----------------------------温度显示-----------------------------
void Temp_DisPlay() {
	Smg(1, Digitron[turetemp / 10]);
	Smg(2, Digitron[turetemp % 10]);
	Smg(3, 0x9C);
	Smg(4, 0xC6);
}

//---------------------------闹钟响起显示---------------------------
void Alarm_DisPlay() {
	
	if(blink > 2) {
		Smg(1, Digitron[alarmhour / 10]);
		Smg(2, DigitronDp[alarmhour % 10]);
		Smg(3, Digitron[alarmmin / 10]);
		Smg(4, Digitron[alarmmin % 10]);
		alarm++;
	} else if(blink <= 2) {
//		Smg(1, 0x88);
//		Smg(2, 0xC7);
//		Smg(3, 0X88);
//		Smg(4, 0XAF);
		Buzzer();
	}
} 


//--------------------------定时器终止显示--------------------------
void Timer_DisPlay() {
	if(blink > 2) {
		Buzzer();
	} else if(blink <= 2) {
			Smg(1, 0xCE);
			Smg(2, 0xFA);
			Smg(3, 0xC8);
			Smg(4, 0x86);
			timer++;
	}
} 


//===============================显示===============================


//===============================按键===============================
//变量
//逻辑变量
//位
sbit SettingKey = P1^4;
sbit PlusKey = P1^5;
sbit SwitchKey = P1^6;
sbit AlarmKey = P5^5;
//函数
//翻页按键
void Plus_Page() {
	if(PlusKey == 0) {
		if(PlusKey == 0 && presscount == 150) {
			if(none == 1) {
				none = 0;
			} 
			Page++;
		}
	}
}


void Key_Detect() {
	switch(Page) {
		//时钟页面的按键功能
		case 0:
			//按下按键1进入/退出设置时间
			if(SettingKey == 0 && State != 3) {
				if(SettingKey == 0 && presscount == 150 && State != 3) {
					if(SettingMode == 0) {
						Set_Time(3);
						SettingMode = 1;
						if(TimePage == 0) {
							SettingLocation = 0;
						} else if(TimePage == 1) {
							SettingLocation = 4;
						}
					} else if(SettingMode == 1) {
						Set_Time(0);
						SettingMode = 0;
					}
				}	
			}
			
			//按键2
				//当进入设置时间时
				if(SettingMode == 1) {
					if(PlusKey == 0) {
						if(PlusKey == 0 && presscount == 150) {
							Set_Time(4);
						}
					} //当不在设置时间时
				} else if(SettingMode == 0) {
						Plus_Page();
				}
			
			//按键3
				//当进入时间设置时
				if(SettingMode == 1) {
					if(SwitchKey == 0) {
						if(SwitchKey == 0 && presscount == 150) {
							SettingLocation++;
							if(SettingLocation >= 4 && TimePage == 0) {
							  TimePage = 1;
							} else if(SettingLocation > 7) {
								TimePage = 0;
								SettingLocation = 0;
							}
						}
					}//当不在时间设置时
				} else if(SettingMode == 0) {
						if(SwitchKey == 0) {
							if(SwitchKey == 0 && presscount == 150) {
								if(TimePage == 1) {
									TimePage = 0;
								} else if(TimePage == 0) {
									TimePage = 1;
								}
							}
						}
				}
		break;
				
			
		//闹钟页面的按键
		case 1:
			//按下按键1进入/退出设置时间
			if(SettingKey == 0 && State != 3) {
				if(SettingKey == 0 && presscount == 150 && State != 3) {
					if(SettingMode == 0) {
						SettingMode = 1;
						Set_Time(5);
						TimePage = 0;
						SettingLocation = 0;
					} else if(SettingMode == 1) {
						Set_Time(1);
						SettingMode = 0;
					}
				}	
			}

			//按键2
				//当进入设置时间时
				if(SettingMode == 1) {
					if(PlusKey == 0) {
						if(PlusKey == 0 && presscount == 150) {
							Set_Time(4);
						}
					} //当不在设置时间时
				} else if(SettingMode == 0) {
						Plus_Page();
				}
				
			//按键3
				//当进入设置时间时
				if(SettingMode == 1) {
					if(SwitchKey == 0) {
						if(SwitchKey == 0 && presscount == 150) {
							SettingLocation++;
							if(SettingLocation > 3) {
								SettingLocation = 0;
							}
						}
					} //当不在设置时间时
				} 
		break;
				
		
		//定时器界面的按键
	  case 2: 
			//按下按键1进入/退出设置时间模式
			if(SettingKey == 0 && State != 3) {
				if(SettingKey == 0 && presscount == 150 && State != 3) {
					if(SettingMode == 0) {
						SettingMode = 1;
						Set_Time(6);
						if(TimePage == 0) {
							SettingLocation = 0;
						} else if(TimePage == 1) {
							SettingLocation = 4;
						}						
					} else if(SettingMode == 1) {
						Set_Time(2);
						TimerSet = 1;
						SettingMode = 0;
					}
				}
			}
			
			
			//按键2
				//当进入时间设置时
				if(SettingMode == 1) {
					if(PlusKey == 0) {
						if(PlusKey == 0 && presscount == 150) {
							Set_Time(4);
					}//当不在时间设置时	
				} 
			}else if(SettingMode == 0) {
					Plus_Page();
				}
			
		
		
		 //按键3
			 //当进入时间设置时
			if(SettingMode == 1) {
				if(SwitchKey == 0) {
					if(SwitchKey == 0 && presscount == 150) {
						SettingLocation++;
						if(SettingLocation >= 4 && TimePage == 0) {
							TimePage = 1;
						} else if(SettingLocation > 7) {
							TimePage = 0;
							SettingLocation = 0;
						}
					}
				} //当不在时间设置时
			} else if(SettingMode == 0) {
				if(SwitchKey == 0) {
					if(SwitchKey == 0 && presscount == 150) {
						if(TimePage == 0) {
							TimePage = 1;
						} else if(TimePage == 1) {
							TimePage = 0;
						}
					}
				}
			}
	  break;
			
	  
		//温度页面的按键
		case 3:
			//按键2
		  Plus_Page();
		break;
		
		case 4:
			//按键1
			if(SettingKey == 0) {
				if(SettingKey == 0 && presscount == 150) {
					pro++;
				}
			}
			
			//按键2
			Plus_Page();
	}
}

//===============================按键===============================


//==============================蜂鸣器==============================
void Buzzer() {
	sound = 20;
	while(sound--) {
		P55 = !P55;
		for(alarmcount = 0; alarmcount <= 10; alarmcount++) {
		}
	}
	sound = 0;
}
//==============================蜂鸣器==============================


//===============================温度===============================
//位定义
sbit DS18B20 = P3^7;

//短延时，用于拉低/释放总线时的延时
void Delay_us(uchar dtime) {
	while(dtime--) {
		_nop_();
	}
}


//初始化DS18B20
void DS18B20_Init() {
  //由单片机拉低总线并延时600us	
	DS18B20 = 0;
	Delay_us(750);
	
	//由单片机释放总线并延时60us
	DS18B20 = 1;
	Delay_us(15);
	
	//当传感器返回低电平时，表明总线上存在传感器
	while(!DS18B20);
	Delay_us(500);
}


//向DS18B20写入数据
void Wr_DS18B20(uchar dat) {
	uchar i;
	bit flag;
	
	for(i = 1; i <= 8; i++) {
		//从低到高依次向DS18B20中写入1Byte数据
		flag = dat&0x01;
		dat = dat >> 1;
		
		//拉低总线并延时2us
		DS18B20 = 0;
		_nop_();
		_nop_();
		
		//将要写的位放进总线，延时60us让传感器有足够时间读取
		DS18B20 = flag;
		Delay_us(60);
		
		//释放总线
		DS18B20 = 1;
		
	}
}


//从DS18B20中读取数据
uchar Rd_DS18B20() {
	uchar flag, dat, i;
	
	for(i = 1; i <= 8; i++) {
		//拉低总线并延时20us
		DS18B20 = 0;
		_nop_();
		_nop_();
		
		//释放总线并延时20us
		DS18B20 = 1;
		_nop_();
		_nop_();
		
		//读取数据
		flag = DS18B20;
		Delay_us(60);
		
		//写入数据，将读出的数据的最低位置于前面
		dat = (dat >> 1) | (flag << 7);
	}
	
	return dat;
}


//温度检测
uint Temp_DS18B20() {
	
	uchar TempL, TempH;
	uint Temp;
	
	DS18B20_Init();			//初始化DS18B20
	Wr_DS18B20(0xCC);		//CCh为跳过ROM的指令
	Wr_DS18B20(0x44);   //44h为启动温度转换的指令
	DS18B20_Init();     //初始化DS18B20
	Wr_DS18B20(0xCC);   //跳过ROM
	Wr_DS18B20(0xBE);		//BEh为读取温度的指令
	
	//读取两个字节的温度值
	TempL = Rd_DS18B20();
	TempH = Rd_DS18B20();
	
	//组成16位的温度变量
	Temp = (TempH << 8) + TempL;
	
	return Temp;
}


//温度反馈
void Temp_FeedBack() {
	if(tempcount >= 1000) {
		tempcount = 0;
		temperature = Temp_DS18B20();
		turetemp = temperature * 0.0625;
	}
}

//===============================温度===============================

//------------------------------主函数------------------------------
int main() {
	P0M0 = 0;
	P0M1 = 0;
	P1M1 = 0;
	P1M0 = 0;
	P2M1 = 0;
	P2M0 = 0;
	P3M0 = 0;
	P3M1 = 0;
	P5M0 = 0;
	P5M1 = 0;
	P4M1 = 0;
	P4M0 = 0;
	
	
	Timer0_Init();
	ET0 = 1;
	EA = 1;
	Start_DisPlay(1);
	while(1) {
		switch(State) {
//			case 0:									//睡眠状态
//				Sleep_DisPlay();
//			break;
			
			case 1:								  //闹钟响起的状态
				Alarm_DisPlay();
			break;
			
			case 2:									//定时器终止的状态
				Timer_DisPlay();
			break;
			
			case 3:									//防误触模式
				Page_DisPlay();
			break;
			
			case 4:								//正常状态
				Page_DisPlay();	
			break;
		}
		
		
	} 
}
