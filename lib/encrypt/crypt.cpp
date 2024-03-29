#include "crypt.h"
#include "../../include/debug.h"
#include "string_utils.h"

std::string Crypto::crypt(std::string t) {
	// setup the encryption key
	char buf[_key.length() + 1];
	copyString(_key, buf, _key.length() + 1);
	myencrypt((char *) NULL, 0);
	myencrypt(buf, _key.length());

	// and en/de-crypt
	char buf2[t.length() + 1];
	copyString(t, buf2, t.length() + 1);
	myencrypt(buf2, t.length());

	std::string ret(buf2);
	return ret;
}


void Crypto::myencrypt(char *bptr, unsigned len)
{
	int cc;	/* current character being considered */

	static long key = 0;	/* 29 bit encipherment key */
	static int salt = 0;	/* salt to spice up key with */

	if (!bptr) {		/* is there anything here to encrypt? */
		key = len;	/* set the new key */
		salt = len;	/* set the new salt */
		return;
	}
	while (len--) {		/* for every character in the buffer */

		cc = *bptr;	/* get a character out of the buffer */

		/* only encipher printable characters */
		if ((cc >= ' ') && (cc <= '~')) {

/**  If the upper bit (bit 29) is set, feed it back into the key.  This 
	assures us that the starting key affects the entire message.  **/

			key &= 0x1FFFFFFFL;	/* strip off overflow */
			if (key & 0x10000000L) {
				key ^= 0x0040A001L;	/* feedback */
			}

/**  Down-bias the character, perform a Beaufort encipherment, and 
	up-bias the character again.  We want key to be positive 
	so that the left shift here will be more portable and the 
	mod95() faster   **/

			cc = mod95((int) (key % 95) - (cc - ' ')) + ' ';

/**  the salt will spice up the key a little bit, helping to obscure 
	any patterns in the clear text, particularly when all the 
	characters (or long sequences of them) are the same.  We do 
	not want the salt to go negative, or it will affect the key 
	too radically.  It is always a good idea to chop off cyclics 
	to prime values.  **/

			if (++salt >= 20857) {	/* prime modulus */
				salt = 0;
			}

/**  our autokey (a special case of the running key) is being 
	generated by a weighted checksum of clear text, cipher 
	text, and salt.   **/

			key = key + key + cc + *bptr + salt;
		}
		*bptr++ = cc;	/* put character back into buffer */
	}
	return;
}

int Crypto::mod95(int val)
{
	/*  The mathematical MOD does not match the computer MOD  */

	/*  Yes, what I do here may look strange, but it gets the
	   job done, and portably at that.  */

	while (val >= 9500)
		val -= 9500;
	while (val >= 950)
		val -= 950;
	while (val >= 95)
		val -= 95;
	while (val < 0)
		val += 95;
	return val;
}

// Define the crypto instance
Crypto crypto;
