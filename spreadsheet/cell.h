#pragma once

#include "common.h"
#include "formula.h"


#include <optional>
#include <forward_list>
#include <set>

class Cell : public CellInterface {
public:
    Cell() = default;
    Cell(SheetInterface& sheet, Position pos);
    ~Cell() = default;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    std::unique_ptr<Impl> impl_;

    mutable std::optional<double> cache_;
    SheetInterface& sheet_;
    Position pos_;

    void UpdateCache() const;
    void CacheInvalidation(const Cell* cell);

    bool CyclicDependencies(Position pos, std::set<Position>& visited, CellInterface* Sheet);

    // Добавьте поля и методы для связи с таблицей, проверки циклических 
    // зависимостей, графа зависимостей и т. д.

    class Impl {
    public:
        Impl() = default;
        virtual ~Impl() = default;

        virtual std::string GetText() const = 0;
        virtual CellInterface::Value GetValue(const SheetInterface& sheet, std::optional<double>& cache) const = 0;
        virtual std::string GetElement() const = 0;
        virtual void Clear() = 0;

        virtual void UpdateCache(std::optional<double>& cache, double value = 0.0) const = 0;
        virtual void CacheInvalidation(std::optional<double>& cache) = 0;
    };

    class EmptyImpl : public Impl {
    public:
        EmptyImpl(std::string text);

        std::string GetText() const override;

        CellInterface::Value GetValue(const SheetInterface& sheet_, std::optional<double>& cache) const override;

        std::string GetElement() const;

        void Clear() override;

        void UpdateCache(std::optional<double>& cache, double value) const override {};

        void CacheInvalidation(std::optional<double>& cache) override {};

    private:
        std::string text_;
    };

    class TextImpl : public Impl {
    public:
        TextImpl(std::string text);

        std::string GetText() const override;

        CellInterface::Value GetValue(const SheetInterface& sheet_, std::optional<double>& cache) const override;

        std::string GetElement() const;

        void Clear() override;

        void UpdateCache(std::optional<double>& cache, double value) const override {};
        void CacheInvalidation(std::optional<double>& cache) override {};

    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::string text);

        std::string GetText() const override;

        CellInterface::Value GetValue(const SheetInterface& sheet_, std::optional<double>& cache) const override;

        std::string GetElement() const;

        void Clear() override;

        void UpdateCache(std::optional<double>& cache, double value) const override;
        void CacheInvalidation(std::optional<double>& cache) override;

    private:
        std::string formula_;
    };
};

