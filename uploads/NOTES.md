400 Bad Request = For Any Problem In The Request;
-----------------------------------------------------------------------
405 Method Not Allowed = Location Has Only GET And The Method Is POST;
-----------------------------------------------------------------------
414 Request-URI Too Long = line/target too big;
-----------------------------------------------------------------------
501 Not Implemented — unsupported method;
-----------------------------------------------------------------------
505 HTTP Version Not Supported — wrong HTTP version;
-----------------------------------------------------------------------

Need To Handle In The URI:
    + control chars (0x00–0x1F, 0x7F) are illegal -> 400
    + Fragment (“#”) must not be present in a Request-URI. unsafe -> 400
    + Reserved characters : ; / ? : @ = &. In HTTP, inside path and query, / ; ? are reserved
        %2F, %3F, %3B etc., decoding them is still allowed by RFC 1945 (server must decode);

Need To Handle In The Headers:
    + CRLF CRLF Directly After RL;
