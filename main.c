#include <proc/p32mx320f128h.h>
#include <stdint.h>

#define DISPLAY_VDD PORTFbits.RF6
#define DISPLAY_VBATT PORTFbits.RF5
#define DISPLAY_COMMAND_DATA PORTFbits.RF4
#define DISPLAY_RESET PORTGbits.RG9

void delay(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!SPI2STATbits.SPITBE);
	SPI2BUF = data;
	while(!SPI2STATbits.SPIRBF);
	return SPI2BUF;
}

void display_init() {
	DISPLAY_COMMAND_DATA = 0x0;
	delay(10);
	DISPLAY_VDD = 0x0;
	delay(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_RESET = 0x0;
	delay(10);
	DISPLAY_RESET = 0x1;
	delay(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_VBATT = 0x0;
	delay(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

int main(void) {
	/* Set up peripheral bus clock */
	OSCCONbits.PBDIV = 1;
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	SPI2STATbits.SPIROV = 0;
	SPI2CONbits.CKP = 1;
	SPI2CONbits.MSTEN = 1;
	SPI2CONbits.ON = 1;
	
	display_init();
	
	int i, j;
	for(i = 0; i < 4; i++) {
		DISPLAY_COMMAND_DATA = 0x0;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_COMMAND_DATA = 0x1;
		for(j = 0; j < 128/4; j++) {
			spi_send_recv('U');
			spi_send_recv('U');
			spi_send_recv('U');
			spi_send_recv('U');
		}
	}
	
	PORTE = 0xFF;
	
	for(;;) {
	}
	
	return 0;
}

