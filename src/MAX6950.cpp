
#include <SPI.h>
#include "MAX6950.h"

int x =0b11001111;

//                    0      1     2     3     4    5     6      7     8     9
int8_t decode[10] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};

MAX6950::MAX6950(uint8_t cs, uint8_t digits) : ChipSelect(cs), digits(digits) {
    init();
}
MAX6950::MAX6950(uint8_t cs ) : ChipSelect(cs), digits(7) {
    init();
}
MAX6950::MAX6950() : ChipSelect(10), digits(7) {
    init();
}

void MAX6950::init(void)
{
	pinMode (ChipSelect, OUTPUT);           // Set ChipSelect pin to output
	SPI.begin();                            // Init SPI connection
    SPIWrite(CONFIG_REG, DEFAULT_CONFIG);           // Write default config (normal mode)
    SPIWrite(SCANLIMIT_REG, digits - 1);        // Set number of display elements
    SPIWrite(INTENSITY_REG, 0x07);          // Set intensity to 7 of 15
    //SPIWrite(DECODE_REG, 0xFF);             // Enable Decoding by default
    SPIWrite(DECODE_REG, 0x00);             // Enable Decoding by default
}

void MAX6950::SPIWrite(uint8_t address, uint8_t value)
{
	digitalWrite(ChipSelect,LOW);           // Pull CS low to select chip
	SPI.transfer(address);                  // Write register address
	SPI.transfer(value);                    // Write value to register
    digitalWrite(ChipSelect, HIGH); // Pull CS high to deselect chip
}

void MAX6950::set_brightness(uint8_t value){
    SPIWrite(INTENSITY_REG, (value & 0x0F));    // Set brightness between 0-15
}
void MAX6950::clear(void){
    for (uint8_t i = 0; i < digits; i++){
        SPIWrite(P0_REG + i, 0x00);                     // Set all digits to 0
    }
}

void MAX6950::display(float value)
{
    int len = (int)floor(log10(abs(value))) + 1;    // number of digits in value
    if (value < 0)  {len += 1;}                       // if number negative len is reduced to fit minus on screen
    //value = len;
    if (len > digits)
    {
        print_error();
        return;
    }
    //if(((int)floor(log10(abs(value)))+1) > digits){
    //    print_error();
    //    return;
    //}
    int buffer[10];
    convert(value, buffer);
    for (int i = 0; i < digits; i++)
    {
        SPIWrite(P0_REG + i, buffer[i]);
    }
}


void MAX6950::blink(uint8_t value){
    switch(value){
        case 1:                                 // 1 blink slow
            config |= BLINK_SLOW;
            break;
        case 2:                                 // 2 blink fast
            config |= BLINK_FAST;
            break;
        default:                                // blink off
            config = DEFAULT_CONFIG;
            break;
    }
    SPIWrite(CONFIG_REG, config);
}
void MAX6950::test(void){
    SPIWrite(TEST_REG, 0x01);
}

template<typename T>
void MAX6950::convert(T value, int *buffer){
    char buf[10];
    int8_t i = 0;
    int8_t j = 0;
    if(value < 0){
        buffer[j++] = 0x01;
        i = 1;
    }
    dtostrf(value, digits, digits, buf);
    
    while (buf[i] != '\0')
    {
        if(buf[i] == '.'){
            buffer[j - 1] |= DOT ;
        }
        else{
            buffer[j++] = decode[(int)buf[i] - 48];
        }
        i++;
    }
}

inline void MAX6950::print_error(void){
    for (int i = 0; i < digits; i++){
        SPIWrite(P0_REG + i, ERROR);
    }

}
