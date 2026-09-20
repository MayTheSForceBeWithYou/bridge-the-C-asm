/* Shared "game library" stand-ins — three binaries should link these once. */
int combat_damage(int atk, int def)
{
    int d = atk - def;
    return d > 0 ? d : 0;
}

int config_answer(void)
{
    return 42;
}

unsigned rng_next(unsigned *state)
{
    *state = *state * 1664525u + 1013904223u;
    return *state;
}
