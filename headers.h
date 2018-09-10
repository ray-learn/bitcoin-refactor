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
using namespace std;

#include "serialize.h"
#include "uint256.h"
#include "util.h"
#include "net.h"

