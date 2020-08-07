
#include "../src/table.hpp"


int main(void) {
    Table<char, int>* t = newTable(10);

    t->insert("dick", 0);

    return 0;
}

