#include <iostream>
#include <string>

void solveTestCase();

int main()
{
    std::ios::sync_with_stdio(false);
    unsigned int testCases = 0;
    std::cin >> testCases;
    for (unsigned int i = 0; i < testCases; ++i)
    {
        solveTestCase();
    }
}

void solveTestCase()
{
    long long testCase;
    std::cin >> testCase;
    auto result = compute(testCase);
    std::cout << result << "\n";
}
