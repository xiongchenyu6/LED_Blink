#!/usr/bin/env python3
# Extract HZK16 bitmap data for specific Chinese characters

import struct

def gb2312_to_offset(gb_code):
    """Convert GB2312 code to HZK16 file offset"""
    high = (gb_code >> 8) & 0xFF
    low = gb_code & 0xFF
    
    # Calculate area and position
    area = high - 0xA0
    pos = low - 0xA0
    
    # Calculate offset: ((area-1)*94 + (pos-1)) * 32
    offset = ((area - 1) * 94 + (pos - 1)) * 32
    return offset

def extract_char_bitmap(hzk16_file, gb_code):
    """Extract 32-byte bitmap data for a character"""
    offset = gb2312_to_offset(gb_code)
    
    with open(hzk16_file, 'rb') as f:
        f.seek(offset)
        data = f.read(32)
    
    return data

def print_bitmap_hex(char, gb_code, data):
    """Print bitmap data in C array format"""
    print(f"    // {char} - GB2312: {gb_code:04X}")
    for i in range(0, 32, 2):
        byte1 = data[i]
        byte2 = data[i+1]
        # Create binary representation
        bin1 = format(byte1, '08b').replace('0', '.').replace('1', '1')
        bin2 = format(byte2, '08b').replace('0', '.').replace('1', '1')
        print(f"    0x{byte1:02X}, 0x{byte2:02X},  // {bin1} {bin2}")

# Characters to extract with verified GB2312 codes
characters = [
    ("我", 0xCED2),  # Verified
    ("爱", 0xB0AE),  # Verified
    ("郭", 0xB9F9),  # Verified
    ("芷", 0xDCC6),  # Finally correct! D6A5 was showing 芝
    ("慧", 0xBBDB),  # Verified
]

# Note: You need to download HZK16 font file from:
# https://github.com/aguegu/BitmapFont/raw/master/font/HZK16

print("// Chinese characters 16x16 font data")
print("// Each character is 16x16 pixels = 32 bytes (2 bytes per row)")
print("const uint8_t ChineseFont16x16[] = {")

# Try to read HZK16 file
try:
    for char, gb_code in characters:
        data = extract_char_bitmap("HZK16", gb_code)
        print_bitmap_hex(char, gb_code, data)
        print()
except FileNotFoundError:
    print("// Error: HZK16 font file not found!")
    print("// Download from: https://github.com/aguegu/BitmapFont/raw/master/font/HZK16")
    
print("};")