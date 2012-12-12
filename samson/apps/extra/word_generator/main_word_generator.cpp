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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "parseArgs/paConfig.h"
#include "parseArgs/parseArgs.h"

#include "logMsg/logMsg.h"

#include "au/log/LogMain.h"
#include "au/Log.h"
#include "au/CommandLine.h"  // au::CommandLine
#include "au/statistics/Cronometer.h"  // au::Cronometer
#include "au/string/StringUtilities.h"  // au::str()

#include "au/CommandLine.h"     // au::CommandLine


#define PROGRESSIVE_NUMBER 20000

static const char *manShortDescription =
"word_generator - a simple tool to generate a random sequence of words. Useful in demos on word counting and topic trending.";

static const char *manSynopsis =
"   [-r] [-t secs] [-l len] num_words\n"
"       [-ramdon] flag to generate randomized sequence of words\n"
"       [-rate w] Max rate of words per second\n"
"       [-l length] Number of letters of generated words ( default 9 ) \n"
"       [-alphabet alphabet size] Number of differnt letters used to generate words ( default 10 ) \n"
"       [-progresive] flag to generate sequences of numbers in incressing order ( hit demo )\n"
"       [-repeat secs] time_to_repeat in seconds with a new wave of words\n";

int word_length;      // Number of different letters used to generate words
int alphabet_length;  // Length of the word in letters
bool rand_flag;       // Random words ( really random )
int max_num_words;    // Max number of words to be generated
bool progresive;      // Flag to indicate sequential generated words 0,1,2,3....
int max_rate;         // Max rate of words per second
int repeat;           // Repeat the same word a number of times before generating another one.

PaArgument paArgs[] =
{
  { "-l",           &word_length,     "", PaInt,  PaOpt, 9,
    1,
    30,
    "Number of letters of generated words ( default 9 )"                           },
  { "-repeat",      &repeat,          "", PaInt,  PaOpt, 1,
    1,
    100000,
    "Number of times the same word is generated ( default 1 )"                         },
  { "-alphabet",    &alphabet_length, "", PaInt,  PaOpt, 10,
    1,
    30,
    "Number of differnt letters used to generate words ( default 10 )"             },
  { "-random",      &rand_flag,       "", PaBool, PaOpt,
    false,
    false, true,
    "Flag to generate completelly randomized sequence of words"                    },
  { "-progressive", &progresive,      "", PaBool, PaOpt,
    false,
    false, true,
    "Flag to generate sequences of numbers in incressing order"                    },
  { "-rate",        &max_rate,        "", PaInt,  PaOpt, 0,
    0,
    10000000000, "Max rate in words / second"                 },
  { " ",            &max_num_words,   "", PaInt,  PaOpt, 0,
    0,
    1000000000,
    "Number of words to be generated"                                              },
  PA_END_OF_ARGS
};

int logFd = -1;

char word[100];                   // Generated word
int progressive_word_slots[100];  // Indexes to generate sequential sequences of words
au::Cronometer cronometer;        // Global cronometer to measure output rate
int repeat_counter = 0;           // Counter of the number of times current word has been used
bool first_word=true;             // Flag to indicate if I am generating the first word

/**
 * \brief Method to check if I need to generate a different word ( see -repeat parameter )
 */
bool IsNecessaryToChangeWord() {
  
  if(first_word) {
    first_word = false;
    repeat_counter=0;
    return true;
  }
  
  if (++repeat_counter >= repeat) {
    repeat_counter = 0;
    return true;
  }
  
  return false;
}


/**
 * \brief Get a new work to be emitted ( taking into account parameters provided at command line )
 */
void GetNewWord() {
  if (!IsNecessaryToChangeWord()) {
    return;  // Not necessary to change the word
  }
  if (progresive) {
    // Use current values in "progressive_word_slots" to generate the word
    for (int i = 0; i < word_length; i++) {
      word[i] = 48 + progressive_word_slots[i];
    }
    word[word_length] = '\n';
    word[word_length + 1] = '\0';
    
    // Increase counter...
    progressive_word_slots[word_length - 1]++;
    int pos = word_length - 1;
    while ((pos >= 0) && (progressive_word_slots[pos] >= alphabet_length)) {
      progressive_word_slots[pos] = 0;
      if (pos > 0) {
        progressive_word_slots[pos - 1]++;
      }
      pos--;
    }
    return;
  }
  
  // Random word
  for (int i = 0; i < word_length; i++) {
    word[i] = 48 + rand() % alphabet_length;
  }
  word[word_length] = '\n';
  word[word_length + 1] = '\0';

}

/**
 * \brief Simple class to buffer content for stdout
 */

class BufferToStdout {
  
public:
  
  BufferToStdout(size_t max_size) {
    max_size_ = max_size;
    buffer_ = (char *)malloc(max_size);
    size_ = 0;
  }
  
  /**
   * \brief Append content
   */
  void Append(char *data, int len) {
    if ((size_ + len) > max_size_) {
      Flush();
    }
    memcpy(buffer_ + size_, data, len);
    size_ += len;
  }
  
  /**
   * \brief Flush current content
   */
  void Flush() {
    size_t w = write(1, buffer_, size_);
    
    if (w != size_) {
      LM_X(1, ("Problem writing %lu bytes to the screen", size_));
    }
    size_ = 0;
  }
  
private:
  
  char *buffer_;
  size_t max_size_;
  size_t size_;
  
};


int main(int argC, const char *argV[]) {
  
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen", (void *)true);
  paConfig("log to file", (void *)false);
  paConfig("screen line format", (void *)"TYPE:EXEC: TEXT");
  paConfig("man shortdescription", (void *)manShortDescription);
  paConfig("man synopsis", (void *)manSynopsis);
  paConfig("log to stderr", (void *)true);
  
  // Parse input arguments
  paParse(paArgs, argC, (char **)argV, 1, false);
  logFd = lmFirstDiskFileDescriptor();
  
  // Init log system
  au::LogCentral::InitLogSystem(argV[0]);
  au::LogCentral::Shared()->AddScreenPlugin("screen","[type] text");
  au::logs.RegisterChannels(); // Register channels for au::log library
  if( paVerbose ){
    au::LogCentral::Shared()->EvalCommand("log_set system V");
  }
  
  // Init progressive
  if (progresive) {
    for (int i = 0; i < word_length; i++) {
      progressive_word_slots[i] = 0;
    }
  }

  // General counter for #words and total size
  size_t num_words = 0;
  size_t total_size = 0;
  
  // Init real-random numbers if -random flag provided
  if (rand_flag) {
    srand(time(NULL));
  } else {
    srand(0);
  }
  
  // Time to show a message on screen in verbose mode
  size_t last_message_time = 0;
  
  // Buffer to flsh data to screen in batches
  BufferToStdout buffer_to_screen(10000);
  
  double words_per_second = 0;
  
  // Generate continuously...
  while (true) {
    // Check the limit of generated words
    if (max_num_words > 0) {
      if (num_words >= (size_t)max_num_words) {
        break;
      }
    }
    
    // Get a new word ( following provided parameters at command line )
    GetNewWord();
    
    // Length of this word
    int word_len = strlen(word);
    
    // Append to the screen
    buffer_to_screen.Append(word, word_len);
    
    // Counter of bytes and words
    total_size += word_len;
    num_words++;
    
    // This avoid exesive calls to timeout
    if (words_per_second > 100000) {
      size_t num_continue = words_per_second / 100;
      if ((num_words % num_continue) != 0) {
        continue;
      }
    }
    
    // Flush accumulated buffer so far
    buffer_to_screen.Flush();
    
    // Get the total number of seconds running...
    size_t total_seconds = cronometer.seconds();
    
    // Compute the number of words per second, so far...
    if (total_seconds > 0) {
      words_per_second = (double)num_words / (double)total_seconds;
    }
    
    // Generate a message every 5 seconds to report progress
    if ((total_seconds - last_message_time) > 5) {
      last_message_time = total_seconds;
      
      if( max_num_words > 0 ){
        LOG_SV(("Generated %s - %s lines ( %s bytes ) in %s. Rate: %s / %s",
                au::str_percentage(num_words, max_num_words).c_str(),
                au::str(num_words).c_str(),
                au::str(total_size).c_str(),
                au::str_time(total_seconds).c_str(),
                au::str((double)num_words / (double)total_seconds,"Lines/s").c_str(),
                au::str((double)total_size / (double)total_seconds, "Bps").c_str()));
      } else {
        LOG_SV(("Generated %s lines ( %s bytes ) in %s. Rate: %s / %s",
                au::str(num_words).c_str(),
                au::str(total_size).c_str(),
                au::str_time(total_seconds).c_str(),
                au::str((double)num_words / (double)total_seconds,"Lines/s").c_str(),
                au::str((double)total_size / (double)total_seconds, "Bps").c_str()));
      }
    }
    
    if (max_rate > 0) {
      size_t theoretical_seconds = num_words / max_rate;
      if (total_seconds < theoretical_seconds) {
        sleep(theoretical_seconds - total_seconds);
      }
    }
  }
  
  // Flush last words generated
  buffer_to_screen.Flush();
  
  size_t total_seconds = cronometer.seconds();
  LOG_SV(("Generated %s lines ( %s bytes ) in %s. Rate: %s / %s",
          au::str(num_words).c_str(), au::str(total_size).c_str(), au::str_time(total_seconds).c_str(),
          au::str((double)num_words / (double)total_seconds,"Lines/s").c_str(),
          au::str((double)total_size / (double)total_seconds, "Bps").c_str()));
}

