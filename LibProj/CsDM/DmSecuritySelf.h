#pragma once

#ifndef DMO_FILE_SYSTEM_ENCRIPT_SELF_H
#define DMO_FILE_SYSTEM_ENCRIPT_SELF_H

#define DmFS_SHIFT(x)	( 0x01 << x )

#define DmFS_IS(x)			( 0 != x )
#define DmFS_SET(x, val)	( x = ( val )?( 1 ):( 0 ) )

namespace DmFS
{
	namespace Enc
	{
		namespace Kind
		{
			enum eDefined
			{
				None = 0,
				Xor = 1, // ok!
				Twist = 2, // ok!
				XorTwist = 4, // ok!
				RvsFull = 8, // ok!
				RvsHalf = 16, // ok!
				Shuffle = 32, // ok!

				All = Xor | Twist | RvsFull | RvsHalf | Shuffle | XorTwist,
			};
		}

		union Field
		{
			Field(dm_byte_t flg)
				: flags( flg )
			{}

			struct
			{
				dm_byte_t xor		: 1;
				dm_byte_t twist		: 1;
				dm_byte_t xortwist 	: 1;
				dm_byte_t rvsfull	: 1;
				dm_byte_t rvshalf	: 1;
				dm_byte_t shuffle	: 1;
			};
			dm_byte_t flags;
		};
	}

	struct SecuritySelf
	{
		// HASHPACK V17 ENCRYPT
		static bool Encript(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec);

		// HASHPACK V17 DECRYPT
		static bool Decript(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec);

		// HASHPACK V18 DECRYPT
		static bool Decript_V18(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec);

		// HASHPACK V18 ENCRYPT
		static bool Encript_V18(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec);

		// HASHPACK V19 DECRYPT
		static bool Decript_V19(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec);

		// HASHPACK V19 ENCRYPT
		static bool Encript_V19(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec);

		// HASHPACK V32 DECRYPT
		static bool Decript_V32(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec);

		// HASHPACK V32 ENCRYPT
		static bool Encript_V32(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec);
	};
}

#endif//DMO_FILE_SYSTEM_ENCRIPT_SELF_H