#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <assert.h>

int
main ()
{
  int fd = open ("/dev/dty00", O_RDWR);
  struct termios t;

  assert (fd >= 0);

  if (tcgetattr (fd, &t) != 0)
    exit (1);

  cfsetispeed(&t, B115200);
  cfsetospeed(&t, B115200);
  t.c_cflag &= ~(CSIZE|PARENB);
  t.c_cflag |= CS8;
  t.c_iflag &= ~(ISTRIP|ICRNL);
  t.c_oflag &= ~OPOST;
  t.c_lflag &= ~(ICANON|ISIG|IEXTEN|ECHO);
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;

  if (tcsetattr (fd, TCSANOW, &t) != 0)
    exit (1);

  uint8_t buf[0x100];
  uint8_t buf2[0x100];

  uint8_t *p = buf2;
  while (/*CONSTCOND*/1)
    {
      size_t sz;

      while ((sz = read (fd, buf, sizeof buf)) != -1 && sz != 0)
	{
	  int i;
	  for (i = 0; i < sz; i++)
	    {
	      uint8_t q = buf[i];
	      int x0, y0, z0;
	      float x, y, z;

	      if (q == '\r')
		continue;
	      if ((*p++ = buf[i]) == '\n')
		{
		  int8_t ce;
		  int8_t axis;
		  *--p = '\0';
		  printf ("%s\n", buf2);
		  sscanf (buf2, "%d%d %c", &axis, &x0, &ce);
		  printf ("%d %d %c\n", axis, x0, ce);
		  p = buf2;
		}
	    }
	}
    }
  close (fd);

  return 0;
}
