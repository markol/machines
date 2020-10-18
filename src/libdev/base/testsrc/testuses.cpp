#include <iostream.h>

#include "base/base.hpp"
#include "base/uses.hpp"

#define	WHERE	cout << __LINE__ << "  " << __FILE__ << endl;

class TestUses
{
public:
	TestUses() { cout << "Constructing " << this << endl; }
	~TestUses() { cout << "Destructing " << this << endl; }
    bool operator ==( const TestUses& ) const { return true; };
private:
	TestUses( const TestUses& );
    const TestUses& operator =( const TestUses& );
    bool operator ==( const TestUses& ) const;
};


void	testUsesA( void )

{
	WHERE;
	UsesA< TestUses >	p1( new TestUses );
	WHERE;

	{
		WHERE;
		UsesA< TestUses >	p2( p1 );
		WHERE;
		UsesA< TestUses >	p3( new TestUses );
		WHERE;
		p1 = p3;
		WHERE;
	}
	WHERE;
}