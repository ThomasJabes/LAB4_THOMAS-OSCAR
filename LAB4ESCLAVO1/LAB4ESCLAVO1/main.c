#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// Inicializaci�n del I2C como esclavo
void I2C_Slave_Init(uint8_t address) {
	TWAR = address << 1; // Configurar direcci�n del esclavo
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE); // Habilitar TWI y las interrupciones
	sei(); // Habilitar interrupciones globales
}

// Inicializaci�n del ADC
void ADC_Init(void) {
	ADMUX = (1 << REFS0); // Seleccionar AVcc como referencia de voltaje
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Habilitar el ADC y configurar el prescaler a 128
}

// Leer un valor del ADC
uint16_t ADC_Read(uint8_t channel) {
	channel &= 0b00000111; // Asegurarse de que el canal est� entre 0 y 7
	ADMUX = (ADMUX & 0xF8) | channel; // Seleccionar el canal
	ADCSRA |= (1 << ADSC); // Iniciar la conversi�n
	while (ADCSRA & (1 << ADSC)); // Esperar a que la conversi�n termine
	return ADC;
}

// Interrupci�n del TWI
ISR(TWI_vect) {
	switch (TWSR & 0xF8) {
		case 0x60: // Direcci�n recibida
		case 0x70: // Direcci�n general recibida
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK
		break;
		case 0xA8: // Solicitud de datos del maestro
		TWDR = ADC_Read(0) >> 2; // Leer el valor del potenci�metro en PC0 y enviar el valor mapeado (0-255)
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK
		break;
		case 0xC0: // Datos enviados y NACK recibido
		case 0xC8: // �ltimo byte enviado
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK
		break;
		default:
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK por defecto
		break;
	}
}

void setup() {
	I2C_Slave_Init(0x08); // Configurar la direcci�n del esclavo
	ADC_Init(); // Inicializar el ADC
}

void loop() {
	// El bucle principal puede estar vac�o
}

int main(void) {
	setup();
	while (1) {
		loop();
	}
	return 0;
}
