#include <string.hpp>
// #include <function.hpp>

#include "ctl/queue.hpp"
#include "ctl/list.hpp"
#include "ctl/deque.hpp"
#include "ctl/vector.hpp"

void	testQueue();
void	testPriorityQueue();

int main()
{
	testQueue();
	testPriorityQueue();

	return 0;
}

void	testQueue()
{
    ctl_queue< ctl_deque< int > >  q;
    
    q.push( 2 );
    q.push( 3 );
    q.push( 1 );
    q.push( -50 );
    
    cout << q.front() << endl;
    q.pop();
    cout << q.front() << endl;
    q.pop();
    cout << q.front() << endl;
    q.pop();
    cout << q.front() << endl;
    q.pop();
    
}

void	testPriorityQueue()
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
