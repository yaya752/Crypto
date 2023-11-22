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

  uint8_t roundFunc(uint8_t input)
  {
    // TODO
    return 0;
  }

  uint8_t roundFunc_inv(uint8_t input)
  {
    // TODO
    return 1;
  }

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
      X =i;
      for (uint8_t j = 0; j < 16; j++)
      {
        Xp = j;
        DX = X ^ Xp;
        if (DX== diffIn)
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

    /* Question 2 : compléter le code afin de produire des paires de chiffrés avec la bonne différence */

    // TODO
  }

  void findGoodPair(int diffOut, int nbPairs)
  {
    printf("\n Searching for good pair:\n");

    /* Question 4 : compléter le code afin de produire une paire avec la bonne caractéristique en se basant sur le chiffrement */

    // TODO
    if (true)
      printf(" No good pair found!\n");
  }

  int testKey(int testK0, int testK1, int nbPairs)
  {
    // TODO
  }

  void crack(int nbPairs)
  {
    printf("\nBrute forcing reduced keyspace:\n");

    // TODO
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
  cryptanalysis.findBestDiffs();              // Find some good differentials in the S-Boxes
  cryptanalysis.genCharData(diffIn, diffOut); // Find inputs that lead a certain characteristic
  // cryptanalysis.genPairs(cipher, diffIn, nbPairs);                                                                //Generate chosen-plaintext pairs
  // cryptanalysis.findGoodPair(diffOut,nbPairs);                                                            //Choose a known pair that satisfies the characteristic
  // cryptanalysis.crack(nbPairs);                                                                    //Use charData and "good pair" in find key

  return 0;
}
