#ifndef SEMINAL_H_
#define SEMINAL_H_

/*
 * seminal.h
 *
 *  Created on: 2012. 12. 26.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef ENVIRONMENT_H_
#include "Environment.h"
#endif

#ifndef PAYOFF_H_
#include "Payoff.h"
#endif

#ifndef PLAYERS_H_
#include "Players.h"
#endif

#ifndef LOADER_H_
#include "Loader.h"
#endif

#ifndef REPEATEDGAME_H_
#include "RepeatedGame.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef READER_H_
#include "Reader.h"
#endif

#ifndef WRITER_H_
#include "Writer.h"
#endif

#include <boost/timer.hpp>
#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <dirent.h>
#include <sys/stat.h>

enum Option {
   NONE,AUTOMATONS,PARA,
   REGULAR,PARA_REGULAR,SPECIAL,
   REWARD,ASYM,PUBLIC,
};

struct Range {
	PreciseNumber from, to, interval;
	bool error;

	Range() : error(false) { }
	Range(string sbuf) {
		boost::regex expr("([0-9\\.]+),([0-9\\.]+),([0-9\\.]+)");
		boost::smatch what;

		error = false;

		if (boost::regex_match(sbuf, what, expr)) {
			from = numberToRational(what[1]);
			to = numberToRational(what[2]);
			interval = numberToRational(what[3]);
		}
		else {
			error = true;
		}
	}

	string numberToRational(string number) {
		int pos = number.find(".");
		string res = "";

		if (pos == (signed)string::npos) {
			res = number;
		}
		else {
			bool negative = false;
			if (number[0] == '-') {
				negative = true;

			}
			string part1 = number.substr((negative ? 1 : 0), pos - (negative ? 1 : 0));
			string part2 = number.substr(pos + 1);

			if (part1.length() == 1 && part1[0] == '0') part1 = "";

			string d = "1";
			for (int i = 0; i < (signed)part2.length(); i++) d += "0";
			res = (negative ? "-" : "") + part1 + part2 + "/" + d;
		}

		return res;
	}

};

#endif /* SEMINAL_H_ */
