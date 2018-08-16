U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

char vaildBuf[16];
void draw() {
	u8g2.drawStr(2, 12, validBuf);
}

void screen_setup() {
	u8g2.setColorIndex(1);
	u8g2.setFont(u8g_font_fur11);
}

void screen_loop()
{
	sprintf(validBuf, "GPS: %c Alt: %c",GPS_Available?'Y':'N',Altitude_Available?'Y':'N');

	u8g2.firstPage();
	do {
		draw();
	} while (u8g2.nextPage());

}
