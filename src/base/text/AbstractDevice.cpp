#include <base/public.h>
#include <base/text/impl.h>

namespace text {

///////////////////////////////////////////////////////////////////////////////

AbstractDevice::AbstractDevice( uint32 cols_, uint32 rows_, uint32 indent_ )
    : cols   ( cols_ )
    , rows   ( rows_ )
    , indent ( indent_ )
{
}
 
///////////////////////////////////////////////////////////////////////////////

AbstractDevice::~AbstractDevice()
{
}

///////////////////////////////////////////////////////////////////////////////

list<string>::size_type
AbstractDevice::format( const Buffer& buffer, list<string>& lines )
{
    lines.clear();
    doFormat( buffer, lines );
    return lines.size();
}

///////////////////////////////////////////////////////////////////////////////

} // namespace text
