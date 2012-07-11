/** \file misc/aduc-erase
 * \brief ADUC Serial sector and mass eraser V1
 *
 *  Copyright (C) 2011 samplemaker
 *  Copyright (C) 2011 Hans Ulrich Niedermann <hun@n-dimensional.de>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 *
 *
 *
 * press the SD and RST switches in the following
 * sequence, as specified in AN-724:
 * SD     ------_________________------
 * RST    ----------_________----------
 * run program:
 * ./aduc-erase [device] [num_sectors]
 *
 * 
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

void
assert_read (const int fd, void *buf, const size_t count)
{
  ssize_t ret = read (fd, buf, count);
  printf ("%d = read(%d, %p, %d)\n", ret, fd, buf, count);
  if (ret != count)
    {
      printf ("  buf = %s\n", buf);
      assert (ret == count);
    }
}

void
assert_write (const int fd, const void *buf, const size_t count)
{
  ssize_t ret = write (fd, buf, count);
  printf ("%d = write(%d, %p, %d)\n", ret, fd, buf, count);
  assert (ret == count);
}

char
get_checksum (const char numbytes, const char command,
              const char *address, const int num_adr,
              const char *data, const int num_data)
{
  int i;
  uint8_t sum;

  sum = (uint8_t) (numbytes);
  sum += (uint8_t) (command);
  for (i = 0; i < num_adr; i++)
    {
      sum += (uint8_t) (address[i]);
    }
  for (i = 0; i < num_data; i++)
    {
      sum += (uint8_t) (data[i]);
    }
  sum = (uint8_t) (0x00 - sum);
  return ((char) (sum));
}

int
main (int argc, char *argv[])
{
  printf (" \n");
  if (argc != 3)
    {
      printf ("*********************************************\n");
      printf ("* ADuC7026 serial sector and mass eraser V2 *\n");
      printf ("*********************************************\n");
      printf (" press the SD and RST switches in the following\n");
      printf (" sequence, as specified in AN-724:\n");
      printf (" SD     ------_________________------\n");
      printf (" RST    ----------_________----------\n");
      printf (" then run eraser:\n");
      printf (" ./aduc-erase [device] [num_sectors]\n");
      printf (" i.)  [1 to 124]: Sectors to be erased from \n");
      printf ("      0x80000 onwards; 0x200kbyte each\n");
      printf (" ii.) [0]: Erasing the entire user code space\n");
      printf ("      and reset the flash/EE protection\n");
      printf (" \n");
      exit (1);
    }

  const uint8_t num_sector = (uint8_t) (atoi (argv[2]));
  if ((num_sector < 0) || ((num_sector > 124)))
    {
      printf (" num_sectors: out of range \n");
      exit (1);
    }

  const char *device_filename = argv[1];

  int fd = open (device_filename, O_RDWR);
  assert (fd >= 0);
  /* presume 8N1 and a default bitrate between 600 and 115200bps to be
     compatible with the ADuC */
  printf ("OPENED DEVICE %s\n", device_filename);

  printf ("Send setup char to device ... \n");
  char setup = 0x08;
  printf (">> 0x%02x \n", setup);
  assert_write (fd, &setup, 1);
  printf ("stand by (2 seconds) ...\n");
  /* two seconds should be long enough for receiving 24 characters */
  sleep (2);

  char buf[256];
  memset (buf, '\0', sizeof (buf));
  assert_read (fd, buf, 24);
  printf ("Received (raw dump):%s\n", buf);

  printf ("PRODUCT ID:                     %.8s\n", buf);
  char *s1 = &buf[11];
  printf ("MEMORY SIZE MODEL:              %.3s\n", s1);
  printf ("SILICON REV.:                   %c\n", buf[15]);
  printf ("LOADER VERSION:                 %c\n", buf[16]);
  printf ("LOADER VERSION REVISION NUMBER: %c\n", buf[17]);

  /* go ahead with the erasing command ... */
  /* trailing 2 commando bytes */
  const char packet_start_id[] = { 0x07, 0x0e };

  /* number of databytes (all three data fields) (5..255) */
  const char number_of_data_bytes = 0x06;

  /* command byte in data 1 field
     Erase Page              E (0x45)
     Write                   W (0x57)
     Verify                  V (0x56)
     Protect                 P (0x50)
     Run (Jump to User Code) R (0x52)
   */
  const char command_function = 0x45;

   /* adress field definitions (data 2 to data 5) */
  char adress[4];

  if (num_sector == 0)
    {
      adress[0] = 0x00;
      adress[1] = 0x00;
      adress[2] = 0x00;
      adress[3] = 0x00;
    }else{
      adress[0] = 0x00;
      adress[1] = 0x80;
      adress[2] = 0x00;
      adress[3] = 0x00;
    };

  /* data or number of pages to erase (0x00 and adr 0x00000000 gives a mass erase) */
  char data_byte[1];
  data_byte[0] = (char) (num_sector);

  /* Checksum Field */
  char checksum = get_checksum (number_of_data_bytes,
                                command_function,
                                adress, sizeof (adress),
                                data_byte, sizeof (data_byte));

  char cmd[256];
  memset (cmd, '\0', sizeof (buf));

  cmd[0] = packet_start_id[0];
  cmd[1] = packet_start_id[1];
  cmd[2] = number_of_data_bytes;
  cmd[3] = command_function;
  cmd[4] = adress[0];
  cmd[5] = adress[1];
  cmd[6] = adress[2];
  cmd[7] = adress[3];
  cmd[8] = data_byte[0];
  cmd[9] = checksum;

  printf ("write erase command to chip now (arg. %d sectors) ... \n",
           data_byte[0]);
  int i;
  printf (">> ");
  for (i = 0; i < 10; i++)
    {
      printf ("0x%02x ", (0xff & (cmd[i])));
    }
  printf ("\n");
  assert_write (fd, &cmd, 10);
  printf ("stand by (6 seconds) ...\n");
  sleep (6);

  char acknowledge;
  assert_read (fd, &acknowledge, 1);

  switch (acknowledge)
    {
    case 0x06:
      printf ("ACK: (0x06) => Erase of flash sectors successful!\n");
      break;
    case 0x07:
      printf ("BEL: (0x07) => ERROR: Negative response!\n");
      break;
    default:
      /* poor man's fallthrough */
      printf ("Unknown return code!\n");
      printf ("Received: 0x%x\n", acknowledge);
      break;
    }


  close (fd);
  return 0;
}
