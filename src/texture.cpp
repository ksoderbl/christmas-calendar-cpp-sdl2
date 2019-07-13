#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#include <string.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <png.h>

#include "main.h"
#include "texture.h"



/*
 * Load an PNG file and return a pointer to the image data.
 * Input:  imageFile - name of .png to read
 * Output:  width - width of image
 *          height - height of image
 *          format - format of image (GL_RGB or GL_RGBA)
 * Return:  pointer to image data or NULL if error
 */

// Based on https://gist.github.com/niw/5963798

static GLubyte *LoadPNGImage(string* imageFile,
			     GLint *width, GLint *height,
			     GLenum *format)
{
	FILE *fp = fopen(imageFile->c_str(), "rb");

	int w, h;
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers;

	cout << "Loading " << imageFile->c_str() << endl;

	if (!fp) {
		cerr << "LoadPNGImage: fopen of " << imageFile->c_str() << " failed." << endl;
		return 0;
	}

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png) abort();
	
	png_infop info = png_create_info_struct(png);
	if(!info) abort();
	
	if(setjmp(png_jmpbuf(png))) abort();
	
	png_init_io(png, fp);
	
	png_read_info(png, info);

	w = png_get_image_width(png, info);
	h = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth = png_get_bit_depth(png, info);
	
	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt
	
	if(bit_depth == 16)
		png_set_strip_16(png);
	
	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
	
	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);
	
	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);
	
	// These color_type don't have an alpha channel then fill it with 0xff.
	if(color_type == PNG_COLOR_TYPE_RGB ||
	   color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	
	if(color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);
	
	png_read_update_info(png, info);
	
	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * h);
	for(int y = 0; y < h; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
	}
	
	png_read_image(png, row_pointers);
	
	fclose(fp);
	
	GLint bytes, bytes_per_row;
	GLubyte *buffer;
	
	bytes_per_row = w * 4;  // 4 = RGBA bytes
	bytes = h * bytes_per_row;
	buffer = (GLubyte *) malloc(bytes);
	if (!buffer)
		return 0;
	
	for(int y = 0 ; y < h; y++) {
		memcpy(buffer + (h - 1 - y) * bytes_per_row, row_pointers[y], bytes_per_row);
		free(row_pointers[y]);
	}
	free(row_pointers);
	
	*width = w;
	*height = h;
	*format = GL_RGBA;
	
	return buffer;
}

texture_t *tex_init(string* filename)
{
	texture_t *p;
	size_t size = sizeof(texture_t);

	if (*filename == "") {
		cerr << "tex_init: no texture file name given\n";
		return NULL;
	}

	p = (texture_t *) malloc(size);
	if (!p) {
		cerr << "tex_init: could not alloc tex struct\n";
		return NULL;
	}
	memset(p, 0, size);

	p->tx_filename = filename;
	
	p->tx_data = LoadPNGImage(p->tx_filename,
				  &p->tx_w, &p->tx_h, &p->tx_fmt);
	if (!p->tx_data) {
		cerr << "tex_init: could not find file " << filename << "\n";
		goto fail;
	}

	glGenTextures(1, &p->tx_name);
	//cout << "got texture name " << p->tx_name << " for " << p->tx_filename << "\n";
	
	glBindTexture(GL_TEXTURE_2D, p->tx_name);
	if (!glIsTexture(p->tx_name)) {
		cerr << "tex_init: failed to bind texture\n";
		goto fail;
	}

	return p;
 fail:
	tex_free(p);
	return NULL;
}



void tex_use(texture_t *tp)
{
	if (!tp || !tp->tx_name)
		return;
	glBindTexture(GL_TEXTURE_2D, tp->tx_name);
}



void tex_fmt_rgb_to_rgba(texture_t *tx, GLubyte alpha)
{
	GLubyte *newdata, *p;
	int i;
	int npixels;
	alpha = alpha;

	cout << "doing tex_rgb_to_rgba(" << tx << ")\n";
	
	if (!tx)
		return;
	if (tx->tx_fmt != GL_RGB || tx->tx_fmt == GL_RGBA) {
		cerr << "tex_rgb_to_rgba - fmt error\n";
		return;
	}

	npixels = tx->tx_w * tx->tx_h;
	newdata = (GLubyte *) malloc(npixels * 4);
	if (!newdata) {
		cerr << "could not alloc tex struct\n";
		exit(1);
	}
	memset(newdata, 0, npixels * 4);
	p = tx->tx_data;
	
	for (i = 0; i < npixels; i++) {
		newdata[4*i + 0] = *p++;  /* r */
		newdata[4*i + 1] = *p++;  /* g */
		newdata[4*i + 2] = *p++;  /* b */
		newdata[4*i + 3] = /*alpha */*(p-1); /* a */
	}

	free(tx->tx_data);
	tx->tx_data = newdata;
	tx->tx_fmt  = GL_RGBA;
}


void tex_free(texture_t *tx)
{
	if (!tx)
		return;
	cout << "doing tex_free(" << tx->tx_filename << ")\n";

	cout << "is texture (" << (int)tx->tx_name << ") = " << glIsTexture(tx->tx_name)  << "\n";
	if (tx->tx_name)
		glDeleteTextures(1, &tx->tx_name);
	cout << "is texture (" << (int)tx->tx_name << ") = " << glIsTexture(tx->tx_name)  << "\n";

	if (tx->tx_data)
		free(tx->tx_data);
	//todo: free string ????
	//if (tx->tx_filename)
	//	free((void *)tx->tx_filename);
	free(tx);
}
