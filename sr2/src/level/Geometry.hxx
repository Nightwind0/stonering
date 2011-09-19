#ifndef QUADTREE_GEOMETRY_H
#define QUADTREE_GEOMETRY_H

#include "Definitions.hxx"
#include <cmath>

namespace Quadtree
{

	namespace Geometry{

		template <class Scalar>
		class Vector
		{
		public:
			Vector(Scalar x, Scalar y)
				:m_x(x),m_y(y)
			{
			}

			~Vector()
			{
			}

			Scalar GetX() const { return m_x; }
			Scalar GetY() const { return m_y; }
			Scalar LengthSquared() {
			    return (m_x * m_x + m_y*m_y);
			}
			Vector<Scalar> operator-(const Vector<Scalar>& rhs){
			    return Vector<Scalar>(m_x-rhs.m_x,m_y-rhs.m_y);
			}
			Vector<Scalar> operator+(const Vector<Scalar>& rhs){
			    return Vector<Scalar>(m_x+rhs.m_x,m_y+rhs.m_y);
			}
			bool operator==(const Vector<Scalar>& rhs) const{
			    return m_x == rhs.m_x && m_y == rhs.m_y;
			}
		private:
			Scalar m_x;
			Scalar m_y;
		};

		template <class Scalar>
		class Square
		{
		public:
			Square(const Vector<Scalar> &center, Scalar size)
				:m_center(center),m_size(size)
			{
			}

			~Square(){}

			Vector<Scalar> GetCenter()const { return m_center;}
			Scalar GetSize()const { return m_size;}
			Scalar GetDiagonalRadius()const
			{
				const static double sqrt2 = std::sqrt(2.0);
				return (Scalar)((m_size * sqrt2) / (Scalar)2);
			}
		private:
			Vector<Scalar> m_center;
			Scalar m_size;
		};

		template <class Scalar>
		class Rect
		{
		public:
			Rect(const Vector<Scalar> &center, Scalar width, Scalar height)
				:m_center(center),m_width(width),m_height(height)
			{
			}

			Rect(const Square<Scalar>& square)
			{
				m_center = square.GetCenter();
				m_width = square.GetSize();
				m_height = square.GetSize();
			}
			~Rect(){}

			Vector<Scalar> GetCenter()const{ return m_center; }
			Scalar GetWidth() const { return m_width;}
			Scalar GetHeight() const { return m_height; }
			Scalar GetDiagonalRadius() const
			{
				return std::sqrt(m_width * m_width + m_height * m_height);
			}
		private:
			Vector<Scalar> m_center;
			Scalar m_width;
			Scalar m_height;
		};

		template <class Scalar>
		class Circle
		{
		public:
                        Circle(const Square<Scalar>& squareToCircumscribe):
                            m_center(squareToCircumscribe.GetCenter()),
                            m_radius(squareToCircumscribe.GetDiagonalRadius())
                        {
                        }
                        Circle(const Rect<Scalar>& rectToCircumscribe):
                            m_center(rectToCircumscribe.GetCenter()),
                            m_radius(rectToCircumscribe.GetDiagonalRadius())
                        {
                        }
			Circle(const Vector<Scalar> &p, Scalar radius)
				:m_center(p),m_radius(radius)
			{
			}
			Circle(Scalar x, Scalar y, Scalar radius)
				:m_center(Vector<Scalar>(x,y)),m_radius(radius)
			{
			}
			~Circle()
			{
			}

			Vector<Scalar> GetCenter() const { return m_center; }
			Scalar GetRadius() const { return m_radius; }
			bool Intersects(const Circle<Scalar> &other)const
			{
				/*
				(11:22:01 PM) Victor Dods: so in order to find the distance between the centers of the objects, we want length(c1 - c2), where length(vec) = sqrt(vec.x*vec.x + vec.y*vec.y)
				(11:22:18 PM) Victor Dods: now let these objects have radii r1 and r2
				(11:22:38 PM) Victor Dods: the objects are intersecting if length(c1 - c2) < r1+r2
				(11:23:01 PM) Victor Dods: but, we can get rid of the square root, if we square both sides of that last equation
				(11:23:24 PM) Victor Dods: so it becomes length_squared(c1 - c2) < (r1 + r2)^2
				(11:23:38 PM) Victor Dods: where length_squared(vec) = vec.x*vec.x + vec.y*vec.y
				*/
				
				Scalar difference = (GetCenter() - other.GetCenter()).LengthSquared();
				if(difference < (m_radius + other.m_radius) * (m_radius + other.m_radius))
				    return true;
				else return false;
			}
		private:
			Vector<Scalar> m_center;
			Scalar m_radius;
		};
	}
}


#endif
