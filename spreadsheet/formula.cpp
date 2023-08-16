#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression) : ast_(ParseFormulaAST(expression)) {
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        try
        {
            return ast_.Execute(sheet);
        }
        catch (const FormulaError& error) {
            return error;
        }
       
    }

    std::string GetExpression() const override {
        std::ostringstream s;
        ast_.PrintFormula(s);
        return s.str();
    }

    FormulaAST& GetFormulaAST() {
        return ast_;
    };

    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> result;
        auto list = ast_.GetCells();
        for (auto& value : list) {
            if (result.empty()) {
                result.push_back(value);
            }
            else if (!(result.back() == value)) {
                result.push_back(value);
            }
        }
        return result;
    };

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try
    {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (const std::exception& exception)
    {
        throw FormulaException("");
    }
    //return std::make_unique<Formula>(std::move(expression));
}