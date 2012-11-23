/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef _H_AU_C4
#define _H_AU_C4

#include <assert.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define ROWS     6
#define COLS     7
#define LINE     4

/*
 * #define ROWS 4
 * #define COLS 4
 * #define LINE 4
 */

#define REF_ROW_COL(r, c) ((r) * COLS + (c))

#define C4_WHITE 1
#define C4_BLACK -1
#define C4_NONE  0
#define C4_DRAW  10

namespace au {
/**
 * Namespace for connect4 library : Solving connect-4 problem
 */

namespace c4 {
template<typename C>
class BitMask {
public:
  BitMask() {
    value_ = 0;
  }

  explicit BitMask(C value) {
    value_ = value;
  }

  void operator=(const BitMask& mask) {
    value_ = mask.value_;
  }

  bool Get(int p) {
    char *byte = reinterpret_cast<char *>(&value_);
    int b = p / 8;
    int r = p % 8;

    return (byte[b] & (1 << r)) != 0;
  }

  void Set(int p) {
    char *byte = reinterpret_cast<char *>(&value_);
    int b = p / 8;
    int r = p % 8;

    byte[b] |= (1 << r);
  }

  size_t value() {
    return value_;
  }

  std::string str() {
    std::ostringstream output;

    output << "[" << value_ << "] ";
    for (int i = 0; i < 64; i++) {
      if (Get(i)) {
        output << "1";
      } else {
        output << "0";
      }
    }
    return output.str();
  }

private:
  C value_;
};

typedef BitMask<size_t>   BitMask64;
typedef BitMask<char>     BitMask8;

class Board {
public:
  Board(size_t white, size_t black);
  Board();

  void SetBoard(size_t white, size_t black) {
    white_ = static_cast<BitMask64>(white);
    black_ = static_cast<BitMask64>(black);
  }

  int Get(int row, int col) {
    if (white_.Get(REF_ROW_COL(row, col))) {
      return C4_WHITE;
    }
    if (black_.Get(REF_ROW_COL(row, col))) {
      return C4_BLACK;
    }
    return C4_NONE;
  }

  void Set(int player, int row, int col) {
    if (player == 1) {
      white_.Set(REF_ROW_COL(row, col));
    }

    if (player == -1) {
      black_.Set(REF_ROW_COL(row, col));
    }
  }

  bool Move(int player, int col) {
    for (int r = 0; r < ROWS; r++) {
      if (Get(r, col) == C4_NONE) {
        Set(player, r, col);
        return true;     // OK
      }
    }
    return false;     // Error
  }

  bool IsWinner(size_t value) {
    if (value == 15) {
      return true;
    }
    if (value == 30) {
      return true;
    }
    if (value == 60) {
      return true;
    }
    if (value == 120) {
      return true;
    }
    if (value == 1920) {
      return true;
    }
    if (value == 3840) {
      return true;
    }
    if (value == 7680) {
      return true;
    }
    if (value == 15360) {
      return true;
    }
    if (value == 245760) {
      return true;
    }
    if (value == 491520) {
      return true;
    }
    if (value == 983040) {
      return true;
    }
    if (value == 1966080) {
      return true;
    }
    if (value == 2113665) {
      return true;
    }
    if (value == 2130440) {
      return true;
    }
    if (value == 4227330) {
      return true;
    }
    if (value == 4260880) {
      return true;
    }
    if (value == 8454660) {
      return true;
    }
    if (value == 8521760) {
      return true;
    }
    if (value == 16843009) {
      return true;
    }
    if (value == 16909320) {
      return true;
    }
    if (value == 17043520) {
      return true;
    }
    if (value == 31457280) {
      return true;
    }
    if (value == 33686018) {
      return true;
    }
    if (value == 33818640) {
      return true;
    }
    if (value == 62914560) {
      return true;
    }
    if (value == 67372036) {
      return true;
    }
    if (value == 67637280) {
      return true;
    }
    if (value == 125829120) {
      return true;
    }
    if (value == 134744072) {
      return true;
    }
    if (value == 135274560) {
      return true;
    }
    if (value == 251658240) {
      return true;
    }
    if (value == 270549120) {
      return true;
    }
    if (value == 272696320) {
      return true;
    }
    if (value == 541098240) {
      return true;
    }
    if (value == 545392640) {
      return true;
    }
    if (value == 1082196480) {
      return true;
    }
    if (value == 1090785280) {
      return true;
    }
    if (value == 2155905152) {
      return true;
    }
    if (value == 2164392960) {
      return true;
    }
    if (value == 2181570560) {
      return true;
    }
    if (value == 4026531840) {
      return true;
    }
    if (value == 4311810304) {
      return true;
    }
    if (value == 4328785920) {
      return true;
    }
    if (value == 8053063680) {
      return true;
    }
    if (value == 8623620608) {
      return true;
    }
    if (value == 8657571840) {
      return true;
    }
    if (value == 16106127360) {
      return true;
    }
    if (value == 17247241216) {
      return true;
    }
    if (value == 17315143680) {
      return true;
    }
    if (value == 32212254720) {
      return true;
    }
    if (value == 34630287360) {
      return true;
    }
    if (value == 34905128960) {
      return true;
    }
    if (value == 69260574720) {
      return true;
    }
    if (value == 69810257920) {
      return true;
    }
    if (value == 138521149440) {
      return true;
    }
    if (value == 139620515840) {
      return true;
    }
    if (value == 275955859456) {
      return true;
    }
    if (value == 277042298880) {
      return true;
    }
    if (value == 279241031680) {
      return true;
    }
    if (value == 515396075520) {
      return true;
    }
    if (value == 551911718912) {
      return true;
    }
    if (value == 554084597760) {
      return true;
    }
    if (value == 1030792151040) {
      return true;
    }
    if (value == 1103823437824) {
      return true;
    }
    if (value == 1108169195520) {
      return true;
    }
    if (value == 2061584302080) {
      return true;
    }
    if (value == 2207646875648) {
      return true;
    }
    if (value == 2216338391040) {
      return true;
    }
    if (value == 4123168604160) {
      return true;
    }
    return false;
  }

  int GetWinner() {
    if (IsWinner(white_.value())) {
      return C4_WHITE;
    }
    if (IsWinner(black_.value())) {
      return C4_BLACK;
    }

    // Return none if still pending movements...
    for (int c = 0; c < COLS; c++) {
      if (Get(ROWS - 1, c) == 0) {
        return C4_NONE;
      }
    }

    // No movemnets, no winner
    return C4_DRAW;
  }

  std::string str() {
    std::ostringstream output;

    for (int c = 0; c < COLS; c++) {
      output << "-";
    }
    output << "\n";

    for (int r = (ROWS - 1); r >= 0; r--) {
      for (int c = 0; c < COLS; c++) {
        switch (Get(r, c)) {
          case C4_NONE:
            output << " ";
            break;
          case C4_WHITE:
            output << "O";
            break;
          case C4_BLACK:
            output << "X";
            break;

          default:
            output << "?";
            break;
        }
        ;
      }
      output << "\n";
    }

    for (int c = 0; c < COLS; c++) {
      output << "-";
    }
    output << "\n";
    /*
     * output << white_.str() << "\n";
     * output << black_.str() << "\n";
     *
     * for ( int c  = 0 ; c < COLS ; c++)
     * output << "-";
     * output << "\n";
     */

    return output.str();
  }

  int GetNextBoards(int player, Board *boards) {
    int pos = 0;

    for (int i = 0; i < 7; i++) {
      boards[pos] = *this;
      if (boards[pos].Move(player, i)) {
        pos++;
      }
    }
    return pos;
  }

  size_t white_value() {
    return white_.value();
  }

  size_t black_value() {
    return black_.value();
  }

private:
  BitMask64 white_;
  BitMask64 black_;
};
}
}

#endif  // ifndef _H_AU_C4
