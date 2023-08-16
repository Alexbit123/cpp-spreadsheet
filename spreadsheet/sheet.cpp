#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid Position in metod SetCell");
	}
	Resize(pos);
	cells_[pos.row][pos.col] = CreateCell(text, pos);
	/*if (!cells_[pos.row][pos.col]->GetReferencedCells().empty()) {
		for (Position pos : cells_[pos.row][pos.col]->GetReferencedCells()) {
			if (GetCell(pos) == nullptr) {
				SetCell(pos, "");
			}
		}
	}*/
	
}

const CellInterface* Sheet::GetCell(Position pos) const {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid Position in metod GetCell");
	}
	if (static_cast<int>(cells_.size()) < pos.row + 1 || static_cast<int>(cells_[pos.row].size()) < pos.col + 1) {
		return nullptr;
	}
	return cells_[pos.row][pos.col].get();
}
CellInterface* Sheet::GetCell(Position pos) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid Position in metod GetCell");
	}
	if (static_cast<int>(cells_.size()) < pos.row + 1 || static_cast<int>(cells_[pos.row].size()) < pos.col + 1) {
		return nullptr;
	}
	return cells_[pos.row][pos.col].get();
}

void Sheet::ClearCell(Position pos) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid Position in metod ClearCell");
	}
	if (cells_.empty() || static_cast<int>(cells_.size()) < pos.row) {
		return;
	}
	if (cells_[pos.row].empty() || static_cast<int>(cells_[pos.row].size()) < pos.col) {
		return;
	}
	cells_[pos.row][pos.col].release();
}

Size Sheet::GetPrintableSize() const {
	Size size;
	if (cells_.empty()) {
		return size;
	}

	for (size_t i = 0; i < cells_.size(); ++i) {
		if (cells_[i].empty()) {
			continue;
		}
		for (size_t j = 0; j < cells_[i].size(); ++j) {
			if (cells_[i][j] != nullptr) {
				if (size.rows < static_cast<int>(i + 1)) {
					size.rows = i + 1;
				}
				if (size.cols < static_cast<int>(j + 1)) {
					size.cols = j + 1;
				}
			}
		}
	}
	return size;
}

void Sheet::PrintValues(std::ostream& output) const {
	Size size = GetPrintableSize();
	size_t size_print;
	if (static_cast<int>(cells_.size()) > size.rows) {
		size_print = size.rows;
	}
	else {
		size_print = cells_.size();
	}
	for (size_t i = 0; i < size_print; ++i) {
		for (size_t j = 0; j < cells_[i].size(); ++j) {
			if (cells_[i][j] != nullptr) {
				if (static_cast<int>(j + 1) == size.cols) {
					switch (cells_[i][j]->GetValue().index()) {
					case 0: { output << std::get<std::string>(cells_[i][j]->GetValue()); break; }
					case 1: { output << std::get<double>(cells_[i][j]->GetValue()); break; }
					default: { output << std::get<FormulaError>(cells_[i][j]->GetValue()); break; }
					}
				}
				else {
					switch (cells_[i][j]->GetValue().index()) {
					case 0: { output << std::get<std::string>(cells_[i][j]->GetValue()) << "\t"; break; }
					case 1: { output << std::get<double>(cells_[i][j]->GetValue()) << "\t"; break; }
					default: { output << std::get<FormulaError>(cells_[i][j]->GetValue()) << "\t"; break; }
					}
				}
			}
			else {
				output << "\t";
			}
		}
		if (size.cols > static_cast<int>(cells_[i].size())) {
			for (int col = 0; col < size.cols - static_cast<int>(cells_[i].size()) - 1; ++col) {
				output << "\t";
			}
		}
		output << "\n";
	}
}
void Sheet::PrintTexts(std::ostream& output) const {
	Size size = GetPrintableSize();
	size_t size_print;
	if (static_cast<int>(cells_.size()) > size.rows) {
		size_print = size.rows;
	}
	else {
		size_print = cells_.size();
	}
	for (size_t i = 0; i < size_print; ++i) {
		for (size_t j = 0; j < cells_[i].size(); ++j) {
			if (cells_[i][j] != nullptr) {
				if (static_cast<int>(j + 1) == size.cols) {
					output << cells_[i][j]->GetText();
				}
				else {
					output << cells_[i][j]->GetText() << "\t";
				}
			}
			else {
				output << "\t";
			}
		}
		if (size.cols > static_cast<int>(cells_[i].size())) {
			for (int col = 0; col < size.cols - static_cast<int>(cells_[i].size()) - 1; ++col) {
				output << "\t";
			}
		}
		output << "\n";
	}
}

std::unique_ptr<Cell> Sheet::CreateCell(const std::string& str, Position pos) {
	std::unique_ptr<Cell> cell = std::make_unique<Cell>(GetSheet(), pos);
	cell->Set(str);
	return cell;
}

SheetInterface& Sheet::GetSheet() {
	return *this;
}

const Cell* Sheet::GetConcreteCell(Position pos) const {
	return cells_[pos.row][pos.col].get();
}

Cell* Sheet::GetConcreteCell(Position pos) {
	return cells_[pos.row][pos.col].get();
}

void Sheet::Resize(Position pos) {
	Size size;
	size.rows = pos.row + 1;
	size.cols = pos.col + 1;
	if (static_cast<int>(cells_.size()) < size.rows) {
		cells_.resize(size.rows);
	}
	if (static_cast<int>(cells_[pos.row].size()) < size.cols) {
		cells_[pos.row].resize(size.cols);
	}
}

std::unique_ptr<SheetInterface> CreateSheet() {
	return std::make_unique<Sheet>();
}