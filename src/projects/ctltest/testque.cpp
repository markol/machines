#include <string>
// #include <function.hpp>

#include "ctl/queue.hpp"
#include "ctl/list.hpp"
#include "ctl/deque.hpp"
#include "ctl/vector.hpp"

void	testQueue( void );
void	testPriorityQueue( void );

int main( void )
{
	testQueue();
	testPriorityQueue();

	return 0;
}

void	testQueue( void )
{
    //ctl_queue< ctl_deque< int > >  q;
    ctl_queue< int >  q;

    q.push( 2 );
    q.push( 3 );
    q.push( 1 );
    q.push( -50 );

    std::cout << q.front() << std::endl;
    q.pop();
    std::cout << q.front() << std::endl;
    q.pop();
    std::cout << q.front() << std::endl;
    q.pop();
    std::cout << q.front() << std::endl;
    q.pop();

}

void	testPriorityQueue( void )
{
// //    ctl_priority_queue< ctl_vector< int >, less< int > >  q;
//     priority_queue< vector< int >, less< int > >  q;
// //    ctl_priority_queue< vector< int >, less< int > >  q;
//
//     q.push( 2 );
//     q.push( 3 );
//     q.push( 1 );
//     q.push( -50 );
//
//     cout << q.top() << endl;
//     q.pop();
//     cout << q.top() << endl;
//     q.pop();
//     cout << q.top() << endl;
//     q.pop();
//     cout << q.top() << endl;
//     q.pop();
}
