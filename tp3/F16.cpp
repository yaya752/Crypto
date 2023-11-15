#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cassert>

using namespace std;

typedef unsigned int aes_t;

const int N = 16;

// P16 = X^{16} + X^5 + X^3 + X^2 + 1
const int PAES = (1 << 16) + (1 << 5) + (1 << 3) + (1 << 2) + 1;

// Multiply a by X in K
aes_t mulbyX (aes_t a)
{
    aes_t ret;
    ret = a << 1;
    if (ret & (1 << N))
	ret ^= PAES;
    return ret;
}

// Multiply two elements of K.
aes_t mul (aes_t a, aes_t b)
{
    aes_t ret = 0;
    aes_t s = a;
    int i;

    for (i = 0; i < N; i++) {
	if (b & (1 << i))
	    ret ^= s;
	s = mulbyX (s);
    }
    return ret;
}

aes_t pow (aes_t a, int n)
{
    if (a == 0)
	return 0;
    if (n == 0)
	return 1;
    if (n & 1)
	return mul (a, pow (mul (a, a), n / 2));
    else
	return pow (mul (a, a), n / 2);
}

aes_t Inverse(aes_t Pol)
{
    return pow(Pol,((1 << 16) - 2));
}

std::array<aes_t, 65536> generateInverseTable() {
    std::array<aes_t, 65536> inverseTable;

    for (unsigned int i = 0; i < 65536; ++i) {
        // Remplir la table avec les inverses des éléments de F216
        aes_t element = static_cast<aes_t>(i);
        aes_t inverse = (element != 0) ? Inverse(element) : 0; // L'élément nul est invariant

        inverseTable[i] = inverse;
    }

    return inverseTable;
}
aes_t subBytes(std::array<aes_t, 65536> inverseTable,aes_t Pol)
{
    return inverseTable[(int)(Pol)];
}
std::array<aes_t, 12> shiftRow(std::array<aes_t, 12> bloc)
{
    std::array<aes_t, 12> ShiftBloc;
    ShiftBloc[0]=bloc[0];
    ShiftBloc[1]=bloc[1];
    ShiftBloc[2]=bloc[2];
    ShiftBloc[3]=bloc[3];
    ShiftBloc[4]=bloc[5];
    ShiftBloc[5]=bloc[6];
    ShiftBloc[6]=bloc[7];
    ShiftBloc[7]=bloc[4];
    ShiftBloc[8]=bloc[10];
    ShiftBloc[9]=bloc[11];
    ShiftBloc[10]=bloc[8];
    ShiftBloc[11]=bloc[9];
    return ShiftBloc;
}
std::array<aes_t, 12> multColumns(std::array<aes_t, 12> bloc)
{
    aes_t pol1 [3], pol2[3],pol3[3],pol4[4]
    ;
    pol1[0]=bloc[0];
    pol1[1]=bloc[4];
    pol1[2]=bloc[8];

    pol2[0]=bloc[1];
    pol2[1]=bloc[5];
    pol2[2]=bloc[9];

    pol3[0]=bloc[2];
    pol3[1]=bloc[6];
    pol3[2]=bloc[10];

    pol4[0]=bloc[3];
    pol4[1]=bloc[7];
    pol4[2]=bloc[11];


}
int main (int argc, char **argv)
{
    std::array<aes_t, 65536> inverseTable = generateInverseTable();
    aes_t a = 0x2;

    cout << "a: " << a << endl;
    cout << "a*X: " << mulbyX(a) << endl;
    cout << "a*a: " << mul(a,a) << endl;
    cout << "Inverse: " << Inverse(a) << endl;
    cout<< "SubBytes: " << subBytes(inverseTable,a) << endl;
    cout << "1: " << mul(a,Inverse(a))<< endl;

    return 0;
}   