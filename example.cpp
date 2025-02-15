#include <iostream>
#include <random>
#include <array>

constexpr size_t POPULATION_SIZE = 15;
constexpr size_t ARRAY_LEN = 5;
constexpr size_t GENERATIONS = 1000;
constexpr double MUTATION_RATE = 0.2;

enum struct codeInput
{
    SQUARE,
    TRIANGLE,
    CIRCLE,
    DIAMOND,
    Count
};

// 0, 2, 1, 1, 1
constexpr std::array TARGET = {codeInput::SQUARE, codeInput::CIRCLE, codeInput::TRIANGLE, codeInput::TRIANGLE, codeInput::TRIANGLE};

using chromoArr = std::array<codeInput, ARRAY_LEN>;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution mutationDist(0.0, 1.0);
std::uniform_int_distribution<size_t> codeChoices(0, static_cast<size_t>(codeInput::Count) - 1);
std::uniform_int_distribution<size_t> selectionDist(0, POPULATION_SIZE - 1);

void printEnum(codeInput input)
{
    switch (input)
    {
    case codeInput::CIRCLE:
        std::cout << "Circle";
        break;
    case codeInput::TRIANGLE:
        std::cout << "Triangle";
        break;
    case codeInput::DIAMOND:
        std::cout << "Diamond";
        break;
    case codeInput::SQUARE:
        std::cout << "Square";
        break;
    }
}

std::array<codeInput, ARRAY_LEN> randomCode()
{
    std::array<codeInput, ARRAY_LEN> randomCode;

    for (size_t i = 0; i < ARRAY_LEN; ++i)
    {
        randomCode[i] = static_cast<codeInput>(codeChoices(gen));
    }

    return randomCode;
}

size_t fitnessFunction(const std::array<codeInput, ARRAY_LEN> &chromo)
{
    size_t score = 0;

    for (size_t i = 0; i < ARRAY_LEN; ++i)
    {
        if (chromo[i] == TARGET[i])
        {
            ++score;
        }
    }

    return score;
}

chromoArr selectParent(const std::vector<chromoArr> &population)
{
    chromoArr firstChild = population[selectionDist(gen)];
    chromoArr seccondChild = population[selectionDist(gen)];

    if (fitnessFunction(firstChild) > fitnessFunction(seccondChild))
    {
        return firstChild;
    }

    return seccondChild;
}

chromoArr crossover(const chromoArr &p1, const chromoArr &p2)
{
    chromoArr child;
    for (size_t i = 0; i < ARRAY_LEN; ++i)
    {
        child[i] = (mutationDist(gen) < 0.5) ? p1[i] : p2[i];
    }

    return child;
}

void mutate(chromoArr &child)
{
    for (size_t i = 0; i < ARRAY_LEN; ++i)
    {
        if (mutationDist(gen) < MUTATION_RATE)
        {
            child[i] = static_cast<codeInput>(codeChoices(gen));
        }
    }
}

int main()
{
    std::vector<chromoArr> population;
    population.reserve(POPULATION_SIZE);

    for (size_t i = 0; i < POPULATION_SIZE; ++i)
    {
        population.push_back(randomCode());
    }

    for (size_t generation = 0; generation < GENERATIONS; ++generation)
    {
        std::vector<chromoArr> newPopulation;
        population.reserve(POPULATION_SIZE);

        for (size_t i = 0; i < POPULATION_SIZE; ++i)
        {
            const auto p1 = selectParent(population);
            const auto p2 = selectParent(population);

            auto child = crossover(p1, p2);
            mutate(child);
            newPopulation.push_back(child);
        }

        population = newPopulation;

        const auto bestElem = *std::max_element(population.begin(), population.end(), [](const chromoArr &p1, const chromoArr &p2)
                                                { return fitnessFunction(p1) < fitnessFunction(p2); });

        std::cout << "Best code so far is: ";

        for (const auto elem : bestElem)
        {
            printEnum(elem);
            std::cout << ' ';
        }

        std::cout << " | in genreation " << generation << '\n';

        size_t similarity = 0;
        for (size_t i = 0; i < ARRAY_LEN; ++i)
        {
            if (bestElem[i] == TARGET[i])
                ++similarity;
        }

        if(similarity == ARRAY_LEN)
        {
            break;
        }
    }
}