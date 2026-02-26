del STM32U5G7_LLTA_100_raw.hex
ren STM32U5G7_LLTA_100.hex STM32U5G7_LLTA_100_raw.hex
powershell -command ..\srec_cat.exe -generate 0x083FFFE0 0x083FFFFD -repeat-string (get-date) -o Datefile.hex -Intel
..\srec_cat.exe STM32U5G7_LLTA_100_raw.hex -Intel Datefile.hex -Intel -o STM32U5G7_LLTA_100_date.hex -Intel
..\srec_cat.exe STM32U5G7_LLTA_100_date.hex -Intel -crop 0x08080000 0x083FFBFE -fill 0xFF 0x08080000 0x083FFFFE -crc16-b-e 0x083FFFFE -o STM32U5G7_LLTA_100.hex -Intel
..\srec_cat.exe STM32U5G7_LLTA_100.hex -Intel -offset -0x08080000 -o CTRS_Controller.bin -Binary