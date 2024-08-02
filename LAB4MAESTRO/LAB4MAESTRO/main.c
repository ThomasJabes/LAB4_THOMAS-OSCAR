#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include "LCD/LCD.h"

// Inicialización del I2C como maestro
void I2C_Init(void) {
	TWSR = 0x00; // Configurar prescaler en 1
	TWBR = 0x47; // SCL frequency is 100KHz for XTAL = 16M
	TWCR = (1 << TWEN); // Habilitar I2C
}

// Generar condición de inicio
void I2C_Start(void) {
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete
}

// Generar condición de parada
void I2C_Stop(void) {
	TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
	while (TWCR & (1 << TWSTO)); // Esperar a que se complete
}

// Escribir un byte de datos
void I2C_Write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete
}

// Leer un byte de datos con NACK
uint8_t I2C_Read_NACK(void) {
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete
	return TWDR;
}

void setup() {
	I2C_Init();
	initLCD8bit();
	LCD_Set_Cursor(1, 1);
	LCD_Write_String("Maestro listo");
	_delay_ms(2000); // Esperar 2 segundos antes de continuar
}

void loop() {
	// Leer el valor del potenciómetro del esclavo 1 (0x08)
	I2C_Start();
	I2C_Write(0x10); // Dirección del esclavo 1 para escribir (8 << 1) + 0
	I2C_Start();
	I2C_Write(0x11); // Dirección del esclavo 1 para leer (8 << 1) + 1
	uint8_t pot_value = I2C_Read_NACK();
	I2C_Stop();

	// Leer el valor del contador del esclavo 2 (0x09)
	I2C_Start();
	I2C_Write(0x12); // Dirección del esclavo 2 para escribir (9 << 1) + 0
	I2C_Start();
	I2C_Write(0x13); // Dirección del esclavo 2 para leer (9 << 1) + 1
	uint8_t counter_value = I2C_Read_NACK();
	I2C_Stop();

	// Mostrar los valores en la pantalla LCD
	LCD_Clear();
	LCD_Set_Cursor(1, 1);
	LCD_Write_String("S1: ");
	char buffer[4];
	itoa(pot_value, buffer, 10);
	LCD_Write_String(buffer);
	
	LCD_Set_Cursor(1, 2); // Ajustar la posición para S2 en la misma línea
	LCD_Write_String("S2: ");
	itoa(counter_value, buffer, 10);
	LCD_Write_String(buffer);

	_delay_ms(1000); // Actualizar cada segundo
}

int main(void) {
	setup();
	while (1) {
		loop();
	}
	return 0;
}
