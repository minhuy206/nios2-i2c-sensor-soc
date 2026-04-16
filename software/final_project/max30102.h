#ifndef MAX30102_H
#define MAX30102_H

#define MAX30102_OK             (0)
#define MAX30102_ERR_INIT       (-1)
#define MAX30102_ERR_ARG        (-2)
#define MAX30102_ERR_IO         (-3)
#define MAX30102_ERR_NOT_READY  (-4)
#define MAX30102_ERR_ID         (-5)

int max30102_init(void);
int max30102_read_part_id(unsigned char *part_id, unsigned char *rev_id);
int max30102_read_sample(unsigned int *red, unsigned int *ir);

#endif
