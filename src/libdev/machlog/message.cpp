/* translation unit to hold persistent methods of canCommunicate */


#include "machlog/message.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachLogCanCommunicate );

void perWrite( PerOstream& , const MachLogCanCommunicate& )
{
}

void perRead( PerIstream& , MachLogCanCommunicate& )
{
}

