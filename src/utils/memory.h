#ifndef _RAVEN_UTILS_MEMORY_H_
#define _RAVEN_UTILS_MEMORY_H_

#include"../core/base.h"
#include"../core/math.h"

namespace Raven {
	template <typename T, int logBlockSize>
	class BlockedArray {
	public:
		// BlockedArray Public Methods
		BlockedArray(int uRes, int vRes, const T* d = nullptr)
			: uRes(uRes), vRes(vRes), uBlocks(RoundUp(uRes) >> logBlockSize) {
			int nAlloc = RoundUp(uRes) * RoundUp(vRes);
			data = (T)malloc(sizeof(T) * nAlloc);
			for (int i = 0; i < nAlloc; ++i) new (&data[i]) T();
			if (d)
				for (int v = 0; v < vRes; ++v)
					for (int u = 0; u < uRes; ++u) (*this)(u, v) = d[v * uRes + u];
		}

		int BlockSize() const { return 1 << logBlockSize; }

		int RoundUp(int x) const {
			return (x + BlockSize() - 1) & ~(BlockSize() - 1);
		}

		int uSize() const { return uRes; }

		int vSize() const { return vRes; }

		~BlockedArray() {
			for (int i = 0; i < uRes * vRes; ++i) data[i].~T();
			free(data);
		}
		int Block(int a) const { return a >> logBlockSize; }
		int Offset(int a) const { return (a & (BlockSize() - 1)); }
		T& operator()(int u, int v) {
			int bu = Block(u), bv = Block(v);
			int ou = Offset(u), ov = Offset(v);
			int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
			offset += BlockSize() * ov + ou;
			return data[offset];
		}
		const T& operator()(int u, int v) const {
			int bu = Block(u), bv = Block(v);
			int ou = Offset(u), ov = Offset(v);
			int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
			offset += BlockSize() * ov + ou;
			return data[offset];
		}
		void GetLinearArray(T* a) const {
			for (int v = 0; v < vRes; ++v)
				for (int u = 0; u < uRes; ++u) *a++ = (*this)(u, v);
		}

	private:
		// BlockedArray Private Data
		T* data;
		const int uRes, vRes, uBlocks;
	};

	//class TransformMemoryManager {
	//public:
	//	std::shared_ptr<Transform> find()
	//private:
	//	std::map<std::string, std::shared_ptr<Transform>> transforms;
	//};

}

#endif