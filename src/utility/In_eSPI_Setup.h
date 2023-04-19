#define TFT_RGB_ORDER 0  // Use the correct RGB order for the display

#define CGRAM_OFFSET  // Display has an offset that needs to be corrected
                      // Offset corrections in In_eSPI_Drivers\ST7789_Rotation.h

// Above values must be set before ST7789_Defines.h is included

#include "In_eSPI_Drivers/ST7789_Defines.h"

#define ST7789_DRIVER  // Full configuration option, define additional
                       // parameters below for this display

#define USE_HSPI_PORT  // Use the second SPI port for the display, keeping the
                       // default SPI port free for use on the Atom expansion
                       // port

#define TFT_DRIVER 0x7789
#define TFT_WIDTH  128
#define TFT_HEIGHT 128

#define TFT_MISO -1
#define TFT_MOSI 21
#define TFT_SCLK 17
#define TFT_CS   15  // Chip select line for TFT display on Shield
#define TFT_DC   33  // Data/command line for TFT on Shield aka RS
#define TFT_RST  34  // Reset line for TFT is handled by seesaw!

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes
                    // in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in
                    // FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in
                    // FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH,
                    // only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in
                    // FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH,
                    // only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower,
// so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free
                    // fonts FF1 to FF48 and custom fonts

#define SPI_FREQUENCY 27000000

#define SPI_READ_FREQUENCY 20000000

#define SPI_TOUCH_FREQUENCY 2500000
