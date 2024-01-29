//
//  TP7_Shamir's Secret Sharing Scheme
//

#include <stdio.h>
#include <iostream>
#include <gmp.h>

#define BITSTRENGTH 14 /* size of prime number (p) in bits */
#define DEBUG true

void cleanup(mpz_t *array, int size)
{
    for (int i = 0; i < size; ++i)
    {
        mpz_clear(array[i]);
    }
}
void generateRandomCoefficients(mpz_t coefficients[], int numCoefficients, const mpz_t p, gmp_randstate_t state)
{

    for (int i = 0; i < numCoefficients; ++i)
    {
        mpz_init(coefficients[i]);

        // Generate a random number within the range [1, p-1]
        mpz_urandomm(coefficients[i], state, p);
        mpz_add_ui(coefficients[i], coefficients[i], 1); // Adjust the range to [1, p-1]
    }
}
void computeShares(mpz_t *x, mpz_t *y, const mpz_t S, const mpz_t a[], int n, int degree, mpz_t p)
{
    mpz_t Somme, term;

    // Initialize arrays
    for (int i = 0; i < n; ++i)
    {
        mpz_init(x[i]);
        mpz_init(y[i]);
    }

    // Compute shares for each user
    for (int i = 0; i < n; ++i)
    {
        mpz_init(Somme);
        mpz_init(term);

        // Set login value x[i]
        mpz_set_ui(x[i], i + 1);

        // Compute shares
        mpz_set(Somme, S);

        // Compute the polynomial term
        mpz_set_ui(term, 0);
        for (int j = 0; j < degree; ++j)
        {
            mpz_t temp;
            mpz_init(temp);
            mpz_pow_ui(temp, x[i], j + 1);
            // term = term * (x[i] - j)
            mpz_mul(temp, temp, a[j]);
            mpz_add(term, term, temp);

            mpz_clear(temp);
        }

        // Add the polynomial term to the share
        mpz_add(y[i], Somme, term);
        mpz_mod(y[i], y[i], p);

        // Clear temporary variables
        mpz_clear(Somme);
        mpz_clear(term);
    }
}
void copyTable(mpz_t *source, mpz_t *destination, int sourceSize, int destinationSize)
{
    // Vérification que la taille de la destination est plus grande ou égale à la taille de la source
    if (destinationSize < sourceSize)
    {
        std::cout << "Erreur : La taille de destination doit être plus grande ou égale à la taille de la source." << std::endl;
        return;
    }

    // Copie des éléments de la source vers la destination
    for (int i = 0; i < sourceSize; ++i)
    {
        mpz_init_set(destination[i], source[i]);
    }

    mpz_init_set_ui(destination[destinationSize - 1], 0);
}
void increment(mpz_t number)
{
    mpz_add_ui(number, number, 1);
}
void addUser(mpz_t **x, mpz_t **y, int new_n, const mpz_t S, const mpz_t a[], int degree, mpz_t p)
{
    mpz_t Somme, term;
    mpz_init(Somme);
    mpz_init(term);

    // Allocate memory for new arrays
    mpz_t *new_y = new mpz_t[new_n];
    mpz_t *new_x = new mpz_t[new_n];

    // Copy old values to new arrays
    copyTable(*x, new_x, new_n - 1, new_n);
    copyTable(*y, new_y, new_n - 1, new_n);
    increment(new_x[new_n - 2]);
    mpz_init_set(new_x[new_n - 1], (*x)[new_n - 2]);

    mpz_set(Somme, S);

    // Compute the polynomial term
    mpz_set_ui(term, 0);
    for (int j = 0; j < degree; ++j)
    {
        mpz_t temp;
        mpz_init(temp);
        mpz_pow_ui(temp, new_x[new_n - 1], j + 1);

        mpz_mul(temp, temp, a[j]);
        mpz_add(term, term, temp);

        mpz_clear(temp);
    }

    // Add the polynomial term to the share
    mpz_add(new_y[new_n - 1], Somme, term);
    //mpz_mod(new_y[new_n - 1], new_y[new_n - 1], p);

    // Clear temporary variables
    mpz_clear(Somme);
    mpz_clear(term);

    // Print new values
    char x_str[1000], y_str[1000];
    mpz_get_str(x_str, 10, new_x[new_n - 1]);
    mpz_get_str(y_str, 10, new_y[new_n - 1]);
    std::cout << "new X : " << x_str << ", new Y : " << y_str << std::endl;
    
    // Deallocate memory for old arrays
    for (int i = 0; i < new_n - 1; ++i)
    {
        mpz_clear((*x)[i]);
        mpz_clear((*y)[i]);
    }

    // Assign new arrays to x and y
    delete[] *x;
    delete[] *y;
    *x = new_x;
    *y = new_y;
}
// Function to compute Lagrange interpolation for secret reconstruction
void lagrangeInterpolation(mpz_t result, const mpz_t x[], const mpz_t y[], int k, const mpz_t p)
{
    mpz_t temp, term, alpha;
    mpz_init(temp);
    mpz_init(alpha);
    mpz_init(term);
    mpz_set_ui(result, 0);

    for (int i = 0; i < k; ++i)
    {
        mpz_set_ui(temp, 1);
        mpz_init_set_str(alpha, "1", 0);
        for (int j = 0; j < k; ++j)
        {
            if (i != j)
            {
                mpz_t temp;
                mpz_init(temp);
                // xi -xj
                mpz_sub(temp, x[j], x[i]);
                // 1/(xi-xj)
                mpz_invert(temp, temp, p);
                // xj/(xi-xj)
                mpz_mul(temp, x[j], temp);
                // multiply with the previous alpha_i
                mpz_mul(alpha, alpha, temp);
                // we are within Z/pZ
                mpz_mod(alpha, alpha, p);
            }
        }
        /* reconstruction of the secret */

        mpz_mul(term, alpha, y[i]); // Compute alpha_i * y_i
        mpz_add(result, result, term);
        mpz_mod(result, result, p); // Calculate the sum modulo prime
    }

    mpz_clear(temp);
    mpz_clear(term);
}
/* Main subroutine */
int main()
{
    /* Declare variables */
    int n, k;
    std::cout << "Enter the number of users (n): ";
    //std::cin >> n;
    std::cout << "Enter the threshold (k): ";
    //std::cin >> k;
    n = 5; 
    k = 4;
    while (k > n)
    {
        std::cout << "Enter a valid threshold (k < n) : ";
        std::cin >> k;
    }
    mpz_t p;  // Prime number
    mpz_t S;  // Secret
    mpz_t Sr; // Reconstruction of the Secret

    mpz_t a[k - 1]; // Coefficients of polynom

    gmp_randstate_t state;
    // Set a random seed for better randomness
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    /* This function creates the shares computation. The basic algorithm is...
     *
     *  1. Initialize Prime Number : we work into Z/pZ
     *  2. Initialize Secret Number : S
     *  3. Compute a random polynom of order k-1
     *  4. Shares computation for each users (xi, yi) for i in [1,n]
     *  5. Reconstruct the secret with k users or more
     *
     */

    /*
     *  Step 1: Initialize Prime Number : we work into Z/pZ
     */

    mpz_init(p);

    // Generate a random number of the specified bit length
    mpz_urandomb(p, state, BITSTRENGTH);

    // Make sure the number is odd
    mpz_setbit(p, 0);

    // Use a probabilistic primality test to find a prime number
    mpz_nextprime(p, p);

    if (DEBUG)
    {
        char p_str[1000];
        mpz_get_str(p_str, 10, p);
        std::cout << "Random Prime 'p' = " << p_str << std::endl;
    }

    /*
     *  Step 2: Initialize Secret Number
     */

    mpz_init(S);

    // Generate a random number within the range [1, prime-1] (in Z/pZ)
    mpz_urandomm(S, state, p);

    if (DEBUG)
    {
        char S_str[1000];
        mpz_get_str(S_str, 10, S);
        std::cout << "Random Secret 'S' = " << S_str << std::endl;
    }

    /*
     *  Step 3: Initialize Coefficient of polynomial
     */
    generateRandomCoefficients(a, k - 1, p, state);
    /*
    mpz_init(a1);

    // Generate a random number within the range [1, prime-1]
    mpz_urandomm(a1, state, p);

    mpz_init(a2);

    // Generate a random number within the range [1, prime-1]
    mpz_urandomm(a2, state, p);*/
    if (DEBUG)
    {
        for (int i = 0; i < k - 1; ++i)
        {
            char coeff_str[1000];
            mpz_get_str(coeff_str, 10, a[i]);
            std::cout << "X^" << i + 1 << "*" << coeff_str << std::endl;
        }
    }

    /*
     *  Step 4: Shares computation for each users (xi, yi)
     */
    mpz_t *x = new mpz_t[n]; // Array of login values
    mpz_t *y = new mpz_t[n]; // Array of shares

    computeShares(x, y, S, a, n, k - 1, p);

    if (DEBUG)
    {
        for (int i = 0; i < n; ++i)
        {
            char x_str[1000], y_str[1000];
            mpz_get_str(x_str, 10, x[i]);
            mpz_get_str(y_str, 10, y[i]);
            std::cout << "User " << i + 1 << ": x = " << x_str << ", y = " << y_str << std::endl;
        }
    }

    /*
     *  Step 5: Sample for reconstruct the secret with 3 users (x1, x2, x3)
     */
    mpz_t alpha[k]; // Array of Lagrangian polynomial coefficients
    mpz_t secret;   // Reconstructed secret
    mpz_init(secret);
    lagrangeInterpolation(secret, x, y, k, p);

    if (DEBUG)
    {
        char Sr_str[1000];
        mpz_get_str(Sr_str, 10, secret);
        std::cout << "Reconstruction of the secret : S = " << Sr_str << std::endl;
    }
    char choice;
    bool run = true;
    while (run)
    {
        std::cout << "Do you want to increment n, k, both, or exit? (n/k/b/e)";
        std::cin >> choice;

        switch (choice)
        {
        case 'n':
            n++;
            addUser(&x, &y, n, S, a, k - 1, p);
            break;
        case 'k':
            std::cout << "Enter new value for k: ";
            std::cin >> k;
            break;
        case 'b':
            std::cout << "Enter new value for n: ";
            n++;
            addUser(&x, &y, n, S, a, k - 1, p);
            std::cout << "Enter new value for k: ";
            std::cin >> k;
            break;
        case 'e':
            run = false;
            break;
        default:
            std::cout << "Invalid choice. Nothing will be changed." << std::endl;
            break;
        };
    };
    /* Clean up the GMP integers */
    // Clean up initialized login values
    cleanup(x, n);
    cleanup(y, n);
    cleanup(a, k);
    mpz_clear(S);
    mpz_clear(p);
    gmp_randclear(state);
}
