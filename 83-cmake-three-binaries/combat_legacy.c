#include "lib.h"

/* An older combat rule, kept around after the floor was added — exactly the
   kind of file that lingers in a real tree. It defines the same symbol as
   combat.c, so no binary can link both, and nothing warns you about which
   one a given source list happened to name. */

int combat_damage(int atk, int def)
{
    return atk - def; /* no floor: can go negative */
}
