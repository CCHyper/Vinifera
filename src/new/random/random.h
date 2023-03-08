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
#pragma once

#include "always.h"


/**
 *  Picks a random number between two values (inclusive).
 *  #WARNING: Generator class must have SIGNIFICANT_BITS exposed as public!
 */
template<class T>
int Pick_Random_Number(T &generator, int minval, int maxval)
{
    /**
     *  Test for shortcut case where the range is null and thus
     *  the number to return is actually implicit from the
     *  parameters.
     */
    if (minval == maxval) {
        return minval;
    }

    /**
     *  Ensure that the min and max range values are in proper order.
     */
    if (minval > maxval) {
        int temp = minval;
        minval = maxval;
        maxval = temp;
    }

    /**
     *  Find the highest bit that fits within the magnitude of the
     *  range of random numbers desired. Notice that the scan is
     *  limited to the range of significant bits returned by the
     *  random number algorithm.
     */
    int magnitude = maxval - minval;
    int highbit = T::SIGNIFICANT_BITS - 1;
    while ((magnitude & (1 << highbit)) == 0 && highbit > 0) {
        --highbit;
    }

    /**
     *  Create a full bit mask pattern that has all bits set that just
     *  barely covers the magnitude of the number range desired.
     */
    int mask = ~((~0L) << (highbit + 1));

    /**
     *  Keep picking random numbers until it fits within the magnitude desired. With a
     *  good random number generator, it will have to perform this loop an average
     *  of one and a half times.
     */
    int pick = magnitude + 1;
    while (pick > magnitude) {
        pick = generator() & mask;
    }

    /**
     *  Finally, bias the random number pick to the start of the range
     *  requested.
     */
    return pick + minval;
}


/*******************************************************************************
 *  @class   RandomClass
 *
 *  @brief   This class functions like a 'magic' int value that returns a random
 *           number every time it is read. To set the "random seed" for this, just
 *           assign a number to the object (just as you would if it were a true
 *           'int' value). Take note that although this class will return an 'int',
 *           the actual significance of the random number is limited to 15 bits (0..32767).
 */
class RandomClass
{
    public:
        RandomClass(unsigned seed = 0);

        operator int() { return operator()(); }
        int operator()();

        // Generates a random number between two values.
        int operator()(int minval, int maxval) { return Pick_Random_Number(*this, minval, maxval); }
        template<typename T> T operator()(T minval, T maxval) { return T(*this)(int(minval), int(maxval)); }

        enum {
            SIGNIFICANT_BITS = 15 // Random number bit significance.
        };

    protected:
        unsigned long Seed;

    private:
        enum {
            MULT_CONSTANT = 0x41C64E6D, // K multiplier value.
            ADD_CONSTANT = 0x00003039, // K additive value.
            THROW_AWAY_BITS = 10 // Low bits to throw away.
        };
};


/*******************************************************************************
 *  @class   Random2Class
 *
 *  @brief   This class functions like a 'magic' number where it returns a different
 *           value every time it is read. It is nearly identical in function to the
 *           RandomClass, but has the following improvements;
 *
 *             1) It generates random numbers very quickly. No multiplies are
 *                used in the algorithm.
 * 
 *                The return value is a full 32 bits rather than 15 bits of
 *                the RandomClass.
 * 
 *                The bit pattern won't ever repeat. (actually it will repeat
 *                in about 10 to the 50th power times).
 */
class Random2Class
{
    public:
        Random2Class(unsigned seed = 0);

        operator int() { return operator()(); }
        int operator()();

        // Generates a random number between two values.
        int operator()(int minval, int maxval) { return Pick_Random_Number(*this, minval, maxval); }
        template<typename T> T operator()(T minval, T maxval) { return T(*this)(int(minval), int(maxval)); }

        enum {
            SIGNIFICANT_BITS = 32 // Random number bit significance.
        };

    protected:
        int Index1;
        int Index2;
        int Table[250];
};


/*******************************************************************************
 *  @class   Random3Class
 *
 *  @brief   This class functions like a 'magic' number where it returns a different
 *           value every time it is read. It is nearly identical in function to the
 *           RandomClass and Random2Class, but has the following improvements.
 *
 *             1) The random number returned is very strongly random. Approaching
 *                cryptographic  quality.
 * 
 *             2) The return value is a full 32 bits rather than 15 bits of
 *                the RandomClass.
 * 
 *             3) The bit pattern won't repeat until 2^32 times.
 *           
 * @note:    This code used in this randomizer is adapted from the book "Inner Loops" by Rick Booth.
 *           
 * @warning: Do not use for cryptography. This random number generator is good for numerical
 *           simulation. Optimized, it's 4 times faster than rand().
 *           http://www.math.keio.ac.jp/~matumoto/emt.html
 */
class Random3Class
{
    public:
        Random3Class(unsigned seed1 = 0, unsigned seed2 = 0);

        operator int() { return operator()(); }
        int operator()();

        // Generates a random number between two values.
        int operator()(int minval, int maxval) { return Pick_Random_Number(*this, minval, maxval); }
        template<typename T> T operator()(T minval, T maxval) { return T(*this)(int(minval), int(maxval)); }

        enum {
            SIGNIFICANT_BITS = 32 // Random number bit significance.
        };

    protected:
        int Seed;
        int Index;

    private:
        static unsigned Mix1[20];
        static unsigned Mix2[20];
};
