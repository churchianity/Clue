
/**
 * Implementation stolen from:
 * https://stackoverflow.com/a/107657
 *
 * Though the same implementation (except replace 101 with 31) exists on page 118 of K&R.
 */
static unsigned int hash(const char* s /*, unsigned int seed = 0*/ ) {
    unsigned int hash = 0; // seed

    while (*s) {
        hash = hash * 101 + *s++;
    }

    return hash;
}

void install(char* value) {

}
