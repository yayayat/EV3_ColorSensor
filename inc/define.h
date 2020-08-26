#ifndef DEFINE
#define DEFINE

#define bS(a) (1<<(a))

#define recR(a,b) (a)=(b)                //record registr - записать в регистр
#define setR(a,b) (a)|=(b)               //set register - установить в 1 значения в регистре
#define resR(a,b) (a)&=~(b)              //reset register - сбросить в 0 значения в регистре
#define togR(a,b) (a)^=(b)               //toggle register - измениеть значения в регистре

#define setRB(a,b) (a)|=(1<<b)           //set register byte - установить в 1 заданный байт в регистре
#define resRB(a,b) (a)&=~(1<<b)          //reset register byte - сбросить в 0 заданный байт в регистре
#define togRB(a,b) (a)^=(1<<b)           //toggle register byte - измениеть заданный байт в регистре

#define getRB(a,b) ((a)>>(b)&1)          //get register bit - получить значение заданного бита

#define waitS(a,b) while(getRB(a, b))    //wait set - ожидание установки байта
#define waitR(a,b) while(!getRB(a, b))   //wait reset - ожидание установки байта

#endif
