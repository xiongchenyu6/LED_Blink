#!/usr/bin/env python3
# Test different GB2312 codes to find the correct one for 芷

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

# Test different codes around the area
test_codes = [
    ("D6A5", 0xD6A5),  # Current attempt
    ("D6B7", 0xD6B7),  # Alternative
    ("C5A5", 0xC5A5),  # Alternative
    ("E5A5", 0xE5A5),  # Alternative
]

print("Testing different GB2312 codes to find 芷:")
print("==========================================")

for name, code in test_codes:
    offset = gb2312_to_offset(code)
    print(f"\nCode {name}: offset = {offset}")
    
    # Try to decode to see what character it is
    try:
        # Convert GB2312 code to bytes and decode
        byte_high = (code >> 8) & 0xFF
        byte_low = code & 0xFF
        gb_bytes = bytes([byte_high, byte_low])
        char = gb_bytes.decode('gb2312')
        print(f"  Character: {char}")
    except:
        print(f"  Character: (decode error)")

# Let's also try to encode 芷 directly
print("\nDirect encoding of 芷:")
try:
    zhi_bytes = "芷".encode('gb2312')
    print(f"  Bytes: {zhi_bytes.hex()}")
    print(f"  As code: 0x{zhi_bytes.hex().upper()}")
except Exception as e:
    print(f"  Error: {e}")