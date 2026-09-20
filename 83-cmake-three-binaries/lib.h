#ifndef LIB_H
#define LIB_H

/* Three modules stand in for the shared half of your RPG. Each has its own
   .c; the point of the exercise is which binary links which one. */

int combat_damage(int atk, int def);
int config_answer(void);
unsigned rng_next(unsigned *state);

#endif
