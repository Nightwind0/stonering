#ifndef HEADER_QUAD_TREE_NODE
#define HEADER_QUAD_TREE_NODE

#include <set>
#include <list>
#include <cassert>
#include "Definitions.hxx"
#include "Visitor.hxx"
#include "Geometry.hxx"
#include <algorithm>

namespace Quadtree
{
    template <class T,unsigned int max_depth,class Scalar,int max_object_radius,bool delete_empty_nodes>
    class NodePool;

    template <class T,unsigned int max_depth=4, class Scalar=double,int max_object_radius=5000, bool delete_empty_nodes=true>
    class Node
    {
    public:
        //typedef TreeIterator<T> iterator;
        typedef Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes> OurNode;
        typedef OurNode * NodePtr;
        typedef Geometry::Vector<Scalar> Vector;
        typedef Geometry::Rect<Scalar> Rectangle;
        typedef Geometry::Square<Scalar> Square;
        typedef Geometry::Circle<Scalar> Circle;
        typedef Visitor<T,OurNode> OurVisitor;
        typedef NodeVisitor<OurNode> OurNodeVisitor;
        typedef NodePool<T,max_depth,Scalar,max_object_radius,delete_empty_nodes> OurNodePool;


        /**
         * @brief Creates a new node
         * @param pParent Vectorer to the parent of this node
         * @param max_depth The maximum depth this tree should build to
         * @param node_depth The depth this node will exist at
         * @param delete_empty_nodes Whether to automatically split
         */
        Node(NodePtr pParent,const Square &quad);


        virtual ~Node();

        /**
         * @brief Add an object to the appropriate node at or below
         * this node
         *
         * @param bounds The rectangle in which the object currently
         * exists
         * @param t the object
         * @return the node that holds the object
         */
        NodePtr Add(const Circle &bounds,const T &t);

        /**
         * @brief Remove the object that exists at or below this node
         *
         * @param bounds the boundary rectangle
         * @param t The instance
	 * 
         */
        void Remove(const Circle &bounds,const T &t);
        /**
         * @brief Change the position of an object already in this node
         *
         * @note must be directly called on the node that contains the object
         * @param t the object that is moving
         * @param newpos where the object is moving to
         */
        void MoveObject(const T &t, const Geometry::Circle<Scalar> & current_pos,const Geometry::Circle<Scalar> & newpos);

        /**
         * @brief The maximum size object this node will contain
         */
        Scalar GetMaxObjectRadius() const;

        /**
         * @brief Whether a Vector falls within the bounds of this quad
         * @note the parent node should know this without asking
         * based on which child we are.
         */
        bool InBounds(const Vector& Vector)const;

        /**
         * @brief Determine if a circle intersects the bounding circle
         *
         */
        bool Intersects(const Circle &circle) const;

        /**
         * @brief Calls the visitor on each object within the view circle
         * @return Whether to stop the traversal, as indicated by the visitor
         */
        bool Traverse(OurVisitor &visitor, const Circle &circle) const;
	
	/**
         * @brief Calls the visitor on each object within the view circle and which the predicate is true
	 *  
         * @return Whether to stop the traversal, as indicated by the visitor
         */
	template<class Predicate>
	bool Traverse(OurVisitor &visitor, const Circle &circle, Predicate pred ) const
	{
	    // First, my objects.
	    for (typename ObjectContainer::const_iterator it = m_objects.begin();
		 it != m_objects.end(); it++)
		 {
		     // Visit and determine if I should stop traversal
		     if(pred(*it)) {
			 if (!visitor.Visit(*it,this))
			     return false;
		     }
		 }
		 bool stop = false;
	    // Now, see if this intersects any of my children
		 if (m_pTopleft && m_pTopleft->Intersects(circle))
		     stop = m_pTopleft->Traverse(visitor,circle);
		 if (!stop && m_pTopright && m_pTopright->Intersects(circle))
		     stop = m_pTopright->Traverse(visitor,circle);
		 if (!stop && m_pBottomleft && m_pBottomleft->Intersects(circle))
		     stop = m_pBottomleft->Traverse(visitor,circle);
		 if (!stop && m_pBottomright && m_pBottomright->Intersects(circle))
		     stop = m_pBottomright->Traverse(visitor,circle);
		 
		 return stop;
	}
	

        /**
        * @brief Calls the visitor on all objects of this node and below
        */
        void TraverseAll(OurVisitor &visitor) const;

		/**
		 * @brief Calls the visitor on all nodes here and below
		 */
        void TraverseNodes(OurNodeVisitor &visitor) const;
		
		
        /**
        * @brief Create all child nodes under this node, even if they will be empty
        */
        void Split();
        /**
        * @brief Delete any nodes that are empty.
        * @note Does not return nodes to the pool (that would be pointless)
        * @note The "delete_empty_nodes" template parameter makes this useless if set true
        */
        void Prune();
        /**
        * @brief Release all data and reset this node
        */
        void Clear(NodePtr pParent, const Square &quad);
		
	Square GetSquare() const{return m_quad;}
	
	/**
         * @brief Get the depth of this node
         * 
         * @note For debugging purposes
         */
	int GetDepth() const { return m_depth; }
    protected:
        virtual OurNodePool * Get_Node_Pool()
        {
            // Default behavior is to pass it along!
            // Only the root node will have the pointer, and it overrides this method
            return m_pParent->Get_Node_Pool();
        }

    private:
        // friend class TreeIterator<T>;
        typedef typename std::set<T> ObjectContainer;
        typedef typename std::set<T>::iterator ObjectIterator;
        typedef typename std::set<T>::const_iterator ConstObjectIterator;

        enum eQuadrant
        {
            ETOPLEFT,
            ETOPRIGHT,
            EBOTTOMLEFT,
            EBOTTOMRIGHT
        };

        Scalar calculate_diagonal_radius()const;
        /// Recursively climbs tree to find out my depth (Root is depth 0)
        uint calculate_depth()const;
        /// Find my boundaries
        Circle calculate_bounds()const;

        bool empty()const;
	
	bool nochildren() const;
        /**
         * @brief Add an object to this node
         * @param p The Vector at which the object currently exists
         * @param t A Vectorer to the object
         */
        void add_specific(const Vector &p, const T &t);
        /// Remove by iterator from this node
        void remove_specific(const T &);
        /// Creates children nodes, and places its nodes within them
        /// Unless max_depth is met
        void split();
        /// create a specific child
        void create_child(NodePtr & ptr, eQuadrant quad);
        /// Returns a reference to the Vectorer to the appropriate child node
        NodePtr & which_child(eQuadrant);
        /// Find the right quadrant for this point
        eQuadrant which_quad(const Vector &center);

        void prune_child(NodePtr &pChild,OurNodePool *pPool);

        Square calculate_quad(eQuadrant quad);

        NodePtr m_pParent;
        NodePtr m_pTopleft;
        NodePtr m_pTopright;
        NodePtr m_pBottomleft;
        NodePtr m_pBottomright;
	int m_depth;
        ObjectContainer m_objects;
		Square m_quad;
        mutable bool m_bNoRemovals;
    };

    template <class T,unsigned int max_depth,class Scalar,int max_object_radius,bool delete_empty_nodes>
    class NodePool
    {
    public:
        typedef Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes> NodeType;
        NodePool(){}
        ~NodePool();

        virtual NodeType * GetNode(NodeType *pParent, const Geometry::Square<Scalar> &);
        virtual void Return(NodeType *pNode); // Required to accept NULL, as a no-op
        /// Delete any spare nodes
        virtual void Prune();
    private:
        std::list<NodeType*> m_pool;
    };

    template <class T,unsigned int max_depth,class Scalar,int max_object_radius,bool delete_empty_nodes>
    NodePool<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::~NodePool()
    {
        Prune();
    }


    template <class T,unsigned int max_depth,class Scalar,int max_object_radius,bool delete_empty_nodes>
    void NodePool<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Prune()
    {


        for (typename std::list< NodeType* >::iterator it = m_pool.begin();
                it != m_pool.end(); it++)
        {
            delete *it;
        }

        m_pool.clear();
    }

    template <class T,unsigned int max_depth,class Scalar,int max_object_radius,bool delete_empty_nodes>
    Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes> *
    NodePool<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::GetNode(NodeType *pParent, const Geometry::Square<Scalar> &quad)
    {
        if (m_pool.empty())
        {
            return new NodeType(pParent,quad);
        }
        else
        {
            NodeType * node = m_pool.front();
            m_pool.pop_front();
            node->Clear(pParent,quad);
            return node;
        }
    }

    template <class T,unsigned int max_depth,class Scalar,int max_object_radius,bool delete_empty_nodes>
    void NodePool<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Return(NodeType *pNode)
    {
        if (pNode != NULL)
        {
            m_pool.push_back(pNode);
        }
    }

    template <typename T>
    class AutoSetter
    {
        public:
        AutoSetter(T& variable, T valueOnDestruction):m_variable(variable),m_value(valueOnDestruction){
        }
        AutoSetter(T& variable, T valuenow, T valueOnDestruction):m_variable(variable),m_value(valueOnDestruction){
            variable = valuenow;
        }
        ~AutoSetter(){
            m_variable = m_value;
        }
        private:
        T& m_variable;
        T m_value;
    };

    template <class T,unsigned int max_depth=4, class Scalar=double, int max_object_radius=5000, bool delete_empty_nodes=true>
    class RootNode: public Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>
    {
    public:
        typedef Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes> OurNode;
        typedef NodePool<T,max_depth,Scalar,max_object_radius,delete_empty_nodes> OurNodePool;
	

        RootNode(Scalar x, Scalar y, Scalar size)
                :OurNode(NULL,Square(Vector(x,y),size))
        {
        }

        RootNode(const Geometry::Vector<Scalar> &v, Scalar size)
                :OurNode(NULL,Square(v,size))
        {
        }

        RootNode(const Geometry::Square<Scalar> &world):
                OurNode(NULL,world)
        {
        }

        ~RootNode()
        {
        }



    protected:
        virtual OurNodePool * Get_Node_Pool()
        {
            return &m_node_pool;
        }
    private:
        OurNodePool m_node_pool;
    };

    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Node(NodePtr pParent,
            const Geometry::Square<Scalar> &quad)
            :m_pParent(pParent),m_pTopleft(NULL),m_pTopright(NULL),
            m_pBottomleft(NULL),m_pBottomright(NULL),m_quad(quad),m_bNoRemovals(false)
    {
	m_depth = calculate_depth();
    }

    /**
     * @brief Creates a new node
     * @param pParent Vectorer to the parent of this node
     * @param max_depth The maximum depth this tree should build to
     * @param node_depth The depth this node will exist at
     * @param delete_empty_nodes Whether to automatically split
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::~Node()
    {
	if(m_pTopleft) delete m_pTopleft;
        if(m_pTopright) delete m_pTopright;
        if(m_pBottomleft) delete m_pBottomleft;
        if(m_pBottomright) delete m_pBottomright;
    }

    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    Scalar Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::GetMaxObjectRadius() const
    {
        Scalar radius = m_quad.GetDiagonalRadius();
        return radius + (radius * ((Scalar)max_object_radius/(Scalar)10000));
    }
    /**
     * @brief Add an object to the appropriate node at or below
     * this node
     *
     * @param bounds The rectangle in which the object currently
     * exists
     * @param t the object
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>*
    Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Add(const Circle &bounds,const T &t)
    {
        Scalar radius = bounds.GetRadius();
        uint cur_depth = m_depth;
        Scalar myradius = GetMaxObjectRadius();
        if (cur_depth == max_depth || radius > myradius / (Scalar) 4 )
        {
            // Too big for any children, add it to me
            if (bounds.GetRadius() <= myradius / (Scalar) 2 ){
                add_specific(bounds.GetCenter(),t);
                return this;
            }
            else{
                assert(0 && "Tried to add an object that was too big.");
                return NULL;
            }
        }
        else
        {
            eQuadrant equad = which_quad(bounds.GetCenter());
            NodePtr & ptr = which_child(equad);
            if (ptr == NULL)
            {
                create_child(ptr,equad);
            }
            assert(ptr != NULL);
            // Mmm... recursivey
            return ptr->Add(bounds,t);
        }

        assert(0);
        return NULL;
    }

    /**
     * @brief Remove the object that exists at or below this node
     *
     * @param center Where the object exists
     * @param it iterator to the object
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Remove(const Circle &bounds,const T &t)
    {
        assert(!m_bNoRemovals);
		this;
		
        Scalar radius = bounds.GetRadius();
        Scalar myradius = GetMaxObjectRadius();
		int depth = m_depth;
		
        if (depth == max_depth || radius > myradius  / (Scalar) 4 )
        {
            // Too big for my children, should be mine.
            remove_specific(t);
        }
        else
        {
            eQuadrant equad = which_quad(bounds.GetCenter());
            NodePtr & ptr = which_child(equad);
			
			assert(ptr != NULL);
			
            ptr->Remove(bounds,t);
			
            if (delete_empty_nodes && ptr->empty() && ptr->nochildren())
            {
                Get_Node_Pool()->Return(ptr);
                ptr = NULL;
            }
        }
    }
	
	
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::MoveObject( const T& object, const Circle& old_pos, const Circle& new_pos)
    {
		assert(!m_bNoRemovals);
		
        Scalar oldradius = old_pos.GetRadius();
		Scalar newradius = new_pos.GetRadius();
        Scalar myradius = GetMaxObjectRadius();
		int depth = m_depth;
		bool doIHaveItNow = false;
		bool shouldIHaveItLater = false;
		
		if (depth == max_depth || oldradius > myradius  / (Scalar) 4 )
        {
            // Too big for my children, should be mine.
            doIHaveItNow = true;
        }
        
        if(depth == max_depth || newradius > myradius  / (Scalar) 4 )
		{
			shouldIHaveItLater = true;
		}
		
		if(doIHaveItNow && shouldIHaveItLater) return; // then I'm done
		
		if(doIHaveItNow)
		{
			// I have it now but I shouldn't. 
			// this means it shrunk
			remove_specific(object);
			Add(new_pos,object);
			return;
		}else if(shouldIHaveItLater){
			// Well, I didn't have it before because it was too small
			// but now I should, so it grew
			Remove(old_pos,object);
			add_specific(new_pos.GetCenter(),object); // I'll take it
			return;
		}
		// Alright it didn't change size.
		
		eQuadrant equadold = which_quad(old_pos.GetCenter());
		eQuadrant equadnew = which_quad(new_pos.GetCenter());
		
		if(equadold == equadnew){
			NodePtr & ptr = which_child(equadold);
			assert(ptr != NULL);
			ptr->MoveObject( object, old_pos,new_pos );
			if (delete_empty_nodes && ptr->empty() && ptr->nochildren())
            {
                Get_Node_Pool()->Return(ptr);
                ptr = NULL;
            }
		}else{
			NodePtr & old = which_child(equadold);
			NodePtr & newc = which_child(equadnew);
			assert(old != NULL);
			old->Remove(old_pos,object);
			if (delete_empty_nodes && old->empty() && old->nochildren())
            {
                Get_Node_Pool()->Return(old);
                old = NULL;
            }
			if(newc == NULL)
			{
				create_child(newc,equadnew);
			}
			newc->Add(new_pos,object);
		}
    }
	
    /**
     * @brief Whether a Vector falls within the bounds of this quad
     * @note the parent node should know this without asking
     * based on which child we are.
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    bool Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::InBounds(const Vector &vector)const
    {
        Scalar halfsize = m_quad.GetSize() / (Scalar)2;

        if (abs(vector.GetX() - m_quad.GetCenter().GetX()) <= halfsize
                && abs(vector.GetY() - m_quad.GetCenter().GetY()) <= halfsize)
        {
            return true;
        }

        return false;
    }

    /**
     * @brief Determine if a circle intersects the bounding circle
     *
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    bool Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Intersects(const Circle &circle) const
    {
        Circle mybounds = calculate_bounds();
        return mybounds.Intersects(circle);
    }
    

    /**
     * @brief Calls the visitor on each object within the view circle
     *
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    bool Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Traverse(OurVisitor &visitor,
            const Circle &circle) const
    {
        AutoSetter<bool> setter(m_bNoRemovals,true,false);
        // First, my objects.
        for (typename ObjectContainer::const_iterator it = m_objects.begin();
                it != m_objects.end(); it++)
        {
            // Visit and determine if I should stop traversal
            if (!visitor.Visit(*it,this))
                return false;
        }
        bool stop = false;
        // Now, see if this intersects any of my children
        if (m_pTopleft && m_pTopleft->Intersects(circle))
            stop = m_pTopleft->Traverse(visitor,circle);
        if (!stop && m_pTopright && m_pTopright->Intersects(circle))
            stop = m_pTopright->Traverse(visitor,circle);
        if (!stop && m_pBottomleft && m_pBottomleft->Intersects(circle))
            stop = m_pBottomleft->Traverse(visitor,circle);
        if (!stop && m_pBottomright && m_pBottomright->Intersects(circle))
            stop = m_pBottomright->Traverse(visitor,circle);

        return stop;
    }

    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    bool Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::empty() const
    {
        return  m_objects.empty();
    }
    
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    bool Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::nochildren() const
    {
        return  !m_pBottomleft && !m_pBottomright && !m_pTopleft && !m_pTopright;
    }



    /**
     * @brief Calls the visitor on all objects of this node and below
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::TraverseAll(OurVisitor &visitor)const
    {
        AutoSetter<bool> setter(m_bNoRemovals,true,false);

        for (ConstObjectIterator iter = m_objects.begin();
                iter != m_objects.end(); iter++)
        {
            visitor.Visit(*iter,this);
        }

        if (m_pTopleft != NULL) m_pTopleft->TraverseAll(visitor);
        if (m_pTopright != NULL) m_pTopright->TraverseAll(visitor);
        if (m_pBottomleft != NULL) m_pBottomleft->TraverseAll(visitor);
        if (m_pBottomright != NULL) m_pBottomright->TraverseAll(visitor);
    }

	
    /**
     * @brief Calls the visitor on all nodes here and below
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::TraverseNodes(OurNodeVisitor &visitor)const
    {
        AutoSetter<bool> setter(m_bNoRemovals,true,false);
		
	visitor.Visit(this);
		
        if (m_pTopleft != NULL) m_pTopleft->TraverseNodes(visitor);
        if (m_pTopright != NULL) m_pTopright->TraverseNodes(visitor);
        if (m_pBottomleft != NULL) m_pBottomleft->TraverseNodes(visitor);
        if (m_pBottomright != NULL) m_pBottomright->TraverseNodes(visitor);
    }
	


    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    Scalar Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::calculate_diagonal_radius()const
    {
        const static double sqrt2 = std::sqrt(2.0);
        return (Scalar)((m_quad.GetSize() * sqrt2) / (Scalar)2);
    }

/// Recursively climbs tree to find out my depth
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    uint Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::calculate_depth()const
    {
        // this COULD be made into a for loop.. but it should be fine, since we won't be very deep
        if (m_pParent == NULL)
            return 0;
        else
            return 1 + m_pParent->calculate_depth();

    }

/// Find my boundaries
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    Geometry::Circle<Scalar> Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::calculate_bounds()const
    {
        return Circle(m_quad.GetCenter(),GetMaxObjectRadius());
    }

    /**
     * @brief Add an object to this node
     * @param p The Vector at which the object currently exists
     * @param t A Vectorer to the object
     */
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::add_specific(const Vector &p, const T &t)
    {
        m_objects.insert(t);
    }
    // Remove by value from this node
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::remove_specific(const T& t)
    {
        assert(!m_bNoRemovals);

        // more efficient than m_objects.remove
        m_objects.erase(t);
    }
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Split()
    {
        if (calculate_depth() < max_depth)
        {
            split();
            m_pTopleft->Split();
            m_pTopright->Split();
            m_pBottomleft->Split();
            m_pBottomright->Split();
        }
    }

    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Prune()
    {
        OurNodePool *pPool = Get_Node_Pool();
        prune_child(m_pTopleft,pPool);
        prune_child(m_pTopright,pPool);
        prune_child(m_pBottomleft,pPool);
        prune_child(m_pBottomright,pPool);

    }
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::prune_child(NodePtr &pChild, OurNodePool *pPool)
    {
        if (pChild != NULL)
        {
            pChild->Prune();
            if (pChild->empty())
            {
                pPool->Return(pChild);
                pChild = NULL;
            }
        }
    }


// Creates children nodes, and places its nodes within them
// Unless max_depth is met
    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::split()
    {
        create_child(m_pTopleft, ETOPLEFT);
        create_child(m_pTopright, ETOPRIGHT);
        create_child(m_pBottomleft, EBOTTOMLEFT);
        create_child(m_pBottomright, EBOTTOMRIGHT);
    }

    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes> * &
    Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::which_child(typename Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::eQuadrant quad)
    {
        switch (quad)
        {
        case ETOPLEFT:
            return m_pTopleft;
        case ETOPRIGHT:
            return m_pTopright;
        case EBOTTOMLEFT:
            return m_pBottomleft;
        case EBOTTOMRIGHT:
            return m_pBottomright;
        default:
            assert(0);
            return m_pTopleft; // arbitrary
        };
    }

    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    typename Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::eQuadrant
    Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::which_quad(const Vector &center)
    {
        bool left = true;
        bool top = true;

        if (center.GetX() > m_quad.GetCenter().GetX())
            left = false; // it's right of my center
        if (center.GetY() > m_quad.GetCenter().GetY())
            top = false; // it's below center

        if (top && left) return ETOPLEFT;
        else if (top && !left) return ETOPRIGHT;
        else if (!top && left) return EBOTTOMLEFT;
        else return EBOTTOMRIGHT;
    }


    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::create_child(NodePtr &ptr, typename OurNode::eQuadrant quad)
    {
        ptr = Get_Node_Pool()->GetNode(this,calculate_quad(quad));
    }

    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    void Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::Clear(NodePtr pParent,const Square &quad)
    {
        OurNodePool *pPool = Get_Node_Pool();
        //Note: there shouldn't be any children to worry about.. but
        if (m_pTopleft) pPool->Return(m_pTopleft);
        if (m_pTopright) pPool->Return(m_pTopright);
        if (m_pBottomleft) pPool->Return(m_pBottomleft);
        if (m_pBottomright) pPool->Return(m_pBottomright);

        m_pTopleft = m_pTopright = m_pBottomleft = m_pBottomright = NULL;

        m_pParent = pParent;
        m_quad = quad;
	m_depth = calculate_depth();

        m_objects.clear();
    }

    template <class T,unsigned int max_depth, class Scalar, int max_object_radius, bool delete_empty_nodes>
    Geometry::Square<Scalar> Node<T,max_depth,Scalar,max_object_radius,delete_empty_nodes>::calculate_quad(typename OurNode::eQuadrant quad)
    {
        Scalar halfsize = m_quad.GetSize() / (Scalar)2;
        Scalar quartersize = halfsize / (Scalar)2;
        switch (quad)
        {
        default:
            assert(0); //fallthrough, to quell compiler
        case ETOPLEFT:
            return Square(Vector(m_quad.GetCenter().GetX() - quartersize,
                                 m_quad.GetCenter().GetY() - quartersize), halfsize);
        case ETOPRIGHT:
            return Square(Vector(m_quad.GetCenter().GetX() + quartersize,
                                 m_quad.GetCenter().GetY() - quartersize), halfsize);
        case EBOTTOMLEFT:
            return Square(Vector(m_quad.GetCenter().GetX() - quartersize,
                                 m_quad.GetCenter().GetY() + quartersize),halfsize);
        case EBOTTOMRIGHT:
            return Square(Vector(m_quad.GetCenter().GetX() + quartersize,
                                 m_quad.GetCenter().GetY() + quartersize),halfsize);
        }
    }
    




}


#endif
