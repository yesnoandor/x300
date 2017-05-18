#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>           

#include <fcntl.h>            
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>
#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
        void *                  start;
        size_t                  length;
};

static char *           dev_name        = "/dev/video0";
static int              fd              = -1;
struct buffer *         buffers         = NULL;

FILE *file_fd;
static unsigned long file_length;
static unsigned char *file_name;

int main (int argc,char ** argv)
{
	struct v4l2_capability cap;
	struct v4l2_format fmt;

	printf ("v4l2-test+++++\r\n");

	file_fd = fopen("test.jpg", "w");

	fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

	ioctl (fd, VIDIOC_QUERYCAP, &cap);

	CLEAR (fmt);
	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = 640;
	fmt.fmt.pix.height      = 480;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
	ioctl (fd, VIDIOC_S_FMT, &fmt);

	file_length = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;

	buffers = calloc (1, sizeof (*buffers));

	buffers[0].length = file_length;
	buffers[0].start = malloc (file_length);

	printf ("v4l2-test+++++++++1\r\n");

	for (;;)
	{
   		fd_set fds;
   		struct timeval tv;
   		int r;

   		FD_ZERO (&fds);
   		FD_SET (fd, &fds);

   		/* Timeout. */
   		tv.tv_sec = 3;
   		tv.tv_usec = 0;

   		r = select (fd + 1, &fds, NULL, NULL, &tv);

   		if (-1 == r) {
    		if (EINTR == errno)
     			continue;

			printf ("select");
		}

   		if (0 == r)
		{
    		fprintf (stderr, "select timeout\n");
    		exit (EXIT_FAILURE);
        }

   		if (read (fd, buffers[0].start, buffers[0].length))
   			break;
	}

	fwrite(buffers[0].start, buffers[0].length, 1, file_fd);

	free (buffers[0].start);
	close (fd);

	fclose (file_fd);

	printf ("v4l2-test+++++\r\n");
	
	exit (EXIT_SUCCESS);
	return 0;
}


#if 0
// mmap mode
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>           

#include <fcntl.h>            
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>        
#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
        void *                  start;
        size_t                  length;
};

static char *           dev_name        = "/dev/video0";//\u6444\u50cf\u5934\u8bbe\u5907\u540d
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;

FILE *file_fd;
static unsigned long file_length;
static unsigned char *file_name;
//////////////////////////////////////////////////////
//\u83b7\u53d6\u4e00\u5e27\u6570\u636e
//////////////////////////////////////////////////////
static int read_frame (void)
{
struct v4l2_buffer buf;
unsigned int i;

CLEAR (buf);
buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
buf.memory = V4L2_MEMORY_MMAP;

ioctl (fd, VIDIOC_DQBUF, &buf); //\u51fa\u5217\u91c7\u96c6\u7684\u5e27\u7f13\u51b2

assert (buf.index < n_buffers);
   printf ("buf.index dq is %d,\n",buf.index);

fwrite(buffers[buf.index].start, buffers[buf.index].length, 1, file_fd); //\u5c06\u5176\u5199\u5165\u6587\u4ef6\u4e2d
 
ioctl (fd, VIDIOC_QBUF, &buf); //\u518d\u5c06\u5176\u5165\u5217

return 1;
}

int main (int argc,char ** argv)
{
struct v4l2_capability cap;
struct v4l2_format fmt;
unsigned int i;
enum v4l2_buf_type type;

file_fd = fopen("test-mmap.jpg", "w");//\u56fe\u7247\u6587\u4ef6\u540d

fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);//\u6253\u5f00\u8bbe\u5907

ioctl (fd, VIDIOC_QUERYCAP, &cap);//\u83b7\u53d6\u6444\u50cf\u5934\u53c2\u6570

CLEAR (fmt);
fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
fmt.fmt.pix.width       = 640;
fmt.fmt.pix.height      = 480;
fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
ioctl (fd, VIDIOC_S_FMT, &fmt); //\u8bbe\u7f6e\u56fe\u50cf\u683c\u5f0f

file_length = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height; //\u8ba1\u7b97\u56fe\u7247\u5927\u5c0f

struct v4l2_requestbuffers req;
CLEAR (req);
req.count               = 4;
req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
req.memory              = V4L2_MEMORY_MMAP;

ioctl (fd, VIDIOC_REQBUFS, &req); //\u7533\u8bf7\u7f13\u51b2\uff0ccount\u662f\u7533\u8bf7\u7684\u6570\u91cf

if (req.count < 2)
   printf("Insufficient buffer memory\n");

buffers = calloc (req.count, sizeof (*buffers));//\u5185\u5b58\u4e2d\u5efa\u7acb\u5bf9\u5e94\u7a7a\u95f4

for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
{
   struct v4l2_buffer buf;   //\u9a71\u52a8\u4e2d\u7684\u4e00\u5e27
   CLEAR (buf);
   buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   buf.memory      = V4L2_MEMORY_MMAP;
   buf.index       = n_buffers;

   if (-1 == ioctl (fd, VIDIOC_QUERYBUF, &buf)) //\u6620\u5c04\u7528\u6237\u7a7a\u95f4
    printf ("VIDIOC_QUERYBUF error\n");

   buffers[n_buffers].length = buf.length;
   buffers[n_buffers].start =
   mmap (NULL /* start anywhere */,    //\u901a\u8fc7mmap\u5efa\u7acb\u6620\u5c04\u5173\u7cfb
    buf.length,
    PROT_READ | PROT_WRITE /* required */,
    MAP_SHARED /* recommended */,
    fd, buf.m.offset);

   if (MAP_FAILED == buffers[n_buffers].start)
    printf ("mmap failed\n");
        }

for (i = 0; i < n_buffers; ++i)
{
   struct v4l2_buffer buf;
   CLEAR (buf);

   buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   buf.memory      = V4L2_MEMORY_MMAP;
   buf.index       = i;

   if (-1 == ioctl (fd, VIDIOC_QBUF, &buf))//\u7533\u8bf7\u5230\u7684\u7f13\u51b2\u8fdb\u5165\u5217\u961f
    printf ("VIDIOC_QBUF failed\n");
}
               
type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

if (-1 == ioctl (fd, VIDIOC_STREAMON, &type)) //\u5f00\u59cb\u6355\u6349\u56fe\u50cf\u6570\u636e
   printf ("VIDIOC_STREAMON failed\n");

for (;;) //\u8fd9\u4e00\u6bb5\u6d89\u53ca\u5230\u5f02\u6b65IO
{
   fd_set fds;
   struct timeval tv;
   int r;

   FD_ZERO (&fds);//\u5c06\u6307\u5b9a\u7684\u6587\u4ef6\u63cf\u8ff0\u7b26\u96c6\u6e05\u7a7a
   FD_SET (fd, &fds);//\u5728\u6587\u4ef6\u63cf\u8ff0\u7b26\u96c6\u5408\u4e2d\u589e\u52a0\u4e00\u4e2a\u65b0\u7684\u6587\u4ef6\u63cf\u8ff0\u7b26

   /* Timeout. */
   tv.tv_sec = 2;
   tv.tv_usec = 0;

   r = select (fd + 1, &fds, NULL, NULL, &tv);//\u5224\u65ad\u662f\u5426\u53ef\u8bfb\uff08\u5373\u6444\u50cf\u5934\u662f\u5426\u51c6\u5907\u597d\uff09\uff0ctv\u662f\u5b9a\u65f6

   if (-1 == r) {
    if (EINTR == errno)
     continue;
    printf ("select err\n");
                        }
   if (0 == r) {
    fprintf (stderr, "select timeout\n");
    exit (EXIT_FAILURE);
                        }

   if (read_frame ())//\u5982\u679c\u53ef\u8bfb\uff0c\u6267\u884cread_frame ()\u51fd\u6570\uff0c\u5e76\u8df3\u51fa\u5faa\u73af
   break;
}

unmap:
for (i = 0; i < n_buffers; ++i)
   if (-1 == munmap (buffers[i].start, buffers[i].length))
    printf ("munmap error");
close (fd);
fclose (file_fd);
exit (EXIT_SUCCESS);
return 0;
}
#endif

