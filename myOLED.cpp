#include "myOLED.h"

MyOLED *MyOLED::pinstance_{nullptr};

MyOLED *MyOLED::GetInstance()
{
  if (pinstance_ == nullptr)
  {
    pinstance_ = new MyOLED();
  }
  return pinstance_;
}

void MyOLED::OLED_Demo(int iDisplay)
{
  uint8_t t = ' ';
  while(iDisplay > 0)
  {
    OLED_DrawBMP(0,0,128,64,BMP1);
    delay(500);
    OLED_Clear();
    OLED_ShowChinese(0,0,0,16);//中
    OLED_ShowChinese(18,0,1,16);//景
    OLED_ShowChinese(36,0,2,16);//园
    OLED_ShowChinese(54,0,3,16);//电
    OLED_ShowChinese(72,0,4,16);//子
    OLED_ShowChinese(90,0,5,16);//科
    OLED_ShowChinese(108,0,6,16);//技
    OLED_ShowString(8,2,"ZHONGJINGYUAN",16);
    OLED_ShowString(20,4,"2014/05/01",16);
    OLED_ShowString(0,6,"ASCII:",16);  
    OLED_ShowString(63,6,"CODE:",16);
    OLED_ShowChar(48,6,t,16);
    t++;
    if(t>'~')t=' ';
    OLED_ShowNum(103,6,t,3,16);
    delay(500);
    OLED_Clear();
    iDisplay--;
  }
}

// 反显函数
void MyOLED::OLED_ColorTurn(u8 i)
{
  if (!i)
    OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示
  else
    OLED_WR_Byte(0xA7, OLED_CMD); // 反色显示
}

// 屏幕旋转180度
void MyOLED::OLED_DisplayTurn(u8 i)
{
  if (i == 0)
  {
    OLED_WR_Byte(0xC8, OLED_CMD); // 正常显示
    OLED_WR_Byte(0xA1, OLED_CMD);
  }
  else
  {
    OLED_WR_Byte(0xC0, OLED_CMD); // 反转显示
    OLED_WR_Byte(0xA0, OLED_CMD);
  }
}

// 发送一个字节
// 向SSD1306写入一个字节。
// mode:数据/命令标志 0,表示命令;1,表示数据;
void MyOLED::OLED_WR_Byte(u8 dat, u8 mode)
{
  Wire.beginTransmission(0x3c);
  if (mode)
  {
    Wire.write(0x40);
  }
  else
  {
    Wire.write(0x00);
  }
  Wire.write(dat);        // sends one byte
  Wire.endTransmission(); // stop transmitting
}

// 坐标设置

void MyOLED::OLED_Set_Pos(u8 x, u8 y)
{
  OLED_WR_Byte(0xb0 + y, OLED_CMD);
  OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
  OLED_WR_Byte((x & 0x0f), OLED_CMD);
}
// 开启OLED显示
void MyOLED::OLED_Display_On(void)
{
  OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
  OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
  OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}
// 关闭OLED显示
void MyOLED::OLED_Display_Off(void)
{
  
  OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
  OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
  OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}
// 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void MyOLED::OLED_Clear(void)
{
    if(!bIsReady)
    return;

  u8 i, n;
  for (i = 0; i < 8; i++)
  {
    OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
    OLED_WR_Byte(0x00, OLED_CMD);     // 设置显示位置—列低地址
    OLED_WR_Byte(0x10, OLED_CMD);     // 设置显示位置—列高地址
    for (n = 0; n < 128; n++)
      OLED_WR_Byte(0, OLED_DATA);
  } // 更新显示
}

// 在指定位置显示一个字符
// x:0~127
// y:0~63
// sizey:选择字体 6x8  8x16
void MyOLED::OLED_ShowChar(u8 x, u8 y, const u8 chr, u8 sizey)
{
    if(!bIsReady)
    return;

  u8 c = 0, sizex = sizey / 2, temp;
  u16 i = 0, size1;
  if (sizey == 8)
    size1 = 6;
  else
    size1 = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * (sizey / 2);
  c = chr - ' '; // 得到偏移后的值
  OLED_Set_Pos(x, y);
  for (i = 0; i < size1; i++)
  {
    if (i % sizex == 0 && sizey != 8)
      OLED_Set_Pos(x, y++);


    if (sizey == 8)
    {
      temp = pgm_read_byte(&asc2_0806[c][i]);
      OLED_WR_Byte(temp, OLED_DATA); // 6X8字号
    }
    else if (sizey == 16)
    {
      temp = pgm_read_byte(&asc2_1608[c][i]);
      OLED_WR_Byte(temp, OLED_DATA); // 8x16字号
    }
    else
      return;
  }
}
// m^n函数
u32 MyOLED::oled_pow(u8 m, u8 n)
{
  u32 result = 1;
  while (n--)
    result *= m;
  return result;
}
// 显示数字
// x,y :起点坐标
// num:要显示的数字
// len :数字的位数
// sizey:字体大小
void MyOLED::OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 sizey)
{
    if(!bIsReady)
    return;

  u8 t, temp, m = 0;
  u8 enshow = 0;
  if (sizey == 8)
    m = 2;
  for (t = 0; t < len; t++)
  {
    temp = (num / oled_pow(10, len - t - 1)) % 10;
    if (enshow == 0 && t < (len - 1))
    {
      if (temp == 0)
      {
        OLED_ShowChar(x + (sizey / 2 + m) * t, y, ' ', sizey);
        continue;
      }
      else
        enshow = 1;
    }
    OLED_ShowChar(x + (sizey / 2 + m) * t, y, temp + '0', sizey);
  }
}
// 显示一个字符号串
void MyOLED::OLED_ShowString(u8 x, u8 y, const char *chr, u8 sizey)
{
    if(!bIsReady)
    return;

  u8 j = 0;
  while (chr[j] != '\0')
  {
    OLED_ShowChar(x, y, chr[j++], sizey);
    if (sizey == 8)
      x += 6;
    else
      x += sizey / 2;
  }
}

// 显示一个字符号串
void MyOLED::OLED_ShowStringLine(uint8_t x, uint8_t y, const char *chr, uint8_t sizey)
{
    if(!bIsReady)
    return;

   uint8_t j = 0;
      uint8_t maxX = 128;  // Chiều rộng màn hình OLED, thường là 128 pixels
      uint8_t maxCharsPerLine = (maxX - x) / (sizey == 8 ? 6 : sizey / 2);  // Số ký tự tối đa trên một dòng

      while (chr[j] != '\0') {
          OLED_ShowChar(x, y, chr[j++], sizey);
          if (sizey == 8) {
              x += 6;
          } else {
              x += sizey / 2;
          }

          // Kiểm tra nếu vượt quá giới hạn chiều ngang của màn hình
          if (x >= maxX) {
              x = 0;      // Đặt lại vị trí ngang
              y += sizey / 8;  // Tăng vị trí dọc lên hàng tiếp theo
          }

          // Kiểm tra nếu vượt quá giới hạn chiều dọc của màn hình (tùy thuộc vào màn hình của bạn)
          if (y >= 8) {  // Giả sử chiều cao màn hình là 64 pixels (8 hàng, mỗi hàng 8 pixels)
              break;  // Dừng hiển thị nếu vượt quá chiều cao màn hình
          }
      }
}

// 显示汉字
void MyOLED::OLED_ShowChinese(u8 x, u8 y, const u8 no, u8 sizey)
{
  if(!bIsReady)
    return;

  u16 i, size1 = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
  u8 temp;
  for (i = 0; i < size1; i++)
  {
    if (i % sizey == 0)
      OLED_Set_Pos(x, y++);
    if (sizey == 16)
    {
      temp = pgm_read_byte(&Hzk[no][i]);
      OLED_WR_Byte(temp, OLED_DATA); // 16x16字号
    }
    //    else if(sizey==xx) OLED_WR_Byte(xxx[c][i],OLED_DATA);//用户添加字号
    else
      return;
  }
}

// 显示图片
// x,y显示坐标
// sizex,sizey,图片长宽
// BMP：要显示的图片
void MyOLED::OLED_DrawBMP(u8 x, u8 y, u8 sizex, u8 sizey, const u8 BMP[])
{
  if(!bIsReady)
    return;
  u16 j = 0;
  u8 i, m, temp;
  sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
  for (i = 0; i < sizey; i++)
  {
    OLED_Set_Pos(x, i + y);
    for (m = 0; m < sizex; m++)
    {
      temp = pgm_read_byte(&BMP[j++]);
      OLED_WR_Byte(temp, OLED_DATA);
    }
  }
} // OLED的初始化
void MyOLED::OLED_Init(void)
{
  Wire.begin(0x3c);     // join i2c bus (address optional for master)

  OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel
  OLED_WR_Byte(0x00, OLED_CMD); //---set low column address
  OLED_WR_Byte(0x10, OLED_CMD); //---set high column address
  OLED_WR_Byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WR_Byte(0x81, OLED_CMD); //--set contrast control register
  OLED_WR_Byte(0xCF, OLED_CMD); // Set SEG Output Current Brightness
  OLED_WR_Byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
  OLED_WR_Byte(0xC8, OLED_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
  OLED_WR_Byte(0xA6, OLED_CMD); //--set normal display
  OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
  OLED_WR_Byte(0x3f, OLED_CMD); //--1/64 duty
  OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WR_Byte(0x00, OLED_CMD); //-not offset
  OLED_WR_Byte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WR_Byte(0xD9, OLED_CMD); //--set pre-charge period
  OLED_WR_Byte(0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WR_Byte(0xDA, OLED_CMD); //--set com pins hardware configuration
  OLED_WR_Byte(0x12, OLED_CMD);
  OLED_WR_Byte(0xDB, OLED_CMD); //--set vcomh
  OLED_WR_Byte(0x40, OLED_CMD); // Set VCOM Deselect Level
  OLED_WR_Byte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WR_Byte(0x02, OLED_CMD); //
  OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
  OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable
  OLED_WR_Byte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)
  OLED_WR_Byte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)
  OLED_Clear();
  OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/

  //TODO: need check, turn color and display
  OLED_ColorTurn(0);//0正常显示 1反色显示
  OLED_DisplayTurn(0);//0正常显示 1翻转180度显示

  //Set ready
  bIsReady = true;
}
