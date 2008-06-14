// Copyright (C) 2008  Davis E. King (davisking@users.sourceforge.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_KCENTROId_
#define DLIB_KCENTROId_

#include <vector>

#include "kcentroid_abstract.h"
#include "../matrix.h"
#include "function.h"
#include "../std_allocator.h"

namespace dlib
{

// ----------------------------------------------------------------------------------------

    template <typename kernel_type>
    class kcentroid
    {
        /*!
            This is an implementation of an online algorithm for recursively estimating the
            centroid of a sequence of training points.  It uses the sparsification technique
            described in the paper The Kernel Recursive Least Squares Algorithm by Yaakov Engel.

            To understand the code it would also be useful to consult page 114 of the book Kernel 
            Methods for Pattern Analysis by Taylor and Cristianini as well as page 554 
            (particularly equation 18.31) of the book Learning with Kernels by Scholkopf and Smola.
        !*/

    public:
        typedef typename kernel_type::scalar_type scalar_type;
        typedef typename kernel_type::sample_type sample_type;
        typedef typename kernel_type::mem_manager_type mem_manager_type;


        explicit kcentroid (
            const kernel_type& kernel_, 
            scalar_type tolerance_ = 0.001,
            unsigned long max_dictionary_size_ = 1000000
        ) : 
            kernel(kernel_), 
            my_tolerance(tolerance_),
            my_max_dictionary_size(max_dictionary_size_)
        {
            clear_dictionary();
        }

        scalar_type tolerance() const
        {
            return my_tolerance;
        }

        unsigned long max_dictionary_size() const
        {
            return my_max_dictionary_size;
        }

        void clear_dictionary ()
        {
            dictionary.clear();
            alpha.clear();

            K_inv.set_size(0,0);
            K.set_size(0,0);
            samples_seen = 0;
            bias = 0;
        }

        scalar_type operator() (
            const sample_type& x
        ) const
        {
            scalar_type temp = 0;
            for (unsigned long i = 0; i < alpha.size(); ++i)
                temp += alpha[i]*kernel(dictionary[i], x);

            return std::sqrt(kernel(x,x) + bias - 2*temp);
        }

        double samples_trained (
        ) const
        {
            return samples_seen;
        }

        scalar_type test_and_train (
            const sample_type& x
        )
        {
            ++samples_seen;
            const double xscale = 1.0/samples_seen;
            const double cscale = 1-xscale;
            return train_and_maybe_test(x,cscale,xscale,true);
        }

        void train (
            const sample_type& x
        )
        {
            ++samples_seen;
            const double xscale = 1.0/samples_seen;
            const double cscale = 1-xscale;
            train_and_maybe_test(x,cscale,xscale,false);
        }

        scalar_type test_and_train (
            const sample_type& x,
            double cscale,
            double xscale
        )
        {
            ++samples_seen;
            return train_and_maybe_test(x,cscale,xscale,true);
        }

        void train (
            const sample_type& x,
            double cscale,
            double xscale
        )
        {
            ++samples_seen;
            train_and_maybe_test(x,cscale,xscale,false);
        }

        void swap (
            kcentroid& item
        )
        {
            exchange(kernel, item.kernel);
            dictionary.swap(item.dictionary);
            alpha.swap(item.alpha);
            K_inv.swap(item.K_inv);
            K.swap(item.K);
            exchange(my_tolerance, item.my_tolerance);
            exchange(samples_seen, item.samples_seen);
            exchange(bias, item.bias);
            a.swap(item.a);
            k.swap(item.k);
        }

        unsigned long dictionary_size (
        ) const { return dictionary.size(); }

        friend void serialize(const kcentroid& item, std::ostream& out)
        {
            serialize(item.kernel, out);
            serialize(item.dictionary, out);
            serialize(item.alpha, out);
            serialize(item.K_inv, out);
            serialize(item.K, out);
            serialize(item.my_tolerance, out);
            serialize(item.samples_seen, out);
            serialize(item.bias, out);
        }

        friend void deserialize(kcentroid& item, std::istream& in)
        {
            deserialize(item.kernel, in);
            deserialize(item.dictionary, in);
            deserialize(item.alpha, in);
            deserialize(item.K_inv, in);
            deserialize(item.K, in);
            deserialize(item.my_tolerance, in);
            deserialize(item.samples_seen, in);
            deserialize(item.bias, in);
        }

    private:

        scalar_type train_and_maybe_test (
            const sample_type& x,
            double cscale,
            double xscale,
            bool do_test
        )
        {
            scalar_type test_result = 0;
            const scalar_type kx = kernel(x,x);
            if (alpha.size() == 0)
            {
                // set initial state since this is the first training example we have seen

                K_inv.set_size(1,1);
                K_inv(0,0) = 1/kx;
                K.set_size(1,1);
                K(0,0) = kx;

                alpha.push_back(xscale);
                dictionary.push_back(x);
            }
            else
            {
                // fill in k
                k.set_size(alpha.size());
                for (long r = 0; r < k.nr(); ++r)
                    k(r) = kernel(x,dictionary[r]);

                if (do_test)
                {
                    test_result = std::sqrt(kx + bias - 2*trans(vector_to_matrix(alpha))*k);
                }

                // compute the error we would have if we approximated the new x sample
                // with the dictionary.  That is, do the ALD test from the KRLS paper.
                a = K_inv*k;
                scalar_type delta = kx - trans(k)*a;

                // if this new vector isn't approximately linearly dependent on the vectors
                // in our dictionary.
                if (std::abs(delta) > my_tolerance)
                {
                    if (dictionary.size() >= my_max_dictionary_size)
                    {
                        // We need to remove one of the old members of the dictionary before
                        // we proceed with adding a new one.  So remove the oldest dictionary vector.
                        const long idx_to_remove = 0;

                        remove_dictionary_vector(idx_to_remove);

                        // recompute these guys since they were computed with the old
                        // kernel matrix
                        k = remove_row(k,idx_to_remove);
                        a = K_inv*k;
                        delta = kx - trans(k)*a;
                    }

                    // add x to the dictionary
                    dictionary.push_back(x);


                    // update K_inv by computing the new one in the temp matrix (equation 3.14)
                    matrix<scalar_type,0,0,mem_manager_type> temp(K_inv.nr()+1, K_inv.nc()+1);
                    // update the middle part of the matrix
                    set_subm(temp, get_rect(K_inv)) = K_inv + a*trans(a)/delta;
                    // update the right column of the matrix
                    set_subm(temp, 0, K_inv.nr(),K_inv.nr(),1) = -a/delta;
                    // update the bottom row of the matrix
                    set_subm(temp, K_inv.nr(), 0, 1, K_inv.nr()) = trans(-a/delta);
                    // update the bottom right corner of the matrix
                    temp(K_inv.nr(), K_inv.nc()) = 1/delta;
                    // put temp into K_inv
                    temp.swap(K_inv);



                    // update K (the kernel matrix)
                    temp.set_size(K.nr()+1, K.nc()+1);
                    set_subm(temp, get_rect(K)) = K;
                    // update the right column of the matrix
                    set_subm(temp, 0, K.nr(),K.nr(),1) = k;
                    // update the bottom row of the matrix
                    set_subm(temp, K.nr(), 0, 1, K.nr()) = trans(k);
                    temp(K.nr(), K.nc()) = kx;
                    // put temp into K
                    temp.swap(K);


                    // now update the alpha vector 
                    for (unsigned long i = 0; i < alpha.size(); ++i)
                    {
                        alpha[i] *= cscale;
                    }
                    alpha.push_back(xscale);
                }
                else
                {
                    // update the alpha vector so that this new sample has been added into
                    // the mean vector we are accumulating
                    for (unsigned long i = 0; i < alpha.size(); ++i)
                    {
                        alpha[i] = cscale*alpha[i] + xscale*a(i);
                    }
                }
            }

            // recompute the bias term
            bias = sum(pointwise_multiply(K, vector_to_matrix(alpha)*trans(vector_to_matrix(alpha))));
            
            return test_result;
        }

        void remove_dictionary_vector (
            long i
        )
        /*!
            requires
                - 0 <= i < dictionary.size()
            ensures
                - #dictionary.size() == dictionary.size() - 1
                - #alpha.size() == alpha.size() - 1
                - updates the K_inv matrix so that it is still a proper inverse of the
                  kernel matrix
                - also removes the necessary row and column from the K matrix
                - uses the this->a variable so after this function runs that variable
                  will contain a different value.  
        !*/
        {
            // remove the dictionary vector 
            dictionary.erase(dictionary.begin()+i);

            // remove the i'th vector from the inverse kernel matrix.  This formula is basically
            // just the reverse of the way K_inv is updated by equation 3.14 during normal training.
            K_inv = removerc(K_inv,i,i) - remove_row(colm(K_inv,i)/K_inv(i,i),i)*remove_col(rowm(K_inv,i),i);

            // now compute the updated alpha values to take account that we just removed one of 
            // our dictionary vectors
            a = (K_inv*remove_row(K,i)*vector_to_matrix(alpha));

            // now copy over the new alpha values
            alpha.resize(alpha.size()-1);
            for (unsigned long k = 0; k < alpha.size(); ++k)
            {
                alpha[k] = a(k);
            }

            // update the K matrix as well
            K = removerc(K,i,i);
        }



        typedef std_allocator<sample_type, mem_manager_type> alloc_sample_type;
        typedef std_allocator<scalar_type, mem_manager_type> alloc_scalar_type;
        typedef std::vector<sample_type,alloc_sample_type> dictionary_vector_type;
        typedef std::vector<scalar_type,alloc_scalar_type> alpha_vector_type;


        kernel_type kernel;
        dictionary_vector_type dictionary;
        alpha_vector_type alpha;

        matrix<scalar_type,0,0,mem_manager_type> K_inv;
        matrix<scalar_type,0,0,mem_manager_type> K;

        scalar_type my_tolerance;
        unsigned long my_max_dictionary_size;
        scalar_type samples_seen;
        scalar_type bias;


        // temp variables here just so we don't have to reconstruct them over and over.  Thus, 
        // they aren't really part of the state of this object.
        matrix<scalar_type,0,1,mem_manager_type> a;
        matrix<scalar_type,0,1,mem_manager_type> k;

    };

// ----------------------------------------------------------------------------------------

    template <typename kernel_type>
    void swap(kcentroid<kernel_type>& a, kcentroid<kernel_type>& b)
    { a.swap(b); }

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_KCENTROId_

