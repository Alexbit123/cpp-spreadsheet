#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h"

#include <optional>
#include <forward_list>
#include <set>

class Cell : public CellInterface {
public:
    Cell() = default;
    Cell(const Sheet& sheet, Position pos) : sheet_(sheet), pos_(pos) {};
    ~Cell() = default;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells(std::string& str) const;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    std::unique_ptr<Impl> impl_;

    mutable std::optional<double> cache_;
    std::forward_list<Position> cells_;
    const Sheet& sheet_;
    Position pos_;

    void UpdateCache() const;
    void CacheInvalidation(const Cell* cell);

    bool CyclicDependencies(std::string& text, std::set<Position>& visited);

    // Добавьте поля и методы для связи с таблицей, проверки циклических 
    // зависимостей, графа зависимостей и т. д.

    class Impl {
    public:
        Impl() = default;
        virtual ~Impl() = default;

        virtual std::string GetText() const = 0;
        virtual CellInterface::Value GetValue(const SheetInterface& sheet, std::optional<double>& cache) const = 0;
        virtual void Clear() = 0;

        virtual void UpdateCache(std::optional<double>& cache, double value = 0.0) const = 0;
        virtual void CacheInvalidation(std::optional<double>& cache) = 0;
    };

    class EmptyImpl : public Impl {
    public:
        EmptyImpl(std::string text);

        std::string GetText() const override;

        CellInterface::Value GetValue(const SheetInterface& sheet_, std::optional<double>& cache) const override;
        void Clear() override;

    private:
        std::string text_;
    };

    class TextImpl : public Impl {
    public:
        TextImpl(std::string text);

        std::string GetText() const override;

        CellInterface::Value GetValue(const SheetInterface& sheet_, std::optional<double>& cache) const override;

        void Clear() override;

    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::string text);

        std::string GetText() const override;

        CellInterface::Value GetValue(const SheetInterface& sheet_, std::optional<double>& cache) const override;

        void Clear() override;

        void UpdateCache(std::optional<double>& cache, double value) const override;
        void CacheInvalidation(std::optional<double>& cache) override;

    private:
        std::string formula_;
    };
};

