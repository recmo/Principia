/* -*- C++ -*- vim: set syntax=cpp:
 * PURPOSE: File containing definition of token-identifier and
 *          a function that maps token identifiers to a string
 *          name.
 *
 * NOTE: This file has been created automatically by Quex.
 *       Visit quex.org for further info.
 *
 * DATE: Mon Nov 16 19:30:17 2015
 *
 * (C) 2005-2010 Frank-Rene Schaefer
 * ABSOLUTELY NO WARRANTY                                           */
#ifndef __QUEX_INCLUDE_GUARD__AUTO_TOKEN_IDS_QUEX_QUEXPARSER__QUEX_TOKEN__
#define __QUEX_INCLUDE_GUARD__AUTO_TOKEN_IDS_QUEX_QUEXPARSER__QUEX_TOKEN__

#ifndef __QUEX_OPTION_PLAIN_C
#   include<cstdio> 
#else
#   include<stdio.h> 
#endif

/* The token class definition file can only be included after 
 * the definition on TERMINATION and UNINITIALIZED.          
 * (fschaef 12y03m24d: "I do not rememember why I wrote this.")    */
#include "QuexParser-token.hpp"

const QUEX_TYPE_TOKEN_ID TokenAxiom              = ((QUEX_TYPE_TOKEN_ID)10004);
const QUEX_TYPE_TOKEN_ID TokenBecause            = ((QUEX_TYPE_TOKEN_ID)10005);
const QUEX_TYPE_TOKEN_ID TokenBlockBegin         = ((QUEX_TYPE_TOKEN_ID)10006);
const QUEX_TYPE_TOKEN_ID TokenBlockEnd           = ((QUEX_TYPE_TOKEN_ID)10007);
const QUEX_TYPE_TOKEN_ID TokenBracketClose       = ((QUEX_TYPE_TOKEN_ID)10008);
const QUEX_TYPE_TOKEN_ID TokenBracketOpen        = ((QUEX_TYPE_TOKEN_ID)10009);
const QUEX_TYPE_TOKEN_ID TokenBracketValue       = ((QUEX_TYPE_TOKEN_ID)10010);
const QUEX_TYPE_TOKEN_ID TokenCall               = ((QUEX_TYPE_TOKEN_ID)10011);
const QUEX_TYPE_TOKEN_ID TokenClosure            = ((QUEX_TYPE_TOKEN_ID)10012);
const QUEX_TYPE_TOKEN_ID TokenDEDENT             = ((QUEX_TYPE_TOKEN_ID)10000);
const QUEX_TYPE_TOKEN_ID TokenEndOfStream        = ((QUEX_TYPE_TOKEN_ID)10013);
const QUEX_TYPE_TOKEN_ID TokenFailure            = ((QUEX_TYPE_TOKEN_ID)10014);
const QUEX_TYPE_TOKEN_ID TokenINDENT             = ((QUEX_TYPE_TOKEN_ID)10001);
const QUEX_TYPE_TOKEN_ID TokenIdentifier         = ((QUEX_TYPE_TOKEN_ID)10015);
const QUEX_TYPE_TOKEN_ID TokenNODENT             = ((QUEX_TYPE_TOKEN_ID)10002);
const QUEX_TYPE_TOKEN_ID TokenNumber             = ((QUEX_TYPE_TOKEN_ID)10016);
const QUEX_TYPE_TOKEN_ID TokenProofs             = ((QUEX_TYPE_TOKEN_ID)10017);
const QUEX_TYPE_TOKEN_ID TokenQuotation          = ((QUEX_TYPE_TOKEN_ID)10018);
const QUEX_TYPE_TOKEN_ID TokenStatementSeparator = ((QUEX_TYPE_TOKEN_ID)10019);
const QUEX_TYPE_TOKEN_ID TokenTERMINATION        = ((QUEX_TYPE_TOKEN_ID)0);
const QUEX_TYPE_TOKEN_ID TokenTherefore          = ((QUEX_TYPE_TOKEN_ID)10020);
const QUEX_TYPE_TOKEN_ID TokenUNINITIALIZED      = ((QUEX_TYPE_TOKEN_ID)10003);


QUEX_NAMESPACE_TOKEN_OPEN
extern const char* QUEX_NAME_TOKEN(map_id_to_name)(const QUEX_TYPE_TOKEN_ID TokenID);
QUEX_NAMESPACE_TOKEN_CLOSE

#endif /* __QUEX_INCLUDE_GUARD__AUTO_TOKEN_IDS_QUEX_QUEXPARSER__QUEX_TOKEN__ */
