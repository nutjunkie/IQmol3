#pragma once

// ColorM is a header-only color conversion and manipulation library for css colors.

// The design of this library is influenced by chroma.js.
// https://gka.github.io/chroma.js/

// This library is released under the Unlicense license.

// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain.We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors.We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>


#include <cmath>
#include <cstring>
#include <cassert>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <array>
#include <initializer_list>
#include <utility>
#include <algorithm>

namespace ColorM
{
	class Rgb;
	class Hsl;
	class Hwb;
	class Lrgb;
	class XyzD50;
	class Lab;
	class Lch;
	class XyzD65;
	class Oklab;
	class Oklch;
	class Srgb;
	class DisplayP3;
	class A98Rgb;
	class ProphotoRgb;
	class Rec2020;


	// CSS's named colors.
	// https://www.w3.org/TR/css-color-4/

	enum class Cn : unsigned int
	{
		Aliceblue = 0xf0f8ff,
		Antiquewhite = 0xfaebd7,
		Aqua = 0x00ffff,
		Aquamarine = 0x7fffd4,
		Azure = 0xf0ffff,
		Beige = 0xf5f5dc,
		Bisque = 0xffe4c4,
		Black = 0x000000,
		Blanchedalmond = 0xffebcd,
		Blue = 0x0000ff,
		Blueviolet = 0x8a2be2,
		Brown = 0xa52a2a,
		Burlywood = 0xdeb887,
		Cadetblue = 0x5f9ea0,
		Chartreuse = 0x7fff00,
		Chocolate = 0xd2691e,
		Coral = 0xff7f50,
		Cornflowerblue = 0x6495ed,
		Cornsilk = 0xfff8dc,
		Crimson = 0xdc143c,
		Cyan = 0x00ffff,
		Darkblue = 0x00008b,
		Darkcyan = 0x008b8b,
		Darkgoldenrod = 0xb8860b,
		Darkgray = 0xa9a9a9,
		Darkgreen = 0x006400,
		Darkgrey = 0xa9a9a9,
		Darkkhaki = 0xbdb76b,
		Darkmagenta = 0x8b008b,
		Darkolivegreen = 0x556b2f,
		Darkorange = 0xff8c00,
		Darkorchid = 0x9932cc,
		Darkred = 0x8b0000,
		Darksalmon = 0xe9967a,
		Darkseagreen = 0x8fbc8f,
		Darkslateblue = 0x483d8b,
		Darkslategray = 0x2f4f4f,
		Darkslategrey = 0x2f4f4f,
		Darkturquoise = 0x00ced1,
		Darkviolet = 0x9400d3,
		Deeppink = 0xff1493,
		Deepskyblue = 0x00bfff,
		Dimgray = 0x696969,
		Dimgrey = 0x696969,
		Dodgerblue = 0x1e90ff,
		Firebrick = 0xb22222,
		Floralwhite = 0xfffaf0,
		Forestgreen = 0x228b22,
		Fuchsia = 0xff00ff,
		Gainsboro = 0xdcdcdc,
		Ghostwhite = 0xf8f8ff,
		Gold = 0xffd700,
		Goldenrod = 0xdaa520,
		Gray = 0x808080,
		Green = 0x008000,
		Greenyellow = 0xadff2f,
		Grey = 0x808080,
		Honeydew = 0xf0fff0,
		Hotpink = 0xff69b4,
		Indianred = 0xcd5c5c,
		Indigo = 0x4b0082,
		Ivory = 0xfffff0,
		Khaki = 0xf0e68c,
		Lavender = 0xe6e6fa,
		Lavenderblush = 0xfff0f5,
		Lawngreen = 0x7cfc00,
		Lemonchiffon = 0xfffacd,
		Lightblue = 0xadd8e6,
		Lightcoral = 0xf08080,
		Lightcyan = 0xe0ffff,
		Lightgoldenrodyellow = 0xfafad2,
		Lightgray = 0xd3d3d3,
		Lightgreen = 0x90ee90,
		Lightgrey = 0xd3d3d3,
		Lightpink = 0xffb6c1,
		Lightsalmon = 0xffa07a,
		Lightseagreen = 0x20b2aa,
		Lightskyblue = 0x87cefa,
		Lightslategray = 0x778899,
		Lightslategrey = 0x778899,
		Lightsteelblue = 0xb0c4de,
		Lightyellow = 0xffffe0,
		Lime = 0x00ff00,
		Limegreen = 0x32cd32,
		Linen = 0xfaf0e6,
		Magenta = 0xff00ff,
		Maroon = 0x800000,
		Mediumaquamarine = 0x66cdaa,
		Mediumblue = 0x0000cd,
		Mediumorchid = 0xba55d3,
		Mediumpurple = 0x9370db,
		Mediumseagreen = 0x3cb371,
		Mediumslateblue = 0x7b68ee,
		Mediumspringgreen = 0x00fa9a,
		Mediumturquoise = 0x48d1cc,
		Mediumvioletred = 0xc71585,
		Midnightblue = 0x191970,
		Mintcream = 0xf5fffa,
		Mistyrose = 0xffe4e1,
		Moccasin = 0xffe4b5,
		Navajowhite = 0xffdead,
		Navy = 0x000080,
		Oldlace = 0xfdf5e6,
		Olive = 0x808000,
		Olivedrab = 0x6b8e23,
		Orange = 0xffa500,
		Orangered = 0xff4500,
		Orchid = 0xda70d6,
		Palegoldenrod = 0xeee8aa,
		Palegreen = 0x98fb98,
		Paleturquoise = 0xafeeee,
		Palevioletred = 0xdb7093,
		Papayawhip = 0xffefd5,
		Peachpuff = 0xffdab9,
		Peru = 0xcd853f,
		Pink = 0xffc0cb,
		Plum = 0xdda0dd,
		Powderblue = 0xb0e0e6,
		Purple = 0x800080,
		Rebeccapurple = 0x663399,
		Red = 0xff0000,
		Rosybrown = 0xbc8f8f,
		Royalblue = 0x4169e1,
		Saddlebrown = 0x8b4513,
		Salmon = 0xfa8072,
		Sandybrown = 0xf4a460,
		Seagreen = 0x2e8b57,
		Seashell = 0xfff5ee,
		Sienna = 0xa0522d,
		Silver = 0xc0c0c0,
		Skyblue = 0x87ceeb,
		Slateblue = 0x6a5acd,
		Slategray = 0x708090,
		Slategrey = 0x708090,
		Snow = 0xfffafa,
		Springgreen = 0x00ff7f,
		Steelblue = 0x4682b4,
		Tan = 0xd2b48c,
		Teal = 0x008080,
		Thistle = 0xd8bfd8,
		Tomato = 0xff6347,
		Turquoise = 0x40e0d0,
		Violet = 0xee82ee,
		Wheat = 0xf5deb3,
		White = 0xffffff,
		Whitesmoke = 0xf5f5f5,
		Yellow = 0xffff00,
		Yellowgreen = 0x9acd32
	};


	// Inline functions.

	namespace impl
	{
		inline double normalize(double value, double vmax) noexcept
		{
			if (value <= 0.0) {
				return 0.0;
			} else if (value > vmax) {
				return vmax;
			}
			return value;
		}

		inline double normalize1(double rate) noexcept
		{
			return normalize(rate, 1.0);
		}

		inline double normalize100(double value) noexcept
		{
			return normalize(value, 100.0);
		}

		inline double normalize255(double value) noexcept
		{
			return normalize(value, 255.0);
		}

		inline double normalizeHue(double hue) noexcept
		{
			hue = std::fmod(hue, 360.0);
			if (!hue) {
				return 0.0;
			} else if (hue < 0.0) {
				return hue + 360.0;
			}
			return hue;
		}

		inline double double255ToInteger(double value) noexcept
		{
			return normalize255(std::round(value));
		}

		inline double toPositiveValue(double value) noexcept
		{
			if (value <= 0.0) {
				return 0.0;
			}
			return value;
		}

		inline double round(double value, double p) noexcept
		{
			double r = std::round(value * p) / p;
			if (!r) {
				return 0.0;
			}
			return r;
		}

		inline int compare(const char* s1, const char* s2) noexcept
		{
#if defined(_MSC_VER)
			return _stricmp(s1, s2);
#else
			return strcasecmp(s1, s2);
#endif
		}

		inline int compare(const char* s1, const char* s2, size_t n) noexcept
		{
#if defined(_MSC_VER)
			return _strnicmp(s1, s2, n);
#else
			return strncasecmp(s1, s2, n);
#endif
		}

		inline const char* search(const char* const* list, int start, int last, const char* s) noexcept
		{
			if (s == nullptr) {
				return nullptr;
			}

			for (;;) {
				const int i = (start + last) / 2;
				const int cmp = compare(s, list[i * 2]);
				if (cmp == 0) {
					return list[i * 2 + 1];
				} else if (cmp < 0) {
					last = i - 1;
					if (start > last) {
						return s;
					}
				} else {
					start = i + 1;
					if (start > last) {
						return s;
					}
				}
			}
		}

		inline const char* toHex(const char* const name) noexcept
		{
			// CSS's named colors.
			// https://www.w3.org/TR/css-color-4/

			static const char* const NamedColor148[296] = {
				"aliceblue",			"#f0f8ff",
				"antiquewhite",			"#faebd7",
				"aqua",					"#00ffff",
				"aquamarine",			"#7fffd4",
				"azure",				"#f0ffff",
				"beige",				"#f5f5dc",
				"bisque",				"#ffe4c4",
				"black",				"#000000",
				"blanchedalmond",		"#ffebcd",
				"blue",					"#0000ff",
				"blueviolet",			"#8a2be2",
				"brown",				"#a52a2a",
				"burlywood",			"#deb887",
				"cadetblue",			"#5f9ea0",
				"chartreuse",			"#7fff00",
				"chocolate",			"#d2691e",
				"coral",				"#ff7f50",
				"cornflowerblue",		"#6495ed",
				"cornsilk",				"#fff8dc",
				"crimson",				"#dc143c",
				"cyan",					"#00ffff",
				"darkblue",				"#00008b",
				"darkcyan",				"#008b8b",
				"darkgoldenrod",		"#b8860b",
				"darkgray",				"#a9a9a9",
				"darkgreen",			"#006400",
				"darkgrey",				"#a9a9a9",
				"darkkhaki",			"#bdb76b",
				"darkmagenta",			"#8b008b",
				"darkolivegreen",		"#556b2f",
				"darkorange",			"#ff8c00",
				"darkorchid",			"#9932cc",
				"darkred",				"#8b0000",
				"darksalmon",			"#e9967a",
				"darkseagreen",			"#8fbc8f",
				"darkslateblue",		"#483d8b",
				"darkslategray",		"#2f4f4f",
				"darkslategrey",		"#2f4f4f",
				"darkturquoise",		"#00ced1",
				"darkviolet",			"#9400d3",
				"deeppink",				"#ff1493",
				"deepskyblue",			"#00bfff",
				"dimgray",				"#696969",
				"dimgrey",				"#696969",
				"dodgerblue",			"#1e90ff",
				"firebrick",			"#b22222",
				"floralwhite",			"#fffaf0",
				"forestgreen",			"#228b22",
				"fuchsia",				"#ff00ff",
				"gainsboro",			"#dcdcdc",
				"ghostwhite",			"#f8f8ff",
				"gold",					"#ffd700",
				"goldenrod",			"#daa520",
				"gray",					"#808080",
				"green",				"#008000",
				"greenyellow",			"#adff2f",
				"grey",					"#808080",
				"honeydew",				"#f0fff0",
				"hotpink",				"#ff69b4",
				"indianred",			"#cd5c5c",
				"indigo",				"#4b0082",
				"ivory",				"#fffff0",
				"khaki",				"#f0e68c",
				"lavender",				"#e6e6fa",
				"lavenderblush",		"#fff0f5",
				"lawngreen",			"#7cfc00",
				"lemonchiffon",			"#fffacd",
				"lightblue",			"#add8e6",
				"lightcoral",			"#f08080",
				"lightcyan",			"#e0ffff",
				"lightgoldenrodyellow",	"#fafad2",
				"lightgray",			"#d3d3d3",
				"lightgreen",			"#90ee90",
				"lightgrey",			"#d3d3d3",
				"lightpink",			"#ffb6c1",
				"lightsalmon",			"#ffa07a",
				"lightseagreen",		"#20b2aa",
				"lightskyblue",			"#87cefa",
				"lightslategray",		"#778899",
				"lightslategrey",		"#778899",
				"lightsteelblue",		"#b0c4de",
				"lightyellow",			"#ffffe0",
				"lime",					"#00ff00",
				"limegreen",			"#32cd32",
				"linen",				"#faf0e6",
				"magenta",				"#ff00ff",
				"maroon",				"#800000",
				"mediumaquamarine",		"#66cdaa",
				"mediumblue",			"#0000cd",
				"mediumorchid",			"#ba55d3",
				"mediumpurple",			"#9370db",
				"mediumseagreen",		"#3cb371",
				"mediumslateblue",		"#7b68ee",
				"mediumspringgreen",	"#00fa9a",
				"mediumturquoise",		"#48d1cc",
				"mediumvioletred",		"#c71585",
				"midnightblue",			"#191970",
				"mintcream",			"#f5fffa",
				"mistyrose",			"#ffe4e1",
				"moccasin",				"#ffe4b5",
				"navajowhite",			"#ffdead",
				"navy",					"#000080",
				"oldlace",				"#fdf5e6",
				"olive",				"#808000",
				"olivedrab",			"#6b8e23",
				"orange",				"#ffa500",
				"orangered",			"#ff4500",
				"orchid",				"#da70d6",
				"palegoldenrod",		"#eee8aa",
				"palegreen",			"#98fb98",
				"paleturquoise",		"#afeeee",
				"palevioletred",		"#db7093",
				"papayawhip",			"#ffefd5",
				"peachpuff",			"#ffdab9",
				"peru",					"#cd853f",
				"pink",					"#ffc0cb",
				"plum",					"#dda0dd",
				"powderblue",			"#b0e0e6",
				"purple",				"#800080",
				"rebeccapurple",		"#663399",
				"red",					"#ff0000",
				"rosybrown",			"#bc8f8f",
				"royalblue",			"#4169e1",
				"saddlebrown",			"#8b4513",
				"salmon",				"#fa8072",
				"sandybrown",			"#f4a460",
				"seagreen",				"#2e8b57",
				"seashell",				"#fff5ee",
				"sienna",				"#a0522d",
				"silver",				"#c0c0c0",
				"skyblue",				"#87ceeb",
				"slateblue",			"#6a5acd",
				"slategray",			"#708090",
				"slategrey",			"#708090",
				"snow",					"#fffafa",
				"springgreen",			"#00ff7f",
				"steelblue",			"#4682b4",
				"tan",					"#d2b48c",
				"teal",					"#008080",
				"thistle",				"#d8bfd8",
				"tomato",				"#ff6347",
				"turquoise",			"#40e0d0",
				"violet",				"#ee82ee",
				"wheat",				"#f5deb3",
				"white",				"#ffffff",
				"whitesmoke",			"#f5f5f5",
				"yellow",				"#ffff00",
				"yellowgreen",			"#9acd32"
			};

			return search(NamedColor148, 0, 148 - 1, name);
		}

		inline const char* toName(const char* const hex) noexcept
		{
			// CSS's named colors.
			// https://www.w3.org/TR/css-color-4/

			static const char* const NamedColor138R[276] = {
				"#000000",				"black",
				"#000080",				"navy",
				"#00008b",				"darkblue",
				"#0000cd",				"mediumblue",
				"#0000ff",				"blue",
				"#006400",				"darkgreen",
				"#008000",				"green",
				"#008080",				"teal",
				"#008b8b",				"darkcyan",
				"#00bfff",				"deepskyblue",
				"#00ced1",				"darkturquoise",
				"#00fa9a",				"mediumspringgreen",
				"#00ff00",				"lime",
				"#00ff7f",				"springgreen",
				//"#00ffff",				"aqua",
				"#00ffff",				"cyan",
				"#191970",				"midnightblue",
				"#1e90ff",				"dodgerblue",
				"#20b2aa",				"lightseagreen",
				"#228b22",				"forestgreen",
				"#2e8b57",				"seagreen",
				"#2f4f4f",				"darkslategray",
				//"#2f4f4f",				"darkslategrey",
				"#32cd32",				"limegreen",
				"#3cb371",				"mediumseagreen",
				"#40e0d0",				"turquoise",
				"#4169e1",				"royalblue",
				"#4682b4",				"steelblue",
				"#483d8b",				"darkslateblue",
				"#48d1cc",				"mediumturquoise",
				"#4b0082",				"indigo",
				"#556b2f",				"darkolivegreen",
				"#5f9ea0",				"cadetblue",
				"#6495ed",				"cornflowerblue",
				"#66cdaa",				"mediumaquamarine",
				"#696969",				"dimgray",
				//"#696969",				"dimgrey",
				"#6a5acd",				"slateblue",
				"#6b8e23",				"olivedrab",
				"#708090",				"slategray",
				//"#708090",				"slategrey",
				"#778899",				"lightslategray",
				//"#778899",				"lightslategrey",
				"#7b68ee",				"mediumslateblue",
				"#7cfc00",				"lawngreen",
				"#7fff00",				"chartreuse",
				"#7fffd4",				"aquamarine",
				"#800000",				"maroon",
				"#800080",				"purple",
				"#808000",				"olive",
				"#808080",				"gray",
				//"#808080",				"grey",
				"#87ceeb",				"skyblue",
				"#87cefa",				"lightskyblue",
				"#8a2be2",				"blueviolet",
				"#8b0000",				"darkred",
				"#8b008b",				"darkmagenta",
				"#8b4513",				"saddlebrown",
				"#8fbc8f",				"darkseagreen",
				"#90ee90",				"lightgreen",
				"#9370db",				"mediumpurple",
				"#9400d3",				"darkviolet",
				"#98fb98",				"palegreen",
				"#9932cc",				"darkorchid",
				"#9acd32",				"yellowgreen",
				"#a0522d",				"sienna",
				"#a52a2a",				"brown",
				"#a9a9a9",				"darkgray",
				//"#a9a9a9",				"darkgrey",
				"#add8e6",				"lightblue",
				"#adff2f",				"greenyellow",
				"#afeeee",				"paleturquoise",
				"#b0c4de",				"lightsteelblue",
				"#b0e0e6",				"powderblue",
				"#b22222",				"firebrick",
				"#b8860b",				"darkgoldenrod",
				"#ba55d3",				"mediumorchid",
				"#bc8f8f",				"rosybrown",
				"#bdb76b",				"darkkhaki",
				"#c0c0c0",				"silver",
				"#c71585",				"mediumvioletred",
				"#cd5c5c",				"indianred",
				"#cd853f",				"peru",
				"#d2691e",				"chocolate",
				"#d2b48c",				"tan",
				"#d3d3d3",				"lightgray",
				//"#d3d3d3",				"lightgrey",
				"#d8bfd8",				"thistle",
				"#da70d6",				"orchid",
				"#daa520",				"goldenrod",
				"#db7093",				"palevioletred",
				"#dc143c",				"crimson",
				"#dcdcdc",				"gainsboro",
				"#dda0dd",				"plum",
				"#deb887",				"burlywood",
				"#e0ffff",				"lightcyan",
				"#e6e6fa",				"lavender",
				"#e9967a",				"darksalmon",
				"#ee82ee",				"violet",
				"#eee8aa",				"palegoldenrod",
				"#f08080",				"lightcoral",
				"#f0e68c",				"khaki",
				"#f0f8ff",				"aliceblue",
				"#f0fff0",				"honeydew",
				"#f0ffff",				"azure",
				"#f4a460",				"sandybrown",
				"#f5deb3",				"wheat",
				"#f5f5dc",				"beige",
				"#f5f5f5",				"whitesmoke",
				"#f5fffa",				"mintcream",
				"#f8f8ff",				"ghostwhite",
				"#fa8072",				"salmon",
				"#faebd7",				"antiquewhite",
				"#faf0e6",				"linen",
				"#fafad2",				"lightgoldenrodyellow",
				"#fdf5e6",				"oldlace",
				"#ff0000",				"red",
				//"#ff00ff",				"fuchsia",
				"#ff00ff",				"magenta",
				"#ff1493",				"deeppink",
				"#ff4500",				"orangered",
				"#ff6347",				"tomato",
				"#ff69b4",				"hotpink",
				"#ff7f50",				"coral",
				"#ff8c00",				"darkorange",
				"#ffa07a",				"lightsalmon",
				"#ffa500",				"orange",
				"#ffb6c1",				"lightpink",
				"#ffc0cb",				"pink",
				"#ffd700",				"gold",
				"#ffdab9",				"peachpuff",
				"#ffdead",				"navajowhite",
				"#ffe4b5",				"moccasin",
				"#ffe4c4",				"bisque",
				"#ffe4e1",				"mistyrose",
				"#ffebcd",				"blanchedalmond",
				"#ffefd5",				"papayawhip",
				"#fff0f5",				"lavenderblush",
				"#fff5ee",				"seashell",
				"#fff8dc",				"cornsilk",
				"#fffacd",				"lemonchiffon",
				"#fffaf0",				"floralwhite",
				"#fffafa",				"snow",
				"#ffff00",				"yellow",
				"#ffffe0",				"lightyellow",
				"#fffff0",				"ivory",
				"#ffffff",				"white"
			};

			return search(NamedColor138R, 0, 138 - 1, hex);
		}

		inline const char* toName4(const char* const hex) noexcept
		{
			if (compare(hex, "#663399") == 0) {
				return "rebeccapurple";
			}
			return toName(hex);
		}

		inline void printInteger(std::ostringstream& o, double f)
		{
			o << std::lround(f);
		}

		inline void printFloat(std::ostringstream& o, double f, double p, bool withoutLeadingZero = false)
		{
			const auto r = round(f, p);
			if (!withoutLeadingZero) {
				o << std::setprecision(14) << std::defaultfloat << r;
				return;
			}

			std::ostringstream o2;
			o2 << std::setprecision(12) << r;
			const auto str = o2.str();
			if (str.length() >= 3U) {
				if (str[0] == '0' && str[1] == '.') {
					o << str.substr(1U);
					return;
				} else if (str[0] == '-' && str[1] == '0' && str[2] == '.') {
					o << '-' << str.substr(2U);
					return;
				}
			}
			o << str;
		}

		inline void printSeparetor(std::ostringstream& o, const char separator)
		{
			o << separator;
			if (separator != ' ') {
				o << ' ';
			}
		}

		inline void printValue(std::ostringstream& o, double f, const char format)
		{
			switch (format) {
				case '!':
					o << std::setprecision(14) << std::defaultfloat << f;
					break;
				case '0':
					printInteger(o, f);
					break;
				case '1':
					printFloat(o, f, 10.0);
					break;
				case '2':
					printFloat(o, f, 100.0);
					break;
				case '3':
					printFloat(o, f, 1000.0);
					break;
				case '4':
					printFloat(o, f, 10000.0);
					break;
				case '5':
					printFloat(o, f, 100000.0);
					break;
				case '6':
					printFloat(o, f, 1000000.0);
					break;
				case '7':
					printFloat(o, f, 10000000.0);
					break;
				case '8':
					printFloat(o, f, 100000000.0);
					break;
				case 'A':
					printInteger(o, f);
					o << '%';
					break;
				case 'B':
					printFloat(o, f, 10.0);
					o << '%';
					break;
				case 'C':
					printFloat(o, f, 100.0);
					o << '%';
					break;
				case 'D':
					printFloat(o, f, 1000.0);
					o << '%';
					break;
				case 'E':
					printFloat(o, f, 10000.0);
					o << '%';
					break;
				case 'F':
					printFloat(o, f, 100000.0);
					o << '%';
					break;
				case 'G':
					printFloat(o, f, 1000000.0);
					o << '%';
					break;
			}
		}

		inline double atan2Deg(double y, double x) noexcept
		{
			if (y == 0.0 && x == 0.0) {
				return 0.0;
			}
			return std::atan2(y, x) * 180.0 / 3.14159265358979323846;
		}

		inline double sinDeg(double x) noexcept
		{
			return std::sin(x / 180.0 * 3.14159265358979323846);
		}

		inline double cosDeg(double x) noexcept
		{
			return std::cos(x / 180.0 * 3.14159265358979323846);
		}
	}


	// A base class of 15 color classes.

	template <typename T> class ColorBase
	{
	protected:
		ColorBase() noexcept
			: m1(0.0), m2(0.0), m3(0.0), ma(1.0) {}

		explicit ColorBase(double al) noexcept
			: m1(0.0), m2(0.0), m3(0.0), ma(al) {}

		ColorBase(double f1, double f2, double f3, double al) noexcept
			: m1(f1), m2(f2), m3(f3), ma(al) {}

		void add(const ColorBase& c) noexcept
		{
			m1 += c.m1;
			m2 += c.m2;
			m3 += c.m3;
			ma += c.ma;
		}

		void sub(const ColorBase& c) noexcept
		{
			m1 -= c.m1;
			m2 -= c.m2;
			m3 -= c.m3;
			ma -= c.ma;
		}

		void mul(const ColorBase& c) noexcept
		{
			m1 *= c.m1;
			m2 *= c.m2;
			m3 *= c.m3;
			ma *= c.ma;
		}

		void div(const ColorBase& c) noexcept
		{
			assert(c.m1 != 0.0 && c.m2 != 0.0 && c.m3 != 0.0 && c.ma != 0.0);
			if (c.m1 == 0.0 || c.m2 == 0.0 || c.m3 == 0.0 || c.ma == 0.0) {
				return;
			}
			m1 /= c.m1;
			m2 /= c.m2;
			m3 /= c.m3;
			ma /= c.ma;
		}

		void add(double f) noexcept
		{
			m1 += f;
			m2 += f;
			m3 += f;
			ma += f;
		}

		void sub(double f) noexcept
		{
			m1 -= f;
			m2 -= f;
			m3 -= f;
			ma -= f;
		}

		void mul(double f) noexcept
		{
			m1 *= f;
			m2 *= f;
			m3 *= f;
			ma *= f;
		}

		void div(double f) noexcept
		{
			assert(f != 0.0);
			if (f == 0.0) {
				return;
			}
			m1 /= f;
			m2 /= f;
			m3 /= f;
			ma /= f;
		}

		bool equals(const ColorBase& c) const noexcept
		{
			return (m1 == c.m1)
				&& (m2 == c.m2)
				&& (m3 == c.m3)
				&& (ma == c.ma);
		}

		bool less(const ColorBase& c) const noexcept
		{
			if (m1 < c.m1) return true;
			if (m1 > c.m1) return false;

			if (m2 < c.m2) return true;
			if (m2 > c.m2) return false;

			if (m3 < c.m3) return true;
			if (m3 > c.m3) return false;

			if (ma < c.ma) return true;
			return false;
		}

		double measureDistance(const ColorBase& c) const noexcept
		{
			const double d1 = m1 - c.m1;
			const double d2 = m2 - c.m2;
			const double d3 = m3 - c.m3;
			return std::sqrt(d1 * d1 + d2 * d2 + d3 * d3);
		}

		void printAlpha(std::ostringstream& o, const char* const separator, const char format) const
		{
			if (ma <= 0.0) {
				o << separator << '0';
				return;
			}

			double n = 0.0;
			bool withoutLeadingZero = false;
			bool percentage = false;
			switch (format) {
				case '!':
					if (!isOpaque(1.0E12)) {
						o << separator << std::setprecision(12) << std::defaultfloat << ma;
					}
					return;

				case '+':
					withoutLeadingZero = true;
					n = 1000.0;
					break;
				case '-':
					withoutLeadingZero = true;
					n = 10000.0;
					break;
				case '*':
					withoutLeadingZero = true;
					n = 100000.0;
					break;
				case '/':
					withoutLeadingZero = true;
					n = 1000000.0;
					break;

				case '1':
					n = 10.0;
					break;
				case '2':
					n = 100.0;
					break;
				case '3':
					n = 1000.0;
					break;
				case '4':
					n = 10000.0;
					break;
				case '5':
					n = 100000.0;
					break;
				case '6':
					n = 1000000.0;
					break;
				case '7':
					n = 10000000.0;
					break;
				case '8':
					n = 100000000.0;
					break;

				case 'A':
					percentage = true;
					n = 100.0;
					break;
				case 'B':
					percentage = true;
					n = 1000.0;
					break;
				case 'C':
					percentage = true;
					n = 10000.0;
					break;
				case 'D':
					percentage = true;
					n = 100000.0;
					break;
				case 'E':
					percentage = true;
					n = 1000000.0;
					break;
				case 'F':
					percentage = true;
					n = 10000000.0;
					break;
				case 'G':
					percentage = true;
					n = 100000000.0;
					break;
			}
			if (n > 0.0 && !isOpaque(n)) {
				o << separator;
				if (percentage) {
					impl::printFloat(o, ma * 100.0, n / 100.0, withoutLeadingZero);
					o << '%';
				} else {
					impl::printFloat(o, ma, n, withoutLeadingZero);
				}
			}
		}

		void printColor(std::ostringstream& o, const char* const s, const char (&format)[6], bool asColorFuntion = false) const
		{
			if (asColorFuntion) {
				o << "color(" << s;
				impl::printSeparetor(o, format[0]);
			} else {
				o << s << '(';
			}
			impl::printValue(o, m1, format[1]);
			impl::printSeparetor(o, format[0]);
			impl::printValue(o, m2, format[2]);
			impl::printSeparetor(o, format[0]);
			impl::printValue(o, m3, format[3]);
			if (format[0] == ',') {
				printAlpha(o, ", ", format[4]);
			} else {
				printAlpha(o, " / ", format[4]);
			}
			o << ')';
			return;
		}

	public:
		double alpha() const noexcept
		{
			return ma;
		}

		bool isOpaque() const noexcept
		{
			return ma >= 1.0;
		}

		bool isOpaque(double p) const noexcept
		{
			return ma >= 1.0 || std::round(ma * p) >= p;
		}

		std::string vec(const char(&format)[6] = ",!!!!", const char* name = "", bool asColorFuntion = false) const
		{
			std::ostringstream o;
			printColor(o, name, format, asColorFuntion);
			return o.str();
		}

		double dot(double f1, double f2, double f3) const noexcept
		{
			return m1 * f1 + m2 * f2 + m3 * f3;
		}

		friend bool operator==(const T& t1, const T& t2) noexcept
		{
			return t1.equals(t2);
		}

		friend bool operator!= (const T& t1, const T& t2) noexcept
		{
			return !t1.equals(t2);
		}

		friend bool operator<(const T& t1, const T& t2) noexcept
		{
			return t1.less(t2);
		}

		friend bool operator>(const T& t1, const T& t2) noexcept
		{
			return !t1.less(t2) && !t1.equals(t2);
		}

		friend bool operator<=(const T& t1, const T& t2) noexcept
		{
			return t1.less(t2) || t1.equals(t2);
		}

		friend bool operator>=(const T& t1, const T& t2) noexcept
		{
			return !t1.less(t2);
		}

		friend T operator+(const T& t1, const T& t2) noexcept
		{
			T t(t1);
			t.add(t2);
			return t;
		}

		friend T operator-(const T& t1, const T& t2) noexcept
		{
			T t(t1);
			t.sub(t2);
			return t;
		}

		friend T operator*(const T& t1, const T& t2) noexcept
		{
			T t(t1);
			t.mul(t2);
			return t;
		}

		friend T operator/(const T& t1, const T& t2) noexcept
		{
			T t(t1);
			t.div(t2);
			return t;
		}

		friend T& operator+=(T& t1, const T& t2) noexcept
		{
			t1.add(t2);
			return t1;
		}

		friend T& operator-=(T& t1, const T& t2) noexcept
		{
			t1.sub(t2);
			return t1;
		}

		friend T& operator*=(T& t1, const T& t2) noexcept
		{
			t1.mul(t2);
			return t1;
		}

		friend T& operator/=(T& t1, const T& t2) noexcept
		{
			t1.div(t2);
			return t1;
		}

		friend T operator+(const T& t1, double f) noexcept
		{
			T t(t1);
			t.add(f);
			return t;
		}

		friend T operator-(const T& t1, double f) noexcept
		{
			T t(t1);
			t.sub(f);
			return t;
		}

		friend T operator*(const T& t1, double f) noexcept
		{
			T t(t1);
			t.mul(f);
			return t;
		}

		friend T operator/(const T& t1, double f) noexcept
		{
			T t(t1);
			t.div(f);
			return t;
		}

		friend T& operator+=(T& t1, double f) noexcept
		{
			t1.add(f);
			return t1;
		}

		friend T& operator-=(T& t1, double f) noexcept
		{
			t1.sub(f);
			return t1;
		}

		friend T& operator*=(T& t1, double f) noexcept
		{
			t1.mul(f);
			return t1;
		}

		friend T& operator/=(T& t1, double f) noexcept
		{
			t1.div(f);
			return t1;
		}

	protected:
			double m1, m2, m3, ma;
	};


	// 15 color classes.

	class Rgb : public ColorBase<Rgb>
	{
	public:
		Rgb() noexcept {}

		Rgb(double r, double g, double b, double al = 1.0) noexcept
			: ColorBase(r, g, b, al) {}

		explicit Rgb(unsigned int num, double al = 1.0) noexcept
			: ColorBase(al)
		{
			setRed(static_cast<double>((num & 0xff0000) >> 16));
			setGreen(static_cast<double>((num & 0xff00) >> 8));
			setBlue(static_cast<double>(num & 0xff));
		}

		explicit Rgb(Cn name, double al = 1.0) noexcept
			: Rgb(static_cast<unsigned int>(name), al) {}

		inline explicit Rgb(const char* const s);

		inline explicit Rgb(const std::string& s);

		inline explicit Rgb(const Hsl& hsl) noexcept;

		inline explicit Rgb(const Hwb& hwb) noexcept;

		inline explicit Rgb(const Lrgb& lrgb) noexcept;

		inline explicit Rgb(const XyzD50& xyzD50) noexcept;

		inline explicit Rgb(const Lab& lab) noexcept;

		inline explicit Rgb(const Lch& lch) noexcept;

		inline explicit Rgb(const XyzD65& xyzD65) noexcept;

		inline explicit Rgb(const Oklab& oklab) noexcept;

		inline explicit Rgb(const Oklch& oklch) noexcept;

		inline explicit Rgb(const Srgb& srgb) noexcept;

		inline explicit Rgb(const DisplayP3& displayP3) noexcept;

		inline explicit Rgb(const A98Rgb& a98Rgb) noexcept;

		inline explicit Rgb(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Rgb(const Rec2020& rec2020) noexcept;

		Rgb& setRed(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Rgb& setGreen(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Rgb& setBlue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Rgb& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double red() const noexcept
		{
			return m1;
		}

		double green() const noexcept
		{
			return m2;
		}

		double blue() const noexcept
		{
			return m3;
		}

		Rgb& setRed8(unsigned int u) noexcept
		{
			if (u >= 255U) {
				m1 = 255.0;
			} else {
				m1 = static_cast<double>(u);
			}
			return *this;
		}

		Rgb& setGreen8(unsigned int u) noexcept
		{
			if (u >= 255U) {
				m2 = 255.0;
			} else {
				m2 = static_cast<double>(u);
			}
			return *this;
		}

		Rgb& setBlue8(unsigned int u) noexcept
		{
			if (u >= 255U) {
				m3 = 255.0;
			} else {
				m3 = static_cast<double>(u);
			}
			return *this;
		}

		Rgb& setAlpha8(unsigned int u) noexcept
		{
			if (u >= 255U) {
				ma = 1.0;
			} else {
				ma = static_cast<double>(u) / 255.0;
			}
			return *this;
		}

		unsigned int red8() const noexcept
		{
			return static_cast<unsigned int>(impl::double255ToInteger(m1));
		}

		unsigned int green8() const noexcept
		{
			return static_cast<unsigned int>(impl::double255ToInteger(m2));
		}

		unsigned int blue8() const noexcept
		{
			return static_cast<unsigned int>(impl::double255ToInteger(m3));
		}

		unsigned int alpha8() const noexcept
		{
			return static_cast<unsigned int>(impl::double255ToInteger(ma * 255.0));
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline Rgb invert() const noexcept;

		inline Rgb blendNormal(const Rgb& rgb) const noexcept;

		inline Rgb blendMultiply(const Rgb& rgb) const noexcept;

		inline Rgb blendDarken(const Rgb& rgb) const noexcept;

		inline Rgb blendLighten(const Rgb& rgb) const noexcept;

		inline Rgb blendScreen(const Rgb& rgb) const noexcept;

		inline Rgb blendOverlay(const Rgb& rgb) const noexcept;

		inline Rgb blendColorDodge(const Rgb& rgb) const noexcept;

		inline Rgb blendColorBurn(const Rgb& rgb) const noexcept;

		inline Rgb blendHardLight(const Rgb& rgb) const noexcept;

		inline Rgb blendSoftLight(const Rgb& rgb) const noexcept;

		inline Rgb blendDifference(const Rgb& rgb) const noexcept;

		inline Rgb blendExclusion(const Rgb& rgb) const noexcept;

		inline Rgb blendHue(const Rgb& rgb) const noexcept;

		inline Rgb blendSaturation(const Rgb& rgb) const noexcept;

		inline Rgb blendColor(const Rgb& rgb) const noexcept;

		inline Rgb blendLuminosity(const Rgb& rgb) const noexcept;

		inline bool isDisplayable() const noexcept;

		inline Rgb clip() const noexcept;

		inline Rgb round() const noexcept;

		inline Rgb fit() const noexcept;

		inline double distance(const Rgb& rgb) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Rgb maximizeLightness() noexcept;

		inline Rgb brighten(double f = 0.1) const noexcept;

		inline Rgb darken(double f = 0.1) const noexcept;

		inline Rgb saturate(double f = 0.04) const noexcept;

		inline Rgb desaturate(double f = 0.04) const noexcept;

		inline Rgb rotate(double f = 36.0) const noexcept;

		inline Rgb flip(double f = 0.0) const noexcept;

		inline Rgb mix(const Rgb& rgb, double f = 0.5) const noexcept;

		inline double deltaE_00(const Rgb& rgb) const noexcept;

		inline double deltaE(const Rgb& rgb) const noexcept;

		inline Rgb grayscale() const noexcept;

		inline double contrast(const Rgb& rgb) const noexcept;

	private:

		inline double nonLinear(double value) const noexcept;

		inline void getMinMaxHue(double& min, double& max, double& hue) const noexcept;

		friend class Hsl;
		friend class Hwb;
	};

	class Hsl : public ColorBase<Hsl>
	{
	public:
		Hsl() noexcept {}

		Hsl(double h, double s, double l, double al = 1.0) noexcept
			: ColorBase(h, s, l, al) {}

		explicit Hsl(unsigned int num, double al = 1.0) noexcept
			: Hsl(Rgb(num, al)) {}

		explicit Hsl(Cn name, double al = 1.0) noexcept
			: Hsl(static_cast<unsigned int>(name), al) {}

		inline explicit Hsl(const char* const s);

		inline explicit Hsl(const std::string& s);

		inline explicit Hsl(const Rgb& rgb) noexcept;

		inline explicit Hsl(const Hwb& hwb) noexcept;

		inline explicit Hsl(const Lrgb& lrgb) noexcept;

		inline explicit Hsl(const XyzD50& xyzD50) noexcept;

		inline explicit Hsl(const Lab& lab) noexcept;

		inline explicit Hsl(const Lch& lch) noexcept;

		inline explicit Hsl(const XyzD65& xyzD65) noexcept;

		inline explicit Hsl(const Oklab& oklab) noexcept;

		inline explicit Hsl(const Oklch& oklch) noexcept;

		inline explicit Hsl(const Srgb& srgb) noexcept;

		inline explicit Hsl(const DisplayP3& displayP3) noexcept;

		inline explicit Hsl(const A98Rgb& a98Rgb) noexcept;

		inline explicit Hsl(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Hsl(const Rec2020& rec2020) noexcept;

		Hsl& setHue(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Hsl& setSaturation(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Hsl& setLightness(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Hsl& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double hue() const noexcept
		{
			return m1;
		}

		double saturation() const noexcept
		{
			return m2;
		}

		double lightness() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline bool hasPowerlessHue() const noexcept;

		inline Hsl clip() const noexcept;

		inline Hsl fit() const noexcept;

		inline double distance(const Hsl& hsl) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Hsl maximizeLightness() noexcept;

		inline Hsl brighten(double f = 0.1) const noexcept;

		inline Hsl darken(double f = 0.1) const noexcept;

		inline Hsl saturate(double f = 0.04) const noexcept;

		inline Hsl desaturate(double f = 0.04) const noexcept;

		inline Hsl rotate(double f = 36.0) const noexcept;

		inline Hsl flip(double f = 0.0) const noexcept;

		inline Hsl mix(const Hsl& hsl, double f = 0.5) const noexcept;

		inline Hsl grayscale() const noexcept;

		inline double deltaE_00(const Hsl& hsl) const noexcept;

		inline double deltaE(const Hsl& hsl) const noexcept;

		inline double contrast(const Hsl& hsl) const noexcept;
	};

	class Hwb : public ColorBase<Hwb>
	{
	public:
		Hwb() noexcept {}

		Hwb(double h, double w, double b, double al = 1.0) noexcept
			: ColorBase(h, w, b, al) {}

		explicit Hwb(unsigned int num, double al = 1.0) noexcept
			: Hwb(Rgb(num, al)) {}

		explicit Hwb(Cn name, double al = 1.0) noexcept
			: Hwb(static_cast<unsigned int>(name), al) {}

		inline explicit Hwb(const char* const s);

		inline explicit Hwb(const std::string& s);

		inline explicit Hwb(const Rgb& rgb) noexcept;

		inline explicit Hwb(const Hsl& hsl) noexcept;

		inline explicit Hwb(const Lrgb& lrgb) noexcept;

		inline explicit Hwb(const XyzD50& xyzD50) noexcept;

		inline explicit Hwb(const Lab& lab) noexcept;

		inline explicit Hwb(const Lch& lch) noexcept;

		inline explicit Hwb(const XyzD65& xyzD65) noexcept;

		inline explicit Hwb(const Oklab& oklab) noexcept;

		inline explicit Hwb(const Oklch& oklch) noexcept;

		inline explicit Hwb(const Srgb& srgb) noexcept;

		inline explicit Hwb(const DisplayP3& displayP3) noexcept;

		inline explicit Hwb(const A98Rgb& a98Rgb) noexcept;

		inline explicit Hwb(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Hwb(const Rec2020& rec2020) noexcept;

		Hwb& setHue(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Hwb& setWhiteness(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Hwb& setBlackness(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Hwb& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double hue() const noexcept
		{
			return m1;
		}

		double whiteness() const noexcept
		{
			return m2;
		}

		double blackness() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline bool hasPowerlessHue() const noexcept;

		inline Hwb clip() const noexcept;

		inline Hwb fit() const noexcept;

		inline double distance(const Hwb& hwb) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Hwb maximizeLightness() noexcept;

		inline Hwb brighten(double f = 0.1) const noexcept;

		inline Hwb darken(double f = 0.1) const noexcept;

		inline Hwb saturate(double f = 0.04) const noexcept;

		inline Hwb desaturate(double f = 0.04) const noexcept;

		inline Hwb rotate(double f = 36.0) const noexcept;

		inline Hwb flip(double f = 0.0) const noexcept;

		inline Hwb mix(const Hwb& hwb, double f = 0.5) const noexcept;

		inline Hwb grayscale() const noexcept;

		inline double deltaE_00(const Hwb& hwb) const noexcept;

		inline double deltaE(const Hwb& hwb) const noexcept;

		inline double contrast(const Hwb& hwb) const noexcept;
	};

	class Lrgb : public ColorBase<Lrgb>
	{
	public:
		Lrgb() noexcept {}

		Lrgb(double r, double g, double b, double al = 1.0) noexcept
			: ColorBase(r, g, b, al) {}

		explicit Lrgb(unsigned int num, double al = 1.0) noexcept
			: Lrgb(Rgb(num, al)) {}

		explicit Lrgb(Cn name, double al = 1.0) noexcept
			: Lrgb(static_cast<unsigned int>(name), al) {}

		inline explicit Lrgb(const char* const s);

		inline explicit Lrgb(const std::string& s);

		inline explicit Lrgb(const Rgb& rgb) noexcept;

		inline explicit Lrgb(const Hsl& hsl) noexcept;

		inline explicit Lrgb(const Hwb& hwb) noexcept;

		inline explicit Lrgb(const XyzD50& xyzD50) noexcept;

		inline explicit Lrgb(const Lab& lab) noexcept;

		inline explicit Lrgb(const Lch& lch) noexcept;

		inline explicit Lrgb(const XyzD65& xyzD65) noexcept;

		inline explicit Lrgb(const Oklab& oklab) noexcept;

		inline explicit Lrgb(const Oklch& oklch) noexcept;

		inline explicit Lrgb(const Srgb& srgb) noexcept;

		inline explicit Lrgb(const DisplayP3& displayP3) noexcept;

		inline explicit Lrgb(const A98Rgb& a98Rgb) noexcept;

		inline explicit Lrgb(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Lrgb(const Rec2020& rec2020) noexcept;

		Lrgb& setRed(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Lrgb& setGreen(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Lrgb& setBlue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Lrgb& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double red() const noexcept
		{
			return m1;
		}

		double green() const noexcept
		{
			return m2;
		}

		double blue() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline Lrgb clip() const noexcept;

		inline Lrgb fit() const noexcept;

		inline double distance(const Lrgb& lrgb) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Lrgb maximizeLightness() noexcept;

		inline Lrgb brighten(double f = 0.1) const noexcept;

		inline Lrgb darken(double f = 0.1) const noexcept;

		inline Lrgb saturate(double f = 0.04) const noexcept;

		inline Lrgb desaturate(double f = 0.04) const noexcept;

		inline Lrgb rotate(double f = 36.0) const noexcept;

		inline Lrgb flip(double f = 0.0) const noexcept;

		inline Lrgb mix(const Lrgb& lrgb, double f = 0.5) const noexcept;

		inline Lrgb grayscale() const noexcept;

		inline double deltaE_00(const Lrgb& lrgb) const noexcept;

		inline double deltaE(const Lrgb& lrgb) const noexcept;

		inline double contrast(const Lrgb& lrgb) const noexcept;

	private:
		inline double linear(double value) const noexcept;

		inline double linearS(double value) const noexcept;
	};

	class XyzD50 : public ColorBase<XyzD50>
	{
	public:
		XyzD50() noexcept {}

		XyzD50(double xx, double yy, double zz, double al = 1.0) noexcept
			: ColorBase(xx, yy, zz, al) {}

		explicit XyzD50(unsigned int num, double al = 1.0) noexcept
			: XyzD50(Rgb(num, al)) {}

		explicit XyzD50(Cn name, double al = 1.0) noexcept
			: XyzD50(static_cast<unsigned int>(name), al) {}

		inline explicit XyzD50(const char* const s);

		inline explicit XyzD50(const std::string& s);

		inline explicit XyzD50(const Rgb& rgb) noexcept;

		inline explicit XyzD50(const Hsl& hsl) noexcept;

		inline explicit XyzD50(const Hwb& hwb) noexcept;

		inline explicit XyzD50(const Lrgb& lrgb) noexcept;

		inline explicit XyzD50(const Lab& lab) noexcept;

		inline explicit XyzD50(const Lch& lch) noexcept;

		inline explicit XyzD50(const XyzD65& xyzD65) noexcept;

		inline explicit XyzD50(const Oklab& oklab) noexcept;

		inline explicit XyzD50(const Oklch& oklch) noexcept;

		inline explicit XyzD50(const Srgb& srgb) noexcept;

		inline explicit XyzD50(const DisplayP3& displayP3) noexcept;

		inline explicit XyzD50(const A98Rgb& a98Rgb) noexcept;

		inline explicit XyzD50(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit XyzD50(const Rec2020& rec2020) noexcept;

		XyzD50& setX(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		XyzD50& setY(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		XyzD50& setZ(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		XyzD50& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double x() const noexcept
		{
			return m1;
		}

		double y() const noexcept
		{
			return m2;
		}

		double z() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline XyzD50 clip() const noexcept;

		inline XyzD50 fit() const noexcept;

		inline double distance(const XyzD50& xyzD50) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline XyzD50 maximizeLightness() noexcept;

		inline XyzD50 brighten(double f = 0.1) const noexcept;

		inline XyzD50 darken(double f = 0.1) const noexcept;

		inline XyzD50 saturate(double f = 0.04) const noexcept;

		inline XyzD50 desaturate(double f = 0.04) const noexcept;

		inline XyzD50 rotate(double f = 36.0) const noexcept;

		inline XyzD50 flip(double f = 0.0) const noexcept;

		inline XyzD50 mix(const XyzD50& xyzD50, double f = 0.5) const noexcept;

		inline XyzD50 grayscale() const noexcept;

		inline double deltaE_00(const XyzD50& xyzD50) const noexcept;

		inline double deltaE(const XyzD50& xyzD50) const noexcept;

		inline double contrast(const XyzD50& xyzD50) const noexcept;

	private:
		inline double fi(double t) const noexcept;
	};

	class Lab : public ColorBase<Lab>
	{
	public:
		Lab() noexcept {}

		Lab(double l, double aa, double bb, double al = 1.0) noexcept
			: ColorBase(l, aa, bb, al) {}

		explicit Lab(unsigned int num, double al = 1.0) noexcept
			: Lab(Rgb(num, al)) {}

		explicit Lab(Cn name, double al = 1.0) noexcept
			: Lab(static_cast<unsigned int>(name), al) {}

		inline explicit Lab(const char* const s);

		inline explicit Lab(const std::string& s);

		inline explicit Lab(const Rgb& rgb) noexcept;

		inline explicit Lab(const Hsl& hsl) noexcept;

		inline explicit Lab(const Hwb& hwb) noexcept;

		inline explicit Lab(const Lrgb& lrgb) noexcept;

		inline explicit Lab(const XyzD50& xyzD50) noexcept;

		inline explicit Lab(const Lch& lch) noexcept;

		inline explicit Lab(const XyzD65& xyzD65) noexcept;

		inline explicit Lab(const Oklab& oklab) noexcept;

		inline explicit Lab(const Oklch& oklch) noexcept;

		inline explicit Lab(const Srgb& srgb) noexcept;

		inline explicit Lab(const DisplayP3& displayP3) noexcept;

		inline explicit Lab(const A98Rgb& a98Rgb) noexcept;

		inline explicit Lab(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Lab(const Rec2020& rec2020) noexcept;

		Lab& setLightness(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Lab& setA(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Lab& setB(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Lab& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double lightness() const noexcept
		{
			return m1;
		}

		double a() const noexcept
		{
			return m2;
		}

		double b() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline Lab clip() const noexcept;

		inline Lab fit() const noexcept;

		inline double distance(const Lab& lab) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Lab maximizeLightness() noexcept;

		inline Lab brighten(double f = 0.1) const noexcept;

		inline Lab darken(double f = 0.1) const noexcept;

		inline Lab saturate(double f = 0.04) const noexcept;

		inline Lab desaturate(double f = 0.04) const noexcept;

		inline Lab rotate(double f = 36.0) const noexcept;

		inline Lab flip(double f = 0.0) const noexcept;

		inline Lab mix(const Lab& lab, double f = 0.5) const noexcept;

		inline Lab grayscale() const noexcept;

		inline double deltaE_00(const Lab& lab) const noexcept;

		inline double deltaE(const Lab& lab) const noexcept;

		inline double contrast(const Lab& lab) const noexcept;

	private:
		inline double fn(double t) const noexcept;
	};

	class Lch : public ColorBase<Lch>
	{
	public:
		Lch() noexcept {}

		Lch(double l, double c, double h, double al = 1.0) noexcept
			: ColorBase(l, c, h, al) {}

		explicit Lch(unsigned int num, double al = 1.0) noexcept
			: Lch(Rgb(num, al)) {}

		explicit Lch(Cn name, double al = 1.0) noexcept
			: Lch(static_cast<unsigned int>(name), al) {}

		inline explicit Lch(const char* const s);

		inline explicit Lch(const std::string& s);

		inline explicit Lch(const Rgb& rgb) noexcept;

		inline explicit Lch(const Hsl& hsl) noexcept;

		inline explicit Lch(const Hwb& hwb) noexcept;

		inline explicit Lch(const Lrgb& lrgb) noexcept;

		inline explicit Lch(const XyzD50& xyzD50) noexcept;

		inline explicit Lch(const Lab& lab) noexcept;

		inline explicit Lch(const XyzD65& xyzD65) noexcept;

		inline explicit Lch(const Oklab& oklab) noexcept;

		inline explicit Lch(const Oklch& oklch) noexcept;

		inline explicit Lch(const Srgb& srgb) noexcept;

		inline explicit Lch(const DisplayP3& displayP3) noexcept;

		inline explicit Lch(const A98Rgb& a98Rgb) noexcept;

		inline explicit Lch(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Lch(const Rec2020& rec2020) noexcept;

		Lch& setLightness(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Lch& setChroma(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Lch& setHue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Lch& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double lightness() const noexcept
		{
			return m1;
		}

		double chroma() const noexcept
		{
			return m2;
		}

		double hue() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline bool hasPowerlessHue() const noexcept;

		inline Lch clip() const noexcept;

		inline Lch fit() const noexcept;

		inline double distance(const Lch& lch) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Lch maximizeLightness() noexcept;

		inline Lch brighten(double f = 0.1) const noexcept;

		inline Lch darken(double f = 0.1) const noexcept;

		inline Lch saturate(double f = 0.04) const noexcept;

		inline Lch desaturate(double f = 0.04) const noexcept;

		inline Lch rotate(double f = 36.0) const noexcept;

		inline Lch flip(double f = 0.0) const noexcept;

		inline Lch mix(const Lch& lch, double f = 0.5) const noexcept;

		inline Lch grayscale() const noexcept;

		inline double deltaE_00(const Lch& lch) const noexcept;

		inline double deltaE(const Lch& lch) const noexcept;

		inline double contrast(const Lch& lch) const noexcept;
	};

	class XyzD65 : public ColorBase<XyzD65>
	{
	public:
		XyzD65() noexcept {}

		XyzD65(double xx, double yy, double zz, double al = 1.0) noexcept
			: ColorBase(xx, yy, zz, al) {}

		explicit XyzD65(unsigned int num, double al = 1.0) noexcept
			: XyzD65(Rgb(num, al)) {}

		explicit XyzD65(Cn name, double al = 1.0) noexcept
			: XyzD65(static_cast<unsigned int>(name), al) {}

		inline explicit XyzD65(const char* const s);

		inline explicit XyzD65(const std::string& s);

		inline explicit XyzD65(const Rgb& rgb) noexcept;

		inline explicit XyzD65(const Hsl& hsl) noexcept;

		inline explicit XyzD65(const Hwb& hwb) noexcept;

		inline explicit XyzD65(const Lrgb& lrgb) noexcept;

		inline explicit XyzD65(const XyzD50& xyzD50) noexcept;

		inline explicit XyzD65(const Lab& lab) noexcept;

		inline explicit XyzD65(const Lch& lch) noexcept;

		inline explicit XyzD65(const Oklab& oklab) noexcept;

		inline explicit XyzD65(const Oklch& oklch) noexcept;

		inline explicit XyzD65(const Srgb& srgb) noexcept;

		inline explicit XyzD65(const DisplayP3& displayP3) noexcept;

		inline explicit XyzD65(const A98Rgb& a98Rgb) noexcept;

		inline explicit XyzD65(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit XyzD65(const Rec2020& rec2020) noexcept;

		XyzD65& setX(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		XyzD65& setY(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		XyzD65& setZ(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		XyzD65& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double x() const noexcept
		{
			return m1;
		}

		double y() const noexcept
		{
			return m2;
		}

		double z() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline XyzD65 clip() const noexcept;

		inline XyzD65 fit() const noexcept;

		inline double distance(const XyzD65& xyzD65) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline XyzD65 maximizeLightness() noexcept;

		inline XyzD65 brighten(double f = 0.1) const noexcept;

		inline XyzD65 darken(double f = 0.1) const noexcept;

		inline XyzD65 saturate(double f = 0.04) const noexcept;

		inline XyzD65 desaturate(double f = 0.04) const noexcept;

		inline XyzD65 rotate(double f = 36.0) const noexcept;

		inline XyzD65 flip(double f = 0.0) const noexcept;

		inline XyzD65 mix(const XyzD65& xyzD65, double f = 0.5) const noexcept;

		inline XyzD65 grayscale() const noexcept;

		inline double deltaE_00(const XyzD65& xyzD65) const noexcept;

		inline double deltaE(const XyzD65& xyzD65) const noexcept;

		inline double contrast(const XyzD65& xyzD65) const noexcept;

	private:
		inline double linearD(double value) const noexcept;
		
		inline double linearA(double value) const noexcept;

		inline double linearP(double value) const noexcept;

		inline double linearR(double value) const noexcept;
	};

	class Oklab : public ColorBase<Oklab>
	{
	public:
		Oklab() noexcept {}

		Oklab(double l, double aa, double bb, double al = 1.0) noexcept
			: ColorBase(l, aa, bb, al) {}

		explicit Oklab(unsigned int num, double al = 1.0) noexcept
			: Oklab(Rgb(num, al)) {}

		explicit Oklab(Cn name, double al = 1.0) noexcept
			: Oklab(static_cast<unsigned int>(name), al) {}

		inline explicit Oklab(const char* const s);

		inline explicit Oklab(const std::string& s);

		inline explicit Oklab(const Rgb& rgb) noexcept;

		inline explicit Oklab(const Hsl& hsl) noexcept;

		inline explicit Oklab(const Hwb& hwb) noexcept;

		inline explicit Oklab(const Lrgb& lrgb) noexcept;

		inline explicit Oklab(const XyzD50& xyzD50) noexcept;

		inline explicit Oklab(const Lab& lab) noexcept;

		inline explicit Oklab(const Lch& lch) noexcept;

		inline explicit Oklab(const XyzD65& xyzD65) noexcept;

		inline explicit Oklab(const Oklch& oklch) noexcept;

		inline explicit Oklab(const Srgb& srgb) noexcept;

		inline explicit Oklab(const DisplayP3& displayP3) noexcept;

		inline explicit Oklab(const A98Rgb& a98Rgb) noexcept;

		inline explicit Oklab(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Oklab(const Rec2020& rec2020) noexcept;

		Oklab& setLightness(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Oklab& setA(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Oklab& setB(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Oklab& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double lightness() const noexcept
		{
			return m1;
		}

		double a() const noexcept
		{
			return m2;
		}

		double b() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline Oklab clip() const noexcept;

		inline Oklab fit() const noexcept;

		inline double distance(const Oklab& oklab) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Oklab maximizeLightness() noexcept;

		inline Oklab brighten(double f = 0.1) const noexcept;

		inline Oklab darken(double f = 0.1) const noexcept;

		inline Oklab saturate(double f = 0.04) const noexcept;

		inline Oklab desaturate(double f = 0.04) const noexcept;

		inline Oklab rotate(double f = 36.0) const noexcept;

		inline Oklab flip(double f = 0.0) const noexcept;

		inline Oklab mix(const Oklab& oklab, double f = 0.5) const noexcept;

		inline Oklab grayscale() const noexcept;

		inline double deltaE_00(const Oklab& oklab) const noexcept;

		inline double deltaE(const Oklab& oklab) const noexcept;

		inline double contrast(const Oklab& oklab) const noexcept;
	};


	class Oklch : public ColorBase<Oklch>
	{
	public:
		Oklch() noexcept {}

		Oklch(double l, double c, double h, double al = 1.0) noexcept
			: ColorBase(l, c, h, al) {}

		explicit Oklch(unsigned int num, double al = 1.0) noexcept
			: Oklch(Rgb(num, al)) {}

		explicit Oklch(Cn name, double al = 1.0) noexcept
			: Oklch(static_cast<unsigned int>(name), al) {}

		inline explicit Oklch(const char* const s);

		inline explicit Oklch(const std::string& s);

		inline explicit Oklch(const Rgb& rgb) noexcept;

		inline explicit Oklch(const Hsl& hsl) noexcept;

		inline explicit Oklch(const Hwb& hwb) noexcept;

		inline explicit Oklch(const Lrgb& lrgb) noexcept;

		inline explicit Oklch(const XyzD50& xyzD50) noexcept;

		inline explicit Oklch(const Lab& lab) noexcept;

		inline explicit Oklch(const Lch& lch) noexcept;

		inline explicit Oklch(const XyzD65& xyzD65) noexcept;

		inline explicit Oklch(const Oklab& oklab) noexcept;

		inline explicit Oklch(const Srgb& srgb) noexcept;

		inline explicit Oklch(const DisplayP3& displayP3) noexcept;

		inline explicit Oklch(const A98Rgb& a98Rgb) noexcept;

		inline explicit Oklch(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Oklch(const Rec2020& rec2020) noexcept;

		Oklch& setLightness(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Oklch& setChroma(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Oklch& setHue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Oklch& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double lightness() const noexcept
		{
			return m1;
		}

		double chroma() const noexcept
		{
			return m2;
		}

		double hue() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline bool hasPowerlessHue() const noexcept;

		inline Oklch clip() const noexcept;

		inline Oklch fit() const noexcept;

		inline double distance(const Oklch& oklch) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Oklch maximizeLightness() noexcept;

		inline Oklch brighten(double f = 0.1) const noexcept;

		inline Oklch darken(double f = 0.1) const noexcept;

		inline Oklch saturate(double f = 0.04) const noexcept;

		inline Oklch desaturate(double f = 0.04) const noexcept;

		inline Oklch rotate(double f = 36.0) const noexcept;

		inline Oklch flip(double f = 0.0) const noexcept;

		inline Oklch mix(const Oklch& oklch, double f = 0.5) const noexcept;

		inline Oklch grayscale() const noexcept;

		inline double deltaE_00(const Oklch& oklch) const noexcept;

		inline double deltaE(const Oklch& oklch) const noexcept;

		inline double contrast(const Oklch& oklch) const noexcept;
	};

	class Srgb : public ColorBase<Srgb>
	{
	public:
		Srgb() noexcept {}

		Srgb(double r, double g, double b, double al = 1.0) noexcept
			: ColorBase(r, g, b, al) {}

		explicit Srgb(unsigned int num, double al = 1.0) noexcept
			: Srgb(Rgb(num, al)) {}

		explicit Srgb(Cn name, double al = 1.0) noexcept
			: Srgb(static_cast<unsigned int>(name), al) {}

		inline explicit Srgb(const char* const s);

		inline explicit Srgb(const std::string& s);

		inline explicit Srgb(const Rgb& rgb) noexcept;

		inline explicit Srgb(const Hsl& hsl) noexcept;

		inline explicit Srgb(const Hwb& hwb) noexcept;

		inline explicit Srgb(const Lrgb& lrgb) noexcept;

		inline explicit Srgb(const XyzD50& xyzD50) noexcept;

		inline explicit Srgb(const Lab& lab) noexcept;

		inline explicit Srgb(const Lch& lch) noexcept;

		inline explicit Srgb(const XyzD65& xyzD65) noexcept;

		inline explicit Srgb(const Oklab& oklab) noexcept;

		inline explicit Srgb(const Oklch& oklch) noexcept;

		inline explicit Srgb(const DisplayP3& displayP3) noexcept;

		inline explicit Srgb(const A98Rgb& a98Rgb) noexcept;

		inline explicit Srgb(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit Srgb(const Rec2020& rec2020) noexcept;

		Srgb& setRed(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Srgb& setGreen(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Srgb& setBlue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Srgb& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double red() const noexcept
		{
			return m1;
		}

		double green() const noexcept
		{
			return m2;
		}

		double blue() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline Srgb clip() const noexcept;

		inline Srgb fit() const noexcept;

		inline double distance(const Srgb& srgb) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Srgb maximizeLightness() noexcept;

		inline Srgb brighten(double f = 0.1) const noexcept;

		inline Srgb darken(double f = 0.1) const noexcept;

		inline Srgb saturate(double f = 0.04) const noexcept;

		inline Srgb desaturate(double f = 0.04) const noexcept;

		inline Srgb rotate(double f = 36.0) const noexcept;

		inline Srgb flip(double f = 0.0) const noexcept;

		inline Srgb mix(const Srgb& srgb, double f = 0.5) const noexcept;

		inline Srgb grayscale() const noexcept;

		inline double deltaE_00(const Srgb& srgb) const noexcept;

		inline double deltaE(const Srgb& srgb) const noexcept;

		inline double contrast(const Srgb& srgb) const noexcept;

	private:
		inline double nonLinear(double value) const noexcept;
	};

	class DisplayP3 : public ColorBase<DisplayP3>
	{
	public:
		DisplayP3() noexcept {}

		DisplayP3(double r, double g, double b, double al = 1.0) noexcept
			: ColorBase(r, g, b, al) {}

		explicit DisplayP3(unsigned int num, double al = 1.0) noexcept
			: DisplayP3(Rgb(num, al)) {}

		explicit DisplayP3(Cn name, double al = 1.0) noexcept
			: DisplayP3(static_cast<unsigned int>(name), al) {}

		inline explicit DisplayP3(const char* const s);

		inline explicit DisplayP3(const std::string& s);

		inline explicit DisplayP3(const Rgb& rgb) noexcept;

		inline explicit DisplayP3(const Hsl& hsl) noexcept;

		inline explicit DisplayP3(const Hwb& hwb) noexcept;

		inline explicit DisplayP3(const Lrgb& lrgb) noexcept;

		inline explicit DisplayP3(const XyzD50& xyzD50) noexcept;

		inline explicit DisplayP3(const Lab& lab) noexcept;

		inline explicit DisplayP3(const Lch& lch) noexcept;

		inline explicit DisplayP3(const XyzD65& xyzD65) noexcept;

		inline explicit DisplayP3(const Oklab& oklab) noexcept;

		inline explicit DisplayP3(const Oklch& oklch) noexcept;

		inline explicit DisplayP3(const Srgb& srgb) noexcept;

		inline explicit DisplayP3(const A98Rgb& a98Rgb) noexcept;

		inline explicit DisplayP3(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit DisplayP3(const Rec2020& rec2020) noexcept;

		DisplayP3& setRed(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		DisplayP3& setGreen(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		DisplayP3& setBlue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		DisplayP3& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double red() const noexcept
		{
			return m1;
		}

		double green() const noexcept
		{
			return m2;
		}

		double blue() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline DisplayP3 clip() const noexcept;

		inline DisplayP3 fit() const noexcept;

		inline double distance(const DisplayP3& displayP3) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline DisplayP3 maximizeLightness() noexcept;

		inline DisplayP3 brighten(double f = 0.1) const noexcept;

		inline DisplayP3 darken(double f = 0.1) const noexcept;

		inline DisplayP3 saturate(double f = 0.04) const noexcept;

		inline DisplayP3 desaturate(double f = 0.04) const noexcept;

		inline DisplayP3 rotate(double f = 36.0) const noexcept;

		inline DisplayP3 flip(double f = 0.0) const noexcept;

		inline DisplayP3 mix(const DisplayP3& displayP3, double f = 0.5) const noexcept;

		inline DisplayP3 grayscale() const noexcept;

		inline double deltaE_00(const DisplayP3& displayP3) const noexcept;

		inline double deltaE(const DisplayP3& displayP3) const noexcept;

		inline double contrast(const DisplayP3& displayP3) const noexcept;

	private:
		inline double nonLinear(double value) const noexcept;
	};

	class A98Rgb : public ColorBase<A98Rgb>
	{
	public:
		A98Rgb() noexcept {}

		A98Rgb(double r, double g, double b, double al = 1.0) noexcept
			: ColorBase(r, g, b, al) {}

		explicit A98Rgb(unsigned int num, double al = 1.0) noexcept
			: A98Rgb(Rgb(num, al)) {}

		explicit A98Rgb(Cn name, double al = 1.0) noexcept
			: A98Rgb(static_cast<unsigned int>(name), al) {}

		inline explicit A98Rgb(const char* const s);

		inline explicit A98Rgb(const std::string& s);

		inline explicit A98Rgb(const Rgb& rgb) noexcept;

		inline explicit A98Rgb(const Hsl& hsl) noexcept;

		inline explicit A98Rgb(const Hwb& hwb) noexcept;

		inline explicit A98Rgb(const Lrgb& lrgb) noexcept;

		inline explicit A98Rgb(const XyzD50& xyzD50) noexcept;

		inline explicit A98Rgb(const Lab& lab) noexcept;

		inline explicit A98Rgb(const Lch& lch) noexcept;

		inline explicit A98Rgb(const XyzD65& xyzD65) noexcept;

		inline explicit A98Rgb(const Oklab& oklab) noexcept;

		inline explicit A98Rgb(const Oklch& oklch) noexcept;

		inline explicit A98Rgb(const Srgb& srgb) noexcept;

		inline explicit A98Rgb(const DisplayP3& displayP3) noexcept;

		inline explicit A98Rgb(const ProphotoRgb& prophotoRgb) noexcept;

		inline explicit A98Rgb(const Rec2020& rec2020) noexcept;

		A98Rgb& setRed(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		A98Rgb& setGreen(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		A98Rgb& setBlue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		A98Rgb& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double red() const noexcept
		{
			return m1;
		}

		double green() const noexcept
		{
			return m2;
		}

		double blue() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline A98Rgb clip() const noexcept;

		inline A98Rgb fit() const noexcept;

		inline double distance(const A98Rgb& a98Rgb) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline A98Rgb maximizeLightness() noexcept;

		inline A98Rgb brighten(double f = 0.1) const noexcept;

		inline A98Rgb darken(double f = 0.1) const noexcept;

		inline A98Rgb saturate(double f = 0.04) const noexcept;

		inline A98Rgb desaturate(double f = 0.04) const noexcept;

		inline A98Rgb rotate(double f = 36.0) const noexcept;

		inline A98Rgb flip(double f = 0.0) const noexcept;

		inline A98Rgb mix(const A98Rgb& a98Rgb, double f = 0.5) const noexcept;

		inline A98Rgb grayscale() const noexcept;

		inline double deltaE_00(const A98Rgb& a98Rgb) const noexcept;

		inline double deltaE(const A98Rgb& a98Rgb) const noexcept;

		inline double contrast(const A98Rgb& a98Rgb) const noexcept;

	private:
		inline double nonLinear(double value) const noexcept;
	};

	class ProphotoRgb : public ColorBase<ProphotoRgb>
	{
	public:
		ProphotoRgb() noexcept {}

		ProphotoRgb(double r, double g, double b, double al = 1.0) noexcept
			: ColorBase(r, g, b, al) {}

		explicit ProphotoRgb(unsigned int num, double al = 1.0) noexcept
			: ProphotoRgb(Rgb(num, al)) {}

		explicit ProphotoRgb(Cn name, double al = 1.0) noexcept
			: ProphotoRgb(static_cast<unsigned int>(name), al) {}

		inline explicit ProphotoRgb(const char* const s);

		inline explicit ProphotoRgb(const std::string& s);

		inline explicit ProphotoRgb(const Rgb& rgb) noexcept;

		inline explicit ProphotoRgb(const Hsl& hsl) noexcept;

		inline explicit ProphotoRgb(const Hwb& hwb) noexcept;

		inline explicit ProphotoRgb(const Lrgb& lrgb) noexcept;

		inline explicit ProphotoRgb(const XyzD50& xyzD50) noexcept;

		inline explicit ProphotoRgb(const Lab& lab) noexcept;

		inline explicit ProphotoRgb(const Lch& lch) noexcept;

		inline explicit ProphotoRgb(const XyzD65& xyzD65) noexcept;

		inline explicit ProphotoRgb(const Oklab& oklab) noexcept;

		inline explicit ProphotoRgb(const Oklch& oklch) noexcept;

		inline explicit ProphotoRgb(const Srgb& srgb) noexcept;

		inline explicit ProphotoRgb(const DisplayP3& displayP3) noexcept;

		inline explicit ProphotoRgb(const A98Rgb& a98Rgb) noexcept;

		inline explicit ProphotoRgb(const Rec2020& rec2020) noexcept;

		ProphotoRgb& setRed(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		ProphotoRgb& setGreen(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		ProphotoRgb& setBlue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		ProphotoRgb& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double red() const noexcept
		{
			return m1;
		}

		double green() const noexcept
		{
			return m2;
		}

		double blue() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline ProphotoRgb clip() const noexcept;

		inline ProphotoRgb fit() const noexcept;

		inline double distance(const ProphotoRgb& prophotoRgb) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline ProphotoRgb maximizeLightness() noexcept;

		inline ProphotoRgb brighten(double f = 0.1) const noexcept;

		inline ProphotoRgb darken(double f = 0.1) const noexcept;

		inline ProphotoRgb saturate(double f = 0.04) const noexcept;

		inline ProphotoRgb desaturate(double f = 0.04) const noexcept;

		inline ProphotoRgb rotate(double f = 36.0) const noexcept;

		inline ProphotoRgb flip(double f = 0.0) const noexcept;

		inline ProphotoRgb mix(const ProphotoRgb& prophotoRgb, double f = 0.5) const noexcept;

		inline ProphotoRgb grayscale() const noexcept;

		inline double deltaE_00(const ProphotoRgb& prophotoRgb) const noexcept;

		inline double deltaE(const ProphotoRgb& prophotoRgb) const noexcept;

		inline double contrast(const ProphotoRgb& prophotoRgb) const noexcept;

	private:
		inline double nonLinear(double value) const noexcept;
	};


	class Rec2020 : public ColorBase<Rec2020>
	{
	public:
		Rec2020() noexcept {}

		Rec2020(double r, double g, double b, double al = 1.0) noexcept
			: ColorBase(r, g, b, al) {}

		explicit Rec2020(unsigned int num, double al = 1.0) noexcept
			: Rec2020(Rgb(num, al)) {}

		explicit Rec2020(Cn name, double al = 1.0) noexcept
			: Rec2020(static_cast<unsigned int>(name), al) {}

		inline explicit Rec2020(const char* const s);

		inline explicit Rec2020(const std::string& s);

		inline explicit Rec2020(const Rgb& rgb) noexcept;

		inline explicit Rec2020(const Hsl& hsl) noexcept;

		inline explicit Rec2020(const Hwb& hwb) noexcept;

		inline explicit Rec2020(const Lrgb& lrgb) noexcept;

		inline explicit Rec2020(const XyzD50& xyzD50) noexcept;

		inline explicit Rec2020(const Lab& lab) noexcept;

		inline explicit Rec2020(const Lch& lch) noexcept;

		inline explicit Rec2020(const XyzD65& xyzD65) noexcept;

		inline explicit Rec2020(const Oklab& oklab) noexcept;

		inline explicit Rec2020(const Oklch& oklch) noexcept;

		inline explicit Rec2020(const Srgb& srgb) noexcept;

		inline explicit Rec2020(const DisplayP3& displayP3) noexcept;

		inline explicit Rec2020(const A98Rgb& a98Rgb) noexcept;

		inline explicit Rec2020(const ProphotoRgb& prophotoRgb) noexcept;

		Rec2020& setRed(double f) noexcept
		{
			m1 = f;
			return *this;
		}

		Rec2020& setGreen(double f) noexcept
		{
			m2 = f;
			return *this;
		}

		Rec2020& setBlue(double f) noexcept
		{
			m3 = f;
			return *this;
		}

		Rec2020& setAlpha(double al) noexcept
		{
			ma = al;
			return *this;
		}

		double red() const noexcept
		{
			return m1;
		}

		double green() const noexcept
		{
			return m2;
		}

		double blue() const noexcept
		{
			return m3;
		}

		inline std::string hex() const;

		inline std::string name() const;

		inline std::string name4() const;

		inline std::string css() const;

		inline std::string cssf() const;

		inline std::string cssp() const;

		inline bool isDisplayable() const noexcept;

		inline Rec2020 clip() const noexcept;

		inline Rec2020 fit() const noexcept;

		inline double distance(const Rec2020& rec2020) const noexcept;

		inline double calculateLuminance() const noexcept;

		inline Rec2020 maximizeLightness() noexcept;

		inline Rec2020 brighten(double f = 0.1) const noexcept;

		inline Rec2020 darken(double f = 0.1) const noexcept;

		inline Rec2020 saturate(double f = 0.04) const noexcept;

		inline Rec2020 desaturate(double f = 0.04) const noexcept;

		inline Rec2020 rotate(double f = 36.0) const noexcept;

		inline Rec2020 flip(double f = 0.0) const noexcept;

		inline Rec2020 mix(const Rec2020& rec2020, double f = 0.5) const noexcept;

		inline Rec2020 grayscale() const noexcept;

		inline double deltaE_00(const Rec2020& rec2020) const noexcept;

		inline double deltaE(const Rec2020& rec2020) const noexcept;

		inline double contrast(const Rec2020& rec2020) const noexcept;

	private:
		inline double nonLinear(double value) const noexcept;
	};


	// Exception to color string parsing

	class ParseError : public std::runtime_error
	{
		public:
			explicit ParseError(char const* const s) noexcept
				: std::runtime_error(s) {}
	};


	// A parser of color strings.

	class ColorParser
	{
		enum class Type {
			Unknown,
			Rgb,
			Hsl,
			Hwb,
			Lrgb,
			XyzD50,
			Lab,
			Lch,
			XyzD65,
			Oklab,
			Oklch,
			Srgb,
			DisplayP3,
			A98Rgb,
			ProphotoRgb,
			Rec2020
		};

	private:
		inline explicit ColorParser(const char* const s);

		void skipSpaces(const char*& s) const noexcept
		{
			for (;;) {
				switch (*s) {
				case '\0':
					return;
				case ' ': case '\t': case '\n': case '\r': case '\f':
					break;
				default:
					return;
				}
				++s;
			}
		}

		template<size_t N> bool checkTop(const char*& s, const char (&word)[N]) const noexcept
		{
			constexpr auto len = N - 1;
			if (impl::compare(s, word, len) != 0) {
				return false;
			}
			const char* const p = s + len;
			if (*p == '(') {
				s = p + 1;
				skipSpaces(s);
				return true;
			}
			return false;
		}

		bool checkCharacter(const char*& s, const char c) const noexcept
		{
			if (*s == c) {
				++s;
				skipSpaces(s);
				return true;
			}
			return false;
		}

		template<size_t N> bool checkWord(const char*& s, const char (&word)[N]) const noexcept
		{
			constexpr auto len = N - 1;
			if (impl::compare(s, word, len) != 0) {
				return false;
			}
			const char* const p = s + len;
			switch (*p) {
			case ' ': case '\t': case '\n': case '\r': case '\f':
				s = p + 1;
				skipSpaces(s);
				return true;
			default:
				return false;
			}
		}

		const char* findValueEnd(const char* const s) const noexcept
		{
			const char* p = s;
			while (*p != '\0') {
				switch (*p) {
				case ' ': case '\t': case '\n': case '\r': case '\f': case ',': case '/': case ')':
					return p;
				}
				++p;
			}
			return p;
		}

		template<size_t N> bool hasUnit(const char* const b, const char* const e, const char (&unit)[N]) const noexcept
		{
			constexpr auto len = N - 1;
			if (b + len == e && impl::compare(b, unit, len) == 0) {
				return true;
			}
			return false;
		}

		bool readValue(double& f, const char*& s, const double oneHundredPercent, int& pccount) const noexcept
		{
			try {
				std::size_t i;
				double d = std::stod(s, &i);
				const char* b = s + i;
				const char* e = findValueEnd(s);
				if (b != e) {
					if (hasUnit(b, e, "%")) {
						if (oneHundredPercent == 0.0) {
							return false;
						}
						++pccount;
						if (oneHundredPercent > 0.0) {
							d *= oneHundredPercent / 100.0;
						} else {
							d *= oneHundredPercent / -100.0;
						}
					} else if (oneHundredPercent == 0.0) {
						if (hasUnit(b, e, "deg")) {
							// d *= 1.0;
						} else if (hasUnit(b, e, "grad")) {
							d *= 360.0 / 400.0;
						} else if (hasUnit(b, e, "rad")) {
							d *= 180.0 / 3.14159265358979323846;
						} else if (hasUnit(b, e, "turn")) {
							d *= 360.0;
						} else {
							return false;
						}
					} else {
						return false;
					}
				} else if (oneHundredPercent < 0.0) {
					// accepts only a percent value
					return false;
				}
				f = d;
				s = e;
				skipSpaces(s);
				return true;
			} catch (...) {
				return false;
			}
		}

		bool readFourValues(const char*& s, const double percent1, const double percent2, const double percent3, int& pccount, bool rgbOrHsl = true) noexcept
		{
			skipSpaces(s);
			double f1, f2, f3, fa = ma;
			if (!readValue(f1, s, percent1, pccount)) return false;
			bool comma = rgbOrHsl && checkCharacter(s, ',');
			if (comma) {
				if (!readValue(f2, s, percent2, pccount)) return false;
				if (!checkCharacter(s, ',')) return false;
				if (!readValue(f3, s, percent3, pccount)) return false;
				if (checkCharacter(s, ',')) {
					int dummy;
					if (!readValue(fa, s, 1.0, dummy)) return false;
				}
			} else {
				if (!readValue(f2, s, percent2, pccount)) return false;
				if (!readValue(f3, s, percent3, pccount)) return false;
				if (checkCharacter(s, '/')) {
					int dummy;
					if (!readValue(fa, s, 1.0, dummy)) return false;
				}
			}
			if (!checkCharacter(s, ')')) {
				return false;
			}
			m1 = f1;
			m2 = f2;
			m3 = f3;
			ma = fa;
			return true;
		}

		double readHex(const char*& s) const noexcept
		{
			switch (*s) {
			case '\0':
				return -1.0;
			case '0':
				++s;
				return 0.0;
			case '1':
				++s;
				return 1.0;
			case '2':
				++s;
				return 2.0;
			case '3':
				++s;
				return 3.0;
			case '4':
				++s;
				return 4.0;
			case '5':
				++s;
				return 5.0;
			case '6':
				++s;
				return 6.0;
			case '7':
				++s;
				return 7.0;
			case '8':
				++s;
				return 8.0;
			case '9':
				++s;
				return 9.0;
			case 'A': case 'a':
				++s;
				return 10.0;
			case 'B': case 'b':
				++s;
				return 11.0;
			case 'C': case 'c':
				++s;
				return 12.0;
			case 'D': case 'd':
				++s;
				return 13.0;
			case 'E': case 'e':
				++s;
				return 14.0;
			case 'F': case 'f':
				++s;
				return 15.0;
			default:
				++s;
				return -1.0;
			}
		}

		bool readHexValue(const char*& s) noexcept
		{
			double f[8];
			int count = 0;
			for (int i = 0; i < 8; ++i) {
				f[i] = readHex(s);
				if (f[i] < 0.0) {
					break;
				}
				++count;
			}
			switch (count) {
			case 3:
				m1 = f[0] * 17.0;
				m2 = f[1] * 17.0;
				m3 = f[2] * 17.0;
				return true;
			case 4:
				m1 = f[0] * 17.0;
				m2 = f[1] * 17.0;
				m3 = f[2] * 17.0;
				ma = f[3] / 15.0;
				return true;
			case 6:
				m1 = f[0] * 16.0;
				m1 += f[1];
				m2 = f[2] * 16.0;
				m2 += f[3];
				m3 = f[4] * 16.0;
				m3 += f[5];
				return true;
			case 8:
				m1 = f[0] * 16.0;
				m1 += f[1];
				m2 = f[2] * 16.0;
				m2 += f[3];
				m3 = f[4] * 16.0;
				m3 += f[5];
				ma = f[6] * 16.0;
				ma += f[7];
				ma /= 255.0;
				return true;
			default:
				return false;
			}
		}

		template <typename T> inline void convertTo(T& t) noexcept;

	private:
		Type type;
		double m1, m2, m3, ma;

		friend class Rgb;
		friend class Hsl;
		friend class Hwb;
		friend class Lrgb;
		friend class XyzD50;
		friend class Lab;
		friend class Lch;
		friend class XyzD65;
		friend class Oklab;
		friend class Oklch;
		friend class Srgb;
		friend class DisplayP3;
		friend class A98Rgb;
		friend class ProphotoRgb;
		friend class Rec2020;
	};

	ColorParser::ColorParser(const char* const color)
		: type(Type::Unknown), m1(0.0), m2(0.0), m3(0.0), ma(1.0)
	{
		if (color == nullptr) {
			return;
		}

		const char* s = color;
		skipSpaces(s);

		bool rgbOrHsl = false;
		double percent1 = 1.0;
		double percent2 = 1.0;
		double percent3 = 1.0;

		switch (*s) {
		case '#':
			++s;
			if (readHexValue(s)) {
				type = Type::Rgb;
				return;
			} else {
				throw ParseError(color);
			}
			--s;
			break;
		case 'C': case 'c':
			if (checkTop(s, "color")) {
				switch (*s) {
				case 'A': case 'a':
					if (checkWord(s, "a98-rgb")) {
						type = Type::A98Rgb;
					}
					break;
				case 'D': case 'd':
					if (checkWord(s, "display-p3")) {
						type = Type::DisplayP3;
					}
					break;
				case 'P': case 'p':
					if (checkWord(s, "prophoto-rgb")) {
						type = Type::ProphotoRgb;
					}
					break;
				case 'R': case 'r':
					if (checkWord(s, "rec2020")) {
						type = Type::Rec2020;
					}
					break;
				case 'S': case 's':
					if (checkWord(s, "srgb-linear")) {
						type = Type::Lrgb;
					} else if (checkWord(s, "srgb")) {
						type = Type::Srgb;
					}
					break;
				case 'X': case 'x':
					if (checkWord(s, "xyz-d50")) {
						type = Type::XyzD50;
					} else if (checkWord(s, "xyz") || checkWord(s, "xyz-d65")) {
						type = Type::XyzD65;
					}
					break;
				default:
					s = color;
					skipSpaces(s);
					break;
				}
			} else {
				s = color;
				skipSpaces(s);
			}
			break;
		case 'H': case 'h':
			if (checkTop(s, "hsl") || checkTop(s, "hsla")) {
				type = Type::Hsl;
				rgbOrHsl = true;
				percent1 = 0.0;
				percent2 = -100.0;
				percent3 = -100.0;
			} else if (checkTop(s, "hwb")) {
				type = Type::Hwb;
				percent1 = 0.0;
				percent2 = -100.0;
				percent3 = -100.0;
			}
			break;
		case 'L': case 'l':
			if (checkTop(s, "lch")) {
				type = Type::Lch;
				percent1 = 100.0;
				percent2 = 150.0;
				percent3 = 0.0;
			} else if (checkTop(s, "lab")) {
				type = Type::Lab;
				percent1 = 100.0;
				percent2 = 125.0;
				percent3 = 125.0;
			}
			break;
		case 'O': case 'o':
			if (checkTop(s, "oklch")) {
				type = Type::Oklch;
				percent1 = 1.0;
				percent2 = 0.4;
				percent3 = 0.0;
			} else if (checkTop(s, "oklab")) {
				type = Type::Oklab;
				percent1 = 1.0;
				percent2 = 0.4;
				percent3 = 0.4;
			}
			break;
		case 'R': case 'r':
			if (checkTop(s, "rgb") || checkTop(s, "rgba")) {
				type = Type::Rgb;
				rgbOrHsl = true;
				percent1 = 255.0;
				percent2 = 255.0;
				percent3 = 255.0;
			}
			break;
		case 'X': case 'x':
			if (checkTop(s, "xyzD50")) {
				type = Type::XyzD50;
			} else if (checkTop(s, "xyzD65")) {
				type = Type::XyzD65;
			}
			break;
		}
		if (type != Type::Unknown) {
			int pccount = 0;
			if (readFourValues(s, percent1, percent2, percent3, pccount, rgbOrHsl)) {
				if (type == Type::Rgb && pccount != 0 && pccount != 3) {
					type = Type::Unknown;
					throw ParseError(color);
				}
				return;
			} else {
				type = Type::Unknown;
				throw ParseError(color);
			}
		}

		const char* hex = impl::toHex(s);
		if (hex == nullptr || *hex != '#') {
			throw ParseError(color);
		}
		++hex;
		if (readHexValue(hex)) {
			type = Type::Rgb;
			return;
		} else {
			throw ParseError(color);
		}
	}

	template <class T> void ColorParser::convertTo(T& t) noexcept
	{
		switch(type) {
		case Type::Rgb:
			t = T(Rgb(m1, m2, m3, ma));
			return;
		case Type::Hsl:
			t = T(Hsl(m1, m2, m3, ma));
			return;
		case Type::Hwb:
			t = T(Hwb(m1, m2, m3, ma));
			return;
		case Type::Lrgb:
			t = T(Lrgb(m1, m2, m3, ma));
			return;
		case Type::XyzD50:
			t = T(XyzD50(m1, m2, m3, ma));
			return;
		case Type::Lab:
			t = T(Lab(m1, m2, m3, ma));
			return;
		case Type::Lch:
			t = T(Lch(m1, m2, m3, ma));
			return;
		case Type::XyzD65:
			t = T(XyzD65(m1, m2, m3, ma));
			return;
		case Type::Oklab:
			t = T(Oklab(m1, m2, m3, ma));
			return;
		case Type::Oklch:
			t = T(Oklch(m1, m2, m3, ma));
			return;
		case Type::Srgb:
			t = T(Srgb(m1, m2, m3, ma));
			return;
		case Type::DisplayP3:
			t = T(DisplayP3(m1, m2, m3, ma));
			return;
		case Type::A98Rgb:
			t = T(A98Rgb(m1, m2, m3, ma));
			return;
		case Type::ProphotoRgb:
			t = T(ProphotoRgb(m1, m2, m3, ma));
			return;
		case Type::Rec2020:
			t = T(Rec2020(m1, m2, m3, ma));
			return;
		default:
			t = T();
			return;
		}
	}

	Rgb::Rgb(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Hsl::Hsl(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Hwb::Hwb(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Lrgb::Lrgb(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	XyzD50::XyzD50(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Lab::Lab(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Lch::Lch(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	XyzD65::XyzD65(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Oklab::Oklab(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Oklch::Oklch(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}
	Srgb::Srgb(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	DisplayP3::DisplayP3(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	A98Rgb::A98Rgb(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	ProphotoRgb::ProphotoRgb(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Rec2020::Rec2020(const char* const s)
	{
		ColorParser(s).convertTo(*this);
	}

	Rgb::Rgb(const std::string& s)
		: Rgb(s.c_str()) {}

	Hsl::Hsl(const std::string& s)
		: Hsl(s.c_str()) {}

	Hwb::Hwb(const std::string& s)
		: Hwb(s.c_str()) {}

	Lrgb::Lrgb(const std::string& s)
		: Lrgb(s.c_str()) {}

	XyzD50::XyzD50(const std::string& s)
		: XyzD50(s.c_str()) {}

	Lab::Lab(const std::string& s)
		: Lab(s.c_str()) {}

	Lch::Lch(const std::string& s)
		: Lch(s.c_str()) {}

	XyzD65::XyzD65(const std::string& s)
		: XyzD65(s.c_str()) {}

	Oklab::Oklab(const std::string& s)
		: Oklab(s.c_str()) {}

	Oklch::Oklch(const std::string& s)
		: Oklch(s.c_str()) {}

	Srgb::Srgb(const std::string& s)
		: Srgb(s.c_str()) {}

	DisplayP3::DisplayP3(const std::string& s)
		: DisplayP3(s.c_str()) {}

	A98Rgb::A98Rgb(const std::string& s)
		: A98Rgb(s.c_str()) {}

	ProphotoRgb::ProphotoRgb(const std::string& s)
		: ProphotoRgb(s.c_str()) {}

	Rec2020::Rec2020(const std::string& s)
		: Rec2020(s.c_str()) {}


	// Color space convert functions.
	// They are adapted from the following websites:
	// https://www.w3.org/TR/css-color-4/
	// http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
	// http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
	// https://en.wikipedia.org/wiki/SRGB
	// https://en.wikipedia.org/wiki/HSL_and_HSV
	// https://en.wikipedia.org/wiki/CIE_1931_color_space
	// https://en.wikipedia.org/wiki/CIELAB_color_space
	// https://bottosson.github.io/posts/oklab/
	// https://en.wikipedia.org/wiki/DCI-P3
	// https://en.wikipedia.org/wiki/Adobe_RGB_color_space
	// https://en.wikipedia.org/wiki/ProPhoto_RGB_color_space
	// https://en.wikipedia.org/wiki/Rec._2020

	double Rgb::nonLinear(double value) const noexcept
	{
		if (value >= 0.0) {
			if (value <= 0.0031308) {
				return 12.92 * value * 255.0;
			} else {
				return (1.055 * std::pow(value, 1.0 / 2.4) - 0.055) * 255.0;
			}
		} else {
			value = -value;
			if (value <= 0.0031308) {
				return -(12.92 * value * 255.0);
			} else {
				return -((1.055 * std::pow(value, 1.0 / 2.4) - 0.055) * 255.0);
			}
		}
	}

	void Rgb::getMinMaxHue(double& min, double& max, double& hue) const noexcept
	{
		if (red() == green() && green() == blue()) {
			min = red();
			max = red();
			hue = 0.0;
		} else if (blue() <= red() && blue() <= green()) {
			min = blue();
			max = (green() > red()) ? green() : red();
			hue = 60.0 * (green() - red()) / (max - min) + 60.0;
		} else if (red() <= green() && red() <= blue()) {
			min = red();
			max = (blue() > green()) ? blue() : green();
			hue = 60.0 * (blue() - green()) / (max - min) + 180.0;
		} else {
			min = green();
			max = (red() > blue()) ? red() : blue();
			hue = 60.0 * (red() - blue()) / (max - min) + 300.0;
		}
	}

	double Lrgb::linear(double value) const noexcept
	{
		if (value > 0.0) {
			value /= 255.0;
			if (value <= 0.04045) {
				return value / 12.92;
			} else {
				return std::pow((value + 0.055) / 1.055, 2.4);
			}
		} else {
			value = -value;
			value /= 255.0;
			if (value <= 0.04045) {
				return -(value / 12.92);
			} else {
				return -(std::pow((value + 0.055) / 1.055, 2.4));
			}
		}
	}

	double Lrgb::linearS(double value) const noexcept
	{
		if (value > 0.0) {
			if (value <= 0.04045) {
				return value / 12.92;
			} else {
				return std::pow((value + 0.055) / 1.055, 2.4);
			}
		} else {
			value = -value;
			if (value <= 0.04045) {
				return -(value / 12.92);
			} else {
				return -(std::pow((value + 0.055) / 1.055, 2.4));
			}
		}
	}

	double XyzD50::fi(double t) const noexcept
	{
		const double d = 6.0 / 29.0;
		if (t > d) {
			return t * t * t;
		}
		return 3 * d * d * (t - 4.0 / 29.0);
	}

	double Lab::fn(double t) const noexcept
	{
		const double d = 6.0 / 29.0;
		if (t > d * d * d) {
			return std::cbrt(t);
		}
		return t / (3 * d * d) + 4.0 / 29.0;
	}

	double XyzD65::linearD(double value) const noexcept
	{
		if (value > 0.0) {
			if (value <= 0.04045) {
				return value / 12.92;
			} else {
				return std::pow((value + 0.055) / 1.055, 2.4);
			}
		} else {
			value = -value;
			if (value <= 0.04045) {
				return -(value / 12.92);
			} else {
				return -(std::pow((value + 0.055) / 1.055, 2.4));
			}
		}
	}

	double XyzD65::linearA(double value) const noexcept
	{
		if (value >= 0.0) {
			return std::pow(value, 563.0 / 256.0);
		} else {
			return -std::pow(-value, 563.0 / 256.0);
		}
	}

	double XyzD65::linearP(double value) const noexcept
	{
		if (value >= 0.0) {
			if (value <= 16.0 / 512.0) {
				return value / 16.0;
			} else {
				return std::pow(value, 1.8);
			}
		} else {
			value = -value;
			if (value <= 16.0 / 512.0) {
				return -value / 16.0;
			} else {
				return -std::pow(value, 1.8);
			}
		}
	}

	double XyzD65::linearR(double value) const noexcept
	{
		if (value >= 0.0) {
			if (value <= 0.018053968510807 * 4.5) {
				return value / 4.5;
			} else {
				return std::pow((value + 0.09929682680944) / 1.09929682680944, 1 / 0.45);
			}
		} else {
			value = -value;
			if (value <= 0.018053968510807 * 4.5) {
				return -value / 4.5;
			} else {
				return -std::pow((value + 0.09929682680944) / 1.09929682680944, 1 / 0.45);
			}
		}
	}

	double Srgb::nonLinear(double value) const noexcept
	{
		if (value >= 0.0) {
			if (value <= 0.0031308) {
				return 12.92 * value;
			} else {
				return (1.055 * std::pow(value, 1.0 / 2.4) - 0.055);
			}
		} else {
			value = -value;
			if (value <= 0.0031308) {
				return -12.92 * value;
			} else {
				return -((1.055 * std::pow(value, 1.0 / 2.4) - 0.055));
			}
		}
	}

	double DisplayP3::nonLinear(double value) const noexcept
	{
		if (value >= 0.0) {
			if (value <= 0.0031308) {
				return 12.92 * value;
			} else {
				return (1.055 * std::pow(value, 1.0 / 2.4) - 0.055);
			}
		} else {
			value = -value;
			if (value <= 0.0031308) {
				return -12.92 * value;
			} else {
				return -((1.055 * std::pow(value, 1.0 / 2.4) - 0.055));
			}
		}
	}

	double A98Rgb::nonLinear(double value) const noexcept
	{
		if (value >= 0.0) {
			return std::pow(value, 256.0 / 563.0);
		} else {
			return -std::pow(-value, 256.0 / 563.0);
		}
	}

	double ProphotoRgb::nonLinear(double value) const noexcept
	{
		if (value >= 0.0) {
			if (value <= 1.0 / 512.0) {
				return 16.0 * value;
			} else {
				return std::pow(value, 1.0 / 1.8);
			}
		} else {
			value = -value;
			if (value <= 1.0 / 512.0) {
				return -16.0 * value;
			} else {
				return -std::pow(value, 1.0 / 1.8);
			}
		}
	}

	double Rec2020::nonLinear(double value) const noexcept
	{
		if (value >= 0.0) {
			if (value <= 0.018053968510807) {
				return 4.5 * value;
			} else {
				return (1.09929682680944 * std::pow(value, 0.45) - 0.09929682680944) ;
			}
		} else {
			value = -value;
			if (value <= 0.018053968510807) {
				return -4.5 * value;
			} else {
				return -(1.09929682680944 * std::pow(value, 0.45) - 0.09929682680944);
			}
		}
	}

	Rgb::Rgb(const Hsl& hsl) noexcept
		: ColorBase(hsl.alpha())
	{
		const double hl = impl::normalize1(hsl.lightness() / 100.0);
		const double hs = impl::normalize1(hsl.saturation() / 100.0);
		const double C = (1.0 - std::fabs(2.0 * hl - 1.0)) * hs;
		const double Hpr = impl::normalizeHue(hsl.hue()) / 60.0;
		const double X = C * (1.0 - std::fabs(std::fmod(Hpr, 2.0) - 1.0));
		double r, g, b;
		switch (static_cast<int>(Hpr)) {
		case 0:
			r = C; g = X; b = 0.0;
			break;
		case 1:
			r = X; g = C; b = 0.0;
			break;
		case 2:
			r = 0.0; g = C; b = X;
			break;
		case 3:
			r = 0.0; g = X; b = C;
			break;
		case 4:
			r = X; g = 0.0; b = C;
			break;
		default:
			r = C; g = 0.0; b = X;
			break;
		}
		const double m = hl - (C / 2.0);
		setRed((r + m) * 255.0);
		setGreen((g + m) * 255.0);
		setBlue((b + m) * 255.0);
	}

	Rgb::Rgb(const Hwb& hwb) noexcept
		: ColorBase(hwb.alpha())
	{
		const double Hpr = impl::normalizeHue(hwb.hue()) / 60.0;
		const double X = 1.0 - std::fabs(std::fmod(Hpr, 2.0) - 1.0);
		double r, g, b;
		switch (static_cast<int>(Hpr)) {
		case 0:
			r = 1.0; g = X; b = 0.0;
			break;
		case 1:
			r = X; g = 1.0; b = 0.0;
			break;
		case 2:
			r = 0.0; g = 1.0; b = X;
			break;
		case 3:
			r = 0.0; g = X; b = 1.0;
			break;
		case 4:
			r = X; g = 0.0; b = 1.0;
			break;
		default:
			r = 1.0; g = 0.0; b = X;
			break;
		}
		const double hw = hwb.whiteness() / 100.0;
		const double hb = hwb.blackness() / 100.0;
		const double v = 1.0 - hb - hw;
		setRed(((r * v) + hw) * 255.0);
		setGreen(((g * v) + hw) * 255.0);
		setBlue(((b * v) + hw) * 255.0);
	}

	Rgb::Rgb(const Lrgb& lrgb) noexcept
		: ColorBase(
			nonLinear(lrgb.red()),
			nonLinear(lrgb.green()),
			nonLinear(lrgb.blue()),
			lrgb.alpha()) {}

	Rgb::Rgb(const XyzD50& xyzD50) noexcept
		: Rgb(Lrgb(xyzD50)) {}

	Rgb::Rgb(const Lab& lab) noexcept
		: Rgb(Lrgb(lab)) {}

	Rgb::Rgb(const Lch& lch) noexcept
		: Rgb(Lrgb(lch)) {}

	Rgb::Rgb(const XyzD65& xyzD65) noexcept
		: Rgb(Lrgb(xyzD65)) {}

	Rgb::Rgb(const Oklab& oklab) noexcept
		: Rgb(Lrgb(oklab)) {}

	Rgb::Rgb(const Oklch& oklch) noexcept
		: Rgb(Lrgb(oklch)) {}

	Rgb::Rgb(const Srgb& srgb) noexcept
		: Rgb(srgb.red() * 255.0, srgb.green() * 255.0, srgb.blue() * 255.0, srgb.alpha()) {}

	Rgb::Rgb(const DisplayP3& displayP3) noexcept
		: Rgb(Lrgb(displayP3)) {}

	Rgb::Rgb(const A98Rgb& a98Rgb) noexcept
		: Rgb(Lrgb(a98Rgb)) {}

	Rgb::Rgb(const ProphotoRgb& prophotoRgb) noexcept
		: Rgb(Lrgb(prophotoRgb)) {}

	Rgb::Rgb(const Rec2020& rec2020) noexcept
		: Rgb(Lrgb(rec2020)) {}

	Hsl::Hsl(const Rgb& rgb) noexcept
		: ColorBase(rgb.alpha())
	{
		double min, max, h;
		rgb.fit().getMinMaxHue(min, max, h);
		setHue(h);
		const double f = 255.0 - std::fabs(max + min - 255.0);
		if (f == 0.0) {
			setSaturation(0.0);
		} else {
			setSaturation(100.0 * (max - min) / f);
		}
		setLightness(100.0 * (max + min) / (2.0 * 255.0));
		if (hasPowerlessHue()) {
			setHue(0.0);
		}
	}

	Hsl::Hsl(const Hwb& hwb) noexcept
		: Hsl(Rgb(hwb)) {}

	Hsl::Hsl(const Lrgb& lrgb) noexcept
		: Hsl(Rgb(lrgb)) {}

	Hsl::Hsl(const XyzD50& xyzD50) noexcept
		: Hsl(Rgb(xyzD50)) {}

	Hsl::Hsl(const Lab& lab) noexcept
		: Hsl(Rgb(lab)) {}

	Hsl::Hsl(const Lch& lch) noexcept
		: Hsl(Rgb(lch)) {}

	Hsl::Hsl(const XyzD65& xyzD65) noexcept
		: Hsl(Rgb(xyzD65)) {}

	Hsl::Hsl(const Oklab& oklab) noexcept
		: Hsl(Rgb(oklab)) {}

	Hsl::Hsl(const Oklch& oklch) noexcept
		: Hsl(Rgb(oklch)) {}

	Hsl::Hsl(const Srgb& srgb) noexcept
		: Hsl(Rgb(srgb)) {}

	Hsl::Hsl(const DisplayP3& displayP3) noexcept
		: Hsl(Rgb(displayP3)) {}

	Hsl::Hsl(const A98Rgb& a98Rgb) noexcept
		: Hsl(Rgb(a98Rgb)) {}

	Hsl::Hsl(const ProphotoRgb& prophotoRgb) noexcept
		: Hsl(Rgb(prophotoRgb)) {}

	Hsl::Hsl(const Rec2020& rec2020) noexcept
		: Hsl(Rgb(rec2020)) {}

	Hwb::Hwb(const Rgb& rgb) noexcept
		: ColorBase(rgb.alpha())
	{
		double min, max, h;
		rgb.getMinMaxHue(min, max, h);
		setHue(h);
		setWhiteness(100.0 * min / 255.0);
		setBlackness(100.0 * (1.0 - (max / 255.0)));
		if (hasPowerlessHue()) {
			setHue(0.0);
		}
	}

	Hwb::Hwb(const Hsl& hsl) noexcept
		: Hwb(Rgb(hsl)) {}

	Hwb::Hwb(const Lrgb& lrgb) noexcept
		: Hwb(Rgb(lrgb)) {}

	Hwb::Hwb(const XyzD50& xyzD50) noexcept
		: Hwb(Rgb(xyzD50)) {}

	Hwb::Hwb(const Lab& lab) noexcept
		: Hwb(Rgb(lab)) {}

	Hwb::Hwb(const Lch& lch) noexcept
		: Hwb(Rgb(lch)) {}

	Hwb::Hwb(const XyzD65& xyzD65) noexcept
		: Hwb(Rgb(xyzD65)) {}

	Hwb::Hwb(const Oklab& oklab) noexcept
		: Hwb(Rgb(oklab)) {}

	Hwb::Hwb(const Oklch& oklch) noexcept
		: Hwb(Rgb(oklch)) {}

	Hwb::Hwb(const Srgb& srgb) noexcept
		: Hwb(Rgb(srgb)) {}

	Hwb::Hwb(const DisplayP3& displayP3) noexcept
		: Hwb(Rgb(displayP3)) {}

	Hwb::Hwb(const A98Rgb& a98Rgb) noexcept
		: Hwb(Rgb(a98Rgb)) {}

	Hwb::Hwb(const ProphotoRgb& prophotoRgb) noexcept
		: Hwb(Rgb(prophotoRgb)) {}

	Hwb::Hwb(const Rec2020& rec2020) noexcept
		: Hwb(Rgb(rec2020)) {}

	Lrgb::Lrgb(const Rgb& rgb) noexcept
		: ColorBase(
			linear(rgb.red()),
			linear(rgb.green()),
			linear(rgb.blue()),
			rgb.alpha()) {}

	Lrgb::Lrgb(const Hsl& hsl) noexcept
		: Lrgb(Rgb(hsl)) {}

	Lrgb::Lrgb(const Hwb& hwb) noexcept
		: Lrgb(Rgb(hwb)) {}

	Lrgb::Lrgb(const XyzD50& xyzD50) noexcept
		: ColorBase(
		xyzD50.dot(3.134135852900119,   -1.617385998018044, -0.4906622179110974),
		xyzD50.dot(-0.9787954765557776,  1.916254377395988,  0.03344287339036688),
		xyzD50.dot(0.07195539255794736, -0.2289767598151819, 1.405386035113117),
		xyzD50.alpha()) {}


	Lrgb::Lrgb(const Lab& lab) noexcept
		: Lrgb(XyzD50(lab)) {}

	Lrgb::Lrgb(const Lch& lch) noexcept
		: Lrgb(XyzD50(lch)) {}

	Lrgb::Lrgb(const XyzD65& xyzD65) noexcept
		: ColorBase(
		xyzD65.dot(3.240969941904523,   -1.537383177570094, -0.4986107602930034),
		xyzD65.dot(-0.9692436362808796,  1.87596750150772,   0.0415550574071756),
		xyzD65.dot(0.05563007969699363, -0.2039769588889765, 1.056971514242878),
		xyzD65.alpha()) {}

	Lrgb::Lrgb(const Oklab& oklab) noexcept
		: ColorBase(oklab.alpha())
	{
		const double l2 = oklab.dot(1.0,  0.3963377774, 0.2158037573);
		const double m2 = oklab.dot(1.0, -0.1055613458, -0.0638541728);
		const double s2 = oklab.dot(1.0, -0.0894841775, -1.291485548);
		const Oklab c(l2 * l2 * l2, m2 * m2 * m2, s2 * s2 * s2);
		setRed(  c.dot(4.0767416621, -3.3077115913, 0.2309699292));
		setGreen(c.dot(-1.2684380046, 2.6097574011, -0.3413193965));
		setBlue( c.dot(-0.0041960863, -0.7034186147, 1.707614701));
	}

	Lrgb::Lrgb(const Oklch& oklch) noexcept
		: Lrgb(Oklab(oklch)) {}

	Lrgb::Lrgb(const Srgb& srgb) noexcept
		: ColorBase(
		linearS(srgb.red()),
		linearS(srgb.green()),
		linearS(srgb.blue()),
		srgb.alpha()) {}

	Lrgb::Lrgb(const DisplayP3& displayP3) noexcept
		: Lrgb(XyzD65(displayP3)) {}

	Lrgb::Lrgb(const A98Rgb& a98Rgb) noexcept
		: Lrgb(XyzD65(a98Rgb)) {}

	Lrgb::Lrgb(const ProphotoRgb& prophotoRgb) noexcept
		: Lrgb(XyzD65(prophotoRgb)) {}

	Lrgb::Lrgb(const Rec2020& rec2020) noexcept
		: Lrgb(XyzD65(rec2020)) {}

	XyzD50::XyzD50(const Rgb& rgb) noexcept
		: XyzD50(Lrgb(rgb)) {}

	XyzD50::XyzD50(const Hsl& hsl) noexcept
		: XyzD50(Lrgb(hsl)) {}

	XyzD50::XyzD50(const Hwb& hwb) noexcept
		: XyzD50(Lrgb(hwb)) {}

	XyzD50::XyzD50(const Lrgb& lrgb) noexcept
		: ColorBase(
		lrgb.dot(0.4360657468742694,  0.3851515095901599,  0.1430784199651387),
		lrgb.dot(0.2224931771105652,  0.7168870130944827,  0.06061980979495238),
		lrgb.dot(0.01392392146316938, 0.09708132423141017, 0.7140993568158811),
		lrgb.alpha()) {}

	XyzD50::XyzD50(const Lab& lab) noexcept
		: ColorBase(lab.alpha())
	{
		const double ll = (lab.lightness() + 16.0) / 116.0;
		setX((0.3457 / 0.3585) * fi(ll + lab.a() / 500.0));
		setY(fi(ll));
		setZ((0.2958 / 0.3585) * fi(ll - lab.b() / 200.0));
	}

	XyzD50::XyzD50(const Lch& lch) noexcept
		: XyzD50(Lab(lch)) {}

	XyzD50::XyzD50(const XyzD65& xyzD65) noexcept
		: XyzD50(Lrgb(xyzD65)) {}

	XyzD50::XyzD50(const Oklab& oklab) noexcept
		: XyzD50(Lrgb(oklab)) {}

	XyzD50::XyzD50(const Oklch& oklch) noexcept
		: XyzD50(Lrgb(oklch)) {}

	XyzD50::XyzD50(const Srgb& srgb) noexcept
		: XyzD50(Lrgb(srgb)) {}

	XyzD50::XyzD50(const DisplayP3& displayP3) noexcept
		: XyzD50(Lrgb(displayP3)) {}

	XyzD50::XyzD50(const A98Rgb& a98Rgb) noexcept
		: XyzD50(Lrgb(a98Rgb)) {}

	XyzD50::XyzD50(const ProphotoRgb& prophotoRgb) noexcept
		: XyzD50(Lrgb(prophotoRgb)) {}

	XyzD50::XyzD50(const Rec2020& rec2020) noexcept
		: XyzD50(Lrgb(rec2020)) {}

	Lab::Lab(const Rgb& rgb) noexcept
		: Lab(XyzD50(rgb)) {}

	Lab::Lab(const Hsl& hsl) noexcept
		: Lab(XyzD50(hsl)) {}

	Lab::Lab(const Hwb& hwb) noexcept
		: Lab(XyzD50(hwb)) {}

	Lab::Lab(const Lrgb& lrgb) noexcept
		: Lab(XyzD50(lrgb)) {}

	Lab::Lab(const XyzD50& xyzD50) noexcept
		: ColorBase(xyzD50.alpha())
	{
		const double fx = fn(xyzD50.x() / (0.3457 / 0.3585));
		const double fy = fn(xyzD50.y());
		const double fz = fn(xyzD50.z() / (0.2958 / 0.3585));
		setLightness(116.0 * fy - 16.0);
		setA(500.0 * (fx - fy));
		setB(200.0 * (fy - fz));
	}

	Lab::Lab(const Lch& lch) noexcept
		: ColorBase(lch.alpha())
	{
		double chroma = lch.chroma();
		if (chroma < 0.0) chroma = 0.0;
		setLightness(lch.lightness());
		setA(chroma * impl::cosDeg(lch.hue()));
		setB(chroma * impl::sinDeg(lch.hue()));
	}

	Lab::Lab(const XyzD65& xyzD65) noexcept
		: Lab(XyzD50(xyzD65)) {}

	Lab::Lab(const Oklab& oklab) noexcept
		: Lab(XyzD50(oklab)) {}

	Lab::Lab(const Oklch& oklch) noexcept
		: Lab(XyzD50(oklch)) {}

	Lab::Lab(const Srgb& srgb) noexcept
		: Lab(XyzD50(srgb)) {}

	Lab::Lab(const DisplayP3& displayP3) noexcept
		: Lab(XyzD50(displayP3)) {}

	Lab::Lab(const A98Rgb& a98Rgb) noexcept
		: Lab(XyzD50(a98Rgb)) {}

	Lab::Lab(const ProphotoRgb& prophotoRgb) noexcept
		: Lab(XyzD50(prophotoRgb)) {}

	Lab::Lab(const Rec2020& rec2020) noexcept
		: Lab(XyzD50(rec2020)) {}

	Lch::Lch(const Rgb& rgb) noexcept
		: Lch(Lab(rgb)) {}

	Lch::Lch(const Hsl& hsl) noexcept
		: Lch(Lab(hsl)) {}

	Lch::Lch(const Hwb& hwb) noexcept
		: Lch(Lab(hwb)) {}

	Lch::Lch(const Lrgb& lrgb) noexcept
		: Lch(Lab(lrgb)) {}

	Lch::Lch(const XyzD50& xyzD50) noexcept
		: Lch(Lab(xyzD50)) {}

	Lch::Lch(const Lab& lab) noexcept
		: ColorBase(lab.alpha())
	{
		setLightness(lab.lightness());
		setChroma(std::sqrt(lab.a() * lab.a() + lab.b() * lab.b()));
		if (hasPowerlessHue()) {
			setChroma(0.0);
			setHue(0.0);
		} else {
			setHue(impl::normalizeHue(impl::atan2Deg(lab.b(), lab.a())));
		}
	}

	Lch::Lch(const XyzD65& xyzD65) noexcept
		: Lch(Lab(xyzD65)) {}

	Lch::Lch(const Oklab& oklab) noexcept
		: Lch(Lab(oklab)) {}

	Lch::Lch(const Oklch& oklch) noexcept
		: Lch(Lab(oklch)) {}

	Lch::Lch(const Srgb& srgb) noexcept
		: Lch(Lab(srgb)) {}

	Lch::Lch(const DisplayP3& displayP3) noexcept
		: Lch(Lab(displayP3)) {}

	Lch::Lch(const A98Rgb& a98Rgb) noexcept
		: Lch(Lab(a98Rgb)) {}

	Lch::Lch(const ProphotoRgb& prophotoRgb) noexcept
		: Lch(Lab(prophotoRgb)) {}

	Lch::Lch(const Rec2020& rec2020) noexcept
		: Lch(Lab(rec2020)) {}

	XyzD65::XyzD65(const Rgb& rgb) noexcept
		: XyzD65(Lrgb(rgb)) {}

	XyzD65::XyzD65(const Hsl& hsl) noexcept
		: XyzD65(Lrgb(hsl)) {}

	XyzD65::XyzD65(const Hwb& hwb) noexcept
		: XyzD65(Lrgb(hwb)) {}

	XyzD65::XyzD65(const Lrgb& lrgb) noexcept
		: ColorBase(
		lrgb.dot(0.4123907992659593,  0.357584339383878, 0.1804807884018343),
		lrgb.dot(0.2126390058715102,  0.715168678767756, 0.07219231536073371),
		lrgb.dot(0.01933081871559182, 0.119194779794626, 0.9505321522496607),
		lrgb.alpha()) {}

	XyzD65::XyzD65(const XyzD50& xyzD50) noexcept
		: XyzD65(Lrgb(xyzD50)) {}

	XyzD65::XyzD65(const Lab& lab) noexcept
		: XyzD65(Lrgb(lab)) {}

	XyzD65::XyzD65(const Lch& lch) noexcept
		: XyzD65(Lrgb(lch)) {}

	XyzD65::XyzD65(const Oklab& oklab) noexcept
		: XyzD65(Lrgb(oklab)) {}

	XyzD65::XyzD65(const Oklch& oklch) noexcept
		: XyzD65(Lrgb(oklch)) {}

	XyzD65::XyzD65(const Srgb& srgb) noexcept
		: XyzD65(Lrgb(srgb)) {}

	XyzD65::XyzD65(const DisplayP3& displayP3) noexcept
		: ColorBase(displayP3.alpha())
	{
		const DisplayP3 c(linearD(displayP3.red()), linearD(displayP3.green()), linearD(displayP3.blue()));
		setX(c.dot(0.486570948648216,  0.2656676931690931, 0.1982172852343625));
		setY(c.dot(0.2289745640697487, 0.6917385218365063, 0.079286914093745));
		setZ(c.dot(0.0,                0.04511338185890263, 1.043944368900976));
	}

	XyzD65::XyzD65(const A98Rgb& a98Rgb) noexcept
		: ColorBase(a98Rgb.alpha())
	{
		const A98Rgb c(linearA(a98Rgb.red()), linearA(a98Rgb.green()), linearA(a98Rgb.blue()));
		setX(c.dot(0.5766690429101305,  0.1855582379065463,  0.1882286462349947));
		setY(c.dot(0.297344975250536,   0.6273635662554661,  0.07529145849399786));
		setZ(c.dot(0.02703136138641234, 0.07068885253582723, 0.9913375368376386));
	}

	XyzD65::XyzD65(const ProphotoRgb& prophotoRgb) noexcept
		: ColorBase(prophotoRgb.alpha())
	{
		const ProphotoRgb c(linearP(prophotoRgb.red()), linearP(prophotoRgb.green()), linearP(prophotoRgb.blue()));
		setX(c.dot(0.7555907422969211,  0.1127198426594051,  0.08214534209534545));
		setY(c.dot(0.268321843578572,   0.7151152566617912,  0.01656289975963685));
		setZ(c.dot(0.0039159727624258, -0.01293344283684182, 1.098075220834295));
	}

	XyzD65::XyzD65(const Rec2020& rec2020) noexcept
		: ColorBase(rec2020.alpha())
	{
		const Rec2020 c(linearR(rec2020.red()), linearR(rec2020.green()), linearR(rec2020.blue()));
		setX(c.dot(0.6369580483012911, 0.1446169035862084,  0.1688809751641721));
		setY(c.dot(0.262700212011267,  0.6779980715188709,  0.05930171646986195));
		setZ(c.dot(0.0,                0.02807269304908743, 1.060985057710791));
	}

	Oklab::Oklab(const Rgb& rgb) noexcept
		: Oklab(Lrgb(rgb)) {}

	Oklab::Oklab(const Hsl& hsl) noexcept
		: Oklab(Lrgb(hsl)) {}

	Oklab::Oklab(const Hwb& hwb) noexcept
		: Oklab(Lrgb(hwb)) {}

	Oklab::Oklab(const Lrgb& lrgb) noexcept
		: ColorBase(lrgb.alpha())
	{
		const double l = lrgb.dot(0.4122214708018041, 0.53633253634543, 0.05144599285276585);
		const double m = lrgb.dot(0.2119034982505858, 0.6806995451361225, 0.1073969566132915);
		const double s = lrgb.dot(0.08830246188874209, 0.2817188376235317, 0.6299787004877261);
		const Lrgb lms2(std::cbrt(l), std::cbrt(m), std::cbrt(s));
		setLightness(lms2.dot(0.2104542682745812, 0.7936177747300267, -0.004072043004608028));
		setA(        lms2.dot(1.977998532388508, -2.428592241936286, 0.4505937095477779));
		setB(        lms2.dot(0.02590404248765818, 0.7827717124269177, -0.8086757549145759));
	}

	Oklab::Oklab(const XyzD50& XyzD50) noexcept
		: Oklab(Lrgb(XyzD50)) {}

	Oklab::Oklab(const Lab& lab) noexcept
		: Oklab(Lrgb(lab)) {}

	Oklab::Oklab(const Lch& lch) noexcept
		: Oklab(Lrgb(lch)) {}

	Oklab::Oklab(const XyzD65& xyzD65) noexcept
		: Oklab(Lrgb(xyzD65)) {}

	Oklab::Oklab(const Oklch& oklch) noexcept
		: ColorBase(oklch.alpha())
	{
		double chroma = oklch.chroma();
		if (chroma < 0.0) chroma = 0.0;
		setLightness(oklch.lightness());
		setA(chroma * impl::cosDeg(oklch.hue()));
		setB(chroma * impl::sinDeg(oklch.hue()));
	}

	Oklab::Oklab(const Srgb& srgb) noexcept
		: Oklab(Lrgb(srgb)) {}

	Oklab::Oklab(const DisplayP3& displayP3) noexcept
		: Oklab(Lrgb(displayP3)) {}

	Oklab::Oklab(const A98Rgb& a98Rgb) noexcept
		: Oklab(Lrgb(a98Rgb)) {}

	Oklab::Oklab(const ProphotoRgb& prophotoRgb) noexcept
		: Oklab(Lrgb(prophotoRgb)) {}

	Oklab::Oklab(const Rec2020& rec2020) noexcept
		: Oklab(Lrgb(rec2020)) {}

	Oklch::Oklch(const Rgb& rgb) noexcept
		: Oklch(Oklab(rgb)) {}

	Oklch::Oklch(const Hsl& hsl) noexcept
		: Oklch(Oklab(hsl)) {}

	Oklch::Oklch(const Hwb& hwb) noexcept
		: Oklch(Oklab(hwb)) {}

	Oklch::Oklch(const Lrgb& lrgb) noexcept
		: Oklch(Oklab(lrgb)) {}

	Oklch::Oklch(const XyzD50& xyzD50) noexcept
		: Oklch(Oklab(xyzD50)) {}

	Oklch::Oklch(const Lab& lab) noexcept
		: Oklch(Oklab(lab)) {}

	Oklch::Oklch(const Lch& lch) noexcept
		: Oklch(Oklab(lch)) {}

	Oklch::Oklch(const XyzD65& xyzD65) noexcept
		: Oklch(Oklab(xyzD65)) {}

	Oklch::Oklch(const Oklab& oklab) noexcept
		: ColorBase(oklab.alpha())
	{
		setLightness(oklab.lightness());
		setChroma(std::sqrt(oklab.a() * oklab.a() + oklab.b() * oklab.b()));
		if (hasPowerlessHue()) {
			setChroma(0.0);
			setHue(0.0);
		} else {
			setHue(impl::normalizeHue(impl::atan2Deg(oklab.b(), oklab.a())));
		}
	}

	Oklch::Oklch(const Srgb& srgb) noexcept
		: Oklch(Oklab(srgb)) {}

	Oklch::Oklch(const DisplayP3& displayP3) noexcept
		: Oklch(Oklab(displayP3)) {}

	Oklch::Oklch(const A98Rgb& a98Rgb) noexcept
		: Oklch(Oklab(a98Rgb)) {}

	Oklch::Oklch(const ProphotoRgb& prophotoRgb) noexcept
		: Oklch(Oklab(prophotoRgb)) {}

	Oklch::Oklch(const Rec2020& rec2020) noexcept
		: Oklch(Oklab(rec2020)) {}

	Srgb::Srgb(const Rgb& rgb) noexcept
		: Srgb(rgb.red() / 255.0, rgb.green() / 255.0, rgb.blue() / 255.0, rgb.alpha()) {}

	Srgb::Srgb(const Hsl& hsl) noexcept
		: Srgb(Rgb(hsl)) {}

	Srgb::Srgb(const Hwb& hwb) noexcept
		: Srgb(Rgb(hwb)) {}

	Srgb::Srgb(const Lrgb& lrgb) noexcept
		: ColorBase(
		nonLinear(lrgb.red()),
		nonLinear(lrgb.green()),
		nonLinear(lrgb.blue()),
		lrgb.alpha()) {}

	Srgb::Srgb(const XyzD50& xyzD50) noexcept
		: Srgb(Lrgb(xyzD50)) {}

	Srgb::Srgb(const Lab& lab) noexcept
		: Srgb(Lrgb(lab)) {}

	Srgb::Srgb(const Lch& lch) noexcept
		: Srgb(Lrgb(lch)) {}

	Srgb::Srgb(const XyzD65& xyzD65) noexcept
		: Srgb(Lrgb(xyzD65)) {}

	Srgb::Srgb(const Oklab& oklab) noexcept
		: Srgb(Lrgb(oklab)) {}

	Srgb::Srgb(const Oklch& oklch) noexcept
		: Srgb(Lrgb(oklch)) {}

	Srgb::Srgb(const DisplayP3& displayP3) noexcept
		: Srgb(Lrgb(displayP3)) {}

	Srgb::Srgb(const A98Rgb& a98Rgb) noexcept
		: Srgb(Lrgb(a98Rgb)) {}

	Srgb::Srgb(const ProphotoRgb& prophotoRgb) noexcept
		: Srgb(Lrgb(prophotoRgb)) {}

	Srgb::Srgb(const Rec2020& rec2020) noexcept
		: Srgb(Lrgb(rec2020)) {}

	DisplayP3::DisplayP3(const Rgb& rgb) noexcept
		: DisplayP3(XyzD65(rgb)) {}

	DisplayP3::DisplayP3(const Hsl& hsl) noexcept
		: DisplayP3(XyzD65(hsl)) {}

	DisplayP3::DisplayP3(const Hwb& hwb) noexcept
		: DisplayP3(XyzD65(hwb)) {}

	DisplayP3::DisplayP3(const Lrgb& lrgb) noexcept
		: DisplayP3(XyzD65(lrgb)) {}

	DisplayP3::DisplayP3(const XyzD50& xyzD50) noexcept
		: DisplayP3(XyzD65(xyzD50)) {}

	DisplayP3::DisplayP3(const Lab& lab) noexcept
		: DisplayP3(XyzD65(lab)) {}

	DisplayP3::DisplayP3(const Lch& lch) noexcept
		: DisplayP3(XyzD65(lch)) {}

	DisplayP3::DisplayP3(const XyzD65& xyzD65) noexcept
		: ColorBase(xyzD65.alpha())
	{
		setRed(  nonLinear(xyzD65.dot( 2.493496911941426,   -0.9313836179191243, -0.402710784450717)));
		setGreen(nonLinear(xyzD65.dot(-0.8294889695615748,   1.762664060318347,   0.0236246858419436)));
		setBlue( nonLinear(xyzD65.dot( 0.03584583024378447, -0.0761723892680418,  0.9568845240076873)));
	}

	DisplayP3::DisplayP3(const Oklab& oklab) noexcept
		: DisplayP3(XyzD65(oklab)) {}

	DisplayP3::DisplayP3(const Oklch& oklch) noexcept
		: DisplayP3(XyzD65(oklch)) {}

	DisplayP3::DisplayP3(const Srgb& srgb) noexcept
		: DisplayP3(XyzD65(srgb)) {}

	DisplayP3::DisplayP3(const A98Rgb& a98Rgb) noexcept
		: DisplayP3(XyzD65(a98Rgb)) {}

	DisplayP3::DisplayP3(const ProphotoRgb& prophotoRgb) noexcept
		: DisplayP3(XyzD65(prophotoRgb)) {}

	DisplayP3::DisplayP3(const Rec2020& rec2020) noexcept
		: DisplayP3(XyzD65(rec2020)) {}

	A98Rgb::A98Rgb(const Rgb& rgb) noexcept
		: A98Rgb(XyzD65(rgb)) {}

	A98Rgb::A98Rgb(const Hsl& hsl) noexcept
		: A98Rgb(XyzD65(hsl)) {}

	A98Rgb::A98Rgb(const Hwb& hwb) noexcept
		: A98Rgb(XyzD65(hwb)) {}

	A98Rgb::A98Rgb(const Lrgb& lrgb) noexcept
		: A98Rgb(XyzD65(lrgb)) {}

	A98Rgb::A98Rgb(const XyzD50& xyzD50) noexcept
		: A98Rgb(XyzD65(xyzD50)) {}

	A98Rgb::A98Rgb(const Lab& lab) noexcept
		: A98Rgb(XyzD65(lab)) {}

	A98Rgb::A98Rgb(const Lch& lch) noexcept
		: A98Rgb(XyzD65(lch)) {}

	A98Rgb::A98Rgb(const XyzD65& xyzD65) noexcept
		: ColorBase(xyzD65.alpha())
	{
		setRed(  nonLinear(xyzD65.dot( 2.041587903810747,   -0.5650069742788597, -0.3447313507783297)));
		setGreen(nonLinear(xyzD65.dot(-0.9692436362808798,   1.875967501507721,   0.04155505740717561)));
		setBlue( nonLinear(xyzD65.dot( 0.01344428063203115, -0.1183623922310184,  1.015174994391206)));
	}

	A98Rgb::A98Rgb(const Oklab& oklab) noexcept
		: A98Rgb(XyzD65(oklab)) {}

	A98Rgb::A98Rgb(const Oklch& oklch) noexcept
		: A98Rgb(XyzD65(oklch)) {}

	A98Rgb::A98Rgb(const Srgb& srgb) noexcept
		: A98Rgb(XyzD65(srgb)) {}

	A98Rgb::A98Rgb(const DisplayP3& displayP3) noexcept
		: A98Rgb(XyzD65(displayP3)) {}

	A98Rgb::A98Rgb(const ProphotoRgb& prophotoRgb) noexcept
		: A98Rgb(XyzD65(prophotoRgb)) {}

	A98Rgb::A98Rgb(const Rec2020& rec2020) noexcept
		: A98Rgb(XyzD65(rec2020)) {}

	ProphotoRgb::ProphotoRgb(const Rgb& rgb) noexcept
		: ProphotoRgb(XyzD65(rgb)) {}

	ProphotoRgb::ProphotoRgb(const Hsl& hsl) noexcept
		: ProphotoRgb(XyzD65(hsl)) {}

	ProphotoRgb::ProphotoRgb(const Hwb& hwb) noexcept
		: ProphotoRgb(XyzD65(hwb)) {}

	ProphotoRgb::ProphotoRgb(const Lrgb& lrgb) noexcept
		: ProphotoRgb(XyzD65(lrgb)) {}

	ProphotoRgb::ProphotoRgb(const XyzD50& xyzD50) noexcept
		: ProphotoRgb(XyzD65(xyzD50)) {}

	ProphotoRgb::ProphotoRgb(const Lab& lab) noexcept
		: ProphotoRgb(XyzD65(lab)) {}

	ProphotoRgb::ProphotoRgb(const Lch& lch) noexcept
		: ProphotoRgb(XyzD65(lch)) {}

	ProphotoRgb::ProphotoRgb(const XyzD65& xyzD65) noexcept
		: ColorBase(xyzD65.alpha())
	{
		setRed(nonLinear(xyzD65.dot(   1.403190463377498, -0.2230151447905165, -0.1016066850741379)));
		setGreen(nonLinear(xyzD65.dot(-0.5262384021633072, 1.481631962923465,   0.0170187902725269)));
		setBlue(nonLinear(xyzD65.dot( -0.0112022652862215, 0.018246403479621,   0.9112472274915047)));
	}

	ProphotoRgb::ProphotoRgb(const Oklab& oklab) noexcept
		: ProphotoRgb(XyzD65(oklab)) {}

	ProphotoRgb::ProphotoRgb(const Oklch& oklch) noexcept
		: ProphotoRgb(XyzD65(oklch)) {}

	ProphotoRgb::ProphotoRgb(const Srgb& srgb) noexcept
		: ProphotoRgb(XyzD65(srgb)) {}

	ProphotoRgb::ProphotoRgb(const DisplayP3& displayP3) noexcept
		: ProphotoRgb(XyzD65(displayP3)) {}

	ProphotoRgb::ProphotoRgb(const A98Rgb& a98Rgb) noexcept
		: ProphotoRgb(XyzD65(a98Rgb)) {}

	ProphotoRgb::ProphotoRgb(const Rec2020& rec2020) noexcept
		: ProphotoRgb(XyzD65(rec2020)) {}

	Rec2020::Rec2020(const Rgb& rgb) noexcept
		: Rec2020(XyzD65(rgb)) {}

	Rec2020::Rec2020(const Hsl& hsl) noexcept
		: Rec2020(XyzD65(hsl)) {}

	Rec2020::Rec2020(const Hwb& hwb) noexcept
		: Rec2020(XyzD65(hwb)) {}

	Rec2020::Rec2020(const Lrgb& lrgb) noexcept
		: Rec2020(XyzD65(lrgb)) {}

	Rec2020::Rec2020(const XyzD50& xyzD50) noexcept
		: Rec2020(XyzD65(xyzD50)) {}

	Rec2020::Rec2020(const Lab& lab) noexcept
		: Rec2020(XyzD65(lab)) {}

	Rec2020::Rec2020(const Lch& lch) noexcept
		: Rec2020(XyzD65(lch)) {}

	Rec2020::Rec2020(const XyzD65& xyzD65) noexcept
		: ColorBase(xyzD65.alpha())
	{
		setRed(  nonLinear(xyzD65.dot( 1.716651187971268,   -0.3556707837763926, -0.2533662813736599)));
		setGreen(nonLinear(xyzD65.dot(-0.6666843518324892,   1.616481236634939,   0.01576854581391114)));
		setBlue( nonLinear(xyzD65.dot( 0.01763985744531079, -0.04277061325780853, 0.9421031212354739)));
	}

	Rec2020::Rec2020(const Oklab& oklab) noexcept
		: Rec2020(XyzD65(oklab)) {}

	Rec2020::Rec2020(const Oklch& oklch) noexcept
		: Rec2020(XyzD65(oklch)) {}

	Rec2020::Rec2020(const Srgb& srgb) noexcept
		: Rec2020(XyzD65(srgb)) {}

	Rec2020::Rec2020(const DisplayP3& displayP3) noexcept
		: Rec2020(XyzD65(displayP3)) {}

	Rec2020::Rec2020(const A98Rgb& a98Rgb) noexcept
		: Rec2020(XyzD65(a98Rgb)) {}

	Rec2020::Rec2020(const ProphotoRgb& prophotoRgb) noexcept
		: Rec2020(XyzD65(prophotoRgb)) {}


	// Functions that returns a string.

	std::string Rgb::hex() const
	{
		std::ostringstream o;
		Rgb rgb = fit();
		o << "#" << std::hex << std::setfill('0');
		o << std::setw(2) << rgb.red8()
			<< std::setw(2) << rgb.green8()
			<< std::setw(2) << rgb.blue8();
		if (rgb.alpha8() < 255U) {
			o << std::setw(2) << rgb.alpha8();
		}
		return o.str();
	}

	std::string Hsl::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Hwb::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Lrgb::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string XyzD50::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Lab::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Lch::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string XyzD65::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Oklab::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Oklch::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Srgb::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string DisplayP3::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string A98Rgb::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string ProphotoRgb::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Rec2020::hex() const
	{
		return Rgb(*this).hex();
	}

	std::string Rgb::name() const
	{
		return impl::toName(hex().c_str());
	}

	std::string Hsl::name() const
	{
		return Rgb(*this).name();
	}

	std::string Hwb::name() const
	{
		return Rgb(*this).name();
	}

	std::string Lrgb::name() const
	{
		return Rgb(*this).name();
	}

	std::string XyzD50::name() const
	{
		return Rgb(*this).name();
	}

	std::string Lab::name() const
	{
		return Rgb(*this).name();
	}

	std::string Lch::name() const
	{
		return Rgb(*this).name();
	}

	std::string XyzD65::name() const
	{
		return Rgb(*this).name();
	}

	std::string Oklab::name() const
	{
		return Rgb(*this).name();
	}

	std::string Oklch::name() const
	{
		return Rgb(*this).name();
	}

	std::string Srgb::name() const
	{
		return Rgb(*this).name();
	}

	std::string DisplayP3::name() const
	{
		return Rgb(*this).name();
	}

	std::string A98Rgb::name() const
	{
		return Rgb(*this).name();
	}

	std::string ProphotoRgb::name() const
	{
		return Rgb(*this).name();
	}

	std::string Rec2020::name() const
	{
		return Rgb(*this).name();
	}

	std::string Rgb::name4() const
	{
		return impl::toName4(hex().c_str());
	}

	std::string Hsl::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Hwb::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Lrgb::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string XyzD50::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Lab::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Lch::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string XyzD65::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Oklab::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Oklch::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Srgb::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string DisplayP3::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string A98Rgb::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string ProphotoRgb::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Rec2020::name4() const
	{
		return Rgb(*this).name4();
	}

	std::string Rgb::css() const
	{
		std::ostringstream o;
		Rgb rgb = fit();
		if (rgb.isOpaque(1000.0)) {
			rgb.printColor(o, "rgb", ",000+");
		} else {
			rgb.printColor(o, "rgba", ",000+");
		}
		return o.str();
	}

	std::string Hsl::css() const
	{
		std::ostringstream o;
		Hsl hsl = clip();
		if (hsl.isOpaque(1000.0)) {
			hsl.printColor(o, "hsl", ",0AA+");
		} else {
			hsl.printColor(o, "hsla", ",0AA+");
		}
		return o.str();
	}

	std::string Hwb::css() const
	{
		std::ostringstream o;
		Hwb hwb = fit();
		hwb.printColor(o, "hwb", " 0AA+");
		return o.str();
	}

	std::string Lrgb::css() const
	{
		std::ostringstream o;
		printColor(o, "srgb-linear", " 4444", true);
		return o.str();
	}

	std::string XyzD50::css() const
	{
		std::ostringstream o;
		printColor(o, "xyz-d50", " 5554", true);
		return o.str();
	}

	std::string Lab::css() const
	{
		std::ostringstream o;
		printColor(o, "lab", " D334");
		return o.str();
	}

	std::string Lch::css() const
	{
		std::ostringstream o;
		printColor(o, "lch", " D334");
		return o.str();
	}

	std::string XyzD65::css() const
	{
		std::ostringstream o;
		printColor(o, "xyz", " 5554", true);
		return o.str();
	}

	std::string Oklab::css() const
	{
		std::ostringstream o;
		Oklab oklab = *this * Oklab(100.0, 1.0, 1.0);
		oklab.printColor(o, "oklab", " D554");
		return o.str();
	}

	std::string Oklch::css() const
	{
		std::ostringstream o;
		Oklch oklch = *this * Oklch(100.0, 1.0, 1.0);
		oklch.printColor(o, "oklch", " D534");
		return o.str();
	}

	std::string Srgb::css() const
	{
		std::ostringstream o;
		printColor(o, "srgb", " 4444", true);
		return o.str();
	}

	std::string DisplayP3::css() const
	{
		std::ostringstream o;
		printColor(o, "display-p3", " 4444", true);
		return o.str();
	}

	std::string A98Rgb::css() const
	{
		std::ostringstream o;
		printColor(o, "a98-rgb", " 4444", true);
		return o.str();
	}

	std::string ProphotoRgb::css() const
	{
		std::ostringstream o;
		printColor(o, "prophoto-rgb", " 4444", true);
		return o.str();
	}

	std::string Rec2020::css() const
	{
		std::ostringstream o;
		printColor(o, "rec2020", " 4444", true);
		return o.str();
	}

	std::string Rgb::cssf() const
	{
		std::ostringstream o;
		Rgb rgb = fit();
		if (rgb.isOpaque(10000.0)) {
			rgb.printColor(o, "rgb", ",111-");
		} else {
			rgb.printColor(o, "rgba", ",111-");
		}
		return o.str();
	}

	std::string Hsl::cssf() const
	{
		std::ostringstream o;
		Hsl hsl = clip();
		if (hsl.isOpaque(10000.0)) {
			hsl.printColor(o, "hsl", ",1BB-");
		} else {
			hsl.printColor(o, "hsla", ",1BB-");
		}
		return o.str();
	}

	std::string Hwb::cssf() const
	{
		std::ostringstream o;
		Hwb hwb = fit();
		hwb.printColor(o, "hwb", " 1BB-");
		return o.str();
	}

	std::string Lrgb::cssf() const
	{
		std::ostringstream o;
		printColor(o, "srgb-linear", " 5555", true);
		return o.str();
	}

	std::string XyzD50::cssf() const
	{
		std::ostringstream o;
		printColor(o, "xyz-d50", " 6665", true);
		return o.str();
	}

	std::string Lab::cssf() const
	{
		std::ostringstream o;
		printColor(o, "lab", " E445");
		return o.str();
	}

	std::string Lch::cssf() const
	{
		std::ostringstream o;
		printColor(o, "lch", " E445");
		return o.str();
	}

	std::string XyzD65::cssf() const
	{
		std::ostringstream o;
		printColor(o, "xyz", " 6665", true);
		return o.str();
	}

	std::string Oklab::cssf() const
	{
		std::ostringstream o;
		Oklab oklab = *this * Oklab(100.0, 1.0, 1.0);
		oklab.printColor(o, "oklab", " E665");
		return o.str();
	}

	std::string Oklch::cssf() const
	{
		std::ostringstream o;
		Oklch oklch = *this * Oklch(100.0, 1.0, 1.0);
		oklch.printColor(o, "oklch", " E645");
		return o.str();
	}

	std::string Srgb::cssf() const
	{
		std::ostringstream o;
		printColor(o, "srgb", " 5555", true);
		return o.str();
	}

	std::string DisplayP3::cssf() const
	{
		std::ostringstream o;
		printColor(o, "display-p3", " 5555", true);
		return o.str();
	}

	std::string A98Rgb::cssf() const
	{
		std::ostringstream o;
		printColor(o, "a98-rgb", " 5555", true);
		return o.str();
	}

	std::string ProphotoRgb::cssf() const
	{
		std::ostringstream o;
		printColor(o, "prophoto-rgb", " 5555", true);
		return o.str();
	}

	std::string Rec2020::cssf() const
	{
		std::ostringstream o;
		printColor(o, "rec2020", " 5555", true);
		return o.str();
	}

	std::string Rgb::cssp() const
	{
		std::ostringstream o;
		Rgb rgb = fit() / Rgb(2.55, 2.55, 2.55);
		rgb.printColor(o, "rgb", " DDDD");
		return o.str();
	}

	std::string Hsl::cssp() const
	{
		std::ostringstream o;
		Hsl hsl = clip();
		hsl.printColor(o, "hsl", " 2DDD");
		return o.str();
	}

	std::string Hwb::cssp() const
	{
		std::ostringstream o;
		Hwb hwb = fit();
		hwb.printColor(o, "hwb", " 2DDD");
		return o.str();
	}

	std::string Lrgb::cssp() const
	{
		std::ostringstream o;
		Lrgb lrgb = *this * Lrgb(100.0, 100.0, 100.0);
		lrgb.printColor(o, "srgb-linear", " DDDD", true);
		return o.str();
	}

	std::string XyzD50::cssp() const
	{
		std::ostringstream o;
		XyzD50 xyzD50 = *this * XyzD50(100.0, 100.0, 100.0);
		xyzD50.printColor(o, "xyz-d50", " DDDD", true);
		return o.str();
	}

	std::string Lab::cssp() const
	{
		std::ostringstream o;
		Lab lab = *this / Lab(1.0, 1.25, 1.25);
		lab.printColor(o, "lab", " DDDD");
		return o.str();
	}

	std::string Lch::cssp() const
	{
		std::ostringstream o;
		Lch lch = *this / Lch(1.0, 1.5, 1.0);
		lch.printColor(o, "lch", " DD2D");
		return o.str();
	}

	std::string XyzD65::cssp() const
	{
		std::ostringstream o;
		XyzD65 xyzD65 = *this * XyzD65(100.0, 100.0, 100.0);
		xyzD65.printColor(o, "xyz", " DDDD", true);
		return o.str();
	}

	std::string Oklab::cssp() const
	{
		std::ostringstream o;
		Oklab oklab = *this / Oklab(0.01, 0.004, 0.004);
		oklab.printColor(o, "oklab", " DDDD");
		return o.str();
	}

	std::string Oklch::cssp() const
	{
		std::ostringstream o;
		Oklch oklch = *this / Oklch(0.01, 0.004, 1.0);
		oklch.printColor(o, "oklch", " DD2D");
		return o.str();
	}

	std::string Srgb::cssp() const
	{
		std::ostringstream o;
		Srgb srgb = *this * Srgb(100.0, 100.0, 100.0);
		srgb.printColor(o, "srgb", " DDDD", true);
		return o.str();
	}

	std::string DisplayP3::cssp() const
	{
		std::ostringstream o;
		DisplayP3 displayP3 = *this * DisplayP3(100.0, 100.0, 100.0);
		displayP3.printColor(o, "display-p3", " DDDD", true);
		return o.str();
	}

	std::string A98Rgb::cssp() const
	{
		std::ostringstream o;
		A98Rgb a98Rgb = *this * A98Rgb(100.0, 100.0, 100.0);
		a98Rgb.printColor(o, "a98-rgb", " DDDD", true);
		return o.str();
	}

	std::string ProphotoRgb::cssp() const
	{
		std::ostringstream o;
		ProphotoRgb prophotoRgb = *this * ProphotoRgb(100.0, 100.0, 100.0);
		prophotoRgb.printColor(o, "prophoto-rgb", " DDDD", true);
		return o.str();
	}

	std::string Rec2020::cssp() const
	{
		std::ostringstream o;
		Rec2020 rec2020 = *this * Rec2020(100.0, 100.0, 100.0);
		rec2020.printColor(o, "rec2020", " DDDD", true);
		return o.str();
	}


	// Additional color manipulation functions of the Rgb class.
	// The blending formulas are adapted from 
	// https://www.w3.org/TR/compositing-1/ and https://drafts.fxtf.org/compositing-2/ .

	namespace impl
	{
		template <typename T> inline T invert(const T& t, double vmax = 1.0)
		{
			return T(vmax - t.red(), vmax - t.green(), vmax - t.blue(), t.alpha()).clip();
		}
	}

	Rgb Rgb::invert() const noexcept
	{
		return impl::invert(*this, 255.0);
	}

	namespace impl
	{
		template <typename T, typename F> inline T blend(const T& ts, const T& tb, double vmax, F&& B)
		{
			const T v(vmax, vmax, vmax, 1);
			T cs = ts / v;
			T cb = tb / v;
			const double as = cs.alpha();
			const double ab = cb.alpha();
			double ao = as + ab * (1.0 - as);
			if (ao <= 0.0) {
				return T();
			}
			T t;
			t.setRed(  (as * (1.0 - ab) * cs.red()   + as * ab * B(cb.red(),   cs.red())   + (1.0 - as) * ab * cb.red())   / ao);
			t.setGreen((as * (1.0 - ab) * cs.green() + as * ab * B(cb.green(), cs.green()) + (1.0 - as) * ab * cb.green()) / ao);
			t.setBlue( (as * (1.0 - ab) * cs.blue()  + as * ab * B(cb.blue(),  cs.blue())  + (1.0 - as) * ab * cb.blue())  / ao);
			t.setAlpha(ao);
			return t * v;
		}
	}

	namespace impl
	{
		template <typename T> inline T blendNormal(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				return normalize1(cs);
			});
		}
	}

	Rgb Rgb::blendNormal(const Rgb& rgb) const noexcept
	{
		return impl::blendNormal(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendMultiply(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				return normalize1(cb * cs);
			});
		}
	}

	Rgb Rgb::blendMultiply(const Rgb& rgb) const noexcept
	{
		return impl::blendMultiply(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendDarken(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				return normalize1(cb < cs ? cb : cs);
			});
		}
	}

	Rgb Rgb::blendDarken(const Rgb& rgb) const noexcept
	{
		return impl::blendDarken(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendLighten(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				return normalize1(cb > cs ? cb : cs);
			});
		}
	}

	Rgb Rgb::blendLighten(const Rgb& rgb) const noexcept
	{
		return impl::blendLighten(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendScreen(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				return normalize1(cb + cs - cb * cs);
			});
		}
	}

	Rgb Rgb::blendScreen(const Rgb& rgb) const noexcept
	{
		return impl::blendScreen(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendOverlay(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				std::swap(cb, cs);
				if (cs <= 0.5) {
					cs = 2.0 * cs;
					return normalize1(cb * cs);
				} else {
					cs = 2.0 * cs - 1.0;
					return normalize1(cb + cs - cb * cs);
				}
			});
		}
	}

	Rgb Rgb::blendOverlay(const Rgb& rgb) const noexcept
	{
		return impl::blendOverlay(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendColorDodge(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				if (cb <= 0.0) {
					return 0.0;
				}
				if (cs >= 1.0) {
					return 1.0;
				}
				double f = cb / (1.0 - cs);
				if (f > 1.0) f = 1.0;
				return f;
			});
		}
	}

	Rgb Rgb::blendColorDodge(const Rgb& rgb) const noexcept
	{
		return impl::blendColorDodge(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendColorBurn(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				if (cb >= 1.0) {
					return 1.0;
				}
				if (cs <= 0.0) {
					return 0.0;
				}
				double f = (1.0 - cb) / cs;
				if (f > 1.0) f = 1.0;
				return 1.0 - f;
			});
		}
	}

	Rgb Rgb::blendColorBurn(const Rgb& rgb) const noexcept
	{
		return impl::blendColorBurn(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendHardLight(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				if (cs <= 0.5) {
					cs = 2.0 * cs;
					return normalize1(cb * cs);
				} else {
					cs = 2.0 * cs - 1.0;
					return normalize1(cb + cs - cb * cs);
				}
			});
		}
	}

	Rgb Rgb::blendHardLight(const Rgb& rgb) const noexcept
	{
		return impl::blendHardLight(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendSoftLight(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				if (cs <= 0.5) {
					return cb - (1 - 2 * cs) * cb * (1 - cb);
				}
				double dcb;
				if (cb <= 0.25) {
					dcb = ((16.0 * cb - 12.0) * cb + 4.0) * cb;
				} else {
					dcb = std::sqrt(cb);
				}
				return normalize1(cb + (2.0 * cs - 1.0) * (dcb - cb));
			});
		}
	}

	Rgb Rgb::blendSoftLight(const Rgb& rgb) const noexcept
	{
		return impl::blendSoftLight(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendDifference(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
				return normalize1(std::fabs(cb - cs));
			});
		}
	}

	Rgb Rgb::blendDifference(const Rgb& rgb) const noexcept
	{
		return impl::blendDifference(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendExclusion(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend(ts, tb, vmax, [](double cb, double cs) -> double {
					return normalize1(cb + cs - 2.0 * cb * cs);
			});
		}
	}

	Rgb Rgb::blendExclusion(const Rgb& rgb) const noexcept
	{
		return impl::blendExclusion(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T, typename F> inline T blend2(const T& ts, const T& tb, double vmax, F&& B)
		{
			const T v(vmax, vmax, vmax, 1);
			T cs = ts / v;
			T cb = tb / v;
			const double as = cs.alpha();
			const double ab = cb.alpha();
			double ao = as + ab * (1.0 - as);
			if (ao <= 0.0) {
				return T();
			}
			T t;
			T b = B(cb, cs);
			t.setRed((  as * (1.0 - ab) * cs.red()   + as * ab * b.red()   + (1.0 - as) * ab * cb.red())   / ao);
			t.setGreen((as * (1.0 - ab) * cs.green() + as * ab * b.green() + (1.0 - as) * ab * cb.green()) / ao);
			t.setBlue(( as * (1.0 - ab) * cs.blue()  + as * ab * b.blue()  + (1.0 - as) * ab * cb.blue())  / ao);
			t.setAlpha(ao);
			return t * v;
		}

		template <typename T> double Lum(const T& c) noexcept
		{
			return 0.3 * c.red() + 0.59 * c.green() + 0.11 * c.blue();
		}

		template <typename T> double Max(const T& c) noexcept
		{
			return (std::max)({c.red(), c.green(), c.blue()});
		}

		template <typename T> double Min(const T& c) noexcept
		{
			return (std::min)({c.red(), c.green(), c.blue()});
		}

		template <typename T> inline T ClipColor(const T& t) noexcept
		{
			T c(t);
			const double L = Lum(c);
			const double vmin = Min(c);
			const double vmax = Max(c);
			if (vmin < 0.0) {
				c.setRed(  L + (c.red()   - L) * L / (L - vmin));
				c.setGreen(L + (c.green() - L) * L / (L - vmin));
				c.setBlue( L + (c.blue()  - L) * L / (L - vmin));
			}
			if (vmax > 1.0) {
				c.setRed(  L + (c.red()   - L) * (1.0 - L) / (vmax - L));
				c.setGreen(L + (c.green() - L) * (1.0 - L) / (vmax - L));
				c.setBlue( L + (c.blue()  - L) * (1.0 - L) / (vmax - L));
			}
			return c;
		}

		template <typename T> inline T SetLum(const T& c, double l) noexcept
		{
			const double d = l - Lum(c);
			return ClipColor(T(c.red() + d, c.green() + d, c.blue() + d, c.alpha()));
		}

		template <typename T> inline double Sat(const T& c) noexcept
		{
			return Max(c) - Min(c);
		}

		template <typename T> struct Vi
		{
			double vmin, vmid, vmax, valpha;
			int imin, imid, imax;

			explicit Vi(const T& t) noexcept
				: vmin(t.red()), vmid(t.green()), vmax(t.blue()), valpha(t.alpha()), imin(0), imid(1), imax(2)
			{
				if (vmin > vmid) {
					std::swap(vmin, vmid);
					std::swap(imin, imid);
				}
				if (vmin > vmax) {
					std::swap(vmin, vmax);
					std::swap(imin, imax);
				}
				if (vmid > vmax) {
					std::swap(vmid, vmax);
					std::swap(imid, imax);
				}
			}

			explicit operator T() noexcept
			{
				if (imin > imid) {
					std::swap(imin, imid);
					std::swap(vmin, vmid);
				}
				if (imin > imax) {
					std::swap(imin, imax);
					std::swap(vmin, vmax);
				}
				if (imid > imax) {
					std::swap(imid, imax);
					std::swap(vmid, vmax);
				}
				return T(vmin, vmid, vmax, valpha);
			}
		};

		template <typename T> inline T SetSat(const T& t, double s) noexcept
		{
			Vi<T> vi(t);
			if (vi.vmax > vi.vmin) {
				vi.vmid = ((vi.vmid - vi.vmin) / (vi.vmax - vi.vmin)) * s;
				vi.vmax = s;
			} else {
				vi.vmid = vi.vmax = 0.0;
			}
			vi.vmin = 0.0;
			return T(vi);
		}
	}

	namespace impl
	{
		template <typename T> inline T blendHue(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend2(ts, tb, vmax, [](T cb, T cs) -> T {
				return SetLum(SetSat(cs, Sat(cb)), Lum(cb));
			});
		}
	}

	Rgb Rgb::blendHue(const Rgb& rgb) const noexcept
	{
		return impl::blendHue(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendSaturation(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend2(ts, tb, vmax, [](T cb, T cs) -> T {
				return SetLum(SetSat(cb, Sat(cs)), Lum(cb));
			});
		}
	}

	Rgb Rgb::blendSaturation(const Rgb& rgb) const noexcept
	{
		return impl::blendSaturation(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendColor(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend2(ts, tb, vmax, [](T cb, T cs) -> T {
				return SetLum(cs, Lum(cb));
			});
		}
	}

	Rgb Rgb::blendColor(const Rgb& rgb) const noexcept
	{
		return impl::blendColor(*this, rgb, 255.0);
	}

	namespace impl
	{
		template <typename T> inline T blendLuminosity(const T& ts, const T& tb, double vmax = 1.0)
		{
			return blend2(ts, tb, vmax, [](T cb, T cs) -> T {
				return SetLum(cb, Lum(cs));
			});
		}
	}

	Rgb Rgb::blendLuminosity(const Rgb& rgb) const noexcept
	{
		return impl::blendLuminosity(*this, rgb, 255.0);
	}


	// Common color manipulation functions of 18 color classes.

	bool Rgb::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Hsl::isDisplayable() const noexcept
	{
		return (saturation() > -1.0E-12 && saturation() < 100.0 + 1.0E-12)
			&& (lightness()  > -1.0E-12 && lightness()  < 100.0 + 1.0E-12);
	}

	bool Hwb::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Lrgb::isDisplayable() const noexcept
	{
		return (red()   > -1.0E-14 && red()   < 1.0 + 1.0E-14)
			&& (green() > -1.0E-14 && green() < 1.0 + 1.0E-14)
			&& (blue()  > -1.0E-14 && blue()  < 1.0 + 1.0E-14);
	}

	bool XyzD50::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Lab::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Lch::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool XyzD65::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Oklab::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Oklch::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Srgb::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool DisplayP3::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool A98Rgb::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool ProphotoRgb::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Rec2020::isDisplayable() const noexcept
	{
		return Lrgb(*this).isDisplayable();
	}

	bool Hsl::hasPowerlessHue() const noexcept
	{
		return saturation() < 1.0E-11
			|| lightness()  < 1.0E-11
			|| lightness()  > 100.0 - 1.0E-11;
	}

	bool Hwb::hasPowerlessHue() const noexcept
	{
		return 100.0 - (whiteness() + blackness()) < 1.0E-11;
	}

	bool Lch::hasPowerlessHue() const noexcept
	{
		return chroma() < 1.0E-11;
	}

	bool Oklch::hasPowerlessHue() const noexcept
	{
		return chroma() < 1.0E-13;
	}

	Rgb Rgb::clip() const noexcept
	{
		return Rgb(
			impl::normalize255(red()),
			impl::normalize255(green()),
			impl::normalize255(blue()),
			impl::normalize1(alpha())
		);
	}

	Rgb Rgb::round() const noexcept
	{
		return Rgb(
			impl::double255ToInteger(red()),
			impl::double255ToInteger(green()),
			impl::double255ToInteger(blue()),
			impl::normalize1(alpha())
		);
	}

	Hsl Hsl::clip() const noexcept
	{
		Hsl hsl;
		if (hasPowerlessHue()) {
			hsl.setHue(0.0);
			hsl.setSaturation(0.0);
		} else {
			hsl.setHue(impl::normalizeHue(hue()));
			hsl.setSaturation(impl::normalize100(saturation()));
		}
		hsl.setLightness(impl::normalize100(lightness()));
		hsl.setAlpha(impl::normalize1(alpha()));
		return hsl;
	}

	Hwb Hwb::clip() const noexcept
	{
		Hwb hwb;
		if (hasPowerlessHue()) {
			hwb.setHue(0.0);
		} else {
			hwb.setHue(impl::normalizeHue(hue()));
		}
		double w = impl::normalize100(whiteness());
		double b = impl::normalize100(blackness());
		const double s = (w + b) / 100.0;
		if (s > 1.0) {
			w /= s;
			b /= s;
		}
		hwb.setWhiteness(w);
		hwb.setBlackness(b);
		hwb.setAlpha(impl::normalize1(alpha()));
		return hwb;
	}

	Lrgb Lrgb::clip() const noexcept
	{
		return Lrgb(
			impl::toPositiveValue(red()),
			impl::toPositiveValue(green()),
			impl::toPositiveValue(blue()),
			impl::normalize1(alpha())
		);
	}

	XyzD50 XyzD50::clip() const noexcept
	{
		return XyzD50(
			x(),
			impl::toPositiveValue(y()),
			z(),
			impl::normalize1(alpha())
		);
	}

	Lab Lab::clip() const noexcept
	{
		return Lab(
			impl::toPositiveValue(lightness()),
			a(),
			b(),
			impl::normalize1(alpha())
		);
	}

	Lch Lch::clip() const noexcept
	{
		Lch lch;
		lch.setLightness(impl::toPositiveValue(lightness()));
		lch.setChroma(impl::toPositiveValue(chroma()));
		if (lch.hasPowerlessHue()) {
			lch.setHue(0.0);
		} else {
			lch.setHue(impl::normalizeHue(hue()));
		}
		lch.setAlpha(impl::normalize1(alpha()));
		return lch;
	}

	XyzD65 XyzD65::clip() const noexcept
	{
		return XyzD65(
			x(),
			impl::toPositiveValue(y()),
			z(),
			impl::normalize1(alpha())
		);
	}

	Oklab Oklab::clip() const noexcept
	{
		return Oklab(
			impl::toPositiveValue(lightness()),
			a(),
			b(),
			impl::normalize1(alpha())
		);
	}

	Oklch Oklch::clip() const noexcept
	{
		Oklch oklch;
		oklch.setLightness(impl::toPositiveValue(lightness()));
		oklch.setChroma(impl::toPositiveValue(chroma()));
		if (oklch.hasPowerlessHue()) {
			oklch.setHue(0.0);
		} else {
			oklch.setHue(impl::normalizeHue(hue()));
		}
		oklch.setAlpha(impl::normalize1(alpha()));
		return oklch;
	}

	Srgb Srgb::clip() const noexcept
	{
		return Srgb(
			impl::toPositiveValue(red()),
			impl::toPositiveValue(green()),
			impl::toPositiveValue(blue()),
			impl::normalize1(alpha())
		);
	}

	DisplayP3 DisplayP3::clip() const noexcept
	{
		return DisplayP3(
			impl::toPositiveValue(red()),
			impl::toPositiveValue(green()),
			impl::toPositiveValue(blue()),
			impl::normalize1(alpha())
		);
	}

	A98Rgb A98Rgb::clip() const noexcept
	{
		return A98Rgb(
			impl::toPositiveValue(red()),
			impl::toPositiveValue(green()),
			impl::toPositiveValue(blue()),
			impl::normalize1(alpha())
		);
	}

	ProphotoRgb ProphotoRgb::clip() const noexcept
	{
		return ProphotoRgb(
			impl::toPositiveValue(red()),
			impl::toPositiveValue(green()),
			impl::toPositiveValue(blue()),
			impl::normalize1(alpha())
		);
	}

	Rec2020 Rec2020::clip() const noexcept
	{
		return Rec2020(
			impl::toPositiveValue(red()),
			impl::toPositiveValue(green()),
			impl::toPositiveValue(blue()),
			impl::normalize1(alpha())
		);
	}

	namespace impl
	{
		template <typename T> inline T fit(const T& t)
		{
			Lrgb lrgb(t);
			if (lrgb.isDisplayable()) {
				return t.clip();
			}
			if (lrgb.red() >= 1.0 && lrgb.green() >= 1.0 && lrgb.blue() >= 1.0) {
				return T(Lrgb(1.0, 1.0, 1.0, impl::normalize1(lrgb.alpha())));
			}

			Oklab high(t);
			high.setAlpha(impl::normalize1(high.alpha()));
			if (high.lightness() >= 1.0) {
				return T(Lrgb(1.0, 1.0, 1.0, high.alpha()));
			} else if (high.lightness() <= 0.0) {
				return T(Lrgb(0.0, 0.0, 0.0, high.alpha()));
			}
			Oklab low(high.lightness(), 0.0, 0.0, high.alpha());
			for (int i = 0; i < 20; ++i) {
				Oklab mid = (high + low) / 2;
				if (mid.isDisplayable()) {
					low = mid;
				} else {
					high = mid;
				}
			}
			return T(low);
		}
	}

	Rgb Rgb::fit() const noexcept
	{
		if (isDisplayable()) {
			return *this;
		}
		return impl::fit(*this);
	}

	Hsl Hsl::fit() const noexcept
	{
		return clip();
	}

	Hwb Hwb::fit() const noexcept
	{
		return impl::fit(*this);
	}

	Lrgb Lrgb::fit() const noexcept
	{
		return impl::fit(*this);
	}

	XyzD50 XyzD50::fit() const noexcept
	{
		return impl::fit(*this);
	}

	Lab Lab::fit() const noexcept
	{
		return impl::fit(*this);
	}

	Lch Lch::fit() const noexcept
	{
		return impl::fit(*this);
	}

	XyzD65 XyzD65::fit() const noexcept
	{
		return impl::fit(*this);
	}

	Oklab Oklab::fit() const noexcept
	{
		return impl::fit(*this);
	}

	Oklch Oklch::fit() const noexcept
	{
		return impl::fit(*this);
	}

	Srgb Srgb::fit() const noexcept
	{
		return impl::fit(*this);
	}

	DisplayP3 DisplayP3::fit() const noexcept
	{
		return impl::fit(*this);
	}

	A98Rgb A98Rgb::fit() const noexcept
	{
		return impl::fit(*this);
	}

	ProphotoRgb ProphotoRgb::fit() const noexcept
	{
		return impl::fit(*this);
	}

	Rec2020 Rec2020::fit() const noexcept
	{
		return impl::fit(*this);
	}

	double Rgb::distance(const Rgb& rgb) const noexcept
	{
		return measureDistance(rgb);
	}

	double Hsl::distance(const Hsl& hsl) const noexcept
	{
		return measureDistance(hsl);
	}

	double Hwb::distance(const Hwb& hwb) const noexcept
	{
		return measureDistance(hwb);
	}

	double Lrgb::distance(const Lrgb& lrgb) const noexcept
	{
		return measureDistance(lrgb);
	}

	double XyzD50::distance(const XyzD50& xyzD50) const noexcept
	{
		return measureDistance(xyzD50);
	}

	double Lab::distance(const Lab& lab) const noexcept
	{
		return measureDistance(lab);
	}

	double Lch::distance(const Lch& lch) const noexcept
	{
		return measureDistance(lch);
	}

	double XyzD65::distance(const XyzD65& xyzD65) const noexcept
	{
		return measureDistance(xyzD65);
	}

	double Oklab::distance(const Oklab& oklab) const noexcept
	{
		return measureDistance(oklab);
	}

	double Oklch::distance(const Oklch& oklch) const noexcept
	{
		return measureDistance(oklch);
	}

	double Srgb::distance(const Srgb& srgb) const noexcept
	{
		return measureDistance(srgb);
	}

	double DisplayP3::distance(const DisplayP3& displayP3) const noexcept
	{
		return measureDistance(displayP3);
	}

	double A98Rgb::distance(const A98Rgb& a98Rgb) const noexcept
	{
		return measureDistance(a98Rgb);
	}

	double ProphotoRgb::distance(const ProphotoRgb& prophotoRgb) const noexcept
	{
		return measureDistance(prophotoRgb);
	}

	double Rec2020::distance(const Rec2020& rec2020) const noexcept
	{
		return measureDistance(rec2020);
	}

	namespace impl
	{
		template <typename T> inline double calculateLuminance(const T& t)
		{
			return Lrgb(t).calculateLuminance();
		}
	}

	double Rgb::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double Hsl::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double Hwb::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double Lrgb::calculateLuminance() const noexcept
	{
		return dot(0.2126390058715102, 0.715168678767756, 0.07219231536073371);
	}

	double XyzD50::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double Lab::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double Lch::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double XyzD65::calculateLuminance() const noexcept
	{
		return y();
	}

	double Oklab::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double Oklch::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double Srgb::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double DisplayP3::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double A98Rgb::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double ProphotoRgb::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	double Rec2020::calculateLuminance() const noexcept
	{
		return impl::calculateLuminance(*this);
	}

	namespace impl
	{
		template <typename T> inline T maximizeLightness(const T& t)
		{
			Lrgb lrgb(t);
			if (lrgb.red() >= 1.0 || lrgb.green() >= 1.0 || lrgb.blue() >= 1.0) {
				return T(Lrgb(
					impl::normalize1(lrgb.red()),
					impl::normalize1(lrgb.green()),
					impl::normalize1(lrgb.blue()),
					impl::normalize1(lrgb.alpha())
				));
			}

			Oklab low(t);
			if (low.lightness() >= 1.0) {
				return T(Lrgb(1.0, 1.0, 1.0, low.alpha()));
			} else if (low.lightness() <= 0.0) {
				return T(Lrgb(0.0, 0.0, 0.0, low.alpha()));
			}
			Oklab high(1.0, low.a(), low.b(), low.alpha());
			for (int i = 0; i < 20; ++i) {
				Oklab mid = (high + low) / 2;
				if (mid.isDisplayable()) {
					low = mid;
				} else {
					high = mid;
				}
			}
			return T(low);
		}
	}

	Rgb Rgb::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Hsl Hsl::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Hwb Hwb::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Lrgb Lrgb::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	XyzD50 XyzD50::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Lab Lab::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Lch Lch::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	XyzD65 XyzD65::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Oklab Oklab::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Oklch Oklch::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Srgb Srgb::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	DisplayP3 DisplayP3::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	A98Rgb A98Rgb::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	ProphotoRgb ProphotoRgb::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	Rec2020 Rec2020::maximizeLightness() noexcept
	{
		return impl::maximizeLightness(*this);
	}

	namespace impl
	{
		template <typename T> inline T brighten(const T& t, double f)
		{
			if (f == 0.0) {
				return t;
			}
			Oklab l(t);
			double g = l.lightness() + f;
			if (g < 0.0) {
				g = 0.0;
			}
			l.setLightness(g);
			return T(l);
		}
	}

	Rgb Rgb::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	Hsl Hsl::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	Hwb Hwb::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	Lrgb Lrgb::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	XyzD50 XyzD50::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	Lab Lab::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	Lch Lch::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	XyzD65 XyzD65::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	Oklab Oklab::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	Oklch Oklch::brighten(double f) const noexcept
	{
		if (f == 0.0) {
			return *this;
		}
		Oklch l(*this);
		double g = l.lightness() + f;
		if (g < 0.0) {
			g = 0.0;
		}
		l.setLightness(g);
		return l;
	}

	Srgb Srgb::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	DisplayP3 DisplayP3::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	A98Rgb A98Rgb::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	ProphotoRgb ProphotoRgb::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	Rec2020 Rec2020::brighten(double f) const noexcept
	{
		return impl::brighten(*this, f);
	}

	namespace impl
	{
		template <typename T> inline T darken(const T& t, double f)
		{
			if (f == 0.0) {
				return t;
			}
			Oklab l(t);
			double g = l.lightness() - f;
			if (g < 0.0) {
				g = 0.0;
			}
			l.setLightness(g);
			return T(l);
		}
	}

	Rgb Rgb::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	Hsl Hsl::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	Hwb Hwb::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	Lrgb Lrgb::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	XyzD50 XyzD50::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	Lab Lab::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	Lch Lch::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	XyzD65 XyzD65::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	Oklab Oklab::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	Oklch Oklch::darken(double f) const noexcept
	{
		if (f == 0.0) {
			return *this;
		}
		Oklch l(*this);
		double g = l.lightness() - f;
		if (g < 0.0) {
			g = 0.0;
		}
		l.setLightness(g);
		return l;
	}

	Srgb Srgb::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	DisplayP3 DisplayP3::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	A98Rgb A98Rgb::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	ProphotoRgb ProphotoRgb::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	Rec2020 Rec2020::darken(double f) const noexcept
	{
		return impl::darken(*this, f);
	}

	namespace impl
	{
		template <typename T> inline T saturate(const T& t, double f)
		{
			if (f == 0.0) {
				return t;
			}
			Oklch l(t);
			double g = l.chroma() + f;
			if (g < 0.0) {
				g = 0.0;
			}
			l.setChroma(g);
			return T(l);
		}
	}

	Rgb Rgb::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Hsl Hsl::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Hwb Hwb::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Lrgb Lrgb::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	XyzD50 XyzD50::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Lab Lab::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Lch Lch::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	XyzD65 XyzD65::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Oklab Oklab::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Oklch Oklch::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Srgb Srgb::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	DisplayP3 DisplayP3::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	A98Rgb A98Rgb::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	ProphotoRgb ProphotoRgb::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	Rec2020 Rec2020::saturate(double f) const noexcept
	{
		return impl::saturate(*this, f);
	}

	namespace impl
	{
		template <typename T> inline T desaturate(const T& t, double f)
		{
			if (f == 0.0) {
				return t;
			}
			Oklch l(t);
			double g = l.chroma() - f;
			if (g < 0.0) {
				g = 0.0;
			}
			l.setChroma(g);
			return T(l);
		}
	}

	Rgb Rgb::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Hsl Hsl::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Hwb Hwb::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Lrgb Lrgb::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	XyzD50 XyzD50::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Lab Lab::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Lch Lch::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	XyzD65 XyzD65::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Oklab Oklab::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Oklch Oklch::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Srgb Srgb::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	DisplayP3 DisplayP3::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	A98Rgb A98Rgb::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	ProphotoRgb ProphotoRgb::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	Rec2020 Rec2020::desaturate(double f) const noexcept
	{
		return impl::desaturate(*this, f);
	}

	namespace impl
	{
		template <typename T> inline T rotate(const T& t, double f)
		{
			if (f == 0.0) {
				return t;
			}
			Oklch l(t);
			l.setHue(l.hue() + f);
			return T(l);
		}
	}

	Rgb Rgb::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Hsl Hsl::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Hwb Hwb::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Lrgb Lrgb::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	XyzD50 XyzD50::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Lab Lab::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Lch Lch::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	XyzD65 XyzD65::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Oklab Oklab::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Oklch Oklch::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Srgb Srgb::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	DisplayP3 DisplayP3::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	A98Rgb A98Rgb::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	ProphotoRgb ProphotoRgb::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	Rec2020 Rec2020::rotate(double f) const noexcept
	{
		return impl::rotate(*this, f);
	}

	namespace impl
	{
		template <typename T> inline T flip(const T& t, double f)
		{
			Oklch l(t);
			l.setHue(2.0 * f - l.hue());
			return T(l);
		}
	}

	Rgb Rgb::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Hsl Hsl::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Hwb Hwb::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Lrgb Lrgb::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	XyzD50 XyzD50::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Lab Lab::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Lch Lch::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	XyzD65 XyzD65::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Oklab Oklab::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Oklch Oklch::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Srgb Srgb::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	DisplayP3 DisplayP3::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	A98Rgb A98Rgb::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	ProphotoRgb ProphotoRgb::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	Rec2020 Rec2020::flip(double f) const noexcept
	{
		return impl::flip(*this, f);
	}

	namespace impl
	{
		template <typename T> inline T mixRgb(const T& t1, const T& t2, double f)
		{
			if (f <= 0.0) {
				return t1;
			} else if (f >= 1.0) {
				return t2;
			}
			const double g = 1.0 - f;
			if (!t1.isOpaque() || !t2.isOpaque()) {
				const double a1 = t1.alpha() * g;
				const double a2 = t2.alpha() * f;
				const double a3 = a1 + a2;
				if (a3 > 0.0) {
					return T(
						(t1.red() * a1 + t2.red() * a2) / a3,
						(t1.green() * a1 + t2.green() * a2) / a3,
						(t1.blue() * a1 + t2.blue() * a2) / a3,
						a3
					);
				}
			}
			return T(t1 * g + t2 * f);
		}

		template <typename T> inline T mixXyz(const T& t1, const T& t2, double f)
		{
			if (f <= 0.0) {
				return t1;
			} else if (f >= 1.0) {
				return t2;
			}
			const double g = 1.0 - f;
			if (!t1.isOpaque() || !t2.isOpaque()) {
				const double a1 = t1.alpha() * g;
				const double a2 = t2.alpha() * f;
				const double a3 = a1 + a2;
				if (a3 > 0.0) {
					return T(
						(t1.x() * a1 + t2.x() * a2) / a3,
						(t1.y() * a1 + t2.y() * a2) / a3,
						(t1.z() * a1 + t2.z() * a2) / a3,
						a3
					);
				}
			}
			return T(t1 * g + t2 * f);
		}

		template <typename T> inline T mixLab(const T& t1, const T& t2, double f)
		{
			if (f <= 0.0) {
				return t1;
			} else if (f >= 1.0) {
				return t2;
			}
			const double g = 1.0 - f;
			if (!t1.isOpaque() || !t2.isOpaque()) {
				const double a1 = t1.alpha() * g;
				const double a2 = t2.alpha() * f;
				const double a3 = a1 + a2;
				if (a3 > 0.0) {
					return T(
						(t1.lightness() * a1 + t2.lightness() * a2) / a3,
						(t1.a() * a1 + t2.a() * a2) / a3,
						(t1.b() * a1 + t2.b() * a2) / a3,
						a3
					);
				}
			}
			return T(t1 * g + t2 * f);
		}

		template <typename T> inline T mixLch(const T& t1, const T& t2, double f)
		{
			if (f <= 0.0) {
				return t1;
			} else if (f >= 1.0) {
				return t2;
			}
			double h1 = t1.hue();
			double h2 = t2.hue();
			if (t2.hasPowerlessHue()) {
				h2 = h1;
			} else if (t1.hasPowerlessHue()) {
				h1 = h2;
			}
			const double g = 1.0 - f;
			const double a1 = t1.alpha() * g;
			const double a2 = t2.alpha() * f;
			const double a3 = a1 + a2;
			if (!t1.isOpaque() || !t2.isOpaque()) {
				if (a3 > 0.0) {
					return T(
						(t1.lightness() * a1 + t2.lightness() * a2) / a3,
						(t1.chroma() * a1 + t2.chroma() * a2) / a3,
						impl::normalizeHue(h1 * g + h2 * f),
						a3
					);
				}
			}
			return T(
				t1.lightness() * g + t2.lightness() * f,
				t1.chroma() * g + t2.chroma() * f,
				impl::normalizeHue(h1 * g + h2 * f),
				a3
			);
		}
	}

	Rgb Rgb::mix(const Rgb& rgb, double f) const noexcept
	{
		return impl::mixRgb(*this, rgb, f);
	}

	Hsl Hsl::mix(const Hsl& hsl, double f) const noexcept
	{
		if (f <= 0.0) {
			return *this;
		} else if (f >= 1.0) {
			return hsl;
		}
		double h1 = hue();
		double h2 = hsl.hue();
		if (hsl.hasPowerlessHue()) {
			h2 = h1;
		} else if (hasPowerlessHue()) {
			h1 = h2;
		}
		const double g = 1.0 - f;
		const double a1 = alpha() * g;
		const double a2 = hsl.alpha() * f;
		const double a3 = a1 + a2;
		if (!isOpaque() || !hsl.isOpaque()) {
			if (a3 > 0.0) {
				return Hsl(
					impl::normalizeHue(h1 * g + h2 * f),
					(saturation() * a1 + hsl.saturation() * a2) / a3,
					(lightness() * a1 + hsl.lightness() * a2) / a3,
					a3
				);
			}
		}
		return Hsl(
			impl::normalizeHue(h1 * g + h2 * f),
			saturation() * g + hsl.saturation() * f,
			lightness() * g + hsl.lightness() * f,
			a3
		);
	}

	Hwb Hwb::mix(const Hwb& hwb, double f) const noexcept
	{
		if (f <= 0.0) {
			return *this;
		} else if (f >= 1.0) {
			return hwb;
		}
		double h1 = hue();
		double h2 = hwb.hue();
		if (hwb.hasPowerlessHue()) {
			h2 = h1;
		} else if (hasPowerlessHue()) {
			h1 = h2;
		}
		const double g = 1.0 - f;
		const double a1 = alpha() * g;
		const double a2 = hwb.alpha() * f;
		const double a3 = a1 + a2;
		if (!isOpaque() || !hwb.isOpaque()) {
			if (a3 > 0.0) {
				return Hwb(
					impl::normalizeHue(h1 * g + h2 * f),
					(whiteness() * a1 + hwb.whiteness() * a2) / a3,
					(blackness() * a1 + hwb.blackness() * a2) / a3,
					a3
				);
			}
		}
		return Hwb(
			impl::normalizeHue(h1 * g + h2 * f),
			whiteness() * g + hwb.whiteness() * f,
			blackness() * g + hwb.blackness() * f,
			a3
		);
	}

	Lrgb Lrgb::mix(const Lrgb& lrgb, double f) const noexcept
	{
		return impl::mixRgb(*this, lrgb, f);
	}

	XyzD50 XyzD50::mix(const XyzD50& xyzD50, double f) const noexcept
	{
		return impl::mixXyz(*this, xyzD50, f);
	}

	Lab Lab::mix(const Lab& lab, double f) const noexcept
	{
		return impl::mixLab(*this, lab, f);
	}

	Lch Lch::mix(const Lch& lch, double f) const noexcept
	{
		return impl::mixLch(*this, lch, f);
	}

	XyzD65 XyzD65::mix(const XyzD65& xyzD65, double f) const noexcept
	{
		return impl::mixXyz(*this, xyzD65, f);
	}

	Oklab Oklab::mix(const Oklab& oklab, double f) const noexcept
	{
		return impl::mixLab(*this, oklab, f);
	}

	Oklch Oklch::mix(const Oklch& oklch, double f) const noexcept
	{
		return impl::mixLch(*this, oklch, f);
	}

	Srgb Srgb::mix(const Srgb& srgb, double f) const noexcept
	{
		return impl::mixRgb(*this, srgb, f);
	}

	DisplayP3 DisplayP3::mix(const DisplayP3& displayP3, double f) const noexcept
	{
		return impl::mixRgb(*this, displayP3, f);
	}

	A98Rgb A98Rgb::mix(const A98Rgb& a98Rgb, double f) const noexcept
	{
		return impl::mixRgb(*this, a98Rgb, f);
	}

	ProphotoRgb ProphotoRgb::mix(const ProphotoRgb& prophotoRgb, double f) const noexcept
	{
		return impl::mixRgb(*this, prophotoRgb, f);
	}

	Rec2020 Rec2020::mix(const Rec2020& rec2020, double f) const noexcept
	{
		return impl::mixRgb(*this, rec2020, f);
	}

	namespace impl
	{
		template <typename T> inline double deltaE_00(const T& t1, const T& t2)
		{
			return Lab(t1).deltaE_00(Lab(t2));
		}
	}

	double Rgb::deltaE_00(const Rgb& rgb) const noexcept
	{
		return impl::deltaE_00(*this, rgb);
	}

	double Hsl::deltaE_00(const Hsl& hsl) const noexcept
	{
		return impl::deltaE_00(*this, hsl);
	}

	double Hwb::deltaE_00(const Hwb& hwb) const noexcept
	{
		return impl::deltaE_00(*this, hwb);
	}

	double Lrgb::deltaE_00(const Lrgb& lrgb) const noexcept
	{
		return impl::deltaE_00(*this, lrgb);
	}

	double XyzD50::deltaE_00(const XyzD50& xyzD50) const noexcept
	{
		return impl::deltaE_00(*this, xyzD50);
	}


	// CIEDE2000
	// This implementation is adapted from https://en.wikipedia.org/wiki/Color_difference .

	double Lab::deltaE_00(const Lab& lab) const noexcept
	{
		const double L1 = lightness();
		const double L2 = lab.lightness();
		const double a1 = a();
		const double a2 = lab.a();
		const double b1 = b();
		const double b2 = lab.b();
		const double C1 = std::sqrt(a1 * a1 + b1 * b1);
		const double C2 = std::sqrt(a2 * a2 + b2 * b2);

		const double dLpr = L2 - L1;
		const double Lbr = (L1 + L2) / 2.0;
		const double Cbr = (C1 + C2) / 2.0;
		const double xx_apr = std::pow(Cbr, 7) / (std::pow(Cbr, 7) + std::pow(25.0, 7));
		const double x_apr = 1.0 - std::sqrt(xx_apr);
		const double apr1 = a1 + (a1 / 2.0) * x_apr;
		const double apr2 = a2 + (a2 / 2.0) * x_apr;
		const double Cpr1 = std::sqrt(apr1 * apr1 + b1 * b1);
		const double Cpr2 = std::sqrt(apr2 * apr2 + b2 * b2);
		const double Cbrpr = (Cpr1 + Cpr2) / 2.0;
		const double dCpr = Cpr2 - Cpr1;
		const double x_hpr1 = impl::atan2Deg(b1, apr1);
		const double hpr1 = (x_hpr1 >= 0) ? x_hpr1 : x_hpr1 + 360.0;
		const double x_hpr2 = impl::atan2Deg(b2, apr2);
		const double hpr2 = (x_hpr2 >= 0) ? x_hpr2 : x_hpr2 + 360.0;
		double dhpr = 0.0;
		double dHpr = 0.0;
		double Hbrpr = hpr1 + hpr2;
		if (Cpr1 != 0.0 && Cpr2 != 0.0) {
			if (std::fabs(hpr1 - hpr2) <= 180.0) {
				dhpr = hpr2 - hpr1;
			} else {
				if (hpr2 <= hpr1) {
					dhpr = hpr2 - hpr1 + 360.0;
				} else {
					dhpr = hpr2 - hpr1 - 360.0;
				}
			}
			dHpr = 2.0 * std::sqrt(Cpr1 * Cpr2) * impl::sinDeg(dhpr / 2.0);
			if (std::fabs(hpr1 - hpr2) <= 180.0) {
				Hbrpr = (hpr1 + hpr2) / 2.0;
			} else {
				if (hpr1 + hpr2 < 360.0) {
					Hbrpr = (hpr1 + hpr2 + 360.0) / 2.0;
				} else {
					Hbrpr = (hpr1 + hpr2 - 360.0) / 2.0;
				}
			}
		}
		const double T = 1.0
			- 0.17 * impl::cosDeg(Hbrpr - 30.0)
			+ 0.24 * impl::cosDeg(2.0 * Hbrpr)
			+ 0.32 * impl::cosDeg(3.0 * Hbrpr + 6.0)
			- 0.20 * impl::cosDeg(4.0 * Hbrpr - 63.0);
		const double x_SL = (Lbr - 50.0) * (Lbr - 50.0);
		const double SL = 1.0 + 0.015 * x_SL / std::sqrt(20.0 + x_SL);
		const double SC = 1.0 + 0.045 * Cbrpr;
		const double SH = 1.0 + 0.015 * Cbrpr * T;
		const double x_RT1 = std::pow(Cbrpr, 7)
			/ (std::pow(Cbrpr, 7) + std::pow(25.0, 7));
		const double x_RT2 = (Hbrpr - 275.0) / 25.0;
		const double RT = -2.0 * std::sqrt(x_RT1) * impl::sinDeg(60.0 * std::exp(-x_RT2 * x_RT2));
		const double x_1 = dLpr / SL;
		const double x_2 = dCpr / SC;
		const double x_3 = dHpr / SH;
		const double dE00 = std::sqrt(x_1 * x_1 + x_2 * x_2 + x_3 * x_3 + RT * x_2 * x_3);
		return dE00;
	}

	double Lch::deltaE_00(const Lch& lch) const noexcept
	{
		return impl::deltaE_00(*this, lch);
	}

	double XyzD65::deltaE_00(const XyzD65& xyzD65) const noexcept
	{
		return impl::deltaE_00(*this, xyzD65);
	}

	double Oklab::deltaE_00(const Oklab& oklab) const noexcept
	{
		return impl::deltaE_00(*this, oklab);
	}

	double Oklch::deltaE_00(const Oklch& oklch) const noexcept
	{
		return impl::deltaE_00(*this, oklch);
	}

	double Srgb::deltaE_00(const Srgb& srgb) const noexcept
	{
		return impl::deltaE_00(*this, srgb);
	}

	double DisplayP3::deltaE_00(const DisplayP3& displayP3) const noexcept
	{
		return impl::deltaE_00(*this, displayP3);
	}

	double A98Rgb::deltaE_00(const A98Rgb& a98Rgb) const noexcept
	{
		return impl::deltaE_00(*this, a98Rgb);
	}

	double ProphotoRgb::deltaE_00(const ProphotoRgb& prophotoRgb) const noexcept
	{
		return impl::deltaE_00(*this, prophotoRgb);
	}

	double Rec2020::deltaE_00(const Rec2020& rec2020) const noexcept
	{
		return impl::deltaE_00(*this, rec2020);
	}

	namespace impl
	{
		template <typename T> inline double deltaE(const T& t1, const T& t2)
		{
			return Oklab(t1).distance(Oklab(t2));
		}
	}

	double Rgb::deltaE(const Rgb& rgb) const noexcept
	{
		return impl::deltaE(*this, rgb);
	}

	double Hsl::deltaE(const Hsl& hsl) const noexcept
	{
		return impl::deltaE(*this, hsl);
	}

	double Hwb::deltaE(const Hwb& hwb) const noexcept
	{
		return impl::deltaE(*this, hwb);
	}

	double Lrgb::deltaE(const Lrgb& lrgb) const noexcept
	{
		return impl::deltaE(*this, lrgb);
	}

	double XyzD50::deltaE(const XyzD50& xyzD50) const noexcept
	{
		return impl::deltaE(*this, xyzD50);
	}

	double Lab::deltaE(const Lab& lab) const noexcept
	{
		return impl::deltaE(*this, lab);
	}

	double Lch::deltaE(const Lch& lch) const noexcept
	{
		return impl::deltaE(*this, lch);
	}

	double XyzD65::deltaE(const XyzD65& xyzD65) const noexcept
	{
		return impl::deltaE(*this, xyzD65);
	}

	double Oklab::deltaE(const Oklab& oklab) const noexcept
	{
		return distance(oklab);
	}

	double Oklch::deltaE(const Oklch& oklch) const noexcept
	{
		return impl::deltaE(*this, oklch);
	}

	double Srgb::deltaE(const Srgb& srgb) const noexcept
	{
		return impl::deltaE(*this, srgb);
	}

	double DisplayP3::deltaE(const DisplayP3& displayP3) const noexcept
	{
		return impl::deltaE(*this, displayP3);
	}

	double A98Rgb::deltaE(const A98Rgb& a98Rgb) const noexcept
	{
		return impl::deltaE(*this, a98Rgb);
	}

	double ProphotoRgb::deltaE(const ProphotoRgb& prophotoRgb) const noexcept
	{
		return impl::deltaE(*this, prophotoRgb);
	}

	double Rec2020::deltaE(const Rec2020& rec2020) const noexcept
	{
		return impl::deltaE(*this, rec2020);
	}

	namespace impl
	{
		template <typename T> inline T grayscale(const T& t)
		{
			return T(Oklab(t).grayscale());
		}
	}

	Rgb Rgb::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	Hsl Hsl::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	Hwb Hwb::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	Lrgb Lrgb::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	XyzD50 XyzD50::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	Lab Lab::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	Lch Lch::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	XyzD65 XyzD65::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	Oklab Oklab::grayscale() const noexcept
	{
		return Oklab(lightness(), 0.0, 0.0, alpha());
	}

	Oklch Oklch::grayscale() const noexcept
	{
		return Oklch(lightness(), 0.0, 0.0, alpha());
	}

	Srgb Srgb::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	DisplayP3 DisplayP3::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	A98Rgb A98Rgb::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	ProphotoRgb ProphotoRgb::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	Rec2020 Rec2020::grayscale() const noexcept
	{
		return impl::grayscale(*this);
	}

	namespace impl
	{
		template <typename T> inline double contrast(const T& t1, const T& t2)
		{
			return Lrgb(t1).contrast(Lrgb(t2));
		}
	}

	double Rgb::contrast(const Rgb& rgb) const noexcept
	{
		return impl::contrast(*this, rgb);
	}

	double Hsl::contrast(const Hsl& hsl) const noexcept
	{
		return impl::contrast(*this, hsl);
	}

	double Hwb::contrast(const Hwb& hwb) const noexcept
	{
		return impl::contrast(*this, hwb);
	}


	// This function caluculates the WCAG contrast ratio.
	// https://www.w3.org/TR/WCAG20-TECHS/G18.html

	double Lrgb::contrast(const Lrgb& lrgb) const noexcept
	{
		const double l1 = calculateLuminance() + 0.05;
		const double l2 = lrgb.calculateLuminance() + 0.05;
		if (l1 <= 0.0 || l2 <= 0.0) {
			return 0.0;
		}
		return (l1 > l2) ? (l1 / l2) : (l2 / l1);
	}

	double XyzD50::contrast(const XyzD50& xyzD50) const noexcept
	{
		return impl::contrast(*this, xyzD50);
	}

	double Lab::contrast(const Lab& lab) const noexcept
	{
		return impl::contrast(*this, lab);
	}

	double Lch::contrast(const Lch& lch) const noexcept
	{
		return impl::contrast(*this, lch);
	}

	double XyzD65::contrast(const XyzD65& xyzD65) const noexcept
	{
		return impl::contrast(*this, xyzD65);
	}

	double Oklab::contrast(const Oklab& oklab) const noexcept
	{
		return impl::contrast(*this, oklab);
	}

	double Oklch::contrast(const Oklch& oklch) const noexcept
	{
		return impl::contrast(*this, oklch);
	}

	double Srgb::contrast(const Srgb& srgb) const noexcept
	{
		return impl::contrast(*this, srgb);
	}

	double DisplayP3::contrast(const DisplayP3& displayP3) const noexcept
	{
		return impl::contrast(*this, displayP3);
	}

	double A98Rgb::contrast(const A98Rgb& a98Rgb) const noexcept
	{
		return impl::contrast(*this, a98Rgb);
	}

	double ProphotoRgb::contrast(const ProphotoRgb& prophotoRgb) const noexcept
	{
		return impl::contrast(*this, prophotoRgb);
	}

	double Rec2020::contrast(const Rec2020& rec2020) const noexcept
	{
		return impl::contrast(*this, rec2020);
	}


	// Dave Green's `cubehelix' color scheme.
	// http://www.mrao.cam.ac.uk/~dag/CUBEHELIX/

	class Cubehelix
	{
	public:
		explicit Cubehelix(double start = 0.5, double rotations = -1.5,
			double hue = 1.0, double gamma = 1.0) noexcept
			: mStart(start), mRotations(rotations),
				mHue(hue), mGamma(gamma) {}

		Cubehelix& setStart(double start = 0.5) noexcept
		{
			mStart = start;
			return *this;
		}

		Cubehelix& setRotations(double rotations = -1.5) noexcept
		{
			mRotations = rotations;
			return *this;
		}

		Cubehelix& setHue(double hue = 1.0) noexcept
		{
			mHue = hue;
			return *this;
		}

		Cubehelix& setGamma(double gamma = 1.0) noexcept
		{
			mGamma = gamma;
			return *this;
		}

		Rgb operator()(double fract) const noexcept
		{
			const double PI = 3.14159265358979323846;
			const double hue = 2.0 * PI * (mStart / 3.0 + 1.0 + mRotations * fract);
			fract = std::pow(fract, mGamma);
			const double amp = mHue * fract * (1.0 - fract) / 2.0;
			Rgb rgb;
			rgb.setRed(255.0 * (fract + amp * (-0.14861 * std::cos(hue) + 1.78277 * std::sin(hue))));
			rgb.setGreen(255.0 * (fract + amp * (-0.29227 * std::cos(hue) - 0.90649 * std::sin(hue))));
			rgb.setBlue(255.0 * (fract + amp *(1.97294 * std::cos(hue))));
			return rgb.clip();
		}

	private:
		double mStart, mRotations, mHue, mGamma;
	};


	// A class template for converting numbers to colors.

	template <typename T> class ColorScale
	{
	public:
		ColorScale()
			: colorList{T(Cn::Black), T(Cn::White)}, domainList{0.0, 1.0} {}

		explicit ColorScale(std::initializer_list<const char*> cl)
		{
			for (auto it = cl.begin(); it != cl.end(); ++it) {
				T t(*it);
				colorList.push_back(t);
			}
			correctColorList();
			correctDomainList();
		}

		ColorScale(std::initializer_list<const char*> cl, const std::vector<double>& dl)
			: domainList(dl)
		{
			for (auto it = cl.begin(); it != cl.end(); ++it) {
				T t(*it);
				colorList.push_back(t);
			}
			correctColorList();
			correctDomainList();
		}

		explicit ColorScale(const std::vector<T>& cl)
			: colorList(cl)
		{
			correctColorList();
			correctDomainList();
		}

		ColorScale(const std::vector<T>& cl, const std::vector<double>& dl)
			: colorList(cl), domainList(dl)
		{
			correctColorList();
			correctDomainList();
		}

		const std::vector<T>& getColorList() const
		{
			return colorList;
		}

		const std::vector<double>& getDomain() const
		{
			return domainList;
		}


		// This function implements linear interpolation between colors.

		T operator()(double f, double gamma = 1.0) const noexcept
		{
			const auto dminmax = std::minmax_element(domainList.cbegin(), domainList.cend());
			const double dmin = *dminmax.first;
			const double dmax = *dminmax.second;
			if (f < dmin) {
				f = dmin;
			} else if (f > dmax) {
				f = dmax;
			} else {
				if (gamma != 1.0 && gamma > 0.0) {
					const double d = dmax - dmin;
					if (d != 0.0) {
						f = std::pow((f - dmin) / d, gamma) * d + dmin;
					}
				}
			}
			const size_t dsize = domainList.size();
			for (size_t i = 1U; i < dsize; ++i) {
				const double f1 = domainList[i - 1U];
				const double f2 = domainList[i];
				if (f1 == f2) {
					if (f == f1) {
						return mixedColor(i, 0.0);
					}
				} else if (f >= f1 && f <= f2) {
					return mixedColor(i, (f - f1) / (f2 - f1));
				} else if (f >= f2 && f <= f1) {
					return mixedColor(i, 1.0 - (f - f2) / (f1 - f2));
				}
			}
			return T();
		}

		std::vector<T> colors(int n = 0, double gamma = 1.0) const
		{
			std::vector<T> c;
			double f1 = domainList.empty() ? 0.0 : domainList.front();
			double f2 = domainList.empty() ? 1.0 : domainList.back();
			if (n <= 0) {
				return colorList;
			}
			if (n == 1) {
				c.push_back(operator()((f1 + f2) / 2.0));
				return c;
			}
			if (f1 > f2) {
				std::swap(f1, f2);
			}
			for (int i = 0; i < n; ++i) {
				const double e = static_cast<double>(i) / (static_cast<double>(n) - 1.0);
				c.push_back(operator()(f1 + e * (f2 - f1), gamma));
			}
			return c;
		}

		// Hue interpolation strategies

		enum class Hs : int
		{
			Shorter = 0,
			Longer = 1,
			Increasing = 2,
			Decreasing = 3
		};

		ColorScale<T> shorter() const
		{
			return adjustHues(Hs::Shorter);
		}

		ColorScale<T> shorter(int index) const
		{
			return adjustHues(Hs::Shorter, index);
		}

		ColorScale<T> longer() const
		{
			return adjustHues(Hs::Longer);
		}

		ColorScale<T> longer(int index) const
		{
			return adjustHues(Hs::Longer, index);
		}

		ColorScale<T> increasing() const
		{
			return adjustHues(Hs::Increasing);
		}

		ColorScale<T> increasing(int index) const
		{
			return adjustHues(Hs::Increasing, index);
		}

		ColorScale<T> decreasing() const
		{
			return adjustHues(Hs::Decreasing);
		}

		ColorScale<T> decreasing(int index) const
		{
			return adjustHues(Hs::Decreasing, index);
		}

		ColorScale<Oklab> correctLightness() const
		{
			std::vector<Oklab> cl;
			for (const auto& t : colorList) {
				cl.push_back(Oklab(t));
			}
			const size_t csize = cl.size();
			if (csize < 3U) {
				return ColorScale<Oklab>(cl, domainList);
			}

			const double lf = cl.front().lightness();
			const double ls = cl.back().lightness() - lf;
			const double df = domainList.front();
			const double ds = domainList.back() - df;

			if (ds == 0) {
				std::vector<Oklab> cl2(cl.size(), cl.front());
				return ColorScale<Oklab>(cl2, domainList);
			}

			assert(csize >= 2U && csize == domainList.size());
			for (size_t i = 1; i < csize - 1U; ++i) {
				cl[i].setLightness(lf + ls * ((domainList[i] - df) / ds));
			}
			return ColorScale<Oklab>(cl, domainList);
		}

	private:
		void correctColorList()
		{
			const size_t csize = colorList.size();
			if (csize < 2U) {
				if (csize == 1U) {
					colorList.push_back(colorList.front());
				} else {
					colorList = {T(Cn::Black), T(Cn::White)};
				}
			}
		}

		void correctDomainList()
		{
			const size_t csize = colorList.size();
			size_t dsize = domainList.size();
			if (dsize < 2U) {
				domainList = {0.0, 1.0};
				dsize = 2U;
			}
			if (csize != dsize) {
				const double dfront = domainList.front();
				const double dback = domainList.back();
				const double d = dback - dfront;
				domainList.clear();
				domainList.push_back(dfront);
				if (csize > 2U) {
					for (size_t n = 1U; n < csize - 1U; ++n) {
						const double f = static_cast<double>(n) / static_cast<double>(csize - 1U);
						domainList.push_back(dfront + d * f);
					}
				}
				domainList.push_back(dback);
			}
		}

		T mixedColor(size_t index, double f) const noexcept
		{
			return colorList[index - 1U].mix(colorList[index], f);
		}

		ColorScale<T> adjustHues(Hs hs) const
		{
			ColorScale<T> s(*this);
			const size_t size = s.colorList.size();
			if (hs == Hs::Shorter) {
				for (size_t i = 0U; i < size - 1U; ++i) {
					auto& c1 = s.colorList[i];
					auto& c2 = s.colorList[i + 1U];
					const double h = c1.hue();
					const double d = impl::normalizeHue(c2.hue() - h);
					if (d <= 180.0) {
						c2.setHue(h + d);
					} else {
						c2.setHue(h + d - 360.0);
					}
				}
			} else if (hs == Hs::Longer) {
				for (size_t i = 0U; i < size - 1U; ++i) {
					auto& c1 = s.colorList[i];
					auto& c2 = s.colorList[i + 1U];
					const double h = c1.hue();
					const double d = impl::normalizeHue(c2.hue() - h);
					if (d > 180.0) {
						c2.setHue(h + d);
					} else if (d != 0.0) {
						c2.setHue(h + d - 360.0);
					} else {
						c2.setHue(h);
					}
				}
			} else if (hs == Hs::Increasing) {
				for (size_t i = 0U; i < size - 1U; ++i) {
					auto& c1 = s.colorList[i];
					auto& c2 = s.colorList[i + 1U];
					const double h = c1.hue();
					const double d = impl::normalizeHue(c2.hue() - h);
					c2.setHue(h + d);
				}
			} else {
				for (size_t i = 0U; i < size - 1U; ++i) {
					auto& c1 = s.colorList[i];
					auto& c2 = s.colorList[i + 1U];
					const double h = c1.hue();
					const double d = impl::normalizeHue(c2.hue() - h);
					if (d != 0.0) {
						c2.setHue(h + d - 360.0);
					} else {
						c2.setHue(h);
					}
				}
			}
			return s;
		}

		ColorScale<T> adjustHues(Hs hs, int index) const
		{
			ColorScale<T> s(*this);
			const size_t size = s.colorList.size();
			if (index < 0 || static_cast<size_t>(index) + 2U > size) {
				return *this;
			}
			const size_t n = static_cast<size_t>(index);
			const double h1 = s.colorList[n].hue();
			const double h2 = s.colorList[n + 1].hue();
			const double d = impl::normalizeHue(h2 - h1);
			double a;
			if (hs == Hs::Shorter) {
				if (d <= 180.0) {
					a = h1 - h2 + d;
				} else {
					a = h1 - h2 + d - 360.0;
				}
			} else if (hs == Hs::Longer) {
				if (d > 180.0) {
					a = h1 - h2 + d;
				} else if (d != 0.0) {
					a = h1 - h2 + d - 360.0;
				} else {
					a = h1 - h2;
				}
			} else if (hs == Hs::Increasing) {
				a = h1 - h2 + d;
			} else {
				if (d != 0.0) {
					a = h1 - h2 + d - 360.0;
				} else {
					a = h1 - h2;
				}
			}
			for (size_t i = n + 1; i < size; ++i) {
				auto& c = s.colorList[i];
				c.setHue(c.hue() + a);
			}
			return s;
		}

	private:
		std::vector<T> colorList;
		std::vector<double> domainList;
	};

	template<> inline ColorScale<Rgb>         ColorScale<Rgb>::adjustHues(Hs)         const { return *this; }
	template<> inline ColorScale<Lrgb>        ColorScale<Lrgb>::adjustHues(Hs)        const { return *this; }
	template<> inline ColorScale<XyzD50>      ColorScale<XyzD50>::adjustHues(Hs)      const { return *this; }
	template<> inline ColorScale<Lab>         ColorScale<Lab>::adjustHues(Hs)         const { return *this; }
	template<> inline ColorScale<XyzD65>      ColorScale<XyzD65>::adjustHues(Hs)      const { return *this; }
	template<> inline ColorScale<Oklab>       ColorScale<Oklab>::adjustHues(Hs)       const { return *this; }
	template<> inline ColorScale<Srgb>        ColorScale<Srgb>::adjustHues(Hs)        const { return *this; }
	template<> inline ColorScale<DisplayP3>   ColorScale<DisplayP3>::adjustHues(Hs)   const { return *this; }
	template<> inline ColorScale<A98Rgb>      ColorScale<A98Rgb>::adjustHues(Hs)      const { return *this; }
	template<> inline ColorScale<ProphotoRgb> ColorScale<ProphotoRgb>::adjustHues(Hs) const { return *this; }
	template<> inline ColorScale<Rec2020>     ColorScale<Rec2020>::adjustHues(Hs)     const { return *this; }

	template<> inline ColorScale<Rgb>         ColorScale<Rgb>::adjustHues(Hs, int)         const { return *this; }
	template<> inline ColorScale<Lrgb>        ColorScale<Lrgb>::adjustHues(Hs, int)        const { return *this; }
	template<> inline ColorScale<XyzD50>      ColorScale<XyzD50>::adjustHues(Hs, int)      const { return *this; }
	template<> inline ColorScale<Lab>         ColorScale<Lab>::adjustHues(Hs, int)         const { return *this; }
	template<> inline ColorScale<XyzD65>      ColorScale<XyzD65>::adjustHues(Hs, int)      const { return *this; }
	template<> inline ColorScale<Oklab>       ColorScale<Oklab>::adjustHues(Hs, int)       const { return *this; }
	template<> inline ColorScale<Srgb>        ColorScale<Srgb>::adjustHues(Hs, int)        const { return *this; }
	template<> inline ColorScale<DisplayP3>   ColorScale<DisplayP3>::adjustHues(Hs, int)   const { return *this; }
	template<> inline ColorScale<A98Rgb>      ColorScale<A98Rgb>::adjustHues(Hs, int)      const { return *this; }
	template<> inline ColorScale<ProphotoRgb> ColorScale<ProphotoRgb>::adjustHues(Hs, int) const { return *this; }
	template<> inline ColorScale<Rec2020>     ColorScale<Rec2020>::adjustHues(Hs, int)     const { return *this; }


	// A class that implements bezier interpolation between colors in Oklab color space.

	class ColorScaleBezier
	{
	public:
		explicit ColorScaleBezier(std::initializer_list<const char*> cl)
			: domainList{0.0, 1.0}
		{
			for (auto it = cl.begin(); it != cl.end(); ++it) {
				Oklab oklab(*it);
				colorList.push_back(oklab);
			}
			correctColorList();
		}

		ColorScaleBezier(std::initializer_list<const char*> cl, const std::array<double, 2>& dl)
			: domainList(dl)
		{
			for (auto it = cl.begin(); it != cl.end(); ++it) {
				Oklab oklab(*it);
				colorList.push_back(oklab);
			}
			correctColorList();
		}

		explicit ColorScaleBezier(const std::vector<Oklab>& cl)
			: colorList(cl), domainList{0.0, 1.0}
		{
			correctColorList();
		}

		ColorScaleBezier(const std::vector<Oklab>& cl, const std::array<double, 2>& dl)
			: colorList(cl), domainList(dl)
		{
			correctColorList();
		}

		const std::vector<Oklab>& getColorList() const
		{
			return colorList;
		}

		const std::array<double, 2>& getDomain() const
		{
			return domainList;
		}

		Oklab operator()(double f, double gamma = 1.0) const noexcept
		{
			const double dfront = domainList.front();
			const double d = domainList.back() - dfront;
			if (d == 0.0) {
				return colorList[0];
			}
			f = (f - dfront) / d;
			if (f < 0.0) {
				f = 0.0;
			} else if (f > 1.0) {
				f = 1.0;
			}
			if (gamma != 1.0 && gamma > 0.0) {
				f = std::pow(f, gamma);
			}
			size_t csize = colorList.size();
			std::vector<Oklab> c = colorList;
			for (; csize > 1U; --csize) {
				for (size_t i = 0U; i < csize - 1U; ++i) {
					c[i] = c[i].mix(c[i + 1U], f);
				}
			}
			return c[0];
		}

		std::vector<Oklab> colors(int n = 0, double gamma = 1.0) const
		{
			if (n <= 0) {
				return colorList;
			}
			std::vector<Oklab> c;
			for (int i = 0; i < n; ++i) {
				const double f = static_cast<double>(i) / (static_cast<double>(n) - 1.0);
				c.push_back(operator()(f, gamma));
			}
			return c;
		}

	private:

		void correctColorList()
		{
			if (colorList.empty()) {
				colorList = {Oklab(Cn::Black), Oklab(Cn::White)};
			}
		}

	private:
		std::vector<Oklab> colorList;
		std::array<double, 2> domainList;
	};

} // namespace colorm
