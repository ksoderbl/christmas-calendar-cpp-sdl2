#ifndef READTEX_H
#define READTEX_H

#include <string>

using std::string;

#include <GL/gl.h>
#include <GL/glu.h>


/*
 * Load an SGI .rgb file and return a pointer to the image data.
 * Input:  imageFile - name of .rgb to read
 * Output:  width - width of image
 *          height - height of image
 *          format - format of image (GL_RGB or GL_RGBA)
 * Return:  pointer to image data or NULL if error
 */
extern GLubyte *LoadRGBImage(string imageFile,
			     GLint *width, GLint *height,
			     GLenum *format);

/*
 * Load an SGI .rgb file and generate a set of 2-D mipmaps from it.
 * Input:  imageFile - name of .rgb to read
 *         intFormat - internal texture format to use, or number of components
 * Return:  GL_TRUE if success, GL_FALSE if error.
 */
extern GLboolean LoadRGBMipmaps(string imageFile,
				GLint intFormat);




#endif /* READTEX_H */
