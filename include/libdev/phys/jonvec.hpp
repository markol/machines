template< class T >
class JonVec
{
public:
	JonVec() : n_( 0 ){};
	size_t size() const {return n_;};
	void push_back( T* p ) {a_[n_++] = p;};
	T* operator[](size_t i) const {return a_[i];};
	T* back() const {return a_[n_-1];};
private:
	size_t n_;
	T* a_[10];
};