#include "system.h"
#include "protocol.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

long map(long x, long in_min, long in_max, long out_min, long out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define RED_ON    PD_ODR &= ~(1<<4)
#define RED_OFF   PD_ODR |=  (1<<4)

#define GREEN_ON  PD_ODR &= ~(1<<2)
#define GREEN_OFF PD_ODR |=  (1<<2)

#define BLUE_ON   PD_ODR &= ~(1<<3)
#define BLUE_OFF  PD_ODR |=  (1<<3)

uint8_t curMode;
volatile uint8_t send = 0;
int16_t r,g,b,a,w,ra,ga,ba;
int16_t col[8];

void sendData(uint8_t mode, uint8_t len, uint8_t* data) {
    while (!(UART1_SR & UART_SR_TXE)) nop();
    uint8_t st = 0xC0 | (len << 3) | mode;
    UART1_DR = st;
    uint8_t crc = 0xFF ^ st;
    for (uint8_t i = 0; i < (1 << len); i++) {
        while (!(UART1_SR & UART_SR_TXE)) nop();
        UART1_DR = data[i];
        crc ^= data[i];
    }
    while (!(UART1_SR & UART_SR_TXE)) nop();
    UART1_DR = crc;
}

void main() {
    xdev_out(uart_char);
    systemInit();
    uartInit(1000000, 16000000);

    // for (uint16_t i = 0; i < sizeof(protocol); i++)
    //     uart_char(protocol[i]);
    
    // IWDG_KR = IWDG_KR_KEY_ENABLE;
    // IWDG_KR = IWDG_KR_KEY_ACCESS;
    // IWDG_PR = 0x5;
    // IWDG_RLR = 0xFF;
    // IWDG_REFRESH;

    // while (1)
    //     if (UART1_SR & UART_SR_RXNE)
    //         if (UART1_DR == 0x04) break;

    // UART1_SR &= ~UART_SR_RXNE;
    // uartInit(57600, 16000000);
    // UART1_CR2 |= UART_CR2_RIEN;

    xprintf("Sys start\n");

    while (1) {
        int8_t i;
        //while (!send) nop();
        //send=0;

        for(i=7; i>=0; i--){
            PD_ODR = (PD_ODR & 0b11100011) | (((i^(i>>1))^7)<<2);
            delay_us(500);
            col[i]=adc(2);
        }

        // RED_ON;
        // GREEN_ON;
        // BLUE_ON;
        // delay_us(1000);
        // w=adc(2);
        // GREEN_OFF;
        // BLUE_OFF;

        // RED_ON;
        // delay_us(1000);
        // r=adc(2);
        // RED_OFF;

        // GREEN_ON;
        // delay_us(1000);
        // g=adc(2);
        // GREEN_OFF;

        // BLUE_ON;
        // delay_us(1000);
        // b=adc(2);
        // BLUE_OFF;

        // delay_us(1000);
        // a=adc(2);
        
        // ra=r-a;
        // ga=g-a;
        // ba=b-a;
        
        uint8_t tmp=0;
        // if(ra>(ga+20) && r>(ba+20)){
        //     tmp=5;
        //     if(r<400)tmp=7;
        // }
        // if(ga>(r+20) && ga>(ba+20))tmp=3;
        // if(ba>(r+20) && ba>(ga+20))tmp=2;
        // if(ra>(b+20) && ga>(ba+20))tmp=4;
        // if(ra<100 && ga<100 && ba<100)tmp=1;
        // if(ra>600 && ga>600 && ba>600)tmp=6;
        // else 
        for(i=0; i<8; i++)xprintf("%4d ", col[i]);
        xprintf("\n");
        //xprintf("%4d %4d %4d %4d %4d %4d\n", r, g, b, r-a, g-a, b-a);
        //xprintf("%d %4d %4d %4d\n", tmp, ra, ga, ba);
        delay_ms(100);

        // switch(curMode){
        //     case 0:
        //         tmp=map(w-a, 0, 1023, 0, 100);
        //         sendData(0, 0, tmp);
        //         break;
        //     case 1:
        //         tmp=map(a, 0, 1023, 0, 100);
        //         sendData(1, 0, tmp);
        //         break;
        //     case 2:
        //         sendData(2, 0, tmp);
        //         break;
        // }

        //sendData(0, 2, buf);
    }
}

void uart_rx(void) __interrupt(18) {
    static uint8_t stat = 0;
    static uint8_t crc = 0;
    static uint8_t m = 0;
    if ((UART1_SR & UART_SR_RXNE)) {
        IWDG_REFRESH;
        uint8_t d = UART1_DR;
        UART1_SR &= ~UART_SR_RXNE;
        switch (stat) {
            case 0:
                if (d == 0x02) send = 1;
                if (d == 0x43) crc = 0xBC, stat = 1;
                break;
            case 1:
                m = d;
                crc ^= d;
                stat = 2;
                break;
            case 2:
                if (crc == d) curMode = m;
                stat = 0;
                break;
        }
    }
}

extern volatile uint16_t delC;

void tim4(void) __interrupt(23) {
    if (delC) delC--;
    TIM4_SR &= ~TIM_SR1_UIF;
}

// 000
// 001
// 011
// 010
// 110
// 111
// 101
// 100