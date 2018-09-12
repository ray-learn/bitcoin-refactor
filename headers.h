/**
 * First include the standard library and set the using declaration
 **/

#define WIN32_LEAN_AND_MEAN 1
#include <openssl/rand.h>

#include <stdio.h>
#include <string>
#include <set>
#include <memory>
#include <assert.h>
#include <winsock2.h>
#include <deque>
#include <map>
#include <algorithm>
#include <array>
#include <process.h>

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

#include "serialize.h"
#include "uint256.h"
#include "util.h"
#include "net.h"
#include "main.h"

