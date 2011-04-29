// Copyright (C) 2005  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#undef DLIB_RECTANGLe_ABSTRACT_
#ifdef DLIB_RECTANGLe_ABSTRACT_

#include "vector_abstract.h"
#include <iostream>
#include "../serialize.h"

namespace dlib
{

    class rectangle
    {
        /*!
            INITIAL VALUE
                The initial value of this object is defined by its constructor.                

            WHAT THIS OBJECT REPRESENTS
                This object represents a rectangular region inside a Cartesian 
                coordinate system.  The region is the rectangle with its top 
                left corner at position (left(),top()) and its bottom right corner 
                at (right(),bottom()).

                Note that the origin of the coordinate system, i.e. (0,0), is located
                at the upper left corner.  That is, points such as (1,1) or (3,5) 
                represent locations that are below and to the right of the origin.

                Also note that rectangles where top() > bottom() or left() > right() 
                represent empty rectangles.
        !*/

    public:

        rectangle (
            const rectangle& rect
        );
        /*!
            ensures
                - #*this represents the same rectangle as rect
        !*/

        rectangle (
        );
        /*!
            ensures
                - #left() == 0
                - #top() == 0
                - #right() == -1 
                - #bottom() == -1 
                - #is_empty() == true
        !*/

        rectangle (
            long left_,
            long top_,
            long right_,
            long bottom_
        );
        /*!
            ensures
                - #left() == left_
                - #top() == top_
                - #right() == right_
                - #bottom() == bottom_
        !*/

        rectangle (
            unsigned long width_,
            unsigned long height_
        );
        /*!
            requires
                - (width_ > 0 && height_ > 0) || (width_ == 0 && height_ == 0)
            ensures
                - #left() == 0  
                - #top() == 0
                - #width() == width_
                - #height() == height_
        !*/

        rectangle (
            const point& p
        );
        /*!
            ensures
                - #left()   == p.x()
                - #top()    == p.y()
                - #right()  == p.x()
                - #bottom() == p.y()
        !*/

        template <typename T>
        rectangle (
            const vector<T,2>& p1,
            const vector<T,2>& p2
        );
        /*!
            ensures
                - #*this == rectangle(p1) + rectangle(p2)
        !*/

        long left (
        ) const;
        /*!
            ensures
                - returns the x coordinate for the left side of this rectangle
        !*/

        long& left (
        );
        /*!
            ensures
                - returns a non-const reference to the x coordinate for the left side 
                  of this rectangle
        !*/

        void set_left (
            long left_
        );
        /*!
            ensures
                - #left() == left_
        !*/

        long top (
        ) const;
        /*!
            ensures
                - returns the y coordinate for the top of this rectangle
        !*/

        long& top (
        );
        /*!
            ensures
                - returns a non-const reference to the y coordinate for the 
                  top of this rectangle
        !*/

        void set_top (
            long top_
        );
        /*!
            ensures
                - #top() == top_
        !*/

        long right (
        ) const;
        /*!
            ensures
                - returns the x coordinate for the right side of this rectangle
        !*/

        long& right (
        );
        /*!
            ensures
                - returns a non-const reference to the x coordinate for the right 
                  side of this rectangle
        !*/

        void set_right (
            long right_
        );
        /*!
            ensures
                - #right() == right_
        !*/

        long bottom (
        ) const;
        /*!
            ensures
                - returns the y coordinate for the bottom of this rectangle
        !*/
       
        long& bottom (
        );
        /*!
            ensures
                - returns a non-const reference to the y coordinate for the bottom 
                  of this rectangle
        !*/
       
        void set_bottom (
            long bottom_
        );
        /*!
            ensures
                - #bottom() == bottom_
        !*/

        const point tl_corner (
        ) const;
        /*!
            ensures
                - returns point(left(), top()) 
                  (i.e. returns the top left corner point for this rectangle)
        !*/

        const point bl_corner (
        ) const;
        /*!
            ensures
                - returns point(left(), bottom()) 
                  (i.e. returns the bottom left corner point for this rectangle)
        !*/

        const point tr_corner (
        ) const;
        /*!
            ensures
                - returns point(right(), top()) 
                  (i.e. returns the top right corner point for this rectangle)
        !*/

        const point br_corner (
        ) const;
        /*!
            ensures
                - returns point(right(), bottom()) 
                  (i.e. returns the bottom right corner point for this rectangle)
        !*/

        bool is_empty (
        ) const;
        /*!
            ensures
                - if (top() > bottom() || left() > right()) then
                    - returns true
                - else
                    - returns false
        !*/

        unsigned long width (
        ) const;
        /*!
            ensures
                - if (is_empty()) then
                    - returns 0
                - else
                    - returns the width of this rectangle.
                      (i.e. right() - left() + 1)
        !*/

        unsigned long height (
        ) const;
        /*!
            ensures
                - if (is_empty()) then
                    - returns 0
                - else
                    - returns the height of this rectangle.
                      (i.e. bottom() - top() + 1)
        !*/

        unsigned long area (
        ) const;
        /*!
            ensures
                - returns width()*height()
        !*/

        rectangle operator + (
            const rectangle& rhs
        ) const;
        /*!
            ensures
                - if (rhs.is_empty() == false && this->is_empty() == false) then
                    - returns the smallest rectangle that contains both *this and 
                      rhs.
                - if (rhs.is_empty() == true && this->is_empty() == false) then
                    - returns *this
                - if (rhs.is_empty() == false && this->is_empty() == true) then
                    - returns rhs
                - if (rhs.is_empty() == true && this->is_empty() == true) then
                    - returns a rectangle that has is_empty() == true
        !*/

        rectangle intersect (
            const rectangle& rhs
        ) const;
        /*!
            ensures
                - if (there is a region of intersection between *this and rhs) then
                    - returns a rectangle that represents the intersection of *this 
                      and rhs.
                - else
                    - returns a rectangle where is_empty() == true
        !*/

        bool contains (
            long x,
            long y
        ) const;
        /*!
            ensures
                - if (the point (x,y) is contained in this rectangle) then
                    - returns true
                - else
                    - returns false
        !*/

        bool contains (
            const point& p
        ) const;
        /*!
            ensures
                - if (the point (p.x(),p.y()) is contained in this rectangle) then
                    - returns true
                - else
                    - returns false
        !*/

        bool contains (
            const rectangle& rect
        ) const;
        /*!
            ensures
                - if (rect + *this == *this) then
                    - returns true
                      (i.e. returns true if *this contains the given rectangle)
                - else
                    - returns false
        !*/

        rectangle& operator= (
            const rectangle& rect
        );
        /*!
            ensures
                - #*this represents the same rectangle as rect
                - returns #*this
        !*/

        rectangle& operator+= (
            const rectangle& rect
        );
        /*!
            ensures
                - #*this == *this + rect 
                - returns #*this
        !*/

        bool operator== (
            const rectangle& rect
        ) const;
        /*!
            ensures
                - if (top() == rect.top() && left() == rect.left() &&
                      right() == rect.right() && bottom() == rect.bottom()) then
                    - returns true
                - else
                    - returns false
        !*/

        bool operator!= (
            const rectangle& rect
        ) const;
        /*!
            ensures
                - returns !(*this == rect)
        !*/
    };

// ----------------------------------------------------------------------------------------

    void serialize (
        const rectangle& item, 
        std::ostream& out
    );   
    /*!
        provides serialization support 
    !*/

    void deserialize (
        rectangle& item, 
        std::istream& in
    );   
    /*!
        provides deserialization support 
    !*/

    std::ostream& operator<< (
        std::ostream& out, 
        const rectangle& item 
    );   
    /*!
        ensures
            - writes item to out in the form "[(left, top) (right, bottom)]"
    !*/

    std::istream& operator>>(
        std::istream& in, 
        rectangle& item 
    );   
    /*!
        ensures
            - reads a rectangle from the input stream in and stores it in #item.
              The data in the input stream should be of the form [(left, top) (right, bottom)]
    !*/

// ----------------------------------------------------------------------------------------

    inline const rectangle centered_rect (
        const point& p,
        unsigned long width,
        unsigned long height
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - if (width == 0 || height == 0)
                    - R.width() == 0 
                    - R.height() == 0 
                - else
                    - R.width() == width
                    - R.height() == height 
                - R.tl_corner() == point(p.x()-width/2, p.y()-height/2)
                - The center of R is a the point p 
    !*/

// ----------------------------------------------------------------------------------------

    const rectangle centered_rect (
        long x,
        long y,
        unsigned long width,
        unsigned long height
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - if (width == 0 || height == 0)
                    - R.width() == 0 
                    - R.height() == 0 
                - else
                    - R.width() == width
                    - R.height() == height 
                - R.tl_corner() == point(x-width/2, y-height/2)
                - The center of R is a the point (x,y)
    !*/

// ----------------------------------------------------------------------------------------

    inline const rectangle centered_rect (
        const rectangle& rect,
        unsigned long width,
        unsigned long height
    );
    /*!
        ensures
            - returns centered_rect( (rect.tl_corner() + rect.br_corner())/2, width, height)
              (i.e. returns a rectangle centered on rect but with the given width
              and height)
    !*/

// ----------------------------------------------------------------------------------------

    inline const rectangle shrink_rect (
        const rectangle& rect,
        long num 
    );
    /*!
        ensures
            - returns rectangle(rect.left()+num, rect.top()+num, rect.right()-num, rect.bottom()-num)
              (i.e. shrinks the given rectangle by shrinking its border by num)
    !*/

// ----------------------------------------------------------------------------------------

    inline const rectangle grow_rect (
        const rectangle& rect,
        long num 
    );
    /*!
        ensures
            - return shrink_rect(rect, -num)
              (i.e. grows the given rectangle by expanding its border by num)
    !*/

// ----------------------------------------------------------------------------------------

    const rectangle translate_rect (
        const rectangle& rect,
        const point& p
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - R.left()   == rect.left()   + p.x()
                - R.right()  == rect.right()  + p.x()
                - R.top()    == rect.top()    + p.y()
                - R.bottom() == rect.bottom() + p.y()
    !*/

// ----------------------------------------------------------------------------------------

    const rectangle translate_rect (
        const rectangle& rect,
        long x,
        long y
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - R.left()   == rect.left()   + x
                - R.right()  == rect.right()  + x
                - R.top()    == rect.top()    + y
                - R.bottom() == rect.bottom() + y
    !*/

// ----------------------------------------------------------------------------------------

    const rectangle resize_rect (
        const rectangle& rect,
        unsigned long width,
        unsigned long height
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - if (width == 0 || height == 0)
                    - R.width() == 0 
                    - R.height() == 0 
                - else
                    - R.width() == width
                    - R.height() == height 
                - R.left() == rect.left() 
                - R.top() == rect.top() 
    !*/

// ----------------------------------------------------------------------------------------

    const rectangle resize_rect_width (
        const rectangle& rect,
        unsigned long width
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - R.width() == width
                - R.left() == rect.left() 
                - R.top() == rect.top() 
                - R.bottom() == rect.bottom() 
    !*/

// ----------------------------------------------------------------------------------------

    const rectangle resize_rect_height (
        const rectangle& rect,
        unsigned long height 
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - R.height() == height 
                - R.left() == rect.left() 
                - R.top() == rect.top() 
                - R.right() == rect.right() 
    !*/

// ----------------------------------------------------------------------------------------

    const rectangle move_rect (
        const rectangle& rect,
        const point& p
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - R.width() == rect.width() 
                - R.height() == rect.height() 
                - R.left() == p.x()
                - R.top() == p.y() 
    !*/

// ----------------------------------------------------------------------------------------

    const rectangle move_rect (
        const rectangle& rect,
        long x,
        long y 
    );
    /*!
        ensures
            - returns a rectangle R such that:
                - R.width() == rect.width() 
                - R.height() == rect.height() 
                - R.left() == x 
                - R.top() == y 
    !*/

// ----------------------------------------------------------------------------------------

    inline const point nearest_point (
        const rectangle& rect,
        const point& p
    );
    /*!
        ensures
            - if (rect.contains(p)) then
                - returns p
            - else
                - returns the point in rect that is closest to p
    !*/

// ----------------------------------------------------------------------------------------

    template <
        typename T 
        >
    const rectangle get_rect (
        const T& m
    );
    /*!
        requires
            - T has nr() and nc() functions that return longs
        ensures
            - returns rectangle(0, 0, m.nc()-1, m.nr()-1)
              (i.e. assuming T represents some kind of rectangular grid, such as
              the dlib::matrix or dlib::array2d objects, this function returns the
              bounding rectangle for that gridded object.)
    !*/

// ----------------------------------------------------------------------------------------

}

namespace std
{
    /*!
        Define std::less<rectangle> so that you can use rectangles in the associative containers.
    !*/
    template<>
    struct less<dlib::rectangle> : public binary_function<dlib::rectangle,dlib::rectangle,bool>
    {
        inline bool operator() (const dlib::rectangle& a, const dlib::rectangle& b) const
        { 
            if      (a.left() < b.left()) return true;
            else if (a.left() > b.left()) return false;
            else if (a.top() < b.top()) return true;
            else if (a.top() > b.top()) return false;
            else if (a.right() < b.right()) return true;
            else if (a.right() > b.right()) return false;
            else if (a.bottom() < b.bottom()) return true;
            else if (a.bottom() > b.bottom()) return false;
            else                    return false;
        }
    };
}

#endif // DLIB_RECTANGLe_ABSTRACT_

