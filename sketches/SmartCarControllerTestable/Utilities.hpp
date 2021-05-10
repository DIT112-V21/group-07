#pragma once

namespace smartcar
{
namespace utils
{

template <typename AnyNumber>
constexpr AnyNumber getAbsolute(AnyNumber number)
{
    return number < 0 ? -number : number;
}

template <typename AnyNumber>
constexpr AnyNumber getConstrain(AnyNumber number, AnyNumber min, AnyNumber max)
{
    return number < min ? min : (number > max ? max : number);
}

template <typename AnyNumber>
AnyNumber getMedian(AnyNumber unsortedNumbers[], const unsigned int& arraySize)
{
    // Sort the array using bubble sort
    for (unsigned int i = 0; i < arraySize - 1; i++)
    {
        for (unsigned int j = 0; j < arraySize - 1 - i; j++)
        {
            if (unsortedNumbers[j] > unsortedNumbers[j + 1])
            {
                auto swappedValue      = unsortedNumbers[j];
                unsortedNumbers[j]     = unsortedNumbers[j + 1];
                unsortedNumbers[j + 1] = swappedValue;
            }
        }
    }
    // Return the Median value now that the array is sorted
    return unsortedNumbers[arraySize / 2];
}

template <typename AnyNumber>
constexpr AnyNumber getMap(
    AnyNumber valueToMap, AnyNumber fromLow, AnyNumber fromHigh, AnyNumber toLow, AnyNumber toHigh)
{
    return fromHigh == fromLow
               ? toLow
               : (valueToMap - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}


constexpr bool areAlmostEqual(float a, float b)
{
    // C++11 does not allow us to declare the delta as a variable within the function body
    // NOLINTNEXTLINE(readability-magic-numbers)
    return getAbsolute(getAbsolute(a) - getAbsolute(b)) <= 0.001F;
}
} // namespace utils
} // namespace smartcar
