/*
    Mosh: the mobile shell
    Copyright 2012 Keith Winstein

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include "ae.h"
#include <string>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

using std::string;

long int myatoi( const char *str );

namespace Crypto {
  class CryptoException {
  public:
    string text;
    bool fatal;
    CryptoException( string s_text, bool s_fatal = false )
      : text( s_text ), fatal( s_fatal ) {};
  };

  /* 16-byte-aligned buffer, with length. */
  class AlignedBuffer {
  private:
    size_t m_len;
    void *m_allocated;
    char *m_data;

  public:
    AlignedBuffer( size_t len, const char *data = NULL );

    ~AlignedBuffer() {
      free( m_allocated );
    }

    char * data( void ) const { return m_data; }
    size_t len( void )  const { return m_len;  }

  private:
    /* Not implemented */
    AlignedBuffer( const AlignedBuffer& );
    AlignedBuffer& operator=( const AlignedBuffer& );
  };

  class Base64Key {
  private:
    unsigned char key[ 16 ];

  public:
    Base64Key(); /* random key */
    Base64Key( string printable_key );
    string printable_key( void ) const;
    unsigned char *data( void ) { return key; }
  };

  class Nonce {
  private:
    char bytes[ 12 ] __attribute__((__aligned__ (16)));

  public:
    Nonce( uint64_t val );
    Nonce( char *s_bytes, size_t len );
    
    string cc_str( void ) { return string( (char *)( bytes + 4 ), 8 ); }
    char *data( void ) { return bytes; }
    uint64_t val( void );
  };
  
  class Message {
  public:
    Nonce nonce;
    string text;
    
    Message( char *nonce_bytes, size_t nonce_len,
	     char *text_bytes, size_t text_len );
    Message( Nonce s_nonce, string s_text );
  };
  
  class Session {
  private:
    Base64Key key;
    ae_ctx *ctx;
    uint64_t blocks_encrypted;
    
  public:
    Session( Base64Key s_key );
    ~Session();
    
    string encrypt( Message plaintext );
    Message decrypt( string ciphertext );
    
    Session( const Session & );
    Session & operator=( const Session & );
  };

  void disable_dumping_core( void );
}

#endif
