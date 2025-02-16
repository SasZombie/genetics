#include <iostream>
#include <random>
#include <unordered_map>

struct creature
{
    int size = 1;
    size_t itersAlive = 0;
};

constexpr size_t INITIAL_POPULATION = 15;
constexpr size_t ARRAY_LEN = 5;
constexpr size_t GENERATIONS = 1000;
constexpr float DEATH_PROB = 0.25;
constexpr double MUTATION_RATE = 0.2;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution mutationDist(0.0, 1.0);

size_t fitnessFunction(const creature &chromo)
{
    return chromo.itersAlive;
}

creature selectParent(const std::vector<creature> &population)
{
    std::uniform_int_distribution<size_t> selectionDist(0, population.size() - 1);

    creature firstParent = population[selectionDist(gen)];
    creature seccondParent = population[selectionDist(gen)];

    if (fitnessFunction(firstParent) > fitnessFunction(seccondParent))
    {
        return firstParent;
    }

    return seccondParent;
}

creature crossover(const creature &p1, const creature &p2)
{
    creature c1 = p1, c2 = p2;

    c1.itersAlive = 0;
    c2.itersAlive = 0;

    return (mutationDist(gen) < 0.5) ? c1 : c2;
}

void mutate(creature &child)
{
    if (mutationDist(gen) < MUTATION_RATE)
    {
        if (mutationDist(gen) < 0.5)
        {
            child.size = child.size + 1;
        }
        else
        {
            child.size = child.size - 1;
        }
    }
}

std::vector<size_t> applyDay(const std::vector<creature> &population)
{
    std::vector<size_t> deaths;
    size_t size = population.size();
    for (size_t i = 0; i < size; ++i)
    {
        // if (mutationDist(gen) < (DEATH_PROB / (population[i].size + 1)))
        if ((mutationDist(gen) * population[i].size) < DEATH_PROB)
        {
            deaths.push_back(i);
        }
    }

    return deaths;
}


int main()
{
    std::vector<creature> population(INITIAL_POPULATION);

    for (size_t generation = 0; generation < 25; ++generation)
    {
        std::vector<creature> newPopulation;

        const auto deaths = applyDay(population);

        for (auto it = deaths.rbegin(); it != deaths.rend(); ++it)
        {
            population.erase(population.begin() + *it);
        }

        for (auto &elem : population)
        {
            elem.itersAlive = elem.itersAlive + 1;
        }


        size_t popSize = population.size();
        for (size_t i = 0; i < popSize/2; ++i)
        {
            const auto p1 = selectParent(population);
            const auto p2 = selectParent(population);

            auto child = crossover(p1, p2);
            mutate(child);
            newPopulation.push_back(child);
        }


        population.insert(population.begin(), newPopulation.begin(), newPopulation.end());

        std::unordered_map<int, size_t> freq;

        for (const auto &crt : newPopulation)
        {
            ++freq[crt.size];
        }

        for (const auto &[num, count] : freq)
        {
            std::cout << num << " appears " << count << " times\n";
        }


        std::cout << "Size of population: " << population.size() << '\n';
        std::cout << "End of " << generation << "Iter\n";
    }
}