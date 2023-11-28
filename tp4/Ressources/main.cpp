/*PArtie I.
    Question 2: La boite S à la fin ne rajoute pas de sécurité car la boite S-1 est connu.
    Donc il suffit juste de repasser le texte chiffré dans une S-1 pour enlever cette dernière étape.
    Cela rajoute du temps de calcul mais pas de la sécurité
*/

#include <iostream>
#include <string>
#include <vector>
using namespace std;

static const uint8_t S[16] = {
    0x3, 0xE, 0x1, 0xA, 0x4, 0x9, 0x5, 0x6, 0x8, 0xB, 0xF, 0x2, 0xD, 0xC, 0x0, 0x7};

static const uint8_t S_inv[16] = {
    0xE, 0x2, 0xB, 0x0, 0x4, 0x6, 0x7, 0xF, 0x8, 0x5, 0x3, 0x9, 0xD, 0xC, 0x1, 0xA};

/* Cipher to cryptanalyse */
class Cipher
{
private:
  uint8_t k0;
  uint8_t k1;

  /*  Boite S et son inverse */
  const uint8_t S[16] = {
      0x3, 0xE, 0x1, 0xA, 0x4, 0x9, 0x5, 0x6, 0x8, 0xB, 0xF, 0x2, 0xD, 0xC, 0x0, 0x7};

  const uint8_t S_inv[16] = {
      0xE, 0x2, 0xB, 0x0, 0x4, 0x6, 0x7, 0xF, 0x8, 0x5, 0x3, 0x9, 0xD, 0xC, 0x1, 0xA};

public:
  Cipher()
  {
    k0 = rand() % 16; // Create random subkey0
    k1 = rand() % 16; // Create random subkey1

    printf(" First sub-key k0 = %x\n", k0);
    printf(" Second sub-key k1 = %x\n\n", k1);
  }

  Cipher(uint8_t key0, uint8_t key1)
  {
    k0 = key0;
    k1 = key1;
  }

  uint8_t encrypt(uint8_t input)
  {
    uint8_t output = S[(int)(k0 ^ input)] ^ k1;
    return output;
  }

  uint8_t decrypt(uint8_t input)
  {
    uint8_t output = S_inv[(int)(k1 ^ input)] ^ k0;
    return output;
  }
};

class Cryptanalysis
{
private:
  // Vector to store the plaintexts and cipertexts
  // Plaintexts
  uint8_t knownP0[1000];
  uint8_t knownP1[1000];

  // Ciphertexts
  uint8_t knownC0[1000];
  uint8_t knownC1[1000];

  uint8_t goodP0, goodP1, goodC0, goodC1;

  int chardatmax;
  int chardat0[16];

public:
  Cryptanalysis()
  {
    chardatmax = 0;
  }

  /* Difference Distribution Table of the S-boxe */
  void findBestDiffs(void)
  {
    uint8_t i, j;
    uint8_t X, Xp, Y, Yp, DX, DY;
    uint8_t T[16][16]; // Tableau pour comptabiliser les occurrences
    for (i = 0; i < 16; ++i)
    {
      for (j = 0; j < 16; ++j)
      {
        T[i][j] = 0;
      }
    }

    printf("\n Creating XOR differential table:\n");
    for (uint8_t i = 0; i < 16; i++)
    {
      X = i;
      for (uint8_t j = 0; j < 16; j++)
      {
        Xp = j;
        DX = X ^ Xp;
        Y = S[(int)X];
        Yp = S[(int)Xp];
        DY = Y ^ Yp;
        T[(int)DX][(int)DY]++;
      }
    }
    for (i = 0; i < 16; ++i)
    {
      printf("[");
      for (j = 0; j < 16; ++j)
      {
        printf(" %u ", T[i][j]);
      }
      printf("]\n");
    }

    printf("\n Displaying most probable differentials:\n");

    /* TODO */
    /* Identifier les différentielles apparaissant avec plus forte probabilité */
    /* Elles seront exploitées dans la suite de l'attaque */
    int occur_max;
    occur_max = T[1][1];
    for (int i = 1; i < 16; i++)
    {
      for (int j = 1; j < 16; j++)
      {
        if (T[i][j] > occur_max)
          occur_max = T[i][j];
      }
    }
    std::vector<std::pair<uint8_t, uint8_t>> diff;
    for (int i = 1; i < 16; i++)
    {
      for (int j = 1; j < 16; j++)
      {
        if (T[i][j] == occur_max)
        {
          diff.push_back(std::make_pair((uint8_t)i, (uint8_t)j));
        }
      }
    }

    // affichage couple (DX,DY)
    for (std::pair<uint8_t, uint8_t> element : diff)
    {
      std::cout << "(DX :" << (int)element.first << ";DY :" << (int)element.second << ")" << std::endl;
    }
  }

  void genCharData(uint8_t diffIn, uint8_t diffOut)
  {
    printf("\n Generating possible intermediate values based on differential (%x --> %x):\n", diffIn, diffOut);
    std::vector<std::pair<uint8_t, uint8_t>> Couples_X_Xp;
    uint8_t X, Xp, Y, Yp, DX, DY;
    for (uint8_t i = 0; i < 16; i++)
    {
      X = i;
      for (uint8_t j = 0; j < 16; j++)
      {
        Xp = j;
        DX = X ^ Xp;
        if (DX == diffIn)
        {
          Y = S[(int)X];
          Yp = S[(int)Xp];
          DY = Y ^ Yp;
          if (diffOut == DY)
          {
            Couples_X_Xp.push_back(std::make_pair(X, Xp));
          }
        }
      }
    }
    std::cout << "-----------------------------------" << std::endl;
    for (std::pair<uint8_t, uint8_t> element : Couples_X_Xp)
    {
      std::cout << "(X :" << (int)element.first << ";Xp :" << (int)element.second << ")" << std::endl;
    }
    // TODO
  }

  void genPairs(Cipher x, uint8_t diffIn, int nbPairs)
  {
    printf("\n Generating %i known pairs with input differential of %x.\n", nbPairs, diffIn);
    for (int i = 0; i < nbPairs; i++)
    {
      uint8_t P0 = rand() % 16;
      uint8_t P1 = P0 ^ diffIn;
      uint8_t C0 = x.encrypt(P0);
      uint8_t C1 = x.encrypt(P1);
      knownP0[i] = P0;
      knownP1[i] = P1;
      knownC0[i] = C0;
      knownC1[i] = C1;
    }
  }

  void findGoodPair(uint8_t diffOut, int nbPairs)
  {
    printf("\n Searching for good pair:\n");
    bool trouver = false;
    int i = 0;
    while (!trouver && i < nbPairs)
    {
      if ((knownC0[i] ^ knownC1[i] )== diffOut)
      {
        goodP0 = knownP0[i];
        goodP1 = knownP1[i];
        goodC0 = knownC0[i];
        goodC1 = knownC1[i];
        trouver = true;
      }
      i++;
    }

    if (!trouver)
    {
      printf(" No good pair found!\n");
    }
    else
    {
      std::cout << "P0 : " << (int)goodP0 << ";P1: " << (int)goodP1 << ";C0: " << (int)goodC0 << ";C1: " << (int)goodC1 << std::endl;
    }
  }

  int testKey(int testK1, int nbPairs)
  {
    int count = 0;

    for (int i = 0; i < nbPairs; i++)
    {
        uint8_t decrypted0 = S_inv[testK1 ^ knownC0[i]];
        uint8_t decrypted1 = S_inv[testK1 ^ knownC1[i]];

        if ((decrypted0 ^ decrypted1) == (knownP0[i] ^ knownP1[i]))
        {
            count++;
        }
    }

    return count;
    
  }
  void crack(int nbPairs)
  {
    printf("\nBrute forcing reduced keyspace:\n");
    uint8_t bestKey0 = -1;
    uint8_t bestKey1 = -1;
    int bestCount = -1;

    for (uint8_t testK1 = 0; testK1 < 16; testK1++)
    {
        int count = testKey( testK1, nbPairs);

        if (count > bestCount)
        {
            bestKey1 = testK1;
            bestCount = count;
        }
    }
    bestKey0 = S_inv[ goodC0 ^ bestKey1] ^ goodP0;
    printf("Best potential key: k0 = %x, k1 = %x\n", bestKey0, bestKey1);

  }
};

//////////////////////////////////////////////////////////////////
//                             MAIN                             //
//////////////////////////////////////////////////////////////////

int main()
{

  srand(time(NULL)); // Randomize values per run
  Cipher cipher;
  uint8_t message = rand() % 16;
  printf(" Producing a random message : %x\n", message);
  uint8_t ciphertext = cipher.encrypt(message);
  printf(" Encrypted message : %x\n", ciphertext);
  uint8_t plaintext = cipher.decrypt(ciphertext);
  printf(" Decrypted message : %x\n", plaintext);

  if (message == plaintext)
    printf(" --> Success\n");
  else
    printf(" --> Failure\n");

  int nbPairs = 6; // Define number of known pairs (note that 16 is a brut force)
  uint8_t diffIn = 4;
  uint8_t diffOut = 7;

  Cryptanalysis cryptanalysis;
  cryptanalysis.findBestDiffs();                   // Find some good differentials in the S-Boxes
  cryptanalysis.genCharData(diffIn, diffOut);      // Find inputs that lead a certain characteristic
  cryptanalysis.genPairs(cipher, diffIn, nbPairs); // Generate chosen-plaintext pairs
  cryptanalysis.findGoodPair(diffOut, nbPairs);    // Choose a known pair that satisfies the characteristic
  cryptanalysis.crack(nbPairs);                                                                    //Use charData and "good pair" in find key

  return 0;
}
