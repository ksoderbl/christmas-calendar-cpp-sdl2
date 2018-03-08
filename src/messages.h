#ifndef MESSAGES_H
#define MESSAGES_H

#define FPS_INTERVAL 20

/* called by main */
void messages_init_fps(void);

/* disable GL_LIGHTING, GL_TEXTURE_2D etc before calling this */
void messages_print(char *s, int count, GLfloat fontcolor[3]);
void messages_toggle(void);
int messages_on(void);

#endif /* MESSAGES_H */
