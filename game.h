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
        throw std::runtime_error("Space cell is always dead");
    }

    bool will_be_born(const std::vector<std::vector<size_t>>& grid) const
    {
        return get_neighbor_count(grid) == 3;
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
                size_t cell = grid[row_num][col_num];
                if (cell) {
                    Critter cell_obj(row_num, col_num);
                    new_row.push_back(cell_obj.will_stay_alive(grid));
                }
                else {
                    Space cell_obj(row_num, col_num);
                    new_row.push_back(cell_obj.will_be_born(grid));
                }
            }
            new_grid.push_back(new_row);
        }
        grid = new_grid;
    }

    const std::vector<std::vector<size_t>>& get_grid() const { return grid; }

private:
    std::vector<std::vector<size_t>> grid;
};
