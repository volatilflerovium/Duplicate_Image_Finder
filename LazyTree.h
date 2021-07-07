/*********************************************************************
* LazyTree class                                                     *
*                                                                    *
* Template class wrapping std::vector to add fast lookup of elements *
* as a binary tree.                                                  *
*                                                                    *
* Date:    22-03-2021                                                *
* Author:  Dan Machado                                               *
**********************************************************************/
#include <vector>
#include <algorithm>
//#include <random>
//#include <iostream>

//#####################################################################

template<typename S>
class LazyTree
{
	public:
		/*
		 * Constructor
		 * */
		LazyTree()
		:m_manage(true), m_vectSize(0), m_vect(new std::vector<S>)
		{}

		/*
		 * Constructor
		 * 
		 * @param initialSize initial capacity for the underlying vector 
		 * 
		 * */
		LazyTree(int initialSize)
		:m_manage(true), m_vectSize(0), m_vect(new std::vector<S>)
		{
			m_vect->reserve(initialSize);
		}

		/*
		 * Construct
		 * 
		 * @param vect reference to an existing vector
		 * */
		LazyTree(std::vector<S>& vect)
		:LazyTree(&vect)
		{}

		/*
		 * Construct
		 * 
		 * @param pointer for an existing vector
		 * 
		 * */
		LazyTree(std::vector<S>* vect)
		:m_manage(false), m_vectSize(vect->size()), m_vect(vect){
			std::sort(m_vect->begin(), m_vect->end()); 
		}

		//Who would make a copy of it??
		LazyTree& operator=(const LazyTree&)=delete;

		virtual ~LazyTree(){
			if(m_manage){
				delete m_vect;
			}
		}

		/*
		 * Insert elements in the underlying vector using build in
		 * std::vector<S>::emplace_back
		 * 
		 * */
		template<typename... Args>
		void emplace_back(Args... args){
			m_vect->emplace_back(std::forward<Args...>(args...));
		}
	
		/*
		 * Insert elements in the underlying vector using build in
		 * std::vector<S>::push_back
		 * 
		 * */
		void push_back(S s){
			m_vect->push_back(std::forward<S>(s));
		}
		
		/*
		 * Sort elements
		 * */
		void sort(){
			std::sort(m_vect->begin(), m_vect->end()); 
			m_vectSize=m_vect->size();	
		}
		
		/*
		 * Find a particular element in the container.
		 * 
		 * return true if the element is found, false otherwise.
		 * */
		int find(const S& s);

		/*
		 * Return the size of the underlying container
		 * */
		int size() const{
			return m_vect->size();
		}

		/*
		 * Class to help to implement read-write functionality for 
		 * LazyTree::operator[]
		 * 
		 * */
		template<typename T>
		class Helper{
			public:
				Helper(LazyTree<T>& data, int index)
				:m_data(data), m_index(index)
				{}
				
				Helper<T>& operator=(T t){
					(*(m_data.m_vect))[m_index]=t;
					return *this;
				}

				operator T()const{
					return (*(m_data.m_vect))[m_index];
				}

			private:
				LazyTree<T>& m_data;
				int m_index;
		};

		const Helper<S> operator[](int index) const{
			return Helper<S>(const_cast< LazyTree<S>& >(*this), index);
		}

		Helper<S> operator[](int index){
			return Helper<S>(*this, index);
		}

	private:
		std::vector<S>* m_vect;
		size_t m_vectSize;
		bool m_manage;

	friend class Helper<S>;
};

template<typename S>
int LazyTree<S>::find(const S& s){
	int q=m_vect->size();
	if(q!=m_vectSize){
		sort();
	}
	
	int n=q/2;
	int t=n, p=-1;
	int r=0;
	while(q-p>1){			
		if(s<(*m_vect)[t]){
			q=t;
			n=-1;
		}
		else if((*m_vect)[t]<s){
			p=t;
			n=1;
		}
		else{
			return t;
		}			
		t+=n*std::max((q-p-1)/2, 1);
	}
	return -1;
}
