#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <set>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    std::unique_ptr<Cell> CreateCell(const std::string& str, Position pos);

    SheetInterface& GetSheet();

    const Cell* GetConcreteCell(Position pos) const;
    Cell* GetConcreteCell(Position pos);

private:
    std::vector<std::vector<std::unique_ptr<Cell>>> cells_;

    void Resize(Position pos);

    bool IsValidPos(Position pos) const;
};