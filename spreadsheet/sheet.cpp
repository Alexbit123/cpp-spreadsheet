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
	IsValidPos(pos);
	Resize(pos);
	cells_[pos.row][pos.col] = CreateCell(text, pos);
}

const CellInterface* Sheet::GetCell(Position pos) const {
	IsValidPos(pos);
	if (static_cast<int>(cells_.size()) < pos.row + 1 || static_cast<int>(cells_[pos.row].size()) < pos.col + 1) {
		return nullptr;
	}
	return cells_[pos.row][pos.col].get();
}
CellInterface* Sheet::GetCell(Position pos) {
	IsValidPos(pos);
	if (static_cast<int>(cells_.size()) < pos.row + 1 || static_cast<int>(cells_[pos.row].size()) < pos.col + 1) {
		return nullptr;
	}
	return cells_[pos.row][pos.col].get();
}

void Sheet::ClearCell(Position pos) {
	IsValidPos(pos);
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

	for (size_t row = 0; row < cells_.size(); ++row) {
		if (cells_[row].empty()) {
			continue;
		}
		for (size_t col = 0; col < cells_[row].size(); ++col) {
			if (cells_[row][col] != nullptr) {
				if (size.rows < static_cast<int>(row + 1)) {
					size.rows = row + 1;
				}
				if (size.cols < static_cast<int>(col + 1)) {
					size.cols = col + 1;
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
	for (size_t row = 0; row < size_print; ++row) {
		for (size_t col = 0; col < cells_[row].size(); ++col) {
			if (cells_[row][col] != nullptr) {
				if (static_cast<int>(col + 1) == size.cols) {
					switch (cells_[row][col]->GetValue().index()) {
					case 0: { output << std::get<std::string>(cells_[row][col]->GetValue()); break; }
					case 1: { output << std::get<double>(cells_[row][col]->GetValue()); break; }
					default: { output << std::get<FormulaError>(cells_[row][col]->GetValue()); break; }
					}
				}
				else {
					switch (cells_[row][col]->GetValue().index()) {
					case 0: { output << std::get<std::string>(cells_[row][col]->GetValue()) << "\t"; break; }
					case 1: { output << std::get<double>(cells_[row][col]->GetValue()) << "\t"; break; }
					default: { output << std::get<FormulaError>(cells_[row][col]->GetValue()) << "\t"; break; }
					}
				}
			}
			else {
				output << "\t";
			}
		}
		if (size.cols > static_cast<int>(cells_[row].size())) {
			for (int col = 0; col < size.cols - static_cast<int>(cells_[row].size()) - 1; ++col) {
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
	for (size_t row = 0; row < size_print; ++row) {
		for (size_t col = 0; col < cells_[row].size(); ++col) {
			if (cells_[row][col] != nullptr) {
				if (static_cast<int>(col + 1) == size.cols) {
					output << cells_[row][col]->GetText();
				}
				else {
					output << cells_[row][col]->GetText() << "\t";
				}
			}
			else {
				output << "\t";
			}
		}
		if (size.cols > static_cast<int>(cells_[row].size())) {
			for (int col = 0; col < size.cols - static_cast<int>(cells_[row].size()) - 1; ++col) {
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

bool Sheet::IsValidPos(Position pos) const {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid Position in metod SetCell");
	}
	return true;
}

std::unique_ptr<SheetInterface> CreateSheet() {
	return std::make_unique<Sheet>();
}