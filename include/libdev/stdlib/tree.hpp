/*
 * T R E E . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _STD_BINTREE_HPP
#define _STD_BINTREE_HPP

/* ///////////////////////////////////////////////////////////////////

Red-black tree class, designed for use in implementing STL
associative containers (set, multiset, map, and multimap). The
insertion and deletion algorithms are based on those in Cormen,
Leiserson, and Rivest, Introduction to Algorithms (MIT Press, 1990),
except that

(1) the header cell is maintained with links not only to the root
but also to the leftmost node of the tree, to enable constant time
begin(), and to the rightmost node of the tree, to enable linear time
performance when used with the generic set algorithms (set_union,
etc.);

(2) when a node being deleted has two children its successor node is
relinked into its place, rather than copied, so that the only
iterators invalidated are those referring to the deleted node.

/////////////////////////////////////////////////////////////////// */

#ifndef rb_tree 
#define rb_tree rb_tree
#endif

//////////////////////////////////////////////////////////////////////

template	< 	class Key, 
				class Value,
				class KeyOfValue,	// map value to key
				class Compare		// key compare
			>
class rb_tree
{
protected:

    enum color_type { red, black };
    typedef Allocator< void >::pointer void_pointer;

    friend class rb_tree_node;

    struct rb_tree_node
    {
        color_type		color_field; 
        void_pointer	parent_link;
        void_pointer	left_link;
        void_pointer	right_link;
        Value			value_field;
    };

    static Allocator< rb_tree_node >	rb_tree_node_allocator;
    static Allocator< Value >			value_allocator;

public:

    typedef Key									key_type;
    typedef Value								value_type;
    typedef Allocator< Value >::pointer			pointer;
    typedef Allocator<Value>::reference			reference;
    typedef Allocator<Value>::const_reference	const_reference;
    typedef Allocator<rb_tree_node>				rb_tree_node_allocator_type;
    typedef Allocator<rb_tree_node>::pointer	link_type;

    typedef Allocator<rb_tree_node>::size_type			size_type;
    typedef Allocator<rb_tree_node>::difference_type	difference_type;

protected:

    size_type buffer_size();

    friend class rb_tree_node_buffer;
    struct rb_tree_node_buffer
    {
        void_pointer	next_buffer;
        link_type		buffer;
    };

public:

    typedef Allocator< rb_tree_node_buffer >			buffer_allocator_type;
    typedef Allocator< rb_tree_node_buffer >::pointer	buffer_pointer;

protected:

    static Allocator< rb_tree_node_buffer >	buffer_allocator;

    static buffer_pointer	buffer_list;
    static link_type 		free_list;
    static link_type 		next_avail;
    static link_type 		last;
    static size_type 		number_of_trees;

    void add_new_buffer();
    void deallocate_buffers();

    link_type get_node();

    void put_node( link_type p );

protected:

    link_type	header;  
    size_type	node_count;
    // keeps track of size of tree
    bool		insert_always;
    // iff( insert_always, add duplicate items to the tree );
    Compare		key_compare;


    link_type& root();
    const link_type& root() const;
    link_type& leftmost();
    const link_type& leftmost() const;

    link_type& rightmost();
    const link_type& rightmost() const;

    static link_type NIL;

    static link_type& left( link_type x );
    static link_type& right( link_type x );
    static link_type& parent( link_type x );
    static reference value( link_type x );

	static Allocator< Key >::const_reference
    key( link_type x )
	{
	    return KeyOfValue()( value( x ) );
	}

    static color_type& color( link_type x );
    static link_type minimum( link_type x );
    static link_type maximum( link_type x );

public:

	class iterator;
    friend class iterator;

	class const_iterator;
    friend class const_iterator;

    class iterator
    : public bidirectional_iterator< Value, difference_type >
    {
		friend class rb_tree< Key, Value, KeyOfValue, Compare >;
		friend class const_iterator;

		// friend bool operator ==( const iterator& x, const iterator& y )

    protected:

        link_type	node;

        iterator( link_type );

    public:

		iterator();
        bool operator ==( const iterator& ) const;

        reference operator *() const;

        iterator& operator ++( PREFIX );
        iterator  operator ++( POSTFIX );

        iterator& operator --( PREFIX );
        iterator operator  --( POSTFIX );
    };

    class const_iterator 
	: public bidirectional_iterator<Value,difference_type>
	{
    	friend class rb_tree< Key, Value, KeyOfValue, Compare >;
		friend class iterator;

    	// friend bool operator ==( const const_iterator&,
		//							const const_iterator& );

    protected:

        link_type	node;

        const_iterator( link_type x );

    public:

        const_iterator();

        const_iterator( const iterator& x );

        bool operator ==( const const_iterator& y ) const;
        bool operator !=( const const_iterator& y ) const;

        const_reference operator *() const;
        const_iterator& operator ++( PREFIX );
        const_iterator  operator ++( POSTFIX );

        const_iterator& operator --( PREFIX );
        const_iterator  operator --( POSTFIX );
    };

	///////////////////////////////

    typedef reverse_bidirectional_iterator<	iterator,
    										value_type,
    										reference,
											difference_type > reverse_iterator; 

    typedef reverse_bidirectional_iterator<	const_iterator,
    										value_type,
                                           	const_reference,
                                           	difference_type > const_reverse_iterator;

	///////////////////////////////

private:

    void		__erase( link_type x );
    link_type	__copy( link_type x, link_type p );
    iterator	__insert( link_type x, link_type y, const value_type& v );

    void init();

public:
    
	// allocation/deallocation
	rb_tree( const Compare& comp = Compare(), bool always = true ); 

    rb_tree( const value_type* first,
    			const value_type* last, 
            	const Compare& comp = Compare(),
            	bool always = true );

	typedef rb_tree< Key, Value, KeyOfValue, Compare > self;

    rb_tree( const self& x, bool always = true );

    ~rb_tree();

	self& operator =( const self& x);
    
	// accessors:

    Compare key_comp() const;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;

    bool empty() const;
    size_type size() const;
    size_type max_size() const;

    void swap( self& t );
    
	// insert/erase

    typedef pair< iterator, bool >	pair_iterator_bool; 
    // typedef done to get around compiler bug

    pair_iterator_bool insert( const value_type& x );
    iterator insert(iterator position, const value_type& x);

    void insert( iterator first, iterator last );
    void insert( const value_type *first, const value_type* last );
    void erase( iterator position );
    size_type erase( const key_type& x );
    void erase( iterator first, iterator last );
    void erase( const key_type *first, const key_type *last );

	// set operations:

    iterator find( const key_type& );
    const_iterator find( const key_type& ) const;
    size_type count( const key_type& ) const;
    iterator lower_bound( const key_type& );
    const_iterator lower_bound( const key_type& ) const;
    iterator upper_bound( const key_type& );
    const_iterator upper_bound( const key_type& ) const;

    typedef  pair<iterator, iterator> pair_iterator_iterator; 
    // typedef done to get around compiler bug

    pair_iterator_iterator equal_range(const key_type& x);

    typedef  pair< const_iterator, const_iterator > pair_citerator_citerator; 
    // typedef done to get around compiler bug

    pair_citerator_citerator equal_range( const key_type& x ) const;

	void rotate_left( link_type x );
	void rotate_right( link_type x );
};

//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "stdlib/tree.itf"
#endif

#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "stdlib/tree.ctp"
#endif

#ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
    #include "stdlib/tree.ctf"
#endif

//////////////////////////////////////////////////////////////////////

#endif	/*	#ifndef	_STD_BINTREE_HPP	*/


