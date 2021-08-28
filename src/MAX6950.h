
#ifndef max6950_h
#define max6950_h

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#define DECODE_REG      0x01
#define INTENSITY_REG   0x02
#define SCANLIMIT_REG   0x03
#define CONFIG_REG      0x04
#define TEST_REG        0x07
#define P0_REG          0x20
#define P1_REG			0x40
#define BLINK_SLOW      0x08
#define BLINK_FAST      0x14
#define DEFAULT_CONFIG  0x01                // normal mode on
#define ERROR           0xCF				// character E. as error
#define DOT             0x80 

class MAX6950
{
public:
	MAX6950();
	MAX6950(uint8_t cs);
	MAX6950(uint8_t cs, uint8_t digits);
	void init(void);
	void set_brightness(uint8_t value);
	void set_digit(uint8_t num, uint8_t value);
	void blink(uint8_t value);
	void clear(void);
    void test(void);
    void display(float value);


private:
	const uint8_t ChipSelect;
	const uint8_t digits;
    uint8_t config = DEFAULT_CONFIG;        
    void SPIWrite(uint8_t address, uint8_t value);
    template<typename T> void convert(T value, int *buffer);
    inline void print_error(void);
    inline int pow_int(int value);
};
#endif