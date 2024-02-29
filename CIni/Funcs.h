#pragma once

#include "CIni.h"

// Global funcs


//!  записываем в структуру ini секции с ключами и значени€ми
void WriteValue(CIni& ini);

//! выбираем у ini секцию и провер€ем ее
void CheckSection(const CIni& ini);

//! сохранить интересующие секции с ключами и значени€ми 
//! в ini файл, мину€ мапу
void SavePassingMap(const CIni& ini);

//! смена значений в мапе
void ChangeValue(CIni& ini);