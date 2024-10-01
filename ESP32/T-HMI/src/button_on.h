#include <lvgl.h>

const uint16_t button_on_map[] = {
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8,
    0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8,
    0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8,
    0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8,
    0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8,
    0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8, 0x39C8,
    0x39C8, 0x39C8, 0x39C8, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x3A28,
    0x3A27, 0x3B47, 0x4428, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448,
    0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448,
    0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448,
    0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448,
    0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448,
    0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448, 0x4448,
    0x4448, 0x4448, 0x4448, 0x4428, 0x3C07, 0x3AE7, 0x3A28, 0x3A09,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x3A09,
    0x3A28, 0x3BA6, 0x45EA, 0x4F2D, 0x57EF, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57EF, 0x57EF, 0x4FEF, 0x4F2D,
    0x4508, 0x3AE6, 0x3A08, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x3A09, 0x3AE6, 0x44C7, 0x4F4D, 0x57F0, 0x57F0, 0x57EF, 0x4FEF,
    0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF,
    0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF,
    0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF,
    0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x4FEF, 0x57EF,
    0x57EF, 0x57EF, 0x57EF, 0x57EF, 0x57EF, 0x57EF, 0x57EF, 0x57EF,
    0x57EF, 0x57EF, 0x57EF, 0x57EF, 0x57EF, 0x57EF, 0x57EF, 0x57F0,
    0x57EF, 0x57F0, 0x57EF, 0x4E6B, 0x3BC6, 0x3A68, 0x39E9, 0x3A09,
    0x39E9, 0x39E9, 0x3A27, 0x44A7, 0x57EF, 0x57F0, 0x57EF, 0x4FEF,
    0x4F6E, 0x570F, 0x5F30, 0x5F10, 0x5F0F, 0x5F0F, 0x5F0F, 0x5F0F,
    0x5F0F, 0x5F0F, 0x5EEF, 0x5EEF, 0x5EEF, 0x5F0F, 0x5F0F, 0x5F0F,
    0x5EEF, 0x5EEF, 0x5EEF, 0x5EEF, 0x56EF, 0x56EF, 0x56EF, 0x56CF,
    0x56CE, 0x56CE, 0x56CE, 0x56CE, 0x56CE, 0x56AE, 0x56AE, 0x56AE,
    0x56AE, 0x56AE, 0x56AE, 0x56AE, 0x4EAE, 0x4EAE, 0x4EAE, 0x4EAE,
    0x4EAE, 0x4EAE, 0x4EAE, 0x4EAE, 0x4EAE, 0x4EAE, 0x4E8E, 0x4E8E,
    0x4EAE, 0x4EAD, 0x4FAF, 0x57F0, 0x57EF, 0x57F0, 0x4F0C, 0x3B66,
    0x39E9, 0x39E9, 0x39E9, 0x3A28, 0x3BA6, 0x4F2D, 0x57F0, 0x57EF,
    0x4F6E, 0x6E30, 0xAE16, 0xB5B6, 0xBD97, 0xBD76, 0xB556, 0xB535,
    0xAD15, 0xACF4, 0xA4F4, 0xA4D4, 0xA4B3, 0x9C93, 0x9C93, 0x9C93,
    0x9C72, 0x9452, 0x9431, 0x8C31, 0x8C11, 0x8BF0, 0x83D0, 0x83CF,
    0x7BAF, 0x7B8F, 0x7B6E, 0x736E, 0x734E, 0x732D, 0x6B2D, 0x6B0D,
    0x6AEC, 0x6AEC, 0x62CC, 0x62CC, 0x62AB, 0x5AAB, 0x5A8B, 0x5A8B,
    0x5A8B, 0x5A8A, 0x5A6A, 0x5A6A, 0x526A, 0x524A, 0x524A, 0x524A,
    0x524A, 0x5229, 0x5249, 0x4228, 0x42E9, 0x3D0B, 0x4FEF, 0x57EF,
    0x57F0, 0x45EA, 0x3AA6, 0x3A09, 0x39E9, 0x3A27, 0x460A, 0x57F0,
    0x57EF, 0x46ED, 0x85B1, 0xD639, 0xDE9A, 0xCE59, 0xBE17, 0xBDF7,
    0xBDD7, 0xB5B6, 0xB5B6, 0xAD95, 0xAD75, 0xA555, 0xA534, 0xA514,
    0x9CF3, 0x9CD3, 0x94D2, 0x94B2, 0x9492, 0x8C91, 0x8C71, 0x8451,
    0x8430, 0x8430, 0x7C0F, 0x7BEF, 0x7BCF, 0x73CE, 0x73AE, 0x73AE,
    0x6B8D, 0x6B6D, 0x6B6D, 0x634D, 0x632C, 0x632C, 0x632C, 0x630C,
    0x5B0B, 0x5B0B, 0x5AEB, 0x5AEB, 0x5ACB, 0x5ACB, 0x52CA, 0x52CA,
    0x52AA, 0x52AA, 0x52AA, 0x52AA, 0x52AA, 0x528A, 0x4A29, 0x3966,
    0x33C8, 0x4FAF, 0x57EF, 0x57CF, 0x4448, 0x39C8, 0x39E9, 0x3B67,
    0x4F4D, 0x57F0, 0x47AE, 0x6D4F, 0xCDF8, 0xCE99, 0xBDD7, 0xB596,
    0xAD75, 0xAD55, 0xA534, 0xA534, 0xA514, 0x9CF3, 0x9CD3, 0x9CD3,
    0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8C51, 0x8430, 0x8410, 0x7BEF,
    0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x738E, 0x6B6D, 0x6B4D, 0x6B4D,
    0x632C, 0x632C, 0x630C, 0x5AEB, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA,
    0x52AA, 0x52AA, 0x528A, 0x528A, 0x528A, 0x4A69, 0x4A69, 0x4A69,
    0x4A49, 0x4A49, 0x4A49, 0x4A49, 0x4A49, 0x4228, 0x4228, 0x4208,
    0x4228, 0x4228, 0x2905, 0x3489, 0x57F0, 0x57F0, 0x4E0B, 0x3A67,
    0x39C8, 0x4407, 0x57EF, 0x57F0, 0x4EEE, 0x9D13, 0xC638, 0xAD95,
    0xB5B6, 0xCE59, 0xC638, 0xBDF7, 0xBDD7, 0xB5B6, 0xB5B6, 0xB5B6,
    0xAD75, 0xAD75, 0xAD55, 0xA534, 0xA514, 0x9CF3, 0x9CD3, 0x94B2,
    0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410, 0x8410, 0x7BEF, 0x7BCF,
    0x73AE, 0x73AE, 0x738E, 0x738E, 0x6B6D, 0x6B4D, 0x6B4D, 0x632C,
    0x632C, 0x630C, 0x630C, 0x5AEB, 0x5AEB, 0x5AEB, 0x5ACB, 0x5ACB,
    0x52AA, 0x52AA, 0x52AA, 0x528A, 0x528A, 0x528A, 0x528A, 0x4A69,
    0x4A49, 0x39E7, 0x2945, 0x31C6, 0x39A7, 0x21C5, 0x4F6E, 0x57F0,
    0x4EAC, 0x3A87, 0x39C8, 0x4428, 0x57EF, 0x4FEF, 0x4D6C, 0xAD14,
    0xBDD7, 0x9CD3, 0xB596, 0xE71C, 0xF79E, 0xE73C, 0xE71C, 0xE71C,
    0xDEFB, 0xDEDB, 0xD6BA, 0xD69A, 0xCE79, 0xCE59, 0xCE59, 0xC638,
    0xC618, 0xBDF7, 0xBDD7, 0xB5B6, 0xB596, 0xB596, 0xAD55, 0xA534,
    0xA534, 0xA514, 0x9CF3, 0x9CF3, 0x9CF3, 0x9CD3, 0x94B2, 0x9492,
    0x9492, 0x8C71, 0x8C71, 0x8C51, 0x8C51, 0x8430, 0x8430, 0x8430,
    0x8410, 0x8410, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BCF, 0x7BCF, 0x7BCF,
    0x7BCF, 0x7BCF, 0x632C, 0x39E7, 0x2945, 0x18E3, 0x39E7, 0x2104,
    0x3DAB, 0x57F1, 0x4EEC, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF,
    0x5D2D, 0xAD14, 0xAD95, 0x8C51, 0xA534, 0xC618, 0xDEFB, 0xDEFB,
    0xDEDB, 0xD6BA, 0xD6BA, 0xCE79, 0xCE79, 0xCE59, 0xC638, 0xC618,
    0xBDF7, 0xBDD7, 0xBDD7, 0xB5B6, 0xB596, 0xAD75, 0xAD55, 0xA534,
    0xA534, 0xA514, 0x9CF3, 0x9CD3, 0x94B2, 0x94B2, 0x9492, 0x8C71,
    0x8C51, 0x8C71, 0x8C51, 0x8C51, 0x8430, 0x8430, 0x8410, 0x7BEF,
    0x7BEF, 0x7BCF, 0x7BCF, 0x73AE, 0x73AE, 0x73AE, 0x738E, 0x738E,
    0x738E, 0x6B6D, 0x6B6D, 0x738E, 0x52AA, 0x31A6, 0x2124, 0x18C3,
    0x39C7, 0x20A3, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428,
    0x57F0, 0x4FEF, 0x5D2D, 0xAD14, 0xAD55, 0x8410, 0x9CF3, 0xAD75,
    0xB5B6, 0xB5B6, 0xB596, 0xAD75, 0xAD55, 0xA534, 0xA514, 0xA514,
    0x9CF3, 0x9CD3, 0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8C51, 0x8430,
    0x8410, 0x7BEF, 0x7BCF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B6D,
    0x6B4D, 0x632C, 0x632C, 0x630C, 0x630C, 0x5AEB, 0x5AEB, 0x5ACB,
    0x5ACB, 0x52AA, 0x52AA, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A69,
    0x4A69, 0x4A49, 0x4A49, 0x4A49, 0x4A49, 0x4228, 0x31A6, 0x3186,
    0x2104, 0x10A2, 0x31C6, 0x1883, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87,
    0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x5D2D, 0xACF4, 0xAD55, 0x8410,
    0x9CD3, 0xAD75, 0xB5B6, 0xAD75, 0xAD55, 0xA534, 0xA514, 0x9CF3,
    0x9CD3, 0x94B2, 0x9492, 0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410,
    0x8410, 0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x738E, 0x6B6D, 0x6B4D,
    0x632C, 0x630C, 0x630C, 0x5AEB, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA,
    0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A69, 0x4A49, 0x4A49, 0x4A49,
    0x4228, 0x4228, 0x4208, 0x4208, 0x4208, 0x4208, 0x39E7, 0x39E7,
    0x31A6, 0x2965, 0x2104, 0x10A2, 0x31A6, 0x1883, 0x3D2A, 0x57F1,
    0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x5D2D, 0xA4F4,
    0xA554, 0x8410, 0x9CD3, 0xAD75, 0xB596, 0xAD75, 0xAD55, 0xA534,
    0xA514, 0x9CF3, 0x9CD3, 0x94B2, 0x9492, 0x9492, 0x8C71, 0x8C51,
    0x8430, 0x8410, 0x7BEF, 0x7BCF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D,
    0x6B4D, 0x6B4D, 0x632C, 0x630C, 0x630C, 0x5AEB, 0x5ACB, 0x5ACB,
    0x52AA, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A69, 0x4A49, 0x4A49,
    0x4A49, 0x4228, 0x4228, 0x4208, 0x4208, 0x4208, 0x39E7, 0x39E7,
    0x39E7, 0x39C7, 0x3186, 0x2965, 0x18E3, 0x1082, 0x3186, 0x1883,
    0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF,
    0x5D2D, 0xA4F4, 0xA534, 0x7BEF, 0x9CD3, 0xAD75, 0xB596, 0xAD55,
    0xA534, 0xA514, 0x9CF3, 0x9CD3, 0x9CD3, 0x94B2, 0x9492, 0x8C71,
    0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BEF, 0x7BCF, 0x73AE, 0x738E,
    0x6B6D, 0x6B4D, 0x6B4D, 0x632C, 0x630C, 0x630C, 0x5AEB, 0x5ACB,
    0x5ACB, 0x52AA, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A69, 0x4A49,
    0x4A49, 0x4228, 0x4228, 0x4208, 0x4208, 0x4208, 0x39E7, 0x39E7,
    0x39E7, 0x39C7, 0x39C7, 0x31A6, 0x2965, 0x2945, 0x18E3, 0x1082,
    0x2985, 0x1862, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428,
    0x57F0, 0x4FEF, 0x5D0D, 0xA4D4, 0xA534, 0x7BEF, 0x94B2, 0xAD55,
    0xAD75, 0xAD55, 0xA534, 0xA514, 0x9CF3, 0x9CD3, 0x94B2, 0x9492,
    0x8C71, 0x8C51, 0x8430, 0x8410, 0x8410, 0x7BEF, 0x7BCF, 0x73AE,
    0x738E, 0x6B6D, 0x6B6D, 0x6B4D, 0x632C, 0x630C, 0x630C, 0x5AEB,
    0x5AEB, 0x5ACB, 0x52AA, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A49,
    0x4A49, 0x4A49, 0x4228, 0x4228, 0x4208, 0x4208, 0x39E7, 0x39E7,
    0x39C7, 0x39C7, 0x39C7, 0x39C7, 0x31A6, 0x31A6, 0x2965, 0x2124,
    0x18E3, 0x1082, 0x2965, 0x1062, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87,
    0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x5D0D, 0xA4D3, 0xA534, 0x7BEF,
    0x94B2, 0xAD55, 0xAD75, 0xAD55, 0xA514, 0x9CF3, 0x9CF3, 0x9CD3,
    0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BCF,
    0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x632C, 0x630C,
    0x5AEB, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x528A, 0x528A, 0x4A69,
    0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4208, 0x4208, 0x39E7, 0x39E7,
    0x39E7, 0x39C7, 0x39C7, 0x31A6, 0x31A6, 0x31A6, 0x31A6, 0x3186,
    0x2945, 0x2124, 0x18C3, 0x1082, 0x2945, 0x1042, 0x3D2A, 0x57F1,
    0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x5D0D, 0xA4B3,
    0xA534, 0x7BEF, 0x94B2, 0xA534, 0xAD75, 0xA534, 0xA514, 0x9CF3,
    0x9CD3, 0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8430, 0x8430, 0x8410,
    0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x6B4D, 0x632C,
    0x630C, 0x630C, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x528A, 0x528A,
    0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4228, 0x4208, 0x4208, 0x39E7,
    0x39E7, 0x39C7, 0x39C7, 0x39C7, 0x31A6, 0x31A6, 0x3186, 0x3186,
    0x3186, 0x2965, 0x2945, 0x2104, 0x18C3, 0x0861, 0x2144, 0x1042,
    0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF,
    0x550D, 0x9CB3, 0xA514, 0x7BCF, 0x9492, 0xA534, 0xAD55, 0xA534,
    0x9CF3, 0x9CD3, 0x9CD3, 0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8430,
    0x8410, 0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x738E, 0x6B6D, 0x6B4D,
    0x632C, 0x630C, 0x630C, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x528A,
    0x528A, 0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4228, 0x4208, 0x4208,
    0x39E7, 0x39E7, 0x39C7, 0x39C7, 0x31A6, 0x31A6, 0x3186, 0x3186,
    0x3186, 0x2965, 0x2965, 0x2965, 0x2124, 0x2104, 0x10A2, 0x0861,
    0x2124, 0x1042, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428,
    0x57F0, 0x4FEF, 0x550D, 0x9CB3, 0x9D13, 0x7BCF, 0x9492, 0xA534,
    0xAD55, 0xA514, 0x9CF3, 0x9CD3, 0x94B2, 0x9492, 0x8C71, 0x8C71,
    0x8430, 0x8430, 0x8410, 0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D,
    0x6B4D, 0x632C, 0x632C, 0x630C, 0x5AEB, 0x5AEB, 0x5ACB, 0x52AA,
    0x52AA, 0x528A, 0x4A69, 0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4208,
    0x4208, 0x39E7, 0x39E7, 0x39C7, 0x39C7, 0x31A6, 0x31A6, 0x3186,
    0x3186, 0x2965, 0x2965, 0x2965, 0x2965, 0x2945, 0x2124, 0x2104,
    0x10A2, 0x0861, 0x2124, 0x1042, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87,
    0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x550D, 0x9C93, 0x9CF3, 0x7BCF,
    0x9492, 0xA514, 0xAD55, 0xA514, 0x9CF3, 0x9CD3, 0x94B2, 0x9492,
    0x8C71, 0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BCF, 0x73AE, 0x738E,
    0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C, 0x630C, 0x5AEB, 0x5ACB,
    0x5ACB, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A49, 0x4A49, 0x4228,
    0x4208, 0x4208, 0x39E7, 0x39C7, 0x39C7, 0x39C7, 0x31A6, 0x3186,
    0x3186, 0x3186, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2945,
    0x2124, 0x2104, 0x10A2, 0x0861, 0x2144, 0x1042, 0x3D2A, 0x57F1,
    0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x550D, 0x9C93,
    0x9CF3, 0x7BCF, 0x8C71, 0xA514, 0xA534, 0xA514, 0x9CD3, 0x94B2,
    0x9492, 0x8C71, 0x8C51, 0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BCF,
    0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C, 0x630C, 0x5AEB,
    0x5ACB, 0x5ACB, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A49, 0x4A49,
    0x4228, 0x4228, 0x4208, 0x39E7, 0x39E7, 0x39C7, 0x31A6, 0x31A6,
    0x3186, 0x3186, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2945, 0x2124, 0x2104, 0x10A2, 0x0861, 0x2144, 0x1042,
    0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF,
    0x550D, 0x9C92, 0x9CF3, 0x7BCF, 0x8C71, 0xA514, 0xA534, 0x9CF3,
    0x9CD3, 0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410, 0x7BEF,
    0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C,
    0x630C, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x528A, 0x4A69, 0x4A69,
    0x4A49, 0x4228, 0x4228, 0x4208, 0x39E7, 0x39E7, 0x39C7, 0x39C7,
    0x31A6, 0x3186, 0x3186, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2945, 0x2124, 0x2104, 0x10A2, 0x0861,
    0x2144, 0x1042, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428,
    0x57F0, 0x4FEF, 0x550D, 0x9C92, 0x9CF3, 0x7BCF, 0x8C71, 0x9CF3,
    0xA534, 0x9CF3, 0x9CD3, 0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8430,
    0x8410, 0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C,
    0x632C, 0x630C, 0x5AEB, 0x5AEB, 0x5ACB, 0x52AA, 0x528A, 0x528A,
    0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4208, 0x4208, 0x39E7, 0x39C7,
    0x39C7, 0x31A6, 0x3186, 0x3186, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2945, 0x2124, 0x2104,
    0x10A2, 0x0861, 0x2144, 0x1042, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87,
    0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x550D, 0x9C72, 0x9CF3, 0x7BCF,
    0x8C71, 0x9CF3, 0xA514, 0x9CF3, 0x94B2, 0x9492, 0x9492, 0x8C51,
    0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D,
    0x6B4D, 0x632C, 0x630C, 0x5AEB, 0x5AEB, 0x5ACB, 0x52AA, 0x52AA,
    0x528A, 0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4208, 0x4208, 0x39E7,
    0x39C7, 0x39C7, 0x31A6, 0x3186, 0x3186, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2124, 0x2104, 0x18C3, 0x0861, 0x2144, 0x1042, 0x3D2A, 0x57F1,
    0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x550D, 0x9C72,
    0x9CD3, 0x7BCF, 0x8C51, 0x9CF3, 0xA514, 0x9CD3, 0x94B2, 0x9492,
    0x8C71, 0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BCF, 0x7BCF, 0x73AE,
    0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C, 0x5AEB, 0x5AEB, 0x5ACB,
    0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4208,
    0x39E7, 0x39E7, 0x39C7, 0x31A6, 0x31A6, 0x3186, 0x3186, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2124, 0x2104, 0x18C3, 0x0861, 0x2144, 0x1042,
    0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF,
    0x550D, 0x9C72, 0x9CD3, 0x73AE, 0x8C51, 0x9CD3, 0xA514, 0x9CD3,
    0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BCF,
    0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C, 0x630C, 0x5AEB,
    0x5ACB, 0x52AA, 0x52AA, 0x528A, 0x4A69, 0x4A69, 0x4A49, 0x4228,
    0x4208, 0x4208, 0x39E7, 0x39C7, 0x39C7, 0x31A6, 0x3186, 0x3186,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2124, 0x2104, 0x18C3, 0x0861,
    0x2144, 0x1042, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428,
    0x57F0, 0x4FEF, 0x550D, 0x9472, 0x94D2, 0x73AE, 0x8C51, 0x9CD3,
    0xA514, 0x9CD3, 0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410,
    0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C,
    0x630C, 0x5AEB, 0x5ACB, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A49,
    0x4228, 0x4228, 0x4208, 0x39E7, 0x39E7, 0x39C7, 0x31A6, 0x3186,
    0x3186, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2124, 0x2104,
    0x18C3, 0x0861, 0x2124, 0x1042, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87,
    0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x550D, 0x9472, 0x94D2, 0x738E,
    0x8C51, 0x9CD3, 0xA514, 0x9CD3, 0x9492, 0x9492, 0x8C71, 0x8C51,
    0x8430, 0x8410, 0x7BCF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D,
    0x632C, 0x630C, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x528A, 0x528A,
    0x4A69, 0x4A49, 0x4228, 0x4228, 0x4208, 0x39E7, 0x39C7, 0x39C7,
    0x31A6, 0x3186, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2124, 0x2104, 0x10A2, 0x0861, 0x2124, 0x1042, 0x3D2A, 0x57F1,
    0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x54ED, 0x9472,
    0x94B2, 0x738E, 0x8C51, 0x9CD3, 0x9CF3, 0x9CD3, 0x9492, 0x8C71,
    0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D,
    0x6B4D, 0x6B4D, 0x630C, 0x630C, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA,
    0x528A, 0x4A69, 0x4A69, 0x4A49, 0x4228, 0x4208, 0x39E7, 0x39E7,
    0x39C7, 0x31A6, 0x31A6, 0x3186, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2945, 0x2124, 0x2104, 0x10A2, 0x0861, 0x2144, 0x1042,
    0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF,
    0x54EC, 0x9452, 0x94B2, 0x738E, 0x8430, 0x9CD3, 0x9CF3, 0x94B2,
    0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410, 0x7BEF, 0x7BCF, 0x73AE,
    0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C, 0x630C, 0x5AEB, 0x5ACB,
    0x52AA, 0x52AA, 0x528A, 0x4A69, 0x4A49, 0x4228, 0x4228, 0x4208,
    0x39E7, 0x39C7, 0x39C7, 0x31A6, 0x3186, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2945, 0x2124, 0x2104, 0x10A2, 0x0861,
    0x2144, 0x1042, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4428,
    0x57F0, 0x4FEF, 0x54EC, 0x9452, 0x94B2, 0x738E, 0x8430, 0x9CD3,
    0x9CF3, 0x94B2, 0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410, 0x7BEF,
    0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C, 0x630C,
    0x5AEB, 0x5ACB, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A49, 0x4228,
    0x4228, 0x4208, 0x39E7, 0x39C7, 0x31A6, 0x31A6, 0x3186, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2945, 0x2124, 0x2104,
    0x10A2, 0x0861, 0x2144, 0x1042, 0x3D2A, 0x57F1, 0x4F0D, 0x3A87,
    0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x54ED, 0x9452, 0x94B2, 0x738E,
    0x8430, 0x9CD3, 0x9CD3, 0x9492, 0x8C71, 0x8C51, 0x8430, 0x8410,
    0x7BEF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x630C,
    0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x528A, 0x4A69, 0x4A49, 0x4A49,
    0x4228, 0x4208, 0x39E7, 0x39E7, 0x39C7, 0x31A6, 0x3186, 0x3186,
    0x2965, 0x2945, 0x2945, 0x2945, 0x2945, 0x2945, 0x2945, 0x2945,
    0x2945, 0x2945, 0x2945, 0x2945, 0x2945, 0x2945, 0x2945, 0x2945,
    0x2124, 0x2104, 0x10A2, 0x0861, 0x2144, 0x1042, 0x3D2A, 0x57F1,
    0x4F0D, 0x3A87, 0x39C8, 0x4428, 0x57F0, 0x4FEF, 0x4CEC, 0x8C51,
    0x94B2, 0x6B6D, 0x7BEF, 0x8C51, 0x8C71, 0x8C51, 0x8C51, 0x8430,
    0x8410, 0x7BCF, 0x7BCF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C,
    0x632C, 0x630C, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x528A, 0x528A,
    0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4208, 0x4208, 0x39E7, 0x39C7,
    0x39C7, 0x31A6, 0x3186, 0x3186, 0x3186, 0x3186, 0x3186, 0x3186,
    0x3186, 0x3186, 0x3186, 0x3186, 0x3186, 0x3186, 0x3186, 0x3186,
    0x3186, 0x3186, 0x2965, 0x2104, 0x10A2, 0x0861, 0x2124, 0x1042,
    0x3D4A, 0x57F1, 0x4F0D, 0x3A87, 0x39C8, 0x4408, 0x57EF, 0x4FEF,
    0x452B, 0x83AF, 0x9CD3, 0x7BEF, 0x6B6D, 0x6B4D, 0x738E, 0x73AE,
    0x738E, 0x6B6D, 0x6B4D, 0x632C, 0x632C, 0x630C, 0x5AEB, 0x5AEB,
    0x5ACB, 0x52AA, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A69, 0x4A49,
    0x4A49, 0x4A49, 0x4228, 0x4208, 0x4208, 0x39E7, 0x39C7, 0x39C7,
    0x31A6, 0x31A6, 0x3186, 0x3186, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2965,
    0x2965, 0x2965, 0x2965, 0x2965, 0x2965, 0x2104, 0x1082, 0x18C3,
    0x2124, 0x10A2, 0x3E0B, 0x57F0, 0x4ECC, 0x3A87, 0x39C9, 0x3BC7,
    0x4F8E, 0x57F0, 0x4EED, 0x638D, 0x8C51, 0x9CD3, 0x8C51, 0x73AE,
    0x5AEB, 0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x52AA, 0x52AA, 0x528A,
    0x528A, 0x4A69, 0x4A69, 0x4A49, 0x4228, 0x4228, 0x4228, 0x4208,
    0x39E7, 0x39E7, 0x39C7, 0x39C7, 0x39C7, 0x31A6, 0x31A6, 0x31A6,
    0x3186, 0x3186, 0x3186, 0x2965, 0x2965, 0x2945, 0x2945, 0x2945,
    0x2945, 0x2945, 0x2945, 0x2124, 0x2124, 0x2124, 0x2124, 0x2124,
    0x2945, 0x2945, 0x2945, 0x2945, 0x2945, 0x2124, 0x2945, 0x2104,
    0x18C3, 0x2144, 0x18A3, 0x19E4, 0x4FAF, 0x57F0, 0x4E4B, 0x3A87,
    0x39E9, 0x3A87, 0x4E8C, 0x57F0, 0x4FCF, 0x4CCB, 0x734D, 0x8410,
    0x94D2, 0x9CD3, 0x8C92, 0x8C71, 0x8C51, 0x8430, 0x8430, 0x7C0F,
    0x7BEF, 0x7BEF, 0x73CF, 0x73AE, 0x738E, 0x6B6D, 0x6B4D, 0x632C,
    0x632C, 0x5AEB, 0x5AEB, 0x5ACB, 0x52AA, 0x52AA, 0x528A, 0x4A69,
    0x4A69, 0x4A69, 0x4A49, 0x4A49, 0x4228, 0x4208, 0x3A07, 0x39E7,
    0x39C7, 0x39C7, 0x39C7, 0x39C7, 0x39C7, 0x39C7, 0x39C7, 0x39C7,
    0x39C7, 0x39C7, 0x39C7, 0x39E7, 0x39E7, 0x39E7, 0x39E7, 0x39E7,
    0x31C6, 0x2965, 0x2965, 0x2104, 0x1042, 0x350A, 0x57F0, 0x57EF,
    0x44A9, 0x3A08, 0x39E9, 0x3A28, 0x4487, 0x4FAE, 0x57F0, 0x470D,
    0x54EC, 0x6B2D, 0x6B2D, 0x738E, 0x7BCF, 0x738E, 0x6B6D, 0x6B4D,
    0x6B4D, 0x632C, 0x632C, 0x630C, 0x5AEB, 0x5ACB, 0x52AA, 0x528A,
    0x528A, 0x4A69, 0x4A69, 0x4A49, 0x4A28, 0x4228, 0x4208, 0x4208,
    0x39E7, 0x39C7, 0x39C7, 0x31A6, 0x31A6, 0x3186, 0x3166, 0x2965,
    0x2945, 0x2925, 0x2124, 0x2124, 0x2124, 0x2124, 0x2124, 0x2124,
    0x2124, 0x2124, 0x2124, 0x2124, 0x2124, 0x2124, 0x2124, 0x2124,
    0x2124, 0x2124, 0x2124, 0x2104, 0x20A3, 0x18C3, 0x3428, 0x4FAF,
    0x57F0, 0x4EAC, 0x3B26, 0x39E9, 0x39E9, 0x3A09, 0x3AA7, 0x45A9,
    0x57F0, 0x57F0, 0x4F8E, 0x4DED, 0x5C2C, 0x5B4B, 0x5B2C, 0x5B0B,
    0x5AEB, 0x52EB, 0x52CA, 0x52CA, 0x4AAA, 0x4A89, 0x4A89, 0x4A69,
    0x4A69, 0x4269, 0x4248, 0x4228, 0x4228, 0x3A07, 0x3A07, 0x3A07,
    0x39E7, 0x31E7, 0x31C6, 0x31A6, 0x31A6, 0x31A6, 0x2985, 0x2985,
    0x2965, 0x2165, 0x2144, 0x2144, 0x2124, 0x2124, 0x2124, 0x2124,
    0x2124, 0x2124, 0x2144, 0x2144, 0x2144, 0x2144, 0x2144, 0x2144,
    0x2144, 0x2144, 0x2124, 0x2124, 0x2144, 0x2184, 0x2B47, 0x45EC,
    0x57EF, 0x57F0, 0x4FAE, 0x3C27, 0x3A28, 0x3A09, 0x39E9, 0x39E9,
    0x3A09, 0x3B86, 0x462A, 0x57CF, 0x57F0, 0x57F0, 0x4FAE, 0x4F2E,
    0x572F, 0x572F, 0x572F, 0x572F, 0x572F, 0x570F, 0x570E, 0x570E,
    0x570E, 0x570E, 0x570E, 0x570E, 0x4F0E, 0x4EEE, 0x4EEE, 0x4EEE,
    0x4EEE, 0x4EEE, 0x4EEE, 0x4EEE, 0x4EEE, 0x4EEE, 0x4ECE, 0x4ECE,
    0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD,
    0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD,
    0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4ECD, 0x4EED,
    0x57F0, 0x57F0, 0x57F0, 0x4F6E, 0x44E7, 0x3AA7, 0x39E9, 0x3A09,
    0x39E9, 0x39E9, 0x39E9, 0x3A48, 0x3AC6, 0x44C8, 0x4F4D, 0x57EF,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0, 0x57F0,
    0x57F0, 0x57F0, 0x57EF, 0x57EF, 0x4E8B, 0x3BC6, 0x3A87, 0x3A09,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x3A87,
    0x3B47, 0x44C8, 0x45CA, 0x462B, 0x4E2B, 0x4E2B, 0x4E2B, 0x4E2B,
    0x4E2B, 0x4E2B, 0x4E2B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B,
    0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B,
    0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B,
    0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B,
    0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B, 0x462B,
    0x462B, 0x462B, 0x462B, 0x460A, 0x45A9, 0x43E7, 0x3AE6, 0x3A48,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39C9, 0x3A28, 0x3A27, 0x3A27, 0x3A27, 0x3A27,
    0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27,
    0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27,
    0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27,
    0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27,
    0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27,
    0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A27, 0x3A08,
    0x39E9, 0x39E9, 0x3A09, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39E9, 0x3A09, 0x3A09, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9,
    0x39E9, 0x39E9, 0x3A09, 0x39E9, 0x39E9, 0x39E9, 0x39E9, 0x39E9
};

const lv_img_dsc_t button_on = {
    .header = {
        .cf = LV_IMG_CF_TRUE_COLOR,
        .w = 66,
        .h = 44
    },
    .data_size = sizeof(button_on_map),
    .data = (const uint8_t *)button_on_map
};
