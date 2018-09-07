#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

GLdouble vertex[][3] =
  {
    { 0.0, 0.0, 0.0 },
    { 1.0, 0.0, 0.0 },
    { 1.0, 1.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    { 1.0, 0.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 0.0, 1.0, 1.0 }
  };


GLdouble color[][3] =
  {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    { 1.0, 1.0, 0.0 },
    { 1.0, 0.0, 1.0 },
    { 0.0, 1.0, 1.0 }
  };

int face[][4] =
  {
    { 0, 1, 2, 3 },
    { 1, 5, 6, 2 },
    { 5, 4, 7, 6 },
    { 4, 0, 3, 7 },
    { 4, 5, 1, 0 },
    { 3, 2, 6, 7 }
  };

GLdouble fx, fy, fz;

void
display ()
{
  int i;
  static int r;
  GLdouble l2 = fx * fx + fy * fy + fz * fz;
  GLdouble l = sqrt (l2) * .2;

  fx /= l, fy /= l, fz /= l;

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  gluLookAt (fx, fy, fz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glTranslated (-.5, -.5, -.5);
  glColor3d (0.0, 0.0, 0.0);
  glBegin (GL_QUADS);

  int j;
  for (j = 0; j < 6; j++)
    {
      glColor3dv(color[j]);
      for (i = 0; i < 4; i++)
	{
	  glVertex3dv(vertex[face[j][i]]);
	}
    }
  glEnd ();

  glutSwapBuffers ();
}

void
resize (int w, int h)
{
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

  glMatrixMode(GL_MODELVIEW);
}

void
init ()
{

  glClearColor (1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);
}

int fd;

void
keyboard (uint8_t key, int x, int y)
{

  close (fd);
  printf ("close serial line.\n");
  exit (0);
}

void
idle ()
{
  static uint8_t buf2[64];
  uint8_t buf[64];
  uint32_t a;
  uint8_t *p;
  int i;
  static int j;
  ssize_t sz;
  uint32_t x, y, z;

  if ((sz = read (fd, buf, sizeof buf)) > 0)
    {
      for (i = 0; i < sz; i++)
	{
	  uint8_t q = buf[i];
	  if (q == '\r')
	    continue;
	  if ((buf2[j++] = buf[i]) == '\n')
	    {
	      buf2[j - 1] = '\0';
	      sscanf (buf2, "%d%d%d", &x, &y, &z);
	      fx = (double)x - 2048.;
	      fy = (double)y - 2048.;
	      fz = (double)z - 2048.;
	      j = 0;
	      glutPostRedisplay ();
	    }
	  if (j == sizeof buf2)
	    {
	      printf ("buffer overflow %s\n", buf2);
	      j = 0;
	    }
	}
    }
}

int
main (int argc, char *argv[])
{

  fd = open_serial ("/dev/dtyU0", B230400);
  assert (fd);
  fcntl (fd ,F_SETFL, O_NONBLOCK);	// Non-block mode.

  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow (argv[0]);
  glutDisplayFunc (display);
  glutReshapeFunc (resize);
  glutKeyboardFunc(keyboard);
  glutIdleFunc (idle);

  init ();
  glutMainLoop ();

  return 0;
}

int
open_serial (const char *device, speed_t speed)
{
  struct termios t;
  int desc;

  if ((desc = open (device, O_RDWR)) == 0)
    {
      perror (device);
      return 0;
    }

  tcsetpgrp(desc, getpgrp ());
  if (tcgetattr (desc, &t) != 0)
    {
      perror (device);
      goto error;
    }

  // 8N1
  cfsetispeed(&t, speed);
  cfsetospeed(&t, speed);
  t.c_cflag &= ~(CSIZE|PARENB);
  t.c_cflag |= CS8;
  t.c_iflag &= ~(ISTRIP|ICRNL);
  t.c_oflag &= ~OPOST;
  t.c_lflag &= ~(ICANON|ISIG|IEXTEN|ECHO);
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;

  if (tcsetattr (desc, TCSANOW, &t) != 0)
    {
      perror ("tcsetattr (1)");
      goto error;
    }

  // Drain buffer.
  if (tcflush (desc, TCIOFLUSH) == -1)
    {
      perror ("tcflush");
      goto error;
    }

  if (tcdrain (desc) != 0)
    {
      perror ("tcdrain");
      goto error;
    }

  return desc;

 error:
  close (desc);

  return 0;
}
