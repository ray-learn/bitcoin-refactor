
#include <vector>
#include <map>
#include <boost/type_traits/is_fundamental.hpp>
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64  int64;
typedef unsigned __int64  uint64;
#else
typedef long long  int64;
typedef unsigned long long  uint64;
#endif
#if defined(_MSC_VER) && _MSC_VER < 1300
#define for  if (false) ; else for
#endif


static const int VERSION = 101;





/////////////////////////////////////////////////////////////////
//
// Templates for serializing to anything that looks like a stream,
// i.e. anything that supports .read(char*, int) and .write(char*, int)
//

enum
{
	// primary actions
	SER_NETWORK = (1 << 0),
	SER_DISK = (1 << 1),
	SER_GETHASH = (1 << 2),

	// modifiers
	SER_SKIPSIG = (1 << 16),
	SER_BLOCKHEADERONLY = (1 << 17),
};

#define IMPLEMENT_SERIALIZE(statements)    \
    unsigned int GetSerializeSize(int nType=0, int nVersion=VERSION) const  \
    {                                           \
        CSerActionGetSerializeSize ser_action;  \
        const bool fGetSize = true;             \
        const bool fWrite = false;              \
        const bool fRead = false;               \
        unsigned int nSerSize = 0;              \
        ser_streamplaceholder s;                \
        s.nType = nType;                        \
        s.nVersion = nVersion;                  \
        {statements}                            \
        return nSerSize;                        \
    }                                           \
    template<typename Stream>                   \
    void Serialize(Stream& s, int nType=0, int nVersion=VERSION) const  \
    {                                           \
        CSerActionSerialize ser_action;         \
        const bool fGetSize = false;            \
        const bool fWrite = true;               \
        const bool fRead = false;               \
        unsigned int nSerSize = 0;              \
        {statements}                            \
    }                                           \
    template<typename Stream>                   \
    void Unserialize(Stream& s, int nType=0, int nVersion=VERSION)  \
    {                                           \
        CSerActionUnserialize ser_action;       \
        const bool fGetSize = false;            \
        const bool fWrite = false;              \
        const bool fRead = true;                \
        unsigned int nSerSize = 0;              \
        {statements}                            \
    }

#define READWRITE(obj)      (nSerSize += ::SerReadWrite(s, (obj), nType, nVersion, ser_action))






//
// Basic types
//
#define WRITEDATA(s, obj)   s.write((char*)&(obj), sizeof(obj))
#define READDATA(s, obj)    s.read((char*)&(obj), sizeof(obj))

inline unsigned int GetSerializeSize(char a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(signed char a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(unsigned char a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(signed short a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(unsigned short a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(signed int a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(unsigned int a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(signed long a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(unsigned long a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(int64 a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(uint64 a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(float a, int, int = 0) { return sizeof(a); }
inline unsigned int GetSerializeSize(double a, int, int = 0) { return sizeof(a); }

template<typename Stream> inline void Serialize(Stream& s, char a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, signed char a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, unsigned char a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, signed short a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, unsigned short a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, signed int a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, unsigned int a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, signed long a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, unsigned long a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int64 a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, uint64 a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, float a, int, int = 0) { WRITEDATA(s, a); }
template<typename Stream> inline void Serialize(Stream& s, double a, int, int = 0) { WRITEDATA(s, a); }

template<typename Stream> inline void Unserialize(Stream& s, char& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, signed char& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, unsigned char& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, signed short& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, unsigned short& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, signed int& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, unsigned int& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, signed long& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, unsigned long& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, int64& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, uint64& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, float& a, int, int = 0) { READDATA(s, a); }
template<typename Stream> inline void Unserialize(Stream& s, double& a, int, int = 0) { READDATA(s, a); }

inline unsigned int GetSerializeSize(bool a, int, int = 0) { return sizeof(char); }
template<typename Stream> inline void Serialize(Stream& s, bool a, int, int = 0) { char f = a; WRITEDATA(s, f); }
template<typename Stream> inline void Unserialize(Stream& s, bool& a, int, int = 0) { char f; READDATA(s, f); a = f; }






//
// If none of the specialized versions above matched, default to calling member function.
// "int nType" is changed to "long nType" to keep from getting an ambiguous overload error.
// The compiler will only cast int to long if none of the other templates matched.
// Thanks to Boost serialization for this idea.
//
template<typename T>
inline unsigned int GetSerializeSize(const T& a, long nType, int nVersion = VERSION)
{
	return a.GetSerializeSize((int)nType, nVersion);
}

template<typename Stream, typename T>
inline void Serialize(Stream& os, const T& a, long nType, int nVersion = VERSION)
{
	a.Serialize(os, (int)nType, nVersion);
}

template<typename Stream, typename T>
inline void Unserialize(Stream& is, T& a, long nType, int nVersion = VERSION)
{
	a.Unserialize(is, (int)nType, nVersion);
}






//
// Support for IMPLEMENT_SERIALIZE and READWRITE macro
//
class CSerActionGetSerializeSize { };
class CSerActionSerialize { };
class CSerActionUnserialize { };

template<typename Stream, typename T>
inline unsigned int SerReadWrite(Stream& s, const T& obj, int nType, int nVersion, CSerActionGetSerializeSize ser_action)
{
	return ::GetSerializeSize(obj, nType, nVersion);
}

template<typename Stream, typename T>
inline unsigned int SerReadWrite(Stream& s, const T& obj, int nType, int nVersion, CSerActionSerialize ser_action)
{
	::Serialize(s, obj, nType, nVersion);
	return 0;
}

template<typename Stream, typename T>
inline unsigned int SerReadWrite(Stream& s, T& obj, int nType, int nVersion, CSerActionUnserialize ser_action)
{
	::Unserialize(s, obj, nType, nVersion);
	return 0;
}

struct ser_streamplaceholder
{
	int nType;
	int nVersion;
};

