/* This file is generated by tdr. */
/* No manual modification is permitted. */

/* metalib version: 1 */
/* metalib md5sum: b0e26f31e35a0db9ed2c5e36b4942a71 */

/* creation time: Thu Dec 03 16:03:11 2015 */
/* tdr version: 2.5.1, build at 20120623 */

#ifndef _svr_proto_metalib_h_
#define _svr_proto_metalib_h_

namespace svr_proto
{


class MetaLib
{
    public:
        static int getVersion()
        {
            return 1;
        }

        static const char* getName()
        {
            return "svr_proto";
        }

        static const char* getMd5Sum()
        {
            return "b0e26f31e35a0db9ed2c5e36b4942a71";
        }

        static const char* getTdrVersion()
        {
            return "2.5.1, build at 20120623";
        }
};


}
#endif