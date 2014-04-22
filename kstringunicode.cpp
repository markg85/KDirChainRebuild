/*
    Copyright (C) 2014 Mark Gaiser <markg85@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "kstringunicode.h"

#include <QChar>

static const ushort nullUnicode = QChar('\0').unicode();

// Constructor
KStringUnicode::KStringUnicode(const QString& data)
    : m_data(new ushort[data.length() + 1])
{
    const int length = data.length();
    for(int i = 0; i < length; i++) {
        m_data[i] = data[i].unicode();
    }
    m_data[length] = nullUnicode;
}

KStringUnicode::KStringUnicode(const ushort data[], int length)
    : m_data(new ushort[length])
{
    for(int i = 0; i < length; i++) {
        m_data[i] = data[i];
    }
    m_data[length] = nullUnicode;
}

KStringUnicode &KStringUnicode::operator=(KStringUnicode&& other)
{
    if(m_data != other.m_data) {
        m_data = std::move(other.m_data);
    }
    return *this;
}

KStringUnicode &KStringUnicode::operator=(std::unique_ptr<ushort[]> data)
{
    m_data = std::move(data);
    return *this;
}

// Move constructor
KStringUnicode::KStringUnicode(KStringUnicode&& other)
    : m_data(std::move(other.m_data))
{
}

KStringUnicode KStringUnicode::mid(int position, int n) const
{
    const int strLength = length();
    const int tillPosition = (n == -1) ? strLength + 1 : position + n;
    const int size = tillPosition - position;

    ushort charList[size];

    for(int i = 0; i < size; i++) {
        charList[i] = m_data[i + position];
    }

    charList[size] = nullUnicode;

    return KStringUnicode(charList, size);
}

void KStringUnicode::midInternal(int position, int n)
{
    const int strLength = length();
    const int tillPosition = (n == -1) ? strLength + 1 : position + n;
    const int size = tillPosition - position;

    std::unique_ptr<ushort[]> charList(new ushort[size]);

    for(int i = 0; i < size; i++) {
        charList[i] = m_data[i + position];
    }

    charList[size] = nullUnicode;

    m_data.reset(); // release and destroy the currently owned pointer
    m_data = std::move(charList);
}

const int KStringUnicode::length() const
{
    int length = 0;
    if(m_data) {
        while(m_data[length] != QChar('\0').unicode()) {
            length++;
        }
    }
    return length;
}

const QString KStringUnicode::toString() const
{
    const int strLength = length();

    QChar data[strLength];
    for(int i = 0; i < strLength; i++) {
        data[i] = QChar(m_data[i]);
    }
    return QString(data, strLength);
}
