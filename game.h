#include <memory>
#include <stdexcept>
#include <vector>

class Cell {
public:
    Cell(size_t row, size_t column) : row(row), column(column) {}
    virtual ~Cell() = default;

    size_t get_neighbor_count(const std::vector<std::vector<size_t>>& grid) const
    {
        size_t rows    = grid.size();
        size_t columns = grid[0].size();

        size_t above = 0;
        size_t below = 0;
        for (int i = -1; i < 2; ++i) {
            size_t col = (column + i + columns) % columns;
            above += grid[(row - 1 + rows) % rows][col];
            below += grid[(row + 1 + rows) % rows][col];
        }
        size_t left  = grid[row][(column - 1 + columns) % columns];
        size_t right = grid[row][(column + 1 + columns) % columns];

        return above + below + left + right;
    }

    virtual bool will_stay_alive(const std::vector<std::vector<size_t>>& grid) const = 0;

protected:
    size_t row;
    size_t column;
};

class Critter final : public Cell {
public:
    using Cell::Cell;

    bool will_stay_alive(const std::vector<std::vector<size_t>>& grid) const final
    {
        auto neighbors = get_neighbor_count(grid);
        return 1 < neighbors && neighbors < 4;
    }
};

class Space final : public Cell {
public:
    using Cell::Cell;

    bool will_stay_alive(const std::vector<std::vector<size_t>>& grid) const final
    {
        return get_neighbor_count(grid) == 3;
    }
};

class Immortal final : public Cell {
public:
    using Cell::Cell;

    bool will_stay_alive(const std::vector<std::vector<size_t>>& grid) const final { return true; }
};

template <typename T>
inline bool
is_instance_of(const std::shared_ptr<Cell>& cell_obj)
{
    return std::dynamic_pointer_cast<T>(cell_obj) != nullptr;
}

class CellFactory {
public:
    static std::shared_ptr<Cell> createCell(std::vector<std::vector<size_t>> grid, size_t row,
                                            size_t column)
    {
        switch (grid[row][column]) {
        case 0:
            return std::make_shared<Space>(row, column);
        case 1:
            return std::make_shared<Critter>(row, column);
        case 2:
            return std::make_shared<Immortal>(row, column);
        default:
            throw std::runtime_error("Invalid cell type");
        }
    }

    static size_t getNextGenerationType(std::shared_ptr<Cell> cell_obj)
    {
        if (is_instance_of<Space>(cell_obj)) {
            return 1;
        }
        if (is_instance_of<Critter>(cell_obj)) {
            return 1;
        }
        if (is_instance_of<Immortal>(cell_obj)) {
            return 2;
        }
        throw std::runtime_error("Invalid cell type");
    }
};

class Game {
public:
    Game(std::vector<std::vector<size_t>> grid) : grid(std::move(grid)) {}

    void iterate()
    {
        std::vector<std::vector<size_t>> new_grid;

        for (size_t row_num = 0; row_num < grid.size(); ++row_num) {
            std::vector<size_t> new_row;

            for (size_t col_num = 0; col_num < grid[row_num].size(); ++col_num) {
                std::shared_ptr<Cell> cell_obj = CellFactory::createCell(grid, row_num, col_num);
                if (cell_obj->will_stay_alive(grid)) {
                    new_row.push_back(CellFactory::getNextGenerationType(cell_obj));
                }
                else {
                    new_row.push_back(0);
                }
            }
            new_grid.push_back(new_row);
        }
        grid = new_grid;
    }

    std::vector<std::vector<size_t>> grid;
};
