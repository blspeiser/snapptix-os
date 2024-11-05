#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <png.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/vt.h>
#include <linux/kd.h>

#define WIDTH	320
#define HEIGHT	240
#define MAX_COLORS 256

/* Global Variables */

/* Color map structures */
unsigned short red[MAX_COLORS], green[MAX_COLORS], blue[MAX_COLORS];
struct fb_cmap cmap = { 0, MAX_COLORS, red, green, blue, NULL };

/* File Descriptor for framebuffer */
int fbfd = 0;

/* PNG structures */
png_infop png_info_ptr = NULL;
png_structp png_ptr = NULL;
png_bytep row_pointer = NULL;

/* Framebuffer area */
unsigned char *fbp = (unsigned char *)-1;

/* File pointer for PNG file */
FILE *fp = NULL;

/* Framebuffer info */
struct fb_var_screeninfo vinfo;

/* Path of PNG file */
char *path = NULL;

/* Virtual console file descriptor */
int vfd = 0;

/* Exits, printing an error message after cleaning up */
/* If you don't pass it a message, it doesn't print one */
/* and exits normally */
void die(char *msg)
{
	if (msg)
		printf("Error: %s\n", msg);
	if (path)
		free(path);
	if (fbp == (unsigned char *)-1)
		munmap(fbp, vinfo.xres * vinfo.yres);
	if (fbfd)
		close(fbfd);
	if (png_ptr)
		png_destroy_read_struct(&png_ptr, &png_info_ptr, png_infopp_NULL);
	if (fp)
		fclose(fp);
	if (row_pointer)
		free(row_pointer);
	if (vfd)
		close(vfd);
	if (msg)
		exit(1);
	exit(0);
}

void display_png()
{
	unsigned long i;

	/* Make sure our file is a PNG */
	if ((fp = fopen(path, "rb")) == NULL)
		die("Unable to open file");

	/* Prepare the PNG structures */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		die("Unable to create PNG read struct");
	png_info_ptr = png_create_info_struct(png_ptr);
	if (!png_info_ptr)
		die("Unable to create PNG info struct");

	/* Read the PNG file using streams */
	png_init_io(png_ptr, fp);

	/* Make sure our PNG file is ok */
	png_read_info(png_ptr, png_info_ptr);
	if (png_info_ptr->color_type != PNG_COLOR_TYPE_PALETTE)
		die("Only paletted images supported");
	if (png_info_ptr->bit_depth > MAX_COLORS)
		die("Exceeded maximum colors in palette");
	if ((png_info_ptr->width != WIDTH) && (png_info_ptr->height != HEIGHT))
		die("Image dimensions incorrect");
	if (png_info_ptr->interlace_type != PNG_INTERLACE_NONE)
		die("Only uninterlaced images supported");

	/* Read palette from PNG file */
	for (i = 0; i < png_info_ptr->num_palette; i++)
	{
		/* Multiply by 257 because the PNG color values are 1 byte and
		 * the kernel color values are 2 bytes.  65535/255 = 257 */
		red[i] = png_info_ptr->palette[i].red * 257;
		green[i] = png_info_ptr->palette[i].green * 257;
		blue[i] = png_info_ptr->palette[i].blue * 257;
#ifdef DEBUG
		printf("Index %d -> Red %d Green %d Blue %d\n", i, red[i], green[i], blue[i]);
#endif
	}

	/* Put the color map (palette) from our PNG file */
	if (ioctl(fbfd, FBIOPUTCMAP, &cmap) < 0)
		die("Unable to set colormap");

	/* Prepare error handling for PNG */
	/* the png_read function jumps here if something goes wrong */
	if (setjmp(png_jmpbuf(png_ptr)))
		die("Error while reading PNG file");

	/* Now we read and display one row at a time, to save memory */
	/* NOTE: I didn't just call png_read_rows with the row pointer set to the framebuffer
	 * and read the whole thing right to framebuffer memory, as the console I am testing this
	 * on is 1024x768 and I want to skip pixels to put it in the upper lefthand corner.  This doesn't
	 * take that much longer, and it will allow your to do transforms by row if you wish. */
	row_pointer = (png_bytep)malloc(WIDTH);
	for (i = 0; i < HEIGHT; i++)
	{
		png_read_rows(png_ptr, &row_pointer, png_bytepp_NULL, 1);
		memcpy(fbp+(i*vinfo.xres), row_pointer, WIDTH);
	}

	/* Free up unused memory */
	fclose(fp);
	fp = NULL;
	free(row_pointer);
	row_pointer = NULL;
	png_destroy_read_struct(&png_ptr, &png_info_ptr, png_infopp_NULL);
	png_ptr = NULL;
	png_info_ptr = NULL;
}

void release_vt(int signal)
{
	ioctl(vfd, VT_RELDISP, 1);
}

void acquire_vt(int signal)
{
	display_png();
	ioctl(vfd, VT_RELDISP, VT_ACKACQ);
}

int main(int argc,
	 char **argv)
{
	struct sigaction sa;
	struct vt_mode vtm;
	int ttyfd;

	/* Check arguements */
	if (argc < 2)
		die("Usage is pngtofb <pngfile>");

	/* Store the path */
	path = malloc(strlen(argv[1]));
	if (!path)
		die("Unable to malloc");
	strcpy(path, argv[1]);

	/* Detach from TTY */
	if ((ttyfd = open("/dev/tty", O_RDWR)) >= 0)
	{
		ioctl(ttyfd, TIOCNOTTY, 0);
		close(ttyfd);
	}

	/* Open the framebuffer device */
	fbfd = open("/dev/fb0", O_RDWR);
	if (! fbfd)
		die("Can't open framebuffer");

	/* Get variable screen info */
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
		die("Unable to read variable info!");

	/* Make sure the framebuffer is set correctly */
	if (vinfo.bits_per_pixel != 8)
		die("Only 8-bpp mode supported");

	/* Memory map the area */
	fbp = (unsigned char *)mmap(0, vinfo.xres * vinfo.yres, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (fbp == (unsigned char *)-1)
		die("Cannot mmap to framebuffer");

	/* move to the visible area of the screen */
	vinfo.xoffset = 0;
	vinfo.yoffset = 0;
	if (ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo))
		die("Unable to pan screen");

	display_png();

	/* Set up signals for when people switch to/from our virtual console */
	sigemptyset(&(sa.sa_mask));
	sa.sa_flags = 0;
	sa.sa_handler = release_vt;
	sigaction(SIGUSR1, &sa, NULL);
	sa.sa_handler = acquire_vt;
	sigaction(SIGUSR2, &sa, NULL);

	/* Link these signals to the vt switch */
	vfd = open("/dev/tty0", O_RDWR);
	if (vfd < 0)
		die("Cannot open /dev/tty0");
	vtm.mode = VT_PROCESS;
	vtm.relsig = SIGUSR1;
	vtm.acqsig = SIGUSR2;
	ioctl(vfd, VT_SETMODE, &vtm);

	while(1);

	die(NULL);

	/* Make compiler happy */
	return 0;
}
