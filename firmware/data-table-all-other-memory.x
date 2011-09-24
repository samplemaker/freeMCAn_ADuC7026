/* \todo needs fix !! */ 
SECTIONS {
  data_table = _end ;
  data_table_end = ( __stack_end__ - MIN_STACK_SIZE - MALLOC_HEAP_SIZE ) ;
  data_table_size = data_table_end - data_table ;
  data_table_size2 = data_table_size / 2 ;
  data_table_size3 = data_table_size / 3 ;
  data_table_size4 = data_table_size / 4 ;
  _end = ALIGN(data_table_end, 4) ;
  end = ALIGN(data_table_end, 4) ;
  __heap_start = ALIGN(data_table_end, 4) ;
  __heap_end = __heap_start + MALLOC_HEAP_SIZE ;
}
INSERT AFTER .noinit ;
ASSERT ( ( __heap_end + MIN_STACK_SIZE ) <= __stack_end__, "(data+stack+table) size is too large for SRAM") ;
ASSERT ( data_table_size >= 1024, "data table size is smaller than 1K" ) ;
