#ifndef PAUSE_H
#define PAUSE_H

extern int pause_reshape_width;
extern int pause_reshape_height;

void pause_request(void);
void pause_activate(void);
bool pause_is_requested(void);

#endif /* PAUSE_H */

