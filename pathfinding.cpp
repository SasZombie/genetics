#include <iostream>
#include <vector>
#include <array>
#include <random>

constexpr size_t POPULATION_SIZE = 25;
constexpr size_t ARRAY_LEN = 5;
constexpr size_t GENERATIONS = 10000;
constexpr double MUTATION_RATE = 0.5;

enum Road
{
    T = -2,
    D = 1,
    G = 100
};

using matrix = std::array<std::array<Road, ARRAY_LEN>, ARRAY_LEN>;

using chromosome = std::vector<std::pair<size_t, size_t>>;

constexpr matrix board = {{
    {D, D, D, D, T},
    {D, T, T, D, T},
    {D, T, D, D, D},
    {D, T, D, T, D},
    {D, D, D, T, G},
}};

enum struct Directions
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    Count
};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution mutationDist(0.0, 1.0);
std::uniform_int_distribution<size_t> codeChoices(0, static_cast<size_t>(Directions::Count) - 1);
std::uniform_int_distribution<size_t> selectionDist(0, POPULATION_SIZE - 1);

bool validateDirection(size_t row, size_t col, Directions direction)
{
    switch (direction)
    {
    case Directions::UP:
        return (row == 0);
    case Directions::DOWN:
        return (row == ARRAY_LEN - 1);
    case Directions::LEFT:
        return (col == 0);
    case Directions::RIGHT:
        return (col == ARRAY_LEN - 1);
    default:
        std::cerr << "Unreachable";
        break;
    }

    return true;
}

bool isRelatedToParent(const std::pair<size_t, size_t> &parent, const std::pair<size_t, size_t> &child)
{
    if ((child.second == parent.second) && (child.first == parent.first + 1 || child.first == parent.second - 1))
        return true;
    if ((child.second == parent.second + 1 || child.second == parent.second - 1) && (child.first == parent.first))
        return true;
    return false;
}

std::pair<size_t, size_t> choseDirection(size_t &row, size_t &col)
{
    Directions direction;
    do
    {
        direction = static_cast<Directions>(codeChoices(gen));

    } while (validateDirection(row, col, direction));

    switch (direction)
    {
    case Directions::UP:
        --row;
        break;
    case Directions::DOWN:
        ++row;
        break;
    case Directions::LEFT:
        --col;
        break;
    case Directions::RIGHT:
        ++col;
        break;
    default:
        std::cerr << "Unreachable";
        break;
    }

    return {row, col};
}

std::vector<std::pair<size_t, size_t>> randomDirection()
{
    std::vector<std::pair<size_t, size_t>> directions = {{0, 0}};

    size_t row = 0;
    size_t col = 0;

    while (board[row][col] != G)
    {
        directions.push_back(choseDirection(row, col));
    }

    return directions;
}

int fitnessFunction(const chromosome &chromo)
{
    bool visited[ARRAY_LEN][ARRAY_LEN] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    };

    int score = 0;
    for (const auto &[row, col] : chromo)
    {
        if (!visited[row][col])
        {
            score = score + board[row][col];
            visited[row][col] = 1;
        }
        else
        {
            score = score - abs(board[row][col]);
        }
    }
    return score;
}

chromosome selectParent(const std::vector<chromosome> &population)
{
    chromosome firstChild = population[selectionDist(gen)];
    chromosome seccondChild = population[selectionDist(gen)];

    if (fitnessFunction(firstChild) > fitnessFunction(seccondChild))
    {
        return firstChild;
    }

    return seccondChild;
}

chromosome crossover(const chromosome &p1, const chromosome &p2)
{
    chromosome child;
    child.push_back((mutationDist(gen) < 0.5) ? p1[0] : p2[0]);

    size_t aveLen = (p1.size() + p2.size()) / 2;

    for (size_t i = 1; i < aveLen; ++i)
    {
        if (i >= p1.size())
        {
            child.push_back(p2[i]);
        }
        else if (i >= p2.size())
        {
            child.push_back(p1[i]);
        }
        else
        {

            bool relation1 = isRelatedToParent(p1[i], child[i - 1]);
            bool relation2 = isRelatedToParent(p2[i], child[i - 1]);
            if (relation1 && relation2)
            {
                child.push_back((mutationDist(gen) < 0.5) ? p1[i] : p2[i]);
            }
            else if (!relation1)
            {
                child.push_back(p2[i]);
            }
            else
            {
                child.push_back(p1[i]);
            }
        }
    }

    return child;
}

void mutate(chromosome &child)
{
    if (mutationDist(gen) < MUTATION_RATE && child.size() > 1)
    {
        child.pop_back();
    }
    else
    {
        auto [row, col] = child.back();
        child.push_back(choseDirection(row, col));
    }
}

void printEnum(Directions input)
{
    switch (input)
    {
    case Directions::UP:
        std::cout << "↓";
        break;
    case Directions::DOWN:
        std::cout << "→";
        break;
    case Directions::LEFT:
        std::cout << "↑";
        break;
    case Directions::RIGHT:
        std::cout << "←";
        break;
    default:
        std::cerr << "Unreachable\n";
        break;
    }
}

int main()
{
    std::vector<chromosome> population;
    population.reserve(POPULATION_SIZE);

    for (size_t i = 0; i < POPULATION_SIZE; ++i)
    {
        population.emplace_back(std::vector<std::pair<size_t, size_t>>{{0, 0}});
    }

    for (size_t generation = 0; generation < GENERATIONS; ++generation)
    {
        std::vector<chromosome> newPopulation;
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

        const auto bestElem = *std::max_element(population.begin(), population.end(), [](const chromosome &p1, const chromosome &p2)
                                                { return fitnessFunction(p1) < fitnessFunction(p2); });

        std::cout << "Best code so far is: ";

        for (const auto &elem : bestElem)
        {
            std::cout << '[' << elem.first << ',' << elem.second << ']';
        }

        std::cout << " | in genreation " << generation << '\n';

        if (bestElem.back() == std::pair{4, 4})
        {
            break;
        }
    }
}