#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

#include <stdint.h>

typedef struct
{
    const uint8_t FontWidth; /*!< Font width in pixels */
    uint8_t FontHeight;      /*!< Font height in pixels */
    const uint16_t *data;    /*!< Pointer to data font data array */
} FontDef;

// Available fonts
extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;
extern FontDef Font_5x7;

#endif // __SSD1306_FONTS_H__
