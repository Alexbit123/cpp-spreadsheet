#include "cell.h"
#include "FormulaAST.h"
#include "sheet.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>


// Реализуйте следующие методы

Cell::Cell(SheetInterface& sheet, Position pos) : sheet_(sheet), pos_(pos) {
}

void Cell::Set(std::string text) {
	if (!text.empty()) {
		if (text[0] == FORMULA_SIGN && text.size() != 1) {
			impl_ = std::make_unique<FormulaImpl>(text);
		}
		else {
			impl_ = std::make_unique<TextImpl>(text);
		}
	}
	else {
		impl_ = std::make_unique<EmptyImpl>(text);
	}
	if (!GetReferencedCells().empty()) {
		for (Position pos : GetReferencedCells()) {
			if (sheet_.GetCell(pos) == nullptr) {
				sheet_.SetCell(pos, "");
			}
		}
	}
	std::set<Position> visited;
	CyclicDependencies(pos_, visited, this);
	if (cache_.has_value()) {
		const Sheet& sheet = dynamic_cast<const Sheet&>(sheet_);
		CacheInvalidation(sheet.GetConcreteCell(pos_));
	}
}

void Cell::Clear() {
	impl_->Clear();
}

Cell::Value Cell::GetValue() const {
	if (cache_.has_value()) {
		return cache_.value();
	}
	return impl_->GetValue(sheet_, cache_);
}
std::string Cell::GetText() const {
	return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
	try
	{
		if (impl_->GetText()[0] == FORMULA_SIGN && impl_->GetText().size() != 1) {
			return ParseFormula(impl_->GetElement())->GetReferencedCells();
		}
		return std::vector<Position>();
	}
	catch (const std::exception&)
	{
		return std::vector<Position>();
	}
}

void Cell::UpdateCache() const{
	impl_->UpdateCache(cache_);
}

void Cell::CacheInvalidation(const Cell* cell) {
	const Sheet& sheet = dynamic_cast<const Sheet&>(sheet_);
	cell->impl_->CacheInvalidation(cache_);
	for (size_t i = 0; i < cell->GetReferencedCells().size(); ++i) {
		sheet.GetConcreteCell(GetReferencedCells()[i])->impl_->CacheInvalidation(cache_);
	}
}

bool Cell::CyclicDependencies(Position pos, std::set<Position>& visited, CellInterface* Sheet) {
	if (visited.count(pos)) {
		//return false;
		throw CircularDependencyException("CircularDependencyException");
	}
	visited.insert(pos);
	if (sheet_.GetCell(pos) == nullptr) {
		return true;
	}
	std::vector<Position> position = Sheet->GetReferencedCells();
	
	for (size_t i = 0; i < position.size(); ++i) {
		CyclicDependencies(position[i], visited, sheet_.GetCell(position[i]));
	}
	return true;
}

Cell::EmptyImpl::EmptyImpl(std::string text) : text_(text){
}

std::string Cell::EmptyImpl::GetText() const {
	return text_;
}

CellInterface::Value Cell::EmptyImpl::GetValue(const SheetInterface& sheet, std::optional<double>& cache) const {
	return text_;
}

std::string Cell::EmptyImpl::GetElement() const {
	return text_;
}

void Cell::EmptyImpl::Clear() {
	text_ = "";
}

Cell::TextImpl::TextImpl(std::string text) : text_(text) {
}

std::string Cell::TextImpl::GetText() const {
	return text_;
}

CellInterface::Value Cell::TextImpl::GetValue(const SheetInterface& sheet, std::optional<double>& cache) const {
	if (text_[0] == ESCAPE_SIGN) {
		std::string buf(text_.begin() + 1, text_.end());
		return buf;
	}
	return text_;
}

std::string Cell::TextImpl::GetElement() const {
	return text_;
}

void Cell::TextImpl::Clear() {
	text_ = "";
}

Cell::FormulaImpl::FormulaImpl(std::string text) {
	std::string buf(text.begin() + 1, text.end());
	formula_ = ParseFormula(buf)->GetExpression();
}

std::string Cell::FormulaImpl::GetText() const {
	std::string buf = "=";
	return buf += formula_;
}

CellInterface::Value Cell::FormulaImpl::GetValue(const SheetInterface& sheet, std::optional<double>& cache) const {
	switch (ParseFormula(formula_)->Evaluate(sheet).index()) {
	case 0: { 
		double value = std::get<double>(ParseFormula(formula_)->Evaluate(sheet));
		UpdateCache(cache, value);
		return std::get<double>(ParseFormula(formula_)->Evaluate(sheet)); 
	}
	default: { return std::get<FormulaError>(ParseFormula(formula_)->Evaluate(sheet)); }
	}
}

std::string Cell::FormulaImpl::GetElement() const {
	return formula_;
}

void Cell::FormulaImpl::Clear() {
	formula_ = "";
}

void Cell::FormulaImpl::UpdateCache(std::optional<double>& cache, double value) const {
	cache = value;
}

void Cell::FormulaImpl::CacheInvalidation(std::optional<double>& cache) {
	cache = std::nullopt;
}
