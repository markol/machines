/*
 * T R E E . C T P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

// #ifndef _INLINE
//     #include "stdlib/tree.itp"
// #endif

//////////////////////////////////////////////////////////////////////

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::buffer_pointer 
rb_tree<K, V, KVtoK, C>::buffer_list = 0;

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type
rb_tree<K, V, KVtoK, C>::free_list = 0;

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type 
rb_tree<K, V, KVtoK, C>::next_avail = 0;

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::link_type 
rb_tree< K, V, KVtoK, C >::last = 0;

template <class K, class V, class KVtoK, class C>
rb_tree< K, V, KVtoK, C >::size_type 
rb_tree< K, V, KVtoK, C >::number_of_trees = 0;

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::rb_tree_node_allocator_type 
rb_tree<K, V, KVtoK, C>::rb_tree_node_allocator;

template < class K, class V, class KVtoK, class C >
Allocator< V >
rb_tree< K, V, KVtoK, C >::value_allocator;

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::buffer_allocator_type 
rb_tree<K, V, KVtoK, C>::buffer_allocator;

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::link_type 
rb_tree< K, V, KVtoK, C >::NIL = 0;

//////////////////////////////////////////////////////////////////////

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::size_type
rb_tree<K, V, KVtoK, C>::buffer_size()
{
    return rb_tree_node_allocator.init_page_size();
}

template < class K, class V, class KVtoK, class C >
void
rb_tree<K, V, KVtoK, C>::add_new_buffer()
{
	buffer_pointer tmp = buffer_allocator.allocate( (size_type)1 );
    tmp->buffer = rb_tree_node_allocator.allocate( buffer_size() );
    tmp->next_buffer = buffer_list;
    buffer_list = tmp;
    next_avail = buffer_list->buffer;
    last = next_avail + buffer_size();
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type
rb_tree<K, V, KVtoK, C>::get_node()
{
    link_type tmp = free_list;
    return free_list ? 
        (free_list = (link_type)(free_list->right_link), tmp) 
            : (next_avail == last ? (add_new_buffer(), next_avail++) 
               : next_avail++);
    // ugly code for inlining - avoids multiple returns
}

template < class K, class V, class KVtoK, class C >
void
rb_tree<K, V, KVtoK, C>::put_node( link_type p )
{
    p->right_link = free_list;
    free_list = p;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type&
rb_tree<K, V, KVtoK, C>::root()
{
	return parent( header );
}

template < class K, class V, class KVtoK, class C >
const rb_tree<K, V, KVtoK, C>::link_type&
rb_tree<K, V, KVtoK, C>::root() const
{
	return parent( header );
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type&
rb_tree<K, V, KVtoK, C>::leftmost()
{
	return left( header );
}

template < class K, class V, class KVtoK, class C >
const rb_tree<K, V, KVtoK, C>::link_type&
rb_tree<K, V, KVtoK, C>::leftmost() const
{
	return left( header );
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type&
rb_tree<K, V, KVtoK, C>::rightmost()
{
	return right( header );
}

template < class K, class V, class KVtoK, class C >
const rb_tree<K, V, KVtoK, C>::link_type& 
rb_tree<K, V, KVtoK, C>::rightmost() const
{
	return right( header );
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type&
rb_tree<K, V, KVtoK, C>::left( rb_tree<K, V, KVtoK, C>::link_type x )
{ 
    return (link_type&)( (*x).left_link );
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type&
rb_tree<K, V, KVtoK, C>::right( rb_tree<K, V, KVtoK, C>::link_type x )
{
    return (link_type&)( (*x).right_link ); 
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type&
rb_tree<K, V, KVtoK, C>::parent( rb_tree<K, V, KVtoK, C>::link_type x )
{
    return (link_type&)( (*x).parent_link );
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::reference
rb_tree<K, V, KVtoK, C>::value( rb_tree<K, V, KVtoK, C>::link_type x )
{
	return (*x).value_field;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::color_type&
rb_tree<K, V, KVtoK, C>::color( rb_tree<K, V, KVtoK, C>::link_type x )
{ 
    return (color_type&)(*x).color_field;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type
rb_tree<K, V, KVtoK, C>::minimum( rb_tree<K, V, KVtoK, C>::link_type x )
{
    while (left(x) != NIL)
        x = left(x);
    return x;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::link_type
rb_tree<K, V, KVtoK, C>::maximum( rb_tree<K, V, KVtoK, C>::link_type x )
{
    while( right(x) != NIL )
        x = right( x );

    return x;
}

//////////////////////////////////////////////////////////////////////

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::iterator::iterator( rb_tree<K, V, KVtoK, C>::link_type x )
: node(x)
{
	/* Intentionally Empty	*/
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::iterator::iterator()
{
	/* Intentionally Empty	*/		
}

template < class K, class V, class KVtoK, class C >
bool
rb_tree<K, V, KVtoK, C>::iterator::operator ==( 
	const rb_tree<K, V, KVtoK, C>::iterator& y ) const
{
	return node == y.node;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::reference
rb_tree<K, V, KVtoK, C>::iterator::operator *() const
{
	return value( node );
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::iterator&
rb_tree<K, V, KVtoK, C>::iterator::operator ++( PREFIX )
{
    if( right( node ) != NIL )
    {
        node = right( node );
        while( left( node ) != NIL )
            node = left( node );
    }
    else
    {
        link_type y = parent( node );
        while( node == right( y ) )
        {
			node = y;
            y = parent( y );
        }

		// necessary because of rightmost 
        if( right( node ) != y ) 
            node = y;
    }
    return *this;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::iterator 
rb_tree<K, V, KVtoK, C>::iterator::operator ++( POSTFIX )
{
    iterator tmp = *this;
    ++*this;
    return tmp;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::iterator&
rb_tree<K, V, KVtoK, C>::iterator::operator --( PREFIX )
{
	// check for header
    if( color( node ) == red 
    		and parent( parent( node ) ) == node )  
        node = right( node );
        // return rightmost
    else if( left( node ) != NIL )
    {
        link_type y = left( node );
        while( right( y ) != NIL )
            y = right( y );

        node = y;
    }
    else
    {
		link_type y = parent( node );
        while( node == left( y ) )
        {
            node = y;
            y = parent( y );
        }
        node = y;
    }
    return *this;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::iterator
rb_tree<K, V, KVtoK, C>::iterator::operator --( POSTFIX )
{
    iterator tmp = *this;
    --*this;
    return tmp;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::const_iterator::const_iterator( 
	rb_tree<K, V, KVtoK, C>::link_type x )
: node( x )
{
	/*	Intentionally Empty	*/
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::const_iterator::const_iterator()
{
	/*	Intentionally Empty	*/
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::const_iterator::const_iterator( 
	const rb_tree<K, V, KVtoK, C>::iterator& x )
: node( x.node )
{
	/*	Intentionally Empty	*/        
}

template < class K, class V, class KVtoK, class C >
bool 
rb_tree<K, V, KVtoK, C>::const_iterator::operator ==( 
	const rb_tree<K, V, KVtoK, C>::const_iterator& y ) const
{ 
	return node == y.node; 
}

template < class K, class V, class KVtoK, class C >
bool
rb_tree<K, V, KVtoK, C>::const_iterator::operator !=( 
	const rb_tree<K, V, KVtoK, C>::const_iterator& y ) const
{ 
	return node != y.node; 
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::const_reference 
rb_tree<K, V, KVtoK, C>::const_iterator::operator *() const
{
	return value( node );
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::const_iterator& 
rb_tree<K, V, KVtoK, C>::const_iterator::operator ++( PREFIX )
{
    if( right( node ) != NIL )
    {
        node = right( node );
        while( left( node ) != NIL )
            node = left( node );
    }
    else
    {
        link_type y = parent( node );
        while( node == right( y ) )
        {
            node = y;
            y = parent( y );
        }
		 // necessary because of rightmost 
        if( right( node ) != y )
            node = y;
    }

    return *this;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::const_iterator 
rb_tree<K, V, KVtoK, C>::const_iterator::operator ++( POSTFIX )
{
    const_iterator tmp = *this;
    ++*this;
    return tmp;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::const_iterator& 
rb_tree<K, V, KVtoK, C>::const_iterator::operator --( PREFIX )
{
	// check for header
	if( color( node ) == red and
			parent( parent( node ) ) == node )  
        node = right(node);
        // return rightmost
    else if( left( node ) != NIL )
    {
		link_type y = left( node );
        while( right( y ) != NIL )
            y = right( y );

        node = y;
    }
    else
    {
        link_type y = parent( node );
        while( node == left( y ) )
        {
            node = y;
            y = parent( y );
        }
        node = y;
    }

    return *this;
}

template < class K, class V, class KVtoK, class C >
rb_tree<K, V, KVtoK, C>::const_iterator
rb_tree<K, V, KVtoK, C>::const_iterator::operator --( POSTFIX )
{
	const_iterator tmp = *this;
    --*this;
    return tmp;
}

template < class K, class V, class KVtoK, class C >
void 
rb_tree< K, V, KVtoK, C >::init()
{
    ++number_of_trees;
    if( NIL == 0 )
    {
		NIL = get_node();
        color(NIL) = black;
        parent(NIL) = 0;
        left(NIL) = 0;
        right(NIL) = 0;
    }
    header = get_node();
    color( header ) = red;
    // used to distinguish header from root,
	// in iterator.operator++

    root() = NIL;
    leftmost() = header;
    rightmost() = header;
}

//////////////////////////////////////////////////////////////////////

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::rb_tree( const C& comp,	bool always ) 
: node_count( 0 ),
  key_compare( comp ),
  insert_always( always )
{ 
	init();
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::rb_tree(
			const value_type* first,
			const value_type *last, 
        	const C& comp,
        	bool always )
: node_count( 0 ),
  key_compare( comp ),
  insert_always( always )
{ 
    init();
    insert( first, last );
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::rb_tree( const rb_tree< K, V, KVtoK, C >& x, 
	bool always )
: node_count( x.node_count ), 
  key_compare(x.key_compare),
  insert_always( always )
{ 
    ++number_of_trees;
    header = get_node();
    color( header ) = red;
    root() = __copy( x.root(), header );

    if( root() == NIL )
    {
        leftmost() = header;
        rightmost() = header;
    }
    else
    {
    	leftmost() = minimum( root() );
        rightmost() = maximum( root() );
    }
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::~rb_tree()
{
	erase( begin(), end() );
    put_node( header );

    if( --number_of_trees == 0 )
    {
        put_node( NIL );
        NIL = 0;
        deallocate_buffers();
        free_list = 0;    
        next_avail = 0;
        last = 0;
    }
}

template < class K, class V, class KVtoK, class C >
C 
rb_tree< K, V, KVtoK, C >::key_comp() const
{
	return key_compare;
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::iterator 
rb_tree< K, V, KVtoK, C >::begin()
{
	return leftmost();
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::const_iterator 
rb_tree< K, V, KVtoK, C >::begin() const
{
	return leftmost();
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::iterator 
rb_tree< K, V, KVtoK, C >::end()
{
	return header;
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::const_iterator 
rb_tree< K, V, KVtoK, C >::end() const
{
	return header;
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::reverse_iterator 
rb_tree< K, V, KVtoK, C >::rbegin()
{
	return reverse_iterator( end() );
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::const_reverse_iterator 
rb_tree< K, V, KVtoK, C >::rbegin() const
{ 
    return const_reverse_iterator( end() ); 
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::reverse_iterator 
rb_tree< K, V, KVtoK, C >::rend()
{
	return reverse_iterator( begin() );
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::const_reverse_iterator 
rb_tree< K, V, KVtoK, C >::rend() const
{ 
    return const_reverse_iterator( begin() );
} 

template < class K, class V, class KVtoK, class C >
bool 
rb_tree< K, V, KVtoK, C >::empty() const
{
	return node_count == 0; 
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::size_type 
rb_tree< K, V, KVtoK, C >::size() const
{
	return node_count;
}

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::size_type 
rb_tree< K, V, KVtoK, C >::max_size() const
{ 
    return rb_tree_node_allocator.max_size(); 
}

template < class K, class V, class KVtoK, class C >
void 
rb_tree< K, V, KVtoK, C >::swap( rb_tree< K, V, KVtoK, C >& t )
{
    ::swap( header, t.header );
    ::swap( node_count, t.node_count );
    ::swap( insert_always, t.insert_always );
    ::swap( key_compare, t.key_compare );
}
    
//////////////////////////////////////////////////////////////////////

template < class K, class V, class KVtoK, class C >
void 
rb_tree<K, V, KVtoK, C>::deallocate_buffers()
{
	while( buffer_list )
	{
        buffer_pointer tmp = buffer_list;
        buffer_list = (buffer_pointer)( buffer_list->next_buffer );
        rb_tree_node_allocator.deallocate( tmp->buffer );
        buffer_allocator.deallocate( tmp );
    }
}

//////////////////////////////////////////////////////////////////////

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>& 
rb_tree<K, V, KVtoK, C>::operator=(const rb_tree<K, V, KVtoK, C>& x) 
{
    if (this != &x)
    {
        // can't be done as in list because K may be a constant type
        erase( begin(), end() );

        root() = __copy(x.root(), header);

        if( root() == NIL ) 
        {
            leftmost() = header;
            rightmost() = header;
        }
        else 
        {
	    	leftmost() = minimum( root() );
            rightmost() = maximum( root() );
        }
        node_count = x.node_count;
    }

    return *this;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::iterator
rb_tree<K, V, KVtoK, C>::__insert( link_type x, link_type y, const V& v )
{
    ++node_count;
    link_type z = get_node();
    construct( value_allocator.address( value( z ) ), v );

    if( y == header
    	|| x != NIL
    	|| key_compare( KVtoK()( v ), key( y ) ) )
    {
        left( y ) = z;
         // also makes leftmost() = z when y == header

        if (y == header)
        {
            root() = z;
            rightmost() = z;
        }
        else if( y == leftmost() )
            leftmost() = z;
            // maintain leftmost() pointing to minimum node
    }
    else
    {
		right( y ) = z;
        if( y == rightmost() )
            rightmost() = z;
            // maintain rightmost() pointing to maximum node
    }

    parent( z ) = y;
    left( z ) = NIL;
    right( z ) = NIL;
    x = z;  // recolor and rebalance the tree
    color( x ) = red;
    while( x != root() && color(parent(x)) == red ) 
        if (parent(x) == left(parent(parent(x)))) {
            y = right(parent(parent(x)));
            if (color(y) == red) {
                color(parent(x)) = black;
                color(y) = black;
                color(parent(parent(x))) = red;
                x = parent(parent(x));
            } else {
                if (x == right(parent(x))) {
                    x = parent(x);
                    rotate_left(x);
                }
                color(parent(x)) = black;
                color(parent(parent(x))) = red;
                rotate_right(parent(parent(x)));
            }
        } else {
            y = left(parent(parent(x)));
            if (color(y) == red) {
                color(parent(x)) = black;
                color(y) = black;
                color(parent(parent(x))) = red;
                x = parent(parent(x));
            } else {
                if (x == left(parent(x))) {
                    x = parent(x);
                    rotate_right(x);
                }
                color(parent(x)) = black;
                color(parent(parent(x))) = red;
                rotate_left(parent(parent(x)));
            }
        }
    color(root()) = black;
    return iterator(z);
}

//////////////////////////////////////////////////////////////////////

template < class K, class V, class KVtoK, class C >
rb_tree< K, V, KVtoK, C >::pair_iterator_bool
rb_tree< K, V, KVtoK, C >::insert( const V& v )
{
    link_type y = header;
    link_type x = root();
    bool comp = true;
    while( x != NIL )
    {
        y = x;
        comp = key_compare( KVtoK()( v ), key( x ) );
        x = comp ? left( x ) : right( x );
    }

    if( insert_always )
		return pair_iterator_bool( __insert( x, y, v ), true );

    iterator j = iterator( y );
    if( comp )
	{
        if( j == begin() )
			return pair_iterator_bool( __insert( x, y, v ), true );
        else
            --j;
	}

    if( key_compare( key( j.node ), KVtoK()( v ) ) )
		return pair_iterator_bool( __insert( x, y, v ), true );

    return pair_iterator_bool( j, false );
}

//////////////////////////////////////////////////////////////////////

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::iterator 
rb_tree<K, V, KVtoK, C>::insert(iterator position,
                                                 const V& v) {
    if (position == iterator(begin()))
        if (size() > 0 && key_compare(KVtoK()(v), key(position.node)))
            return __insert(position.node, position.node, v);
            // first argument just needs to be non-NIL 
        else
            return insert(v).first;
    else if (position == iterator(end()))
        if (key_compare(key(rightmost()), KVtoK()(v)))
            return __insert(NIL, rightmost(), v);
        else
            return insert(v).first;
    else {
        iterator before = --position;
        if (key_compare(key(before.node), KVtoK()(v))
            && key_compare(KVtoK()(v), key(position.node)))
            if (right(before.node) == NIL)
                return __insert(NIL, before.node, v); 
            else
                return __insert(position.node, position.node, v);
                // first argument just needs to be non-NIL 
        else
            return insert(v).first;
    }
}

template <class K, class V, class KVtoK, class C>
void rb_tree<K, V, KVtoK, C>::insert(iterator first, 
                                                      iterator last) {
    while (first != last) insert(*first++);
}

template <class K, class V, class KVtoK, class C>
void rb_tree<K, V, KVtoK, C>::insert(const V* first, 
                                                      const V* last) {
    while (first != last) insert(*first++);
}
         
template <class K, class V, class KVtoK, class C>
void rb_tree<K, V, KVtoK, C>::erase(iterator position) {
    link_type z = position.node;
    link_type y = z;
    link_type x;
    if (left(y) == NIL)
        x = right(y);
    else
        if (right(y) == NIL) 
            x = left(y);
        else {
            y = right(y);
            while (left(y) != NIL)
                y = left(y);
            x = right(y);
        }
    if (y != z) { // relink y in place of z
        parent(left(z)) = y; 
        left(y) = left(z);
        if (y != right(z)) {
            parent(x) = parent(y); // possibly x == NIL
            left(parent(y)) = x;   // y must be a left child
            right(y) = right(z);
            parent(right(z)) = y;
        } else
            parent(x) = y;  // needed in case x == NIL
        if (root() == z)
            root() = y;
        else if (left(parent(z)) == z)
            left(parent(z)) = y;
        else 
            right(parent(z)) = y;
        parent(y) = parent(z);
        ::swap(color(y), color(z));
        ::swap(y, z);  
                       // y points to node to be actually deleted,
                       // z points to old z's former successor
    } else {  // y == z
        parent(x) = parent(y);   // possibly x == NIL
        if (root() == z)
            root() = x;
        else 
            if (left(parent(z)) == z)
                left(parent(z)) = x;
            else
                right(parent(z)) = x;
        if (leftmost() == z) 
            if (right(z) == NIL)  // left(z) must be NIL also
                leftmost() = parent(z);
                // makes leftmost() == header if z == root()
        else
            leftmost() = minimum(x);
        if (rightmost() == z)  
            if (left(z) == NIL) // right(z) must be NIL also
                rightmost() = parent(z);  
                // makes rightmost() == header if z == root()
        else  // x == left(z)
            rightmost() = maximum(x);
    }
    if (color(y) != red) { 
        while (x != root() && color(x) == black)
            if (x == left(parent(x))) {
                link_type w = right(parent(x));
                if (color(w) == red) {
                    color(w) = black;
                    color(parent(x)) = red;
                    rotate_left(parent(x));
                    w = right(parent(x));
                }
                if (color(left(w)) == black && color(right(w)) == black) {
                    color(w) = red;
                    x = parent(x);
                } else {
                    if (color(right(w)) == black) {
                        color(left(w)) = black;
                        color(w) = red;
                        rotate_right(w);
                        w = right(parent(x));
                    }
                    color(w) = color(parent(x));
                    color(parent(x)) = black;
                    color(right(w)) = black;
                    rotate_left(parent(x));
                    break;
                }
            } else {  // same as then clause with "right" and "left" exchanged
                link_type w = left(parent(x));
                if (color(w) == red) {
                    color(w) = black;
                    color(parent(x)) = red;
                    rotate_right(parent(x));
                    w = left(parent(x));
                }
                if (color(right(w)) == black && color(left(w)) == black) {
                    color(w) = red;
                    x = parent(x);
                } else {
                    if (color(left(w)) == black) {
                        color(right(w)) = black;
                        color(w) = red;
                        rotate_left(w);
                        w = left(parent(x));
                    }
                    color(w) = color(parent(x));
                    color(parent(x)) = black;
                    color(left(w)) = black;
                    rotate_right(parent(x));
                    break;
                }
            }
        color(x) = black;
    }
    destroy( value_allocator.address( value(y)));
    put_node(y);
    --node_count;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::size_type 
rb_tree<K, V, KVtoK, C>::erase(const K& x) {
    pair_iterator_iterator p = equal_range(x);
    size_type n = 0;
    distance(p.first, p.second, n);
    erase(p.first, p.second);
    return n;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::link_type 
rb_tree<K, V, KVtoK, C>::__copy(link_type x, link_type p) {
   // structural copy
   link_type r = x;
   while (x != NIL) {
      link_type y = get_node();
      if (r == x) r = y;  // save for return value
      construct( value_allocator.address( value(y)), value(x));
      left(p) = y;
      parent(y) = p;
      color(y) = color(x);
      right(y) = __copy(right(x), y);
      p = y;
      x = left(x);
   }
   left(p) = NIL;
   return r;
}

template <class K, class V, class KVtoK, class C>
void rb_tree<K, V, KVtoK, C>::__erase(link_type x) {
    // erase without rebalancing
    while (x != NIL) {
       __erase(right(x));
       link_type y = left(x);
       destroy( value_allocator.address( value( x ) ) );
       put_node(x);
       x = y;
    }
}

template <class K, class V, class KVtoK, class C>
void rb_tree<K, V, KVtoK, C>::erase(iterator first, 
                                                     iterator last) {
    if (first == begin() && last == end() && node_count != 0) {
        __erase(root());
        leftmost() = header;
        root() = NIL;
        rightmost() = header;
        node_count = 0;
    } else
        while (first != last) erase(first++);
}

template <class K, class V, class KVtoK, class C>
void rb_tree<K, V, KVtoK, C>::erase(const K* first, 
					const K* last )
{
    while (first != last) erase(*first++);
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::iterator 
rb_tree<K, V, KVtoK, C>::find(const K& k) {
    link_type y = header;
    link_type x = root();
    bool comp = false;
    while (x != NIL) {
        y = x;
        comp = key_compare(key(x), k);
        x = comp ? right(x) : left(x);
    }
    iterator j = iterator(y);   
    if (comp) ++j;
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::const_iterator 
rb_tree<K, V, KVtoK, C>::find(const K& k) const {
    link_type y = header;
    link_type x = root();
    bool comp = false;
    while (x != NIL) {
        y = x;
        comp = key_compare(key(x), k);
        x = comp ? right(x) : left(x);
    }
    const_iterator j = const_iterator(y);   
    if (comp) ++j;
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::size_type 
rb_tree<K, V, KVtoK, C>::count(const K& k) const {
    pair<const_iterator, const_iterator> p = equal_range(k);
    size_type n = 0;
    distance(p.first, p.second, n);
    return n;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::iterator 
rb_tree<K, V, KVtoK, C>::lower_bound(const K& k) {
    link_type y = header;
    link_type x = root();
    bool comp = false;
    while (x != NIL) {
        y = x;
        comp = key_compare(key(x), k);
        x = comp ? right(x) : left(x);
    }
    iterator j = iterator(y);   
    return comp ? ++j : j;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::const_iterator 
rb_tree<K, V, KVtoK, C>::lower_bound(const K& k) const {
    link_type y = header;
    link_type x = root();
    bool comp = false;
    while (x != NIL) {
        y = x;
        comp = key_compare(key(x), k);
        x = comp ? right(x) : left(x);
    }
    const_iterator j = const_iterator(y);   
    return comp ? ++j : j;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::iterator 
rb_tree<K, V, KVtoK, C>::upper_bound(const K& k) {
    link_type y = header;
    link_type x = root();
    bool comp = true;
    while (x != NIL) {
        y = x;
        comp = key_compare(k, key(x));
        x = comp ? left(x) : right(x);
    }
    iterator j = iterator(y);   
    return comp ? j : ++j;
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::const_iterator 
rb_tree<K, V, KVtoK, C>::upper_bound(const K& k) const {
    link_type y = header;
    link_type x = root();
    bool comp = true;
    while (x != NIL) {
        y = x;
        comp = key_compare(k, key(x));
        x = comp ? left(x) : right(x);
    }
    const_iterator j = const_iterator(y);   
    return comp ? j : ++j;
}


template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::pair_iterator_iterator 
rb_tree<K, V, KVtoK, C>::equal_range(const K& k) {
    return pair_iterator_iterator(lower_bound(k), upper_bound(k));
}

template <class K, class V, class KVtoK, class C>
rb_tree<K, V, KVtoK, C>::pair_citerator_citerator 
rb_tree<K, V, KVtoK, C>::equal_range(const K& k) const {
    return pair_citerator_citerator(lower_bound(k), upper_bound(k));
}

/* End TREE.CTP *****************************************************/


