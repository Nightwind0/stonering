
#ifndef QUADTREE_VISITOR_H
#define QUADTREE_VISITOR_H

namespace Quadtree
{
    template<class T, class Node>
    class Visitor
    {
    public:
		/**
		* @brief visit an object in the quadtree
		* @return Whether to continue traversal
		*/
        virtual bool Visit(T,const Node* node)=0;
    };
    
    template<class Node>
    class NodeVisitor
    {
    public:
        virtual bool Visit(const Node* node)=0;
    };
}

#endif

