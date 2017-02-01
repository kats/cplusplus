#include "Huffman.h"
#include "bitvector.h"
#include <iostream>

using namespace std;

int main()
{
    {
        bitvector b(10, false);
        cout << b << endl;
        b.push(true);
        cout << b << endl;
    }
    {
        bitvector b(29, true);
        cout << b << endl;
        b.push(true);
        cout << b << endl;
    }
    {
        bitvector b(3229, true);
        cout << b << endl;
    }
    {
        bitvector b;
        b.push(false);
        cout << b[0] << endl;
        b[0] = true;
        cout << b[0] << endl;
        b[0] = true;
        cout << b[0] << endl;
        b[0] = false;
        cout << b[0] << endl;
    }
    {
        bitvector b;

        b.push(true);
        cout << b << " " << b[0] << endl;
        b.push(false);
        cout << b << " " << b[1] << endl;
        b.push(true);
        cout << b << " " << b[2] << endl;
        b.push(true);
        cout << b << " " << b[3] << endl;
        b.push(false);
        cout << b << " " << b[4] << endl;
        b.push(true);
        cout << b << " " << b[5] << endl;
        b.push(true);
        cout << b << " " << b[6] << endl;
        b.push(true);
        cout << b << " " << b[7] << endl;
        b.push(false);
        cout << b << " " << b[8] << endl;
        b.push(true);
        cout << b << " " << b[9] << endl;
    }
    return 0;
}