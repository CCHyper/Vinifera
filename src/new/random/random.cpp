/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RANDOM.H
 *
 *  @author        Joe L. Bostic (see notes below)
 *
 *  @contributors  CCHyper
 *
 *  @brief         This implements the random number generators.
 *
 *  @license       Vinifera is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 Vinifera is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 *  @note          This file contains heavily modified code from the source code
 *                 released by Electronic Arts for the C&C Remastered Collection
 *                 under the GPL3 license. Source:
 *                 https://github.com/ElectronicArts/CnC_Remastered_Collection
 *
 ******************************************************************************/
#include "random.h"


/**
 *  Constructor for the random number class.
 */
RandomClass::RandomClass(unsigned seed) :
    Seed(seed)
{
}


/**
 *  
 */
int RandomClass::operator()()
{
    // Transform the seed value into the next number in the sequence.
    Seed = (Seed * MULT_CONSTANT) + ADD_CONSTANT;

    // Extract the 'random' bits from the seed and return that value as the random number result.
    return (Seed >> THROW_AWAY_BITS) & (~((~0) << SIGNIFICANT_BITS));
}


/**
 *  Constructor for the random 2 class.
 */
Random2Class::Random2Class(unsigned seed) :
    Index1(0),
    Index2(103),
    Table()
{
    Random3Class random(seed);

    for (int index = 0; index < ARRAY_SIZE(Table); ++index) {
        Table[index] = random;
    }
}


/**
 *  Randomizer function that returns value.
 */
int Random2Class::operator()()
{
    Table[Index1] ^= Table[Index2];
    int val = Table[Index1];

    ++Index1;
    ++Index2;

    if (Index1 >= ARRAY_SIZE(Table)) {
        Index1 = 0;
    }
    if (Index2 >= ARRAY_SIZE(Table)) {
        Index2 = 0;
    }

    return val;
}


/**
 *  Constructor for the random 3 class.
 */
Random3Class::Random3Class(unsigned seed1, unsigned seed2) :
    Seed(seed1),
    Index(seed2)
{
}


/**
 *  Randomizer function that returns value.
 */
int Random3Class::operator() ()
{
    /*
     * This is the seed table for the Random3Class generator. These ensure
     * that the algorithm is not vulnerable to being primed with a weak seed
     * and thus prevents the algorithm from breaking down as a result.
     */
    static unsigned _mix1[20] = {
        0x0baa96887, 0x01e17d32c, 0x003bcdc3c, 0x00f33d1b2,
        0x076a6491d, 0x0c570d85d, 0x0e382b1e3, 0x078db4362,
        0x07439a9d4, 0x09cea8ac5, 0x089537c5c, 0x02588f55d,
        0x0415b5e1d, 0x0216e3d95, 0x085c662e7, 0x05e8ab368,
        0x03ea5cc8c, 0x0d26a0f74, 0x0f3a9222b, 0x048aad7e4
    };

    static unsigned _mix2[20] = {
        0x04b0f3b58, 0x0e874f0c3, 0x06955c5a6, 0x055a7ca46,
        0x04d9a9d86, 0x0fe28a195, 0x0b1ca7865, 0x06b235751,
        0x09a997a61, 0x0aa6e95c8, 0x0aaa98ee1, 0x05af9154c,
        0x0fc8e2263, 0x0390f5e8c, 0x058ffd802, 0x0ac0a5eba,
        0x0ac4874f6, 0x0a9df0913, 0x086be4c74, 0x0ed2c123b
    };

    int loword = Seed;
    int hiword = Index++;
    for (int i = 0; i < 4; i++) { // loop limit can be 2..20, we choose 4.
        int hihold = hiword; // save hiword for later.
        int temp = hihold ^ _mix1[i]; // mix up bits of hiword.
        int itmpl = temp & 0xffff; // decompose to hi & lo.
        int itmph = temp >> 16; // 16-bit words.
        temp = itmpl * itmpl + ~(itmph * itmph); // do a multiplicative mix.
        temp = (temp >> 16) | (temp << 16); // swap hi and lo halves.
        hiword = loword ^ ((temp ^ _mix2[i]) + itmpl * itmph); // loword mix.
        loword = hihold; // old hiword is loword.
    }
    return hiword;
}
