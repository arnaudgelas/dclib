// Copyright (C) 2005  Davis E. King (davisking@users.sourceforge.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_MEMBER_FUNCTION_POINTER_KERNEl_1_
#define DLIB_MEMBER_FUNCTION_POINTER_KERNEl_1_

#include "../algs.h"
#include "member_function_pointer_kernel_abstract.h"
#include "../enable_if.h"
#include <new>

namespace dlib
{

// ----------------------------------------------------------------------------------------

    template <
        typename PARAM1 = void,
        typename PARAM2 = void,
        typename PARAM3 = void,
        typename PARAM4 = void
        >
    class member_function_pointer_kernel_1;

// ----------------------------------------------------------------------------------------

    template <unsigned long num_args>
    class mfp_kernel_1_base_class
    {
        /*
            All member function pointer classes inherit from this class.  This
            is where most of the things in a member function pointer are defined.

            The reason for the num_args template argument to this class is to prevent
            any sort of implicit casting between derived member function pointer classes
            that take different numbers of arguments.
        */
    protected:
        enum mfp_type { mfp_nonconst, mfp_const, mfp_null};

        class mp_base_base
        {
        public:
            mp_base_base(void* ptr, mfp_type type_) : o(ptr),type(type_) {}
            virtual ~mp_base_base(){}
            virtual void clone(void* ptr) const = 0;
            virtual bool is_same (const mp_base_base* item) const = 0;
            bool is_set () const { return o!=0; }

            void* const o;
            const mfp_type type;
        };

        template <typename T>
        class mp_null : public mp_base_base 
        {
        public:
            typedef void (T::*mfp_pointer_type)() ;

            mp_null (void* , mfp_pointer_type ) : mp_base_base(0,mfp_null), callback(0) {}
            mp_null () : mp_base_base(0,mfp_null), callback(0) {}

            const mfp_pointer_type callback;
        };

        template <typename mp_impl>
        class mp_impl_T : public mp_impl 
        {
            /*
                This class supplies the implementations clone() and is_same() for any
                classes that inherit from mp_base_base.  It does this in a very
                roundabout way...
            */
               
        public:
            typedef typename mp_impl::mfp_pointer_type mfp_pointer_type;

            mp_impl_T() : mp_impl(0,0) {}
            mp_impl_T(void* ptr, mfp_pointer_type cb) : mp_impl(ptr,cb) {}
            void clone   (void* ptr) const  { new(ptr) mp_impl_T(this->o,this->callback); }
            bool is_same (const mp_base_base* item) const 
            {
                if (item->o == 0 && this->o == 0)
                {
                    return true;
                }
                else if (item->o == this->o && this->type == item->type)
                {
                    const mp_impl* i = reinterpret_cast<const mp_impl*>(item);
                    return (i->callback == this->callback);
                }
                return false;
            }
        };

        struct dummy { void nonnull() {}; };

        typedef mp_impl_T<mp_null<dummy> > mp_null_impl;
    public:

        mfp_kernel_1_base_class (
            const mfp_kernel_1_base_class& item
        ) { item.mp()->clone(mp_memory.data); }

        mfp_kernel_1_base_class (  
        ) { mp_null_impl().clone(mp_memory.data); }

        bool operator == (
            const mfp_kernel_1_base_class& item
        ) const { return mp()->is_same(item.mp()); }

        bool operator != (
            const mfp_kernel_1_base_class& item
        ) const { return !(*this == item); }

        mfp_kernel_1_base_class& operator= (
            const mfp_kernel_1_base_class& item
        ) { mfp_kernel_1_base_class(item).swap(*this); return *this;  }

        ~mfp_kernel_1_base_class (
        ) { destroy_mp_memory(); }

        void clear(
        ) { mfp_kernel_1_base_class().swap(*this); }

        bool is_set (
        ) const { return mp()->is_set(); } 

    private:
        typedef void (dummy::*safe_bool)();

    public:
        operator safe_bool () const { return is_set() ? &dummy::nonnull : 0; }
        bool operator!() const { return !is_set(); }

        void swap (
            mfp_kernel_1_base_class& item
        ) 
        {  
            // make a temp copy of item
            mfp_kernel_1_base_class temp(item);

            // destory the stuff in item
            item.destroy_mp_memory();
            // copy *this into item
            mp()->clone(item.mp_memory.data);

            // destory the stuff in this 
            destroy_mp_memory();
            // copy temp into *this
            temp.mp()->clone(mp_memory.data);
        }

    protected:

        // make a block of memory big enough to hold a mp_null object.
        union data_block_type
        {
            // All of this garbage is to make sure this union is properly aligned 
            // (a union is always aligned such that everything in it would be properly
            // aligned.  So the assumption here is that one of these things big good
            // alignment to satisfy mp_null_impl objects (or other objects like it).
            void* void_ptr;
            struct {
                void (mp_base_base::*callback)();
                mp_base_base* o; 
            } stuff;

            char data[sizeof(mp_null_impl)];
        } mp_memory;

        void destroy_mp_memory (
        )
        {
            // Honestly this probably doesn't even do anything but I'm putting
            // it here just for good measure.
            mp()->~mp_base_base();
        }

        mp_base_base*       mp ()       { void* ptr = mp_memory.data;       return static_cast<mp_base_base*>(ptr); }
        const mp_base_base* mp () const { const void* ptr = mp_memory.data; return static_cast<const mp_base_base*>(ptr); }
        
    };

// ----------------------------------------------------------------------------------------

    template <>
    class member_function_pointer_kernel_1<void,void,void,void> : public mfp_kernel_1_base_class<0>
    {
        class mp_base : public mp_base_base {
        public:
            mp_base(void* ptr, mfp_type type_) : mp_base_base(ptr,type_) {}
            virtual void call() const = 0;
        };

        template <typename T>
        class mp_impl : public mp_base {
        public:
            typedef void (T::*mfp_pointer_type)() ;
            void call () const { (static_cast<T*>(this->o)->*callback)(); }

            mp_impl      ( void* object, mfp_pointer_type cb) : mp_base(object, mfp_nonconst), callback(cb) {}
            const mfp_pointer_type callback;
        };

        template <typename T>
        class mp_impl_const : public mp_base {
        public:
            typedef void ((T::*mfp_pointer_type)()const);
            void call () const  { (static_cast<const T*>(this->o)->*callback)(); }

            mp_impl_const ( void* object, mfp_pointer_type cb) : mp_base(object,mfp_const), callback(cb) {}
            const mfp_pointer_type callback;
        };

    public:
        typedef void param1_type;
        typedef void param2_type;
        typedef void param3_type;
        typedef void param4_type;

        void operator() () const { static_cast<const mp_base*>((const void*)mp_memory.data)->call(); }

        // This is here just to validate the assumption that our block of memory we have made
        // in mp_memory.data is the right size to store the data for this object.  If you
        // get a compiler error on this line then email me :)
        member_function_pointer_kernel_1()
        { COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl<dummy> >));
          COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl_const<dummy> >)); }

        // the reason for putting disable_if on this function is that it avoids an overload
        // resolution bug in visual studio.
        template <typename T> typename disable_if<is_const_type<T>,void>::type 
        set(T& object, typename mp_impl<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl<T> >(&object,cb).clone(mp_memory.data); }

        template <typename T> void set(const T& object, typename mp_impl_const<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl_const<T> >((void*)&object,cb).clone(mp_memory.data); }

    };    

// ----------------------------------------------------------------------------------------

    template <
        typename PARAM1
        >
    class member_function_pointer_kernel_1<PARAM1,void,void,void> : public mfp_kernel_1_base_class<1>
    {
        class mp_base : public mp_base_base {
        public:
            mp_base(void* ptr, mfp_type type_) : mp_base_base(ptr,type_) {}
            virtual void call(PARAM1) const = 0;
        };

        template <typename T>
        class mp_impl : public mp_base {
        public:
            typedef void (T::*mfp_pointer_type)(PARAM1) ;
            void call (PARAM1 p1) const { (static_cast<T*>(this->o)->*callback)(p1); }

            mp_impl      ( void* object, mfp_pointer_type cb) : mp_base(object, mfp_nonconst), callback(cb) {}
            const mfp_pointer_type callback;
        };

        template <typename T>
        class mp_impl_const : public mp_base {
        public:
            typedef void ((T::*mfp_pointer_type)(PARAM1)const);
            void call (PARAM1 p1) const  { (static_cast<const T*>(this->o)->*callback)(p1); }

            mp_impl_const ( void* object, mfp_pointer_type cb) : mp_base(object,mfp_const), callback(cb) {}
            const mfp_pointer_type callback;
        };

    public:
        typedef PARAM1 param1_type;
        typedef void param2_type;
        typedef void param3_type;
        typedef void param4_type;

        void operator() (PARAM1 p1) const { static_cast<const mp_base*>((const void*)mp_memory.data)->call(p1); }

        // This is here just to validate the assumption that our block of memory we have made
        // in mp_memory.data is the right size to store the data for this object.  If you
        // get a compiler error on this line then email me :)
        member_function_pointer_kernel_1()
        { COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl<dummy> >));
          COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl_const<dummy> >)); }

        // the reason for putting disable_if on this function is that it avoids an overload
        // resolution bug in visual studio.
        template <typename T> typename disable_if<is_const_type<T>,void>::type 
        set(T& object, typename mp_impl<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl<T> >(&object,cb).clone(mp_memory.data); }

        template <typename T> void set(const T& object, typename mp_impl_const<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl_const<T> >((void*)&object,cb).clone(mp_memory.data); }

    };    

// ----------------------------------------------------------------------------------------

    template <
        typename PARAM1,
        typename PARAM2
        >
    class member_function_pointer_kernel_1<PARAM1,PARAM2,void,void> : public mfp_kernel_1_base_class<2>
    {
        class mp_base : public mp_base_base {
        public:
            mp_base(void* ptr, mfp_type type_) : mp_base_base(ptr,type_) {}
            virtual void call(PARAM1,PARAM2) const = 0;
        };

        template <typename T>
        class mp_impl : public mp_base {
        public:
            typedef void (T::*mfp_pointer_type)(PARAM1,PARAM2) ;
            void call (PARAM1 p1, PARAM2 p2) const { (static_cast<T*>(this->o)->*callback)(p1,p2); }

            mp_impl      ( void* object, mfp_pointer_type cb) : mp_base(object, mfp_nonconst), callback(cb) {}
            const mfp_pointer_type callback;
        };

        template <typename T>
        class mp_impl_const : public mp_base {
        public:
            typedef void ((T::*mfp_pointer_type)(PARAM1,PARAM2)const);
            void call (PARAM1 p1, PARAM2 p2) const  { (static_cast<const T*>(this->o)->*callback)(p1,p2); }

            mp_impl_const ( void* object, mfp_pointer_type cb) : mp_base(object,mfp_const), callback(cb) {}
            const mfp_pointer_type callback;
        };

    public:
        typedef PARAM1 param1_type;
        typedef PARAM2 param2_type;
        typedef void param3_type;
        typedef void param4_type;

        void operator() (PARAM1 p1, PARAM2 p2) const { static_cast<const mp_base*>((const void*)mp_memory.data)->call(p1,p2); }

        // This is here just to validate the assumption that our block of memory we have made
        // in mp_memory.data is the right size to store the data for this object.  If you
        // get a compiler error on this line then email me :)
        member_function_pointer_kernel_1()
        { COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl<dummy> >));
          COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl_const<dummy> >)); }

        // the reason for putting disable_if on this function is that it avoids an overload
        // resolution bug in visual studio.
        template <typename T> typename disable_if<is_const_type<T>,void>::type 
        set(T& object, typename mp_impl<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl<T> >(&object,cb).clone(mp_memory.data); }

        template <typename T> void set(const T& object, typename mp_impl_const<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl_const<T> >((void*)&object,cb).clone(mp_memory.data); }

    };    

// ----------------------------------------------------------------------------------------

    template <
        typename PARAM1,
        typename PARAM2,
        typename PARAM3
        >
    class member_function_pointer_kernel_1<PARAM1,PARAM2,PARAM3,void> : public mfp_kernel_1_base_class<3>
    {
        class mp_base : public mp_base_base {
        public:
            mp_base(void* ptr, mfp_type type_) : mp_base_base(ptr,type_) {}
            virtual void call(PARAM1,PARAM2,PARAM3) const = 0;
        };

        template <typename T>
        class mp_impl : public mp_base {
        public:
            typedef void (T::*mfp_pointer_type)(PARAM1,PARAM2,PARAM3) ;
            void call (PARAM1 p1, PARAM2 p2, PARAM3 p3) const { (static_cast<T*>(this->o)->*callback)(p1,p2,p3); }

            mp_impl      ( void* object, mfp_pointer_type cb) : mp_base(object, mfp_nonconst), callback(cb) {}
            const mfp_pointer_type callback;
        };

        template <typename T>
        class mp_impl_const : public mp_base {
        public:
            typedef void ((T::*mfp_pointer_type)(PARAM1,PARAM2,PARAM3)const);
            void call (PARAM1 p1, PARAM2 p2, PARAM3 p3) const  { (static_cast<const T*>(this->o)->*callback)(p1,p2,p3); }

            mp_impl_const ( void* object, mfp_pointer_type cb) : mp_base(object,mfp_const), callback(cb) {}
            const mfp_pointer_type callback;
        };

    public:
        typedef PARAM1 param1_type;
        typedef PARAM2 param2_type;
        typedef PARAM3 param3_type;
        typedef void param4_type;

        void operator() (PARAM1 p1, PARAM2 p2, PARAM3 p3) const { static_cast<const mp_base*>((const void*)mp_memory.data)->call(p1,p2,p3); }

        // This is here just to validate the assumption that our block of memory we have made
        // in mp_memory.data is the right size to store the data for this object.  If you
        // get a compiler error on this line then email me :)
        member_function_pointer_kernel_1()
        { COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl<dummy> >));
          COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl_const<dummy> >)); }

        // the reason for putting disable_if on this function is that it avoids an overload
        // resolution bug in visual studio.
        template <typename T> typename disable_if<is_const_type<T>,void>::type 
        set(T& object, typename mp_impl<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl<T> >(&object,cb).clone(mp_memory.data); }

        template <typename T> void set(const T& object, typename mp_impl_const<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl_const<T> >((void*)&object,cb).clone(mp_memory.data); }

    };    

// ----------------------------------------------------------------------------------------

    template <
        typename PARAM1,
        typename PARAM2,
        typename PARAM3,
        typename PARAM4
        >
    class member_function_pointer_kernel_1 : public mfp_kernel_1_base_class<4>
    {
        class mp_base : public mp_base_base {
        public:
            mp_base(void* ptr, mfp_type type_) : mp_base_base(ptr,type_) {}
            virtual void call(PARAM1,PARAM2,PARAM3,PARAM4) const = 0;
        };

        template <typename T>
        class mp_impl : public mp_base {
        public:
            typedef void (T::*mfp_pointer_type)(PARAM1,PARAM2,PARAM3, PARAM4) ;
            void call (PARAM1 p1, PARAM2 p2, PARAM3 p3, PARAM4 p4) const { (static_cast<T*>(this->o)->*callback)(p1,p2,p3,p4); }

            mp_impl      ( void* object, mfp_pointer_type cb) : mp_base(object, mfp_nonconst), callback(cb) {}
            const mfp_pointer_type callback;
        };

        template <typename T>
        class mp_impl_const : public mp_base {
        public:
            typedef void ((T::*mfp_pointer_type)(PARAM1,PARAM2,PARAM3,PARAM4)const);
            void call (PARAM1 p1, PARAM2 p2, PARAM3 p3, PARAM4 p4) const  { (static_cast<const T*>(this->o)->*callback)(p1,p2,p3,p4); }

            mp_impl_const ( void* object, mfp_pointer_type cb) : mp_base(object,mfp_const), callback(cb) {}
            const mfp_pointer_type callback;
        };

    public:
        typedef PARAM1 param1_type;
        typedef PARAM2 param2_type;
        typedef PARAM3 param3_type;
        typedef PARAM4 param4_type;

        void operator() (PARAM1 p1, PARAM2 p2, PARAM3 p3, PARAM4 p4) const 
        { static_cast<const mp_base*>((const void*)mp_memory.data)->call(p1,p2,p3,p4); }

        // This is here just to validate the assumption that our block of memory we have made
        // in mp_memory.data is the right size to store the data for this object.  If you
        // get a compiler error on this line then email me :)
        member_function_pointer_kernel_1()
        { COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl<dummy> >));
          COMPILE_TIME_ASSERT(sizeof(mp_memory.data) >= sizeof(mp_impl_T<mp_impl_const<dummy> >)); }

        // the reason for putting disable_if on this function is that it avoids an overload
        // resolution bug in visual studio.
        template <typename T> typename disable_if<is_const_type<T>,void>::type 
        set(T& object, typename mp_impl<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl<T> >(&object,cb).clone(mp_memory.data); }

        template <typename T> void set(const T& object, typename mp_impl_const<T>::mfp_pointer_type cb) 
        { destroy_mp_memory(); mp_impl_T<mp_impl_const<T> >((void*)&object,cb).clone(mp_memory.data); }

    };    

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_MEMBER_FUNCTION_POINTER_KERNEl_1_

