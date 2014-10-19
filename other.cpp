using namespace std;

/**
 * Return sum of 1 in the char n
 * @brief bitCount
 * @param n
 */
static unsigned bitCount (unsigned char n){
    unsigned count = 0;
    while (n){
        ++count;
        n &= (n - 1);
    }
    return count;
}
