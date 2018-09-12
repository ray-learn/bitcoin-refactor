#pragma once

class CBlockIndex;

class CBlock;
class CTransaction;	// Test the sequence about declaration
class CBlockIndex;

bool ProcessMessages(CNode* pfrom);
bool ProcessMessage(CNode* pfrom, string strCommand, CDataStream& vRecv);

extern map<uint256, CBlockIndex*> mapBlockIndex;

/**
 * Describes a place in the block chain to another node such that if the other node doesn't have the same branch, it can find
 * recent common trunk. The further back it is, the further before the fork it may be. 
 **/
class CBlockLocator
{
protected:
	vector<uint256> vHave;
public:
	CBlockLocator() { }
	explicit CBlockLocator(const CBlockIndex* pindex)
	{
		//Set(pindex);
	}
	explicit CBlockLocator(uint256 hashBlock)
	{
		map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hashBlock);

	}
};

// Nodes collect new transactions into a block, hash them into a hash tree,
// and scan through nonce values to make the block's hash satisfy proof-of-work
// requirements. When they solve the proof of work, they broadcast the block
// to everyone and the block is added to the block chain. The first transaction
// in the block is a special one that creates a new coin owned by the creator
// of the block.
// Blocks are appended to blk0001.dat files on disk. Their location on disk 
// is indexed by CBlockIndex objects in memory
class CBlock
{
public:
	// header
	int nVersion;
	uint256 hashPrevBlock;
	uint256 hashMerkleRoot;
	unsigned int nTime;
	unsigned int nBits;
	unsigned int nNonce;

	// network and disk
	//vector<CTransaction> vtx;

	// memory only
	mutable vector<uint256> vMerkleTree;

	CBlock()
	{
		SetNull();
	}

	IMPLEMENT_SERIALIZE
	(
		READWRITE(this->nVersion);
		nVersion = this->nVersion;
		READWRITE(hashPrevBlock);
		READWRITE(hashMerkleRoot);
		READWRITE(nTime);
		READWRITE(nBits);
		READWRITE(nNonce);

		// ConnectBlock depends on vtx being last so it can calculate offset
		//if (!(nType & (SER_GETHASH | SER_BLOCKHEADERONLY)))
			//READWRITE(vtx);
		//else if (fRead)
			//const_cast<CBlock*>(this)->vtx.clear();
	)

	void SetNull()
	{
		nVersion = 1;
		hashPrevBlock = 0;
		hashMerkleRoot = 0;
		nTime = 0;
		nBits = 0;
		nNonce = 0;
		//vtx.clear();
		vMerkleTree.clear();
	}

	bool IsNull() const
	{
		return (nBits == 0);
	}

	uint256 GetHash() const
	{
		//return Hash(BEGIN(nVersion), END(nNonce));
	}

};

/* The block chain is a tree shaped structure starting with the genesis block at the root,
 * with each potentially having multiple candidates to be the next block. pprev and pnext link a path through
 * the main/longest chain. A blockindex may have multiple pprev pointing back to it, 
 * but pnext will only point forward to the longest brance, or will be null if the block is not part of the longest chain.
 */
class CBlockIndex
{
public:
	const uint256* phashBlock;
	CBlockIndex* pprev;
	CBlockIndex* pnext;
	unsigned int nFile;
	unsigned int nBlockPos;
	int nHeight;

	// block header
	int nVersion;
	uint256 hashMerkleRoot;
	unsigned int nTime;
	unsigned int nBits;
	unsigned int nNonce;

	CBlockIndex()
	{
		phashBlock = NULL;
		pprev = NULL;
		pnext = NULL;
		nFile = 0;
		nBlockPos = 0;
		nHeight = 0;

		nVersion = 0;
		hashMerkleRoot = 0;
		nTime = 0;
		nBits = 0;
		nNonce = 0;
	}

	CBlockIndex(unsigned int nFileIn, unsigned int nBlockPosIn, CBlock& block)
	{
		phashBlock = NULL;
		pprev = NULL;
		pnext = NULL;
		nFile = nFileIn;
		nBlockPos = nBlockPosIn;
		nHeight = 0;

		nVersion = block.nVersion;
		hashMerkleRoot = block.hashMerkleRoot;
		nTime = block.nTime;
		nBits = block.nBits;
		nNonce = block.nNonce;
	}

};