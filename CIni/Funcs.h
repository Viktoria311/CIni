#pragma once

#include "CIni.h"

// Global funcs


//!  ���������� � ��������� ini ������ � ������� � ����������
void WriteValue(CIni& ini);

//! �������� � ini ������ � ��������� ��
void CheckSection(const CIni& ini);

//! ��������� ������������ ������ � ������� � ���������� 
//! � ini ����, ����� ����
void SavePassingMap(const CIni& ini);

//! ����� �������� � ����
void ChangeValue(CIni& ini);