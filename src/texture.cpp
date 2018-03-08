#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::string;

#include <string.h>
#include <stdlib.h>
#include <GL/gl.h>
#include "main.h"
#include "texture.h"

texture_t *tex_init(string filename)
{
	texture_t *p;
	size_t size = sizeof(texture_t);

	if (filename == "") {
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
	
	p->tx_data = LoadRGBImage(p->tx_filename,
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
