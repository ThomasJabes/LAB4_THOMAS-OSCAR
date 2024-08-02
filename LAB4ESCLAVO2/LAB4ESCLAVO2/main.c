#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void I2C_Slave_Init(uint8_t address) {
	TWAR = address << 1; // Configurar direcci�n del esclavo
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE); // Habilitar TWI y las interrupciones
	sei(); // Habilitar interrupciones globales
}

volatile uint8_t counter = 0; // Contador binario de 4 bits

// Inicializaci�n de pines y configuraci�n de entradas/salidas
void setup() {
	// Configurar PC0, PC1, PC2 y PC3 como salidas para los LEDs
	DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3);
	
	// Configurar PB0 y PB1 como entradas para los botones
	DDRB &= ~((1 << PB0) | (1 << PB1));
	
	// Habilitar resistencias pull-up internas para los botones
	PORTB |= (1 << PB0) | (1 << PB1);

	// Inicializar el I2C como esclavo
	I2C_Slave_Init(0x09); // Direcci�n del esclavo (0x09)
}

// Interrupci�n del TWI
ISR(TWI_vect) {
	switch (TWSR & 0xF8) {
		case 0x60: // Direcci�n recibida y ACK
		case 0x70: // Direcci�n general recibida y ACK
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK
		break;
		case 0xA8: // Direcci�n con datos solicitados por el maestro y ACK
		TWDR = counter; // Enviar el valor del contador
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK
		break;
		case 0xB8: // Datos enviados y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK
		break;
		case 0xC0: // Datos enviados y NACK recibido
		case 0xC8: // �ltimo byte enviado y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK
		break;
		default:
		TWCR |= (1 << TWINT) | (1 << TWEA); // Responder con ACK por defecto
		break;
	}
}

int main(void) {
	setup();
	
	while (1) {
		// Incrementar el contador si el bot�n en PB0 est� presionado
		if (!(PINB & (1 << PB0))) {
			_delay_ms(50); // Delay para evitar rebotes
			if (!(PINB & (1 << PB0))) {
				counter++;
				counter &= 0x0F; // Asegurarse de que el contador sea de 4 bits
				while (!(PINB & (1 << PB0))); // Esperar hasta que el bot�n se libere
			}
		}
		
		// Decrementar el contador si el bot�n en PB1 est� presionado
		if (!(PINB & (1 << PB1))) {
			_delay_ms(50); // Delay para evitar rebotes
			if (!(PINB & (1 << PB1))) {
				counter--;
				counter &= 0x0F; // Asegurarse de que el contador sea de 4 bits
				while (!(PINB & (1 << PB1))); // Esperar hasta que el bot�n se libere
			}
		}
		
		// Mostrar el valor del contador en los LEDs
		PORTC = (PORTC & 0xF0) | (counter & 0x0F);
		
		_delay_ms(100); // Peque�o delay para evitar conteos muy r�pidos
	}
	
	return 0;
}
