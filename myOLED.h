#ifndef myOLED_H
#define myOLED_H

#include "my_lib.h"
#include "fonts.h"


#define OLED_CMD  0  //写命令
#define OLED_DATA 1 //写数据

class MyOLED {

private:
    static MyOLED * pinstance_;
    bool bIsReady;
protected:
    MyOLED():bIsReady(false)
    {}
    ~MyOLED() {}

public:
 	MyOLED(MyOLED &other) = delete;
    void operator=(const MyOLED &) = delete;

 	static MyOLED *GetInstance();

	void OLED_Demo(int iDisplay=10);

    void OLED_ColorTurn(u8 i);
    void OLED_ShowStringLine(uint8_t x, uint8_t y, const char *chr, uint8_t sizey);
    void OLED_DisplayTurn(u8 i);
    void OLED_WR_Byte(u8 dat, u8 mode);
    void OLED_Set_Pos(u8 x, u8 y);
    void OLED_Display_On(void);
    void OLED_Display_Off(void);
    void OLED_Clear(void);
    void OLED_ShowChar(u8 x, u8 y, const u8 chr, u8 sizey);
    u32 oled_pow(u8 m, u8 n);
    void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 sizey);
    void OLED_ShowString(u8 x, u8 y, const char *chr, u8 sizey);
    void OLED_ShowChinese(u8 x, u8 y, const u8 no, u8 sizey);
    void OLED_DrawBMP(u8 x, u8 y, u8 sizex, u8 sizey, const u8 BMP[]);
    void OLED_Init(void);

};
#endif