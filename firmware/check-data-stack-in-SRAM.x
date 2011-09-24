ASSERT(__bss_end + MIN_STACK_SIZE < __stack_end__, "(data size + stack size) is too large for SRAM");
