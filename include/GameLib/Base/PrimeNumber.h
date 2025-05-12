#ifndef INCLUDED_GAMELIB_PRIMENUMBER_H
#define INCLUDED_GAMELIB_PRIMENUMBER_H

namespace GameLib{
	namespace PrimeNumber{
		bool isPrimeNumber( int );
		//引数より大きな最小の素数を返す
		int next( int );
		//引数より小さな最大の素数を返す
		int previous( int );
		//整数の平方根。小数点以下は切り上げ
		int sqrt( int );
	}
}

#endif
