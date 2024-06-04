/* ascii2pcm.cpp new main file with fixed character codes for cwpcm 
generator by
Coypright (C) 2020 Fritz Crusius
Copyright (C) 2001-2007 Randall S. Bohn

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/
/* envelope support by Steve Conklin http://www.morseresource.com */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <iostream>
#include <vector>

extern "C" {
#include "morse.h"
#include "pcm.h"
}

//#define SAMPLERATE 8000
#define SAMPLERATE 44100

#define RISETIME 10
#define FALLTIME 10

/* tbase (dit time in samples) is 1200 msec * 8 samples per msec */
//#define tbase 9600 // for 8000 Samples per second

/* 900 Hz */
#define PITCH "800"
#define MAX_PITCH 8000
/* volume on 0..100 percent */
#define VOLUME "70"

#define MIN_SPEED 1
#define MAX_SPEED 100
#define DEFAULT_SPEED "24"
#define DEFAULT_LETTER_SPEED "28"

/* program settings */
int pcm = 1;
int hvox = 0;
/* character time, space time (in samples) */
int cTime, sTime;
/* sample rate (samples per second) */
int sample_rate=44100;

/* get the dit-time for characters */
int getCharacterTime(int c_rate) {
	// dit time is 1200 msec at 1 WPM
	// this returns the number of samples per dit
	// at the desired character rate
	float tbase = sample_rate/1000.0 * 1200;
	return (int)(tbase/c_rate);
}

/* get the dit-time for space between characters */
int getSpaceTime(int c_rate, int w_rate) {
	/* NONB helped with this section. Thanks Nate! */
	int t_total;
	int t_chars;
	int t_space;

	if (w_rate < MIN_SPEED) w_rate = MIN_SPEED;
	if (w_rate >= c_rate) return getCharacterTime(c_rate);

	/* spaces take longer but how much longer? */
	t_total = getCharacterTime(w_rate) * 50;
	t_chars = getCharacterTime(c_rate) * 36;

	t_space = t_total - t_chars;
	return t_space / 14;
}

extern "C" {
/* morse-speak: */
void dit(FILE *out) {
 mark(hvox, cTime, out);
 space(hvox, cTime, out);
}
void dah(FILE *out) {
 mark(hvox, cTime * 3, out);
 space(hvox, cTime, out);
}
}
void err(FILE *out) {
}

extern "C" {
void cspace(FILE *out) {
 space(hvox, sTime * 2, out);
}
void wspace(FILE *out) {
 space(hvox, sTime * 4, out);
 fflush(out);
}
}

void setupVoice(int hz, int amp, int risetime, int falltime) {
  /* freq, amplitude, zero, sample rate */
 hvox = voiceFactory(hz, amp, 128, sample_rate);
 setRisetime(hvox, risetime);
 setFalltime(hvox, falltime);
}

int getInt(const std::string& s, int low, int high) {
	int ival = std::stoi(s);
	if (ival < low) return low;
	if (ival > high) return high;
	return ival;
}

class Codes {
  typedef std::vector<std::pair<char,const char*>> code_vector;
private:
  // NB: the cast of -1 is exactly what we need as it gives the max unsigned value
  // of the char type, which could be multibyte [-Wmultichar]
  Codes() : _codes(static_cast<unsigned char>(-1), "") {
    code_vector cv(getCodeMap());
    for (const auto& cp : cv) {
      // NB: a static_cast<size_t> does not result in a segfault only if the code_vector
      // does not contain any negative values
      _codes[static_cast<unsigned char>(cp.first)] = cp.second;
    } 
  }

  static code_vector getCodeMap() {
    code_vector cv = {
      { 'a', ".-" },
      { 'b', "-..." },
      { 'c', "-.-." },
      { 'd', "-.." },
      { 'e', "." },
      { 'f', "..-." },
      { 'g', "--." },
      { 'h', "...." },
      { 'i', ".." },
      { 'j', ".---" },
      { 'k', "-.-" },
      { 'l', ".-.." },
      { 'm', "--" },
      { 'n', "-." },
      { 'o', "---" },
      { 'p', ".--." },
      { 'q', "--.-" },
      { 'r', ".-." },
      { 's', "..." },
      { 't', "-" },
      { 'u', "..-" },
      { 'v', "...-" },
      { 'w', ".--" },
      { 'x', "-..-" },
      { 'y', "-.--" },
      { 'z', "--.." },
      { '0', "-----" },
      { '1', ".----" },
      { '2', "..---" },
      { '3', "...--" },
      { '4', "....-" },
      { '5', "....." },
      { '6', "-...." },
      { '7', "--..." },
      { '8', "---.." },
      { '9', "----." },
      { '+', ".-.-." },
      { '=', "-...-" },
      { '-', "-....-" },
      { '\'', ".----." },
      { '"', ".-..-." },
      { '&', ".-..." },
      { '@', ".--.-." },
      { '.', ".-.-.-" },
      { ',', "--..--" },
      { '?', "..--.." },
      { '/', "-..-." },
      { ':', "---..." },
      { '(', "-.--." },
      { ')', "-.--.-" },
      { '!', "-.-.--" },
      /* requires multichar support [-Wmultichar]
      { 'ü', "..--" },
      { 'ö', "---." },
      { 'ä', ".-.-" },
      { 'é', "..-.." },
      { 'à', ".--.-" }
      */
      // codes may not reflect proper ascii, but match what's coming from driver ;)
      { '\x81', "..--" }, // ue
      { '\x92', "---." }, // oe
      { '\x84', ".-.-" }, // ae
      { '\x82', "..-.." },// e'
      { '\x8a', ".-..-" },// e`
      { '\x85', ".--.-" },// a`
      { '\x87', "-.-.." } // c^
    };
    return cv;
  }

public:
  static Codes& getInstance() {
    static Codes codes;
    return codes;
  }
  const std::vector<const char*>& getCodeVector() const {
    return _codes;
  }
private:
  std::vector<const char*> _codes;
};

/* generate morse code equivalent for each character */
void encodeMorse(char c, FILE *out) {
  // NB: no cast to unsigned char or a static_cast<size_t> will expand
  // negative chars to something 0xffffffab giving a segfault
  const char* code = Codes::getInstance().getCodeVector()[static_cast<unsigned char>(c)];
  while (*code != '\0') {
    if (*code == '.') {
      dit(out);
    } else if (*code == '-') {
      dah(out);
    } else {
      std::cerr << "invalid code char '" << *code << std::endl;
    }
    ++code;
  }
}

void ascii2pcm(FILE *in, FILE *out) {
  int ch;
 
  while ((ch = fgetc(in)) != EOF) {
    if (isspace(ch)) {
      wspace(out);
    } else {
      encodeMorse(tolower(ch), out);
      cspace(out);
    }
  }
}

int main(int argc, char *argv[]) {
 int verbose = 0;
 const std::string optPitch("-f"), optVol("-v"), optSpeed("-w"), optCSpeed("-F");

 std::map<std::string, std::string> argMap = {
	{ optPitch, PITCH },
	{ optVol, VOLUME },
        { optSpeed, DEFAULT_SPEED },
        { optCSpeed, DEFAULT_LETTER_SPEED }
 };
 std::map<std::string, std::string>::iterator argIter = argMap.end();
 for (int i = 1; i < argc; ++i) {
	 if (argv[i][0] == '-') {
		argIter = argMap.insert(std::make_pair(argv[i],"")).first; 
	 } else if (argIter != argMap.end()) {
		 argIter->second = argv[i];
		 argIter = argMap.end();
	 } else {
		 std::cerr << "unmatched command line arg " << argv[i] << std::endl;
	 }
 }

 int pitch = getInt(argMap[optPitch], 40, MAX_PITCH);
 int volume = getInt(argMap[optVol], 0, 100);
 int wwpm = getInt(argMap[optSpeed], MIN_SPEED, MAX_SPEED);
 int cwpm = getInt(argMap[optCSpeed], MIN_SPEED, MAX_SPEED);
 if (argMap.find("-d") != argMap.end()) {
	 verbose = 1;
 }

 cTime = getCharacterTime(cwpm);
 sTime = getSpaceTime(cwpm, wwpm);
 int risetime = RISETIME / cwpm;
 int falltime = FALLTIME / cwpm;
 if (risetime > RISETIME) { risetime = RISETIME; }
 if (falltime > FALLTIME) { falltime = FALLTIME; }
 if (risetime < 1) { risetime = 1; }
 if (falltime > 1) { falltime = 1; }
 setupVoice(pitch, volume*127/100, risetime, falltime);
 // you might want to adjust falltime for high rates:
 // if (cwpm > 20) setFalltime(hvox, 0);



 if (verbose) {
   fprintf(stderr, "Pitch: %d Volume: %d%%\n", pitch, volume);
   fprintf(stderr, "WPM: %d Farnsworth: %d\n", wwpm, cwpm);
 }

 ascii2pcm(stdin, stdout);
 return 0;
}
