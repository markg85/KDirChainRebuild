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

#ifndef KSTRINGUNICODE_H
#define KSTRINGUNICODE_H

#include <QString>
#include <memory>

/**
 * This class is meant as a very small string representation in it's rawest UTF form.
 * Each character is stored in a ushort (2 byte) unicode number. The unicode number
 * is fetched from each character by calling QChar::unicode() call.
 */

class KStringUnicode
{
public:
    // Forbid this class from being copied
    KStringUnicode(const KStringUnicode&) = delete;
    KStringUnicode& operator=(const KStringUnicode&) = delete;

    // Constructor
    KStringUnicode(const QString& data);
    KStringUnicode(const ushort data[], int length);

    // Move constructor
    KStringUnicode(KStringUnicode&& other);

    // Move assignment operator
    KStringUnicode& operator=(KStringUnicode&& other);

    // ushort assignment operator
    KStringUnicode& operator=(std::unique_ptr<ushort[]> data);

    // Convenience function to get any part of the string as a new KStringUnicode object.
    KStringUnicode mid(int position, int n = -1) const;
    void midInternal(int position, int n = -1);

    // Ruturns the length of the current string.
    const int length() const;

    const QString toString() const;
    const ushort operator[] (const int index) const
    {
        return m_data[index];
    }

private:
    std::unique_ptr<ushort[]> m_data;
};

#endif // KSTRINGUNICODE_H
